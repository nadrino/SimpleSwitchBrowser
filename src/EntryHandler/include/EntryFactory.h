//
// Created by Adrien Blanchet on 03/05/2023.
//

#ifndef SIMPLESWITCHBROWSER_ENTRYFACTORY_H
#define SIMPLESWITCHBROWSER_ENTRYFACTORY_H

#include "EntryContainer.h"

class EntryFactory {
  EntryFactory() = default;
  ~EntryFactory() = default;

  EntryContainer* makeEntry();

};


#endif //SIMPLESWITCHBROWSER_ENTRYFACTORY_H
