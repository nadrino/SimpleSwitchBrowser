//
// Created by Adrien Blanchet on 30/04/2023.
//

#ifndef SIMPLESWITCHBROWSER_MAINFRAME_H
#define SIMPLESWITCHBROWSER_MAINFRAME_H


#include "borealis.hpp"

class MainFrame : public brls::TabFrame {

public:
  MainFrame();

  bool onCancel() override;


};


#endif //SIMPLESWITCHBROWSER_MAINFRAME_H
