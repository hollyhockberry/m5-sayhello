// Copyright (c) 2024 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef TEXTBLOCK_HPP_
#define TEXTBLOCK_HPP_

#include <M5Unified.h>

namespace hhb {

class TextBlock {
  uint8_t x = 0;
  uint8_t y = 0;
 public:
  int width() const {
    return text() ? ::strlen(text()) * M5.Display.fontWidth() : 0;
  }
  virtual bool update() {
    return false;
  }
  void locate(uint8_t x, uint8_t y) {
    this->x = x;
    this->y = y;
  }
  void print() {
    auto t = text();
    if (t) {
      M5.Display.setCursor(x, y);
      M5.Display.print(t);
    }
  }

 protected:
  virtual void makeText(char* buffer) const = 0;
 private:
  const char* text() const {
    static char buffer[64];
    makeText(buffer);
    return buffer;
  }  
};

class TextLabel : public TextBlock {
  void (*_text)(char*) = nullptr;
 public:
  TextLabel(void (*text)(char*))
    : _text(text) {}
 protected:
  void makeText(char* buffer) const override {
    if (_text) {
      _text(buffer);
    } else {
      buffer[0] = '\0';
    }
  }
};

template <typename T>
class ValueLabel : public TextBlock {
  std::string _label;
  T (*_value)() = nullptr;
  T _lastValue;
 public:
  ValueLabel(const char* label, T(* value)()) : _label(label), _value(value) {
    _lastValue = _value();
  }
  bool update() override {
    if (_value != nullptr) {
      T v = _value();
      if (v != _lastValue) {
        _lastValue = v;
        return true;
      }
    }
    return false;
  }
 protected:
  void makeText(char* buffer) const override {
    text(buffer, _label.c_str(), _value());
  }
 private:
  static void text(char* buffer, const char* label, bool value) {
    ::sprintf(buffer, "%s:%s", label, value ? "*" : " ");
  }
  static void text(char* buffer, const char* label, char value) {
    ::sprintf(buffer, "%s:%c", label, value);
  }
};

class Canvas {
  std::vector<TextBlock*> textblocks = {};
 public:
  void loop() {
    M5.update();
    for (auto block : textblocks) {
      if (block->update()) {
        block->print();
      }
    }
  }

  int add(int y, TextBlock* text) {
    std::vector<TextBlock*> texts = {text};
    return add(y, texts);
  }

  int add(int y, TextBlock* text1, TextBlock* text2) {
    std::vector<TextBlock*> texts = {text1, text2};
    if (width(texts) > M5.Display.width()) {
      y = add(y, text1);
      return add(y, text2);
    }
    return add(y, texts);
  }

  int add(int y, TextBlock* text1, TextBlock* text2, TextBlock* text3) {
    std::vector<TextBlock*> texts = {text1, text2, text3};
    if (width(texts) > M5.Display.width()) {
      y = add(y, text1, text2);
      return add(y, text3);
    }
    return add(y, texts);
  }

  int add(int y, TextBlock* text1, TextBlock* text2, TextBlock* text3, TextBlock* text4) {
    std::vector<TextBlock*> texts = {text1, text2, text3, text4};
    if (width(texts) > M5.Display.width()) {
      y = add(y, text1, text2);
      return add(y, text3, text4);
    }
    return add(y, texts);
  }

 private:
  static int width(std::vector<TextBlock*> labels) {
    int w = 0;
    for (auto label : labels) {
      if (label) {
        w += label->width();
      }
    }
    return w;
  }

  int add(int y, std::vector<TextBlock*> labels) {
    const auto offs = M5.Display.width() / labels.size();
    auto x = 0;
    for (auto label : labels) {
      if (label) {
        label->locate(x, y);
        label->print();
        textblocks.push_back(label);
      }
      x += offs;
    }
    return y + M5.Display.fontHeight();
  }
};

}

#endif  // TEXTBLOCK_HPP_