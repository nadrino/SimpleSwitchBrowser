//
// Created by Adrien Blanchet on 30/04/2023.
//

#ifndef SIMPLESWITCHBROWSER_TABBROWSER_H
#define SIMPLESWITCHBROWSER_TABBROWSER_H

#include "borealis.hpp"

#include "vector"

class FrameMain;

class TabBrowser : public brls::List {

public:
  explicit TabBrowser(FrameMain* owner_);

  void cd( const std::string& folder_ );
  void ls();

  [[nodiscard]] std::string getCwd() const;


private:
  FrameMain* _owner_;

  std::vector<std::string> _walkPath_;

};


#endif //SIMPLESWITCHBROWSER_TABBROWSER_H
