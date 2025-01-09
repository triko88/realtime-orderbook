#pragma once

#include "clay/clay.h"
#include <memory>

const uint16_t L1_BAR_SIZE = 50;

struct Colors {
  Clay_Color panel;
  Clay_Color panelAlt;
  Clay_Color text;
  Clay_Color textAlt;
  Clay_Color bid;
  Clay_Color ask;
  Clay_Color button;
};

struct _Font {
  uint16_t id;
  uint16_t size;
};

class UIManager {
  Colors colors;
  _Font body;

  static std::shared_ptr<UIManager> instance;
  UIManager(UIManager &other) = delete;
  void operator=(const UIManager&) = delete;

public:

  UIManager() { }

  static std::shared_ptr<UIManager> getInstance() {
    if (not instance)
      instance = std::make_shared<UIManager>();
    return instance;
  }

  Clay_Color getPanelColor() { return colors.panel; }
  Clay_Color getTextColor() { return colors.text; }
  Clay_Color getPanelAltColor() { return colors.panelAlt; }
  Clay_Color getTextAltColor() { return colors.textAlt; }
  Clay_Color getBidColor() { return colors.bid; }
  Clay_Color getAskColor() { return colors.ask; }
  Clay_Color getBtnColor() { return colors.button; }

  void setColors(Colors cols) { colors = cols; }
  void setPanelColor(Clay_Color panelCol) { colors.panel = panelCol; }
  void setTextColor(Clay_Color textCol) { colors.text = textCol; }
  void setPanelAltColor(Clay_Color panelCol) { colors.panelAlt = panelCol; }
  void setTextAltColor(Clay_Color textCol) { colors.textAlt = textCol; }
  void setBidColor(Clay_Color bidCol) { colors.bid = bidCol; }
  void setAskColor(Clay_Color askCol) { colors.ask = askCol; }
  void setBtnColor(Clay_Color buttonCol) { colors.button = buttonCol; }

  void setBodyFont(_Font bfont) {body = bfont;}
  _Font getBodyFont() { return body; }

};

