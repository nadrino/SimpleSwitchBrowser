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

}

void TabBrowser::cd( const std::string& folder_ ){
  if( not folder_.empty() ){
    _walkPath_.emplace_back(folder_);
  }

  std::string cwd{this->getCwd()};
  _owner_->setTitle( this->getCwd() );

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
      auto *item = new brls::ListItem(folder, "", "");
      item->setHeight(50);
      item->setValue("folder");

      this->addView(item);
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

      this->addView(item);
    }
  }

}
std::string TabBrowser::getCwd() const{
  return "/" + GenericToolbox::joinVectorString(_walkPath_, "/");
}
