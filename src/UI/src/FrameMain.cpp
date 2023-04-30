//
// Created by Adrien Blanchet on 30/04/2023.
//

#include "FrameMain.h"
#include "TabAbout.h"

#include "Logger.h"



LoggerInit([]{
  Logger::setUserHeaderStr("[MainFrame]");
});

FrameMain::FrameMain() {
  LogWarning << "Build MainFrame..." << std::endl;

  this->setTitle("SimpleSwitchBrowser");
  this->setFooterText( "v0.0.1" );
  this->setIcon("romfs:/images/icon_corner.png");


  this->addTab( "About", new TabAbout() );


  LogInfo << "MainFrame built." << std::endl;
}

bool FrameMain::onCancel() {
  // fetch the current focus
  auto* lastFocus = brls::Application::getCurrentFocus();

  // perform the cancel
  bool onCancel = TabFrame::onCancel();

  // if the focus is the same, then quit the app
  if( lastFocus == brls::Application::getCurrentFocus() ){ brls::Application::quit(); }

  return onCancel;
}
