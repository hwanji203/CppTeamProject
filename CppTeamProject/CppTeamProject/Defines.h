#pragma once

constexpr int SCREEN_WIDTH = 160;
constexpr int SCREEN_HEIGHT = 20;
constexpr int FRAME = 120;
#define SAFE_DELETE(p) if(p) { delete p; p=nullptr;}