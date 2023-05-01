//
// Created by Adrien BLANCHET on 20/06/2020.
//

#include "TabAbout.h"

#include "Logger.h"

#include <borealis.hpp>


LoggerInit([]{
  Logger::setUserHeaderStr("[TabAbout]");
});

TabAbout::TabAbout() : brls::List() {
  LogWarning << "Building TabAbout" << std::endl;

  // Subtitle
  auto* shortDescription = new brls::Label(
    brls::LabelStyle::REGULAR,
    "SimpleSwitchBrowser is a Nintendo Switch homebrew app that allows to browse files on your Switch.",
    true
  );
  shortDescription->setHorizontalAlign(NVG_ALIGN_CENTER);
  this->addView(shortDescription);

  auto* table = new brls::BoxLayout(brls::BoxLayoutOrientation::HORIZONTAL);
  table->setSpacing(22);
  table->setHeight(260);

  auto* leftBox = new brls::BoxLayout(brls::BoxLayoutOrientation::VERTICAL);
  leftBox->setSpacing(22);
  leftBox->setWidth(500);
  leftBox->setParent(table);

  leftBox->addView(new brls::Header("Version 1.0.0 - What's new ?"));
  auto *changelog = new brls::Label(
    brls::LabelStyle::DESCRIPTION,
    " - First release\n \n",
    true
  );
  changelog->setHorizontalAlign(NVG_ALIGN_LEFT);
  leftBox->addView(changelog);

  leftBox->addView(new brls::Header("Copyright"));
  auto *copyright = new brls::Label(
    brls::LabelStyle::DESCRIPTION,
    "SimpleSwitchBrowser is licensed under LGPL-2.1\n" \
        "\u00A9 2023 Nadrino",
    true
  );
  copyright->setHorizontalAlign(NVG_ALIGN_CENTER);
  leftBox->addView(copyright);

  auto* rightBox = new brls::BoxLayout(brls::BoxLayoutOrientation::VERTICAL);
  rightBox->setSpacing(22);
  rightBox->setWidth(200);
  rightBox->setParent(table);

  rightBox->addView(new brls::Label(brls::LabelStyle::DESCRIPTION, " "));

  auto* portrait = new brls::Image("romfs:/images/portrait.jpg");
  portrait->setScaleType(brls::ImageScaleType::SCALE);
  portrait->setHeight(200);
  portrait->setParent(rightBox);
  rightBox->addView(portrait);
  auto* portraitText = new brls::Label(brls::LabelStyle::SMALL, "Author: Nadrino", true);
  portraitText->setHorizontalAlign(NVG_ALIGN_CENTER);
  rightBox->addView(portraitText);

  table->addView(leftBox);
  table->addView(rightBox);

  this->addView(table);

  this->addView(new brls::Header("Remerciements"));
  auto *links = new brls::Label(
    brls::LabelStyle::SMALL,
    "\uE017  SimpleSwitchBrowser is powered by Borealis, an hardware accelerated UI library\n" \
         "\uE017  Special thanks to the RetroNX team for their support with Borealis\n",
    true
  );
  this->addView(links);
}
