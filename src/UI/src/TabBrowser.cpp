//
// Created by Adrien Blanchet on 30/04/2023.
//

#include "TabBrowser.h"
#include "FrameMain.h"

#include "Logger.h"
#include "GenericToolbox.h"


LoggerInit([]{
  Logger::setUserHeaderStr("[TabBrowser]");
});


TabBrowser::TabBrowser(FrameMain* owner_) : brls::List(), _owner_(owner_) {
  LogWarning << "Building TabBrowser" << std::endl;

  // root
  this->cd("");
  this->ls();

}
TabBrowser::~TabBrowser() {
  // make sure brls don't try to delete already deleted ptrs
  this->clear( false );
}

void TabBrowser::setRequestedCd(const std::string &requestedCd) {
  _requestedCd_ = requestedCd;
}

std::string TabBrowser::getCwd() const{
  return "/" + GenericToolbox::joinVectorString(_walkPath_, "/");
}

void TabBrowser::draw(NVGcontext *vg, int x, int y, unsigned int width, unsigned int height, brls::Style *style,
                      brls::FrameContext *ctx) {

  if( _coolDownFrames_ == 0 and not brls::Application::hasViewDisappearing() ){

    if( _requestScroll_ ){
      LogDebug << "Updating scrolling triggerd..." << std::endl;
      // update scroll can only happen if no animation is going on
      this->setUpdateScrollingOnNextFrame( true );
      _requestScroll_ = false;
    }
    else if( not _requestedCd_.empty() and not brls::Application::hasViewDisappearing() ){
      std::scoped_lock<std::mutex> g(_mutex_);
      LogDebug << "Updating list..." << std::endl;
      this->cd( _requestedCd_ );
      this->ls();
      _requestedCd_.clear();
      _requestScroll_ = true;
    }

  }

  if( _coolDownFrames_ != 0 ){ _coolDownFrames_--; }

  brls::List::draw(vg, x, y, width, height, style, ctx);
}

void TabBrowser::sortEntries(std::vector<DirEntry>& entryList_){
  GenericToolbox::sortVector( entryList_, [](const DirEntry &a_, const DirEntry &b_) {
    if( a_.isDir() and not b_.isDir() ) return true;
    if( not a_.isDir() and b_.isDir() ) return false;
    if( a_.name.empty() ) return false;
    if( b_.name.empty() ) return true;
    if( a_.name[0] != '.' and b_.name[0] == '.') return true;
    if( b_.name[0] != '.' and a_.name[0] == '.') return false;
    return GenericToolbox::toLowerCase(a_.name) < GenericToolbox::toLowerCase(b_.name);
  });
}

void TabBrowser::cd( const std::string& folder_ ){
  if( folder_.empty() ){
    _walkFocus_.resize(1, 0);
    _walkPath_.clear();
  }
  else if( folder_ == "./" ){
    _walkFocus_.back() = GenericToolbox::findElementIndex(brls::Application::getCurrentFocus(), _entryList_, [](const DirEntry& e){ return e.item.get(); });
  }
  else if( folder_ == "../" ){
    _walkFocus_.pop_back();
    _walkPath_.pop_back();
  }
  else{
    _walkFocus_.back() = GenericToolbox::findElementIndex(folder_, _entryList_, [](const DirEntry& e){ return e.name; });
    _walkFocus_.emplace_back( 0 );
    _walkPath_.emplace_back( folder_ );
  }

  if( _walkPath_.empty() ){ _owner_->setTitle( "SimpleSwitchBrowser" ); }
  else{ _owner_->setTitle( this->getCwd() ); }
}
void TabBrowser::ls(){
  this->clear( false );
  _entryList_.clear();

  std::string cwd{this->getCwd()};
  LogInfo << "Walking in " << cwd << std::endl;

  auto foldersList = GenericToolbox::getListOfSubFoldersInFolder( cwd );
  _entryList_.reserve(foldersList.size() );
  for (auto &folder: foldersList) {
    _entryList_.emplace_back();

    _entryList_.back().name = folder;
    _entryList_.back().fullPath = GenericToolbox::joinPath(cwd, folder);
    _entryList_.back().type = IS_DIR;
  }

  auto fileList = GenericToolbox::getListOfFilesInFolder( cwd );
  for (auto &file: fileList) {
    _entryList_.emplace_back();

    _entryList_.back().name = file;
    _entryList_.back().fullPath = GenericToolbox::joinPath(cwd, file);
    _entryList_.back().type = IS_FILE;
    _entryList_.back().size = double( GenericToolbox::getFileSize( _entryList_.back().fullPath ) );
  }

  // case of IO error or empty
  if( _entryList_.empty() ){
    _entryList_.emplace_back();
    _entryList_.back().type = EMPTY;
  }

  TabBrowser::sortEntries( _entryList_ );

  // build items
  for( auto& entry : _entryList_ ){
    entry.item = std::make_shared<brls::ListItem>("");
    entry.item->setHeight( 50 );
    entry.item->setValueActiveColor( nvgRGB(80, 80, 80) );
    entry.item->updateActionHint(brls::Key::A, "");

    if( not _walkPath_.empty() ){
      entry.item->registerAction("Back", brls::Key::B, [this]{
        std::scoped_lock<std::mutex> g(_mutex_);
        this->setRequestedCd( "../" );
        return true;
      });
    }

    if     ( entry.type == IS_DIR ){
      entry.item->setLabel( GenericToolbox::joinAsString(" ", "\uE2C7", entry.name) );

      auto folder{entry.name};
      entry.item->getClickEvent()->subscribe([this, folder](brls::View*){
        this->setRequestedCd( folder );
        return true;
      });
      entry.item->updateActionHint(brls::Key::A, "Open");
    }
    else if( entry.type == IS_FILE ){
      entry.item->setLabel( entry.name );
      entry.item->setValue( GenericToolbox::parseSizeUnits( entry.size ) );

      std::string filePath{entry.fullPath};
      entry.item->registerAction("Delete", brls::Key::X, [this, filePath]{

        auto* dialog = new brls::Dialog("Do you want to remove \"" + filePath + "\" ?");
        dialog->addButton("Yes", [this, dialog, filePath](brls::View* view) {
          dialog->close();
          LogAlert << "Deleting: " << filePath << std::endl;
          GenericToolbox::deleteFile( filePath );

          std::scoped_lock<std::mutex> g(_mutex_);
          _coolDownFrames_ = 2;
          this->setRequestedCd( "./" );
        });
        dialog->addButton("No", [dialog](brls::View* view) { dialog->close(); });

        dialog->setCancelable(true);
        dialog->open();
        return true;
      });
    }
    else if( entry.type == EMPTY ){
      entry.item->setValue("empty folder");
    }

  }

  // add to view now
  for( auto& entry : _entryList_ ){
    this->addView( entry.item.get() );
  }

  int focusIndex{_walkFocus_.back()};
  while( focusIndex >= int(_entryList_.size()) ){ focusIndex--; }

  brls::Application::giveFocus( _entryList_[focusIndex].item.get() );
}