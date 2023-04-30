//
// Created by Adrien Blanchet on 30/04/2023.
//

#include "TabBrowser.h"

#include "Logger.h"


LoggerInit([]{
  Logger::setUserHeaderStr("[TabBrowser]");
});


TabBrowser::TabBrowser() {
  LogWarning << "Building TabBrowser" << std::endl;

}
