//
// Created by Adrien Blanchet on 30/04/2023.
//

#ifndef SIMPLESWITCHBROWSER_TABBROWSER_H
#define SIMPLESWITCHBROWSER_TABBROWSER_H

#include "borealis.hpp"

#include "vector"
#include "string"
#include "mutex"

class FrameMain;

class TabBrowser : public brls::List {

public:
  explicit TabBrowser(FrameMain* owner_);

  void setRequestedCd(const std::string &requestedCd);

  void cd( const std::string& folder_ );
  void ls();

  [[nodiscard]] std::string getCwd() const;

  // overrides
  void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;

private:
  FrameMain* _owner_;

  std::mutex _mutex_;
  std::string _requestedCd_;
  std::vector<std::string> _walkPath_;

};


#endif //SIMPLESWITCHBROWSER_TABBROWSER_H
