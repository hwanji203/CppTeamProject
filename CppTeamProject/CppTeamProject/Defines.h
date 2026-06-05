#pragma once

constexpr int SCREEN_WIDTH = 60;
constexpr int SCREEN_HEIGHT = 20;
constexpr int FRAME = 60;
#define SAFE_DELETE(p) if(p) { delete p; p=nullptr;}