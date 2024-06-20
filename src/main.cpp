// Copyright (c) 2024 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <M5Unified.h>
#include <boardname.hpp>

void setup() {
  M5.begin();
  M5.Log.setLogLevel(m5::log_target_t::log_target_serial, ESP_LOG_INFO);
  const auto name = BoardName::getBoardName(M5.getBoard());

  auto& display = M5.Display;
  if (display.width() > 0 && display.height() > 0) {
    if (display.width() < display.height()) {
      display.setRotation(display.getRotation() ^ 1);
    }

    display.clear();
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextSize(2.f);
    display.setCursor(0, 0);
    display.println("Hello,");
    display.printf("I'm %s.", name);
  }
  M5_LOGI("Hello, I'm %s.", name);
}

void loop() {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    M5_LOGI("Hello, I'm %s.", BoardName::getBoardName(M5.getBoard()));
  }
}
