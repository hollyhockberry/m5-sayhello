// Copyright (c) 2024 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <M5Unified.h>
#include <boardname.hpp>
#include <gitTagVersion.h>
#include <textblock.hpp>

namespace {

hhb::Canvas canvas;

class TouchLabel : public hhb::TextBlock {
  m5::touch_state_t _state = m5::none;
 public:
  bool update() override {
    auto t = M5.Touch.getDetail();
    if (_state != t.state) {
      _state = t.state;
      return true;
    }
    return false;
  }
  void makeText(char* buffer) const override {
    ::sprintf(buffer, "Touch: %-12s", BoardName::getTouchStateName(_state));
  }
};

void splash() {
  auto& display = M5.Display;
  auto print = [&](int y, const char* text) {
    display.setCursor((display.width() - display.textWidth(text)) / 2, y);
    display.print(text);
    return y + display.fontHeight();
  };
  auto y = display.height() / 2 - display.fontHeight();
  y = print(y, "Hello,");
  char buffer[32];
  ::sprintf(buffer, "I'm %s.", BoardName::getBoardName(M5.getBoard()));
  y = print(y, buffer) + M5.Display.fontHeight();
  ::sprintf(buffer, "M5Unified %d.%d.%d", M5UNIFIED_VERSION_MAJOR, M5UNIFIED_VERSION_MINOR, M5UNIFIED_VERSION_PATCH);
  y = print(display.height() - display.fontHeight(), buffer);
  M5.delay(3000);
  display.clear();
}

char fflag(float f) {
  if (f > 0.1f) {
    return '+';
  }
  if (f < -0.1f) {
    return '-';
  }
  return ' ';
}

void assignLabels() {
  auto y = 0;
  y = canvas.add(y, new hhb::TextLabel([](char* buf) -> void {
    ::sprintf(buf, "%s", BoardName::getBoardName(M5.getBoard()));
  }));
  if (M5.Display.width() > 0 && M5.Display.height() > 0) {
    y = canvas.add(y, new hhb::TextLabel([](char* buf) -> void {
      auto& display = M5.Display;
      ::sprintf(buf, "LCD:%dx%d", display.width(), display.height());
    }));
  }
  y = canvas.add(y,
    new hhb::ValueLabel<bool>(" A", []() -> bool { return M5.BtnA.isPressed(); }),
    new hhb::ValueLabel<bool>(" B", []() -> bool { return M5.BtnB.isPressed(); }),
    new hhb::ValueLabel<bool>(" C", []() -> bool { return M5.BtnC.isPressed(); })
  );
  y = canvas.add(y,
    new hhb::ValueLabel<bool>("PW", []() -> bool { return M5.BtnPWR.isPressed(); }),
    new hhb::ValueLabel<bool>("EX", []() -> bool { return M5.BtnEXT.isPressed(); }),
    nullptr
  );
  if (M5.Touch.isEnabled()) {
    y = canvas.add(y, new TouchLabel());
  }
  if (M5.Imu.isEnabled()) {
    y = canvas.add(y, new hhb::TextLabel([](char* buf) -> void {
      ::sprintf(buf, "IMU: %s", BoardName::getImuTypeName(M5.Imu.getType()));
    }));
    y = canvas.add(y,
      new hhb::ValueLabel<char>("AX", []() -> char {
        float x, y, z;
        return M5.Imu.getAccel(&x, &y, &z) ? fflag(x) : ' ';
      }),
      new hhb::ValueLabel<char>("AY", []() -> char {
        float x, y, z;
        return M5.Imu.getAccel(&x, &y, &z) ? fflag(y) : ' ';
      }),
      new hhb::ValueLabel<char>("AZ", []() -> char {
        float x, y, z;
        return M5.Imu.getAccel(&x, &y, &z) ? fflag(z) : ' ';
    }));
    y = canvas.add(y,
      new hhb::ValueLabel<char>("GX", []() -> char {
        float x, y, z;
        return M5.Imu.getGyro(&x, &y, &z) ? fflag(x) : ' ';
      }),
      new hhb::ValueLabel<char>("GY", []() -> char {
        float x, y, z;
        return M5.Imu.getGyro(&x, &y, &z) ? fflag(y) : ' ';
      }),
      new hhb::ValueLabel<char>("GZ", []() -> char {
        float x, y, z;
        return M5.Imu.getGyro(&x, &y, &z) ? fflag(z) : ' ';
    }));
  }
}

}  // namespace

void setup() {
  M5.begin();

  auto& display = M5.Display;
  if (display.width() > 0 && display.height() > 0) {
    if (display.width() < display.height()) {
      display.setRotation(display.getRotation() ^ 1);
    }
    display.clear();
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    if (display.width() > 100 && display.height() > 100) {
      display.setTextSize(2.f);
    }
    splash();
  }
  assignLabels();
}

void loop() {
  canvas.loop();
}

#if defined ( SDL_h_ )
__attribute__((weak))
int user_func(bool* running) {
  setup();
  do {
    loop();
  } while (*running);
  return 0;
}

int main(int, char**) {
  // The second argument is effective for step execution with breakpoints.
  // You can specify the time in milliseconds to perform slow execution that ensures screen updates.
  return lgfx::Panel_sdl::main(user_func, 128);
}
#endif  // SDL_h_
