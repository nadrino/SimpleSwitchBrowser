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

void TabBrowser::draw(NVGcontext *vg, int x, int y, unsigned int width, unsigned int height, brls::Style *style,
                         brls::FrameContext *ctx) {

  if( not _requestedCd_.empty() ){
    std::scoped_lock<std::mutex> g(_mutex_);
    LogTrace << "Updating list..." << std::endl;
    this->cd( _requestedCd_ );
    this->ls();
    this->onWindowSizeChanged();
    _requestedCd_.clear();
  }

  brls::List::draw(vg, x, y, width, height, style, ctx);
}

void TabBrowser::cd( const std::string& folder_ ){
  if( folder_.empty() ){
    _walkPath_.clear();
  }
  else if( folder_ == "../" ){
    _walkPath_.pop_back();
  }
  else{
    _walkPath_.emplace_back(folder_);
  }


  if( _walkPath_.empty() ){ _owner_->setTitle( "SimpleSwitchBrowser" ); }
  else{ _owner_->setTitle( this->getCwd() ); }
}
void TabBrowser::ls(){
  this->clear( false );
  LogDebug << "clear list..." << std::endl;
  for( auto& entry : _entryList_ ){ delete entry.item; }
  _entryList_.clear();

  std::string cwd{this->getCwd()};

  LogDebug << "looking in " << cwd << std::endl;

  auto foldersList = GenericToolbox::getListOfSubFoldersInFolder( cwd );
  LogDebug << "folders: " << GenericToolbox::parseVectorAsString(foldersList) << std::endl;
  _entryList_.reserve(foldersList.size() );
  for (auto &folder: foldersList) {
    _entryList_.emplace_back();

    _entryList_.back().name = folder;
    _entryList_.back().isDir = true;
    _entryList_.back().item = new brls::ListItem("\uE2C7 " + folder);
    _entryList_.back().item->setHeight( 50 );

    auto folderCopy{folder};
    _entryList_.back().item->getClickEvent()->subscribe([this, folderCopy](brls::View*){
      this->setRequestedCd( folderCopy );
      return true;
    });
    _entryList_.back().item->updateActionHint(brls::Key::A, "Enter");

    if( not _walkPath_.empty() ){
      _entryList_.back().item->registerAction("Back", brls::Key::B, [this]{
        this->setRequestedCd( "../" );
        return true;
      });
    }
  }


  auto fileList = GenericToolbox::getListOfFilesInFolder( cwd );
  LogDebug << "fileList: " << GenericToolbox::parseVectorAsString(fileList) << std::endl;
  for (auto &file: fileList) {
    _entryList_.emplace_back();

    _entryList_.back().name = file;
    _entryList_.back().isDir = false;
    _entryList_.back().item = new brls::ListItem(file);
    _entryList_.back().item->setHeight( 50 );

    if( not _walkPath_.empty() ){
      _entryList_.back().item->registerAction("Back", brls::Key::B, [this]{
        std::scoped_lock<std::mutex> g(_mutex_);
        this->setRequestedCd( "../" );
        return true;
      });
    }
  }

  if( _entryList_.empty() ){
    _entryList_.emplace_back();
    _entryList_.back().item = new brls::ListItem("IO error or empty");
    _entryList_.back().item->setHeight( 50 );

    if( not _walkPath_.empty() ){
      _entryList_.back().item->registerAction("Back", brls::Key::B, [this]{
        std::scoped_lock<std::mutex> g(_mutex_);
        this->setRequestedCd( "../" );
        return true;
      });
    }
  }

  TabBrowser::sortEntries( _entryList_ );

  for( auto& entry : _entryList_ ){
    this->addView( entry.item );
  }

  brls::Application::giveFocus( _entryList_[0].item );
}

std::string TabBrowser::getCwd() const{
  return "/" + GenericToolbox::joinVectorString(_walkPath_, "/");
}

void TabBrowser::setRequestedCd(const std::string &requestedCd) {
  _requestedCd_ = requestedCd;
}


void TabBrowser::sortEntries(std::vector<DirEntry>& entryList_){
  GenericToolbox::sortVector( entryList_, [](const DirEntry &a_, const DirEntry &b_) {
    if( a_.isDir and not b_.isDir ) return true;
    if( not a_.isDir and b_.isDir ) return false;
    if( a_.name.empty() ) return false;
    if( b_.name.empty() ) return true;
    if( a_.name[0] != '.' and b_.name[0] == '.') return true;
    if( b_.name[0] != '.' and a_.name[0] == '.') return false;
    return GenericToolbox::toLowerCase(a_.name) < GenericToolbox::toLowerCase(b_.name);
  });
}
