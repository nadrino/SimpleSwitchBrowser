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
    this->cd( _requestedCd_ );
    this->ls();
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
  this->clear();

  std::string cwd{this->getCwd()};
  std::vector<brls::ListItem*> itemList;

  {
    auto foldersList = GenericToolbox::getListOfSubFoldersInFolder( cwd );
    GenericToolbox::sortVector(foldersList, [](const std::string &a_, const std::string &b_) {
      if( a_.empty() ) return false;
      if( b_.empty() ) return true;
      if( a_[0] != '.' and b_[0] == '.') return true;
      if( b_[0] != '.' and a_[0] == '.') return false;
      return GenericToolbox::toLowerCase(a_) < GenericToolbox::toLowerCase(b_);
    });

    for (auto &folder: foldersList) {

      auto *item = new brls::ListItem("\uE2C7 " + folder, "", "");
      item->setHeight(50);

      auto folderCopy{folder};
      item->getClickEvent()->subscribe([this, folderCopy](brls::View*){
        this->setRequestedCd( folderCopy );
        return true;
      });
      item->updateActionHint(brls::Key::A, "Enter");

      if( not _walkPath_.empty() ){
        item->registerAction("Back", brls::Key::B, [this]{
          this->setRequestedCd( "../" );
          return true;
        });
      }

      this->addView(item);
      itemList.emplace_back(item);
    }
  }

  {
    auto fileList = GenericToolbox::getListOfFilesInFolder( cwd );
    GenericToolbox::sortVector(fileList, [](const std::string &a_, const std::string &b_) {
      if( a_.empty() ) return false;
      if( b_.empty() ) return true;
      if( a_[0] != '.' and b_[0] == '.') return true;
      if( b_[0] != '.' and a_[0] == '.') return false;
      return GenericToolbox::toLowerCase(a_) < GenericToolbox::toLowerCase(b_);
    });

    for (auto &file: fileList) {
      auto *item = new brls::ListItem(file, "", "");
      item->setHeight(50);

      if( not _walkPath_.empty() ){
        item->registerAction("Back", brls::Key::B, [this]{
          std::scoped_lock<std::mutex> g(_mutex_);
          this->setRequestedCd( "../" );
          return true;
        });
      }

      this->addView(item);
      itemList.emplace_back(item);
    }
  }

  brls::Application::giveFocus( itemList[0] );
  this->onWindowSizeChanged();
}

std::string TabBrowser::getCwd() const{
  return "/" + GenericToolbox::joinVectorString(_walkPath_, "/");
}

void TabBrowser::setRequestedCd(const std::string &requestedCd) {
  _requestedCd_ = requestedCd;
}
