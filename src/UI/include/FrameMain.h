//
// Created by Adrien Blanchet on 30/04/2023.
//

#ifndef SIMPLESWITCHBROWSER_FRAMEMAIN_H
#define SIMPLESWITCHBROWSER_FRAMEMAIN_H


#include "borealis.hpp"

class FrameMain : public brls::TabFrame {

public:
  FrameMain();

  bool onCancel() override;


};


#endif //SIMPLESWITCHBROWSER_FRAMEMAIN_H
