//
// Created by Adrien Blanchet on 30/04/2023.
//


// this project
#include "MainFrame.h"

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


  auto* mainFrame = new MainFrame();
  brls::Application::pushView( mainFrame );


  while(brls::Application::mainLoop()){
    // brls handles inputs. Nothing to do here
  }

  nsExit();
  return EXIT_SUCCESS;
}