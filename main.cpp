#include <iostream>
#include <string>
#include <vector>
#define CLAY_IMPLEMENTATION
#include "clay/clay.h"
#include "clay/renderers/raylib/clay_renderer_raylib.c"
#include "gui.hpp"
#include "marketfeed.hpp"
#include "bookmanager.hpp"

std::shared_ptr<UIManager> UIManager::instance = nullptr;
std::shared_ptr<BookManager> BookManager::instance = nullptr;

void HandleClayErrors(Clay_ErrorData data) {
  std::cerr << std::string(data.errorText.chars) << std::endl;
}

void setColors() {
  Colors cols = {
    .panel = {60, 56, 54, 255},
    .panelAlt = {235, 219, 178, 255},
    .text = {235, 219, 178, 255},
    .textAlt = {50, 48, 47, 255},
    .bid = {184, 187, 38, 255},
    .ask = {251, 73, 52, 255},
    .button = {60, 56, 54, 255},
  };

  UIManager::getInstance()->setColors(cols);
  UIManager::getInstance()->setBodyFont(_Font {
      .id = 0,
      .size = 20
      });
}

int main() {
  Clay_Raylib_Initialize(800, 600, "Realtime Orderbook", not FLAG_WINDOW_RESIZABLE);

  uint64_t requiredMemory = Clay_MinMemorySize();
  Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(requiredMemory,
      malloc(requiredMemory));

  Clay_Initialize(clayMemory, (Clay_Dimensions) { 
    .width = static_cast<float>(GetScreenWidth()),
    .height = static_cast<float>(GetScreenHeight())
  }, (Clay_ErrorHandler){HandleClayErrors});

  setColors();

  Clay_SetMeasureTextFunction(Raylib_MeasureText);
  Raylib_fonts[UIManager::getInstance()->getBodyFont().id] = (Raylib_Font) {
    .fontId = UIManager::getInstance()->getBodyFont().id,
    .font = LoadFontEx("fonts/Roboto-Regular.ttf", 48, 0, 400)
  };

  SetTextureFilter(Raylib_fonts[UIManager::getInstance()->getBodyFont().id].font.texture, TEXTURE_FILTER_BILINEAR);

  std::vector<std::string> symbols = {"BTCUSD","ETHUSD","ETHBTC"};
  MarketFeed feed("api.gemini.com", symbols);
  feed.subscribe();

  int currId = 0;

  beast::flat_buffer web_buffer;
  json order_info;

  while (not WindowShouldClose()) {

    // Load market data here...
    web_buffer.clear();
    order_info = feed.getMarketData(web_buffer);
    BookManager::getInstance()->addL2Update(order_info);

    std::string symbolName = std::format("[ {} ]", symbols[currId]);
    std::string bidText = std::format("Bid: {}", "[value]");
    std::string askText = std::format("Ask: {}", "[value]");
    std::string sentiText = std::format("Sentiment: {}", "[value]");

    Clay_BeginLayout();
    // Build UI here
    Clay_SetLayoutDimensions((Clay_Dimensions) { 
        .width = static_cast<float>(GetScreenWidth()),
        .height = static_cast<float>(GetScreenHeight())
        });

    CLAY(
        CLAY_ID("OuterContainer"),
        CLAY_RECTANGLE({ 
          .color = {28, 28, 28, 255}, 
          .cornerRadius = {0},
        }),
        CLAY_LAYOUT({
          .sizing = (Clay_Sizing){
            .width = CLAY_SIZING_GROW(),
            .height = CLAY_SIZING_GROW()
          },
          .padding = {.x = 16, .y = 16},
          .childGap = 16,
          .layoutDirection = CLAY_TOP_TO_BOTTOM,
          })
        ) {
        CLAY(
            CLAY_ID("Level 1a Panel"),
            CLAY_RECTANGLE({.color = UIManager::getInstance()->getPanelColor()}),
            CLAY_LAYOUT({
                .sizing = (Clay_Sizing) {
                  .width = CLAY_SIZING_GROW(),
                  .height = CLAY_SIZING_FIXED(L1_BAR_SIZE)
                },
                .padding = {.x = 16, .y = 16},
                .childGap = 16,
                .childAlignment = {
                  .y = CLAY_ALIGN_Y_TOP
                }
              })
            ) {
          // Level 1 UI
          CLAY_TEXT(CLAY_STRING(symbolName.c_str()), CLAY_TEXT_CONFIG({
                .textColor = UIManager::getInstance()->getTextColor(),
                .fontId = UIManager::getInstance()->getBodyFont().id,
                .fontSize = UIManager::getInstance()->getBodyFont().size
                }));

          CLAY_TEXT(CLAY_STRING(bidText.c_str()), CLAY_TEXT_CONFIG({
                .textColor = UIManager::getInstance()->getBidColor(),
                .fontId = UIManager::getInstance()->getBodyFont().id,
                .fontSize = UIManager::getInstance()->getBodyFont().size
                }));

          CLAY_TEXT(CLAY_STRING(askText.c_str()), CLAY_TEXT_CONFIG({
                .textColor = UIManager::getInstance()->getAskColor(),
                .fontId = UIManager::getInstance()->getBodyFont().id,
                .fontSize = UIManager::getInstance()->getBodyFont().size
                }));

          CLAY_TEXT(CLAY_STRING(sentiText.c_str()), CLAY_TEXT_CONFIG({
                .textColor = UIManager::getInstance()->getTextColor(),
                .fontId = UIManager::getInstance()->getBodyFont().id,
                .fontSize = UIManager::getInstance()->getBodyFont().size
                }));

        }

        CLAY(
            CLAY_ID("Level 1b Panel"),
            CLAY_RECTANGLE({.color = UIManager::getInstance()->getPanelColor()}),
            CLAY_LAYOUT({
                .sizing = (Clay_Sizing) {
                  .width = CLAY_SIZING_GROW(),
                  .height = CLAY_SIZING_FIXED(L1_BAR_SIZE)
                },
                .padding = {.x = 16, .y = 16},
                .childGap = 16,
                .childAlignment = {
                  .y = CLAY_ALIGN_Y_TOP
                }
              })
            ) {
          CLAY_TEXT(CLAY_STRING("Spread: "), CLAY_TEXT_CONFIG({
                .textColor = UIManager::getInstance()->getTextColor(),
                .fontId = UIManager::getInstance()->getBodyFont().id,
                .fontSize = UIManager::getInstance()->getBodyFont().size
                }));

          CLAY_TEXT(CLAY_STRING("[Spread Value]"), CLAY_TEXT_CONFIG({
                .textColor = UIManager::getInstance()->getTextColor(),
                .fontId = UIManager::getInstance()->getBodyFont().id,
                .fontSize = UIManager::getInstance()->getBodyFont().size
                }));

          CLAY_TEXT(CLAY_STRING("Support: "), CLAY_TEXT_CONFIG({
                .textColor = UIManager::getInstance()->getTextColor(),
                .fontId = UIManager::getInstance()->getBodyFont().id,
                .fontSize = UIManager::getInstance()->getBodyFont().size
                }));

          CLAY_TEXT(CLAY_STRING("[Support Value]"), CLAY_TEXT_CONFIG({
                .textColor = UIManager::getInstance()->getTextColor(),
                .fontId = UIManager::getInstance()->getBodyFont().id,
                .fontSize = UIManager::getInstance()->getBodyFont().size
                }));

          CLAY_TEXT(CLAY_STRING("Resistance: "), CLAY_TEXT_CONFIG({
                .textColor = UIManager::getInstance()->getTextColor(),
                .fontId = UIManager::getInstance()->getBodyFont().id,
                .fontSize = UIManager::getInstance()->getBodyFont().size
                }));

          CLAY_TEXT(CLAY_STRING("[Resistance Value]"), CLAY_TEXT_CONFIG({
                .textColor = UIManager::getInstance()->getTextColor(),
                .fontId = UIManager::getInstance()->getBodyFont().id,
                .fontSize = UIManager::getInstance()->getBodyFont().size
                }));


        }
        CLAY(
              CLAY_ID("Level 2 Panel"),
              CLAY_RECTANGLE({.color = UIManager::getInstance()->getPanelColor()}),
            CLAY_LAYOUT({
                .sizing = (Clay_Sizing) {
                  .width = CLAY_SIZING_GROW(),
                  .height = CLAY_SIZING_GROW()
                },
                .padding = {.x = 16, .y = 16},
                .childGap = 16,
                .childAlignment = {
                  .y = CLAY_ALIGN_Y_TOP
                }
              })
            ) {
          CLAY(
              CLAY_ID("Bid Panel"),
              CLAY_RECTANGLE({.color = UIManager::getInstance()->getPanelAltColor()}),
            CLAY_LAYOUT({
                .sizing = (Clay_Sizing) {
                  .width = CLAY_SIZING_GROW(),
                  .height = CLAY_SIZING_GROW()
                },
                .padding = {.x = 16, .y = 16},
                .childGap = 16,
                .childAlignment = {
                  .y = CLAY_ALIGN_Y_TOP
                }
              })) {
            //Bid Orders
          }

          CLAY(
              CLAY_ID("Ask Panel"),
              CLAY_RECTANGLE({.color = UIManager::getInstance()->getPanelAltColor()}),
            CLAY_LAYOUT({
                .sizing = (Clay_Sizing) {
                  .width = CLAY_SIZING_GROW(),
                  .height = CLAY_SIZING_GROW()
                },
                .padding = {.x = 16, .y = 16},
                .childGap = 16,
                .childAlignment = {
                  .y = CLAY_ALIGN_Y_TOP
                }
              })) {
            // Ask Orders
          }
          // Level 2 UI
        }
    }

    Clay_RenderCommandArray commands = Clay_EndLayout();

    BeginDrawing();
    ClearBackground(BLACK);
    Clay_Raylib_Render(commands);

    EndDrawing();
  }

  return 0;
}
