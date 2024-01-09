//
// Created by Adrien Blanchet on 30/04/2023.
//

#ifndef SIMPLESWITCHBROWSER_TABBROWSER_H
#define SIMPLESWITCHBROWSER_TABBROWSER_H


#include "GenericToolbox.Borealis.h"

#include "borealis.hpp"

#include "vector"
#include "string"
#include "mutex"
#include "memory"
#include "future"


#define MAKE_ENUM \
  ENUM_NAME(EntryType) \
  ENUM_ENTRY(UNSET, -1) \
  ENUM_ENTRY(IS_DIR) \
  ENUM_ENTRY(IS_FILE) \
  ENUM_ENTRY(EMPTY)
#include "GenericToolbox.MakeEnum.h"
#undef MAKE_ENUM

struct DirEntry {
  std::string name{};
  std::string fullPath{};

  EntryType type{EntryType::UNSET};
  double size{-1};

  std::shared_ptr<brls::ListItem> item{nullptr};

  [[nodiscard]] bool isDir() const { return type == EntryType::IS_DIR; }
};

class FrameMain;

class TabBrowser : public brls::List {

public:
  explicit TabBrowser(FrameMain* owner_);
  ~TabBrowser() override;

  // setters
  void setRequestedCd(const std::string &requestedCd_){ _requestedCd_ = requestedCd_; }

  // getters
  [[nodiscard]] std::string getCwd() const;

  // overrides
  void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;

  // statics
  static void sortEntries(std::vector<DirEntry>& entryList_);
  static brls::Image* getIcon( const std::string& filePath_ );

protected:
  void cd( const std::string& folder_ = "" );
  void ls();

  bool removeFolderFct( const std::string& folderPath_ );

private:
  FrameMain* _owner_;

  int _coolDownFrames_{0};
  bool _requestScroll_{false};

  std::mutex _mutex_;
  std::string _requestedCd_;
  std::vector<std::string> _walkPath_;
  std::vector<int> _walkFocus_{};

  std::vector<DirEntry> _entryList_;

  std::future<bool> _asyncResponse_{};
  GenericToolbox::Borealis::PopupLoadingBox _loadingBox_{};

};


#endif //SIMPLESWITCHBROWSER_TABBROWSER_H
