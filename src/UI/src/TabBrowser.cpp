//
// Created by Adrien Blanchet on 30/04/2023.
//

#include "TabBrowser.h"
#include "FrameMain.h"

#include "Logger.h"
#include "GenericToolbox.h"
#include "GenericToolbox.Switch.h"
#include <switch.h>

LoggerInit([]{
  Logger::setUserHeaderStr("[TabBrowser]");
});


TabBrowser::TabBrowser(FrameMain* owner_) : brls::List(), _owner_(owner_) {
  LogWarning << "Building TabBrowser" << std::endl;

  // root
  this->cd();
  this->ls();
}
TabBrowser::~TabBrowser() {
  // make sure brls don't try to delete already deleted ptrs
  this->clear( false );
}

std::string TabBrowser::getCwd() const{
  return GenericToolbox::joinVectorString(_walkPath_, "/").insert(0, "/");
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


  LogInfo << "Listing folders in " << cwd << std::endl;
  auto foldersList = GenericToolbox::lsDirs( cwd );
  LogWarning << "Found " << foldersList.size() << " folders." << std::endl;
  _entryList_.reserve( _entryList_.size() + foldersList.size() );
  for (auto &folder: foldersList) {
    auto& entry = _entryList_.emplace_back();

    entry.name = folder;
    entry.fullPath = GenericToolbox::joinPath(cwd, folder);
    entry.type = EntryType::IS_DIR;
  }

  LogInfo << "Listing files in " << cwd << std::endl;
  auto fileList = GenericToolbox::lsFiles( cwd );
  LogWarning << "Found " << fileList.size() << " files." << std::endl;
  _entryList_.reserve( _entryList_.size() + fileList.size() );
  for (auto &file: fileList) {
    auto& entry = _entryList_.emplace_back();

    entry.name = file;
    entry.fullPath = GenericToolbox::joinPath(cwd, file);
    entry.type = EntryType::IS_FILE;
    if( fileList.size() < 256 ){
      // if too many files it takes too much time
      LogTrace << "Checking size of " << entry.fullPath << std::endl;
      entry.size = double( GenericToolbox::getFileSize( entry.fullPath ) );
    }
  }

  // case of IO error or empty
  if( _entryList_.empty() ){
    _entryList_.emplace_back();
    _entryList_.back().type = EntryType::EMPTY;
  }

  LogInfo << "Sorting entries..." << std::endl;
  TabBrowser::sortEntries( _entryList_ );

  // build items
  LogInfo << "Building list items..." << std::endl;
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

    if     ( entry.type == EntryType::IS_DIR ){
      entry.item->setLabel( GenericToolbox::joinAsString(" ", "\uE2C7", entry.name) );

      auto folder{entry.name};
      entry.item->getClickEvent()->subscribe([this, folder](brls::View*){
        this->setRequestedCd( folder );
        return true;
      });
      entry.item->updateActionHint(brls::Key::A, "Open");

      auto folderPath{entry.fullPath};
      entry.item->registerAction("Delete", brls::Key::X, [this, folderPath]{

        auto* dialog = new brls::Dialog("Do you want to remove \"" + folderPath + "\" ?");
        dialog->addButton("Yes", [this, dialog, folderPath](brls::View* view) {
          // first, close the dialog box before the async routine starts
          dialog->close();

          _asyncResponse_ = std::async(&TabBrowser::removeFolderFct, this, folderPath);
        });
        dialog->addButton("No", [dialog](brls::View* view) { dialog->close(); });

        dialog->setCancelable(true);
        dialog->open();
        return true;
      });

      if( GenericToolbox::Switch::Utils::isTidLike(entry.name) ){
        auto *icon = GenericToolbox::Switch::Utils::getIconFromTitleId( entry.name );
        if( icon != nullptr ){
          entry.item->setThumbnail( icon, 0x20000 );
          entry.item->setHeight( 75 );
        }
      }

    }
    else if( entry.type == EntryType::IS_FILE ){
      entry.item->setLabel( entry.name );

      if( entry.size != -1 ){ entry.item->setValue( GenericToolbox::parseSizeUnits( entry.size ) ); }

      std::string filePath{entry.fullPath};
      entry.item->registerAction("Delete", brls::Key::X, [this, filePath]{

        auto* dialog = new brls::Dialog("Do you want to remove \"" + filePath + "\" ?");
        dialog->addButton("Yes", [this, dialog, filePath](brls::View* view) {
          dialog->close();
          LogAlert << "Deleting: " << filePath << std::endl;

          if( not GenericToolbox::rm( filePath ) ){
            LogError << "Could not delete: " << std::strerror(errno) << std::endl;

            auto* dialogFail = new brls::Dialog("Fail to remove \"" + filePath + "\" with error: " + std::strerror(errno));
            dialogFail->addButton("Ok", [dialogFail](brls::View*){ dialogFail->close(); });

            _asyncResponse_ = std::async([dialogFail]{
              while( brls::Application::hasViewDisappearing() ){
                // wait for one extra frame before push
                std::this_thread::sleep_for(std::chrono::milliseconds( 16 ));
              }

              // push the box to the view
              brls::Application::pushView( dialogFail );

              return true;
            });

          }
          else{
            std::scoped_lock<std::mutex> g(_mutex_);
            _coolDownFrames_ = 2;
            this->setRequestedCd( "./" );
          }
        });
        dialog->addButton("No", [dialog](brls::View* view) { dialog->close(); });

        dialog->setCancelable(true);
        dialog->open();
        return true;
      });

      // option to exec
      if( GenericToolbox::getExtension(entry.name) == "nro" ){

        entry.item->getClickEvent()->subscribe([filePath](brls::View*){
          auto *dialog = new brls::Dialog("Do you want to load \"" + filePath + "\" ?");

          dialog->addButton("Yes", [filePath, dialog](brls::View *view) {
            LogInfo << "Launching " << filePath << std::endl;
            envSetNextLoad(filePath.c_str(), filePath.c_str());
            brls::Application::quit();
            dialog->close();
          });
          dialog->addButton("No", [dialog](brls::View *view) {
            dialog->close();
          });

          dialog->setCancelable(true);
          dialog->open();

          return true;
        });
        entry.item->updateActionHint(brls::Key::A, "Load");

        brls::Image *icon = TabBrowser::getIcon( filePath );
        entry.item->setThumbnail( icon );
        entry.item->setHeight( 75 );

      }
      if( GenericToolbox::getExtension(entry.name) == "nsp" ){
        // not working
        brls::Image *icon = TabBrowser::getIcon( filePath );
        entry.item->setThumbnail( icon );
        entry.item->setHeight( 75 );
      }

    }
    else if( entry.type == EntryType::EMPTY ){
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


bool TabBrowser::removeFolderFct( const std::string& folderPath_ ){
  // push the progress bar to the view
  _loadingBox_.pushView();

  std::string progressFileTitle;
  double progressFraction;

  LogAlert << "Removing: " << folderPath_ << std::endl;
  _loadingBox_.getMonitorView()->setHeaderTitle("Removing folder...");
  _loadingBox_.getMonitorView()->setProgressColor(nvgRGB(0xff, 0x64, 0x64));
  _loadingBox_.getMonitorView()->resetMonitorAddresses();
  _loadingBox_.getMonitorView()->setTitlePtr( &folderPath_ );
  _loadingBox_.getMonitorView()->setSubTitlePtr( &progressFileTitle );
  _loadingBox_.getMonitorView()->setProgressFractionPtr( &progressFraction );

  LogInfo << "Listing files..." << std::endl;
  progressFileTitle = "Listing files...";
  auto fileList = GenericToolbox::lsFilesRecursive( folderPath_ );
  int iFile{0};
  for( auto& entry : fileList ){
    iFile++;

    if( not _loadingBox_.isOnTopView() ){
      LogWarning << "Delete has been canceled" << std::endl;
      brls::Application::unblockInputs();
      return true;
    }

    auto fullPath = GenericToolbox::joinPath(folderPath_, entry);

    LogTrace << "rm " << fullPath << std::endl;
    progressFileTitle = entry;
    progressFileTitle += " (" + GenericToolbox::joinAsString("/", iFile+1, fileList.size()) + ")";
    progressFraction = (iFile + 1.) / double(fileList.size());

    GenericToolbox::rm( fullPath );
  }

  LogInfo << "Listing leftover folders..." << std::endl;
  progressFileTitle = "Listing leftover folders...";
  auto folderList = GenericToolbox::lsDirsRecursive( folderPath_ );
  for( auto& folder : folderList ){
    iFile++;

    if( not _loadingBox_.isOnTopView() ){
      LogWarning << "Delete has been canceled" << std::endl;
      brls::Application::unblockInputs();
      return true;
    }

    auto fullPath = GenericToolbox::joinPath(folderPath_, folder);

    LogTrace << "rm " << fullPath << std::endl;
    progressFileTitle = folder
                        + " (" + std::to_string(iFile+1) + "/" + std::to_string(fileList.size()) + ")"
        ;
    progressFraction = (iFile + 1.) / double(fileList.size());

    GenericToolbox::rmDir( fullPath );
  }
  GenericToolbox::rmDir( folderPath_ );


  _loadingBox_.popView();
  brls::Application::unblockInputs();
  std::scoped_lock<std::mutex> g(_mutex_);
  _coolDownFrames_ = 2;
  this->setRequestedCd( "./" );
  return true;
}

brls::Image* TabBrowser::getIcon( const std::string& filePath_ ){
  brls::Image *image = nullptr;

  FILE *file = fopen(filePath_.c_str(), "rb");
  if (file)
  {
    fseek(file, sizeof(NroStart), SEEK_SET);
    NroHeader header;
    fread(&header, sizeof(header), 1, file);
    fseek(file, header.size, SEEK_SET);
    NroAssetHeader asset_header;
    fread(&asset_header, sizeof(asset_header), 1, file);

    size_t iconSize = asset_header.icon.size;
    auto *icon = (uint8_t *) malloc(iconSize);
    if (icon != nullptr && iconSize != 0) {
      memset(icon, 0, iconSize);
      fseek(file, header.size + asset_header.icon.offset, SEEK_SET);
      fread(icon, iconSize, 1, file);
      image = new brls::Image(icon, iconSize);
    }
    else{ image = new brls::Image("romfs:/images/unknown.png"); }

    fclose(file);
    free(icon);
    icon = nullptr;
  }
  else
  {
    LogDebug << "Using Unknown Icon For: " << filePath_ << std::endl;
    image = new brls::Image("romfs:/images/unknown.png");
  }

  return image;
}
