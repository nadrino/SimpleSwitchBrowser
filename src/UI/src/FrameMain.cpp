//
// Created by Adrien Blanchet on 30/04/2023.
//

#include "FrameMain.h"
#include "TabAbout.h"
#include "TabBrowser.h"

#include "Logger.h"

#include "VersionConfig.h"



LoggerInit([]{
  Logger::setUserHeaderStr("[MainFrame]");
});

FrameMain::FrameMain() : brls::TabFrame() {
  LogWarning << "Building FrameMain" << std::endl;

  this->setFooterText( "SimpleSwitchBrowser " + VersionConfig::getVersionStr() );
  this->setIcon("romfs:/images/icon_corner.png");


  this->addTab( "Browser", new TabBrowser( this ) );
  this->addSeparator();
  this->addTab( "About", new TabAbout() );
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
