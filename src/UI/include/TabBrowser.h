//
// Created by Adrien Blanchet on 30/04/2023.
//

#ifndef SIMPLESWITCHBROWSER_TABBROWSER_H
#define SIMPLESWITCHBROWSER_TABBROWSER_H

#include "borealis.hpp"

#include "vector"
#include "string"
#include "mutex"
#include "memory"


struct DirEntry {
  std::string name{};
  bool isDir{false};

  std::shared_ptr<brls::ListItem> item{nullptr};
};

class FrameMain;

class TabBrowser : public brls::List {

public:
  explicit TabBrowser(FrameMain* owner_);

  ~TabBrowser() override;

  void setRequestedCd(const std::string &requestedCd);

  void cd( const std::string& folder_ );
  void ls();

  [[nodiscard]] std::string getCwd() const;

  // overrides
  void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;

  static void sortEntries(std::vector<DirEntry>& entryList_);

private:
  FrameMain* _owner_;

  std::mutex _mutex_;
  std::string _requestedCd_;
  std::vector<std::string> _walkPath_;

  std::vector<DirEntry> _entryList_;

};


#endif //SIMPLESWITCHBROWSER_TABBROWSER_H
