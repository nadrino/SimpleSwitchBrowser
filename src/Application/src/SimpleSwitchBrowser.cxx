//
// Created by Adrien Blanchet on 30/04/2023.
//


// this project
#include "FrameMain.h"

// submodules
#include "Logger.h"
#include "borealis.hpp"

// nxlib
#include "switch.h"

// std
#include "cstdlib"


LoggerInit([]{
  Logger::setUserHeaderStr("[SimpleSwitchBrowser.nro]");
});


int main( int argc, char* argv[] ){
  LogInfo << "SimpleSwitchBrowser is starting..." << std::endl;

  LogThrowIf(R_FAILED(nsInitialize()), "nsInitialize Failed");

  brls::Logger::setLogLevel(brls::LogLevel::ERROR);

  brls::i18n::loadTranslations("en-US");
  LogThrowIf(not brls::Application::init("SimpleSwitchBrowser"), "Unable to init Borealis application");


  auto* mainFrame = new FrameMain();
  brls::Application::pushView( mainFrame );

  // these options have to be put after pushView
  // disable + as quit
//  mainFrame->registerAction("", brls::Key::PLUS, []{return true;}, true);
//  mainFrame->updateActionHint(brls::Key::PLUS, ""); // make the change visible


  while(brls::Application::mainLoop()){
    // brls handles inputs. Nothing to do here
  }

  nsExit();
  return EXIT_SUCCESS;
}