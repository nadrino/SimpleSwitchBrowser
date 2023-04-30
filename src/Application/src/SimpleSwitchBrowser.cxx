//
// Created by Adrien Blanchet on 30/04/2023.
//

// submodules
#include "borealis.hpp"

// nxlib
#include "switch.h"

// std
#include "cstdlib"


int main( int argc, char* argv[] ){
  nsInitialize();

  brls::i18n::loadTranslations("en-US");
  brls::Application::init("SimpleSwitchBrowser");

  while(brls::Application::mainLoop()){
    // brls handles inputs. Nothing to do here
  }

  nsExit();
  return EXIT_SUCCESS;
}