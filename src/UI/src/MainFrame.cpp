//
// Created by Adrien Blanchet on 30/04/2023.
//

#include "MainFrame.h"

#include "Logger.h"



LoggerInit([]{
  Logger::setUserHeaderStr("[MainFrame]");
});

MainFrame::MainFrame() {
  LogWarning << "Build MainFrame..." << std::endl;

  this->setTitle("SimpleSwitchBrowser");
  this->setFooterText( "v0.0.1" );
  this->setIcon("romfs:/images/icon_corner.png");





  this->updateActionHint(brls::Key::PLUS, ""); // make the change visible

  LogInfo << "MainFrame built." << std::endl;
}

bool MainFrame::onCancel() {
  // fetch the current focus
  auto* lastFocus = brls::Application::getCurrentFocus();

  // perform the cancel
  bool onCancel = TabFrame::onCancel();

  // if the focus is the same, then quit the app
  if( lastFocus == brls::Application::getCurrentFocus() ){ brls::Application::quit(); }

  return onCancel;
}
