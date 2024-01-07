//
// Created by Adrien Blanchet on 03/05/2023.
//

#ifndef SIMPLESWITCHBROWSER_ENTRYCONTAINER_H
#define SIMPLESWITCHBROWSER_ENTRYCONTAINER_H

#include "GenericToolbox.h"

#include "borealis.hpp"

#include "string"
#include "memory"



class EntryContainer {

public:
  EntryContainer() = default;
  virtual ~EntryContainer() = default;

  virtual void buildItem() = 0;

private:
  static const std::string type{__CLASS_NAME__};
  std::string name{};
  std::string fullPath{};

  EntryType type{UNSET};
  double size{-1};

  std::shared_ptr<brls::ListItem> item{nullptr};

};


#endif //SIMPLESWITCHBROWSER_ENTRYCONTAINER_H
