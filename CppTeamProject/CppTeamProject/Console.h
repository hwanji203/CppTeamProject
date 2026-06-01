#pragma once
#include<iostream>
#include<conio.h>
#define NOMINMAX
#include<Windows.h>
#include<io.h>
#include<fcntl.h>
#include<iomanip>
#include<string>
#pragma comment(lib, "winmm.lib")

using std::string;
using std::wstring;
using std::cin;
using std::cout;
using std::endl;
using std::wcout;

// 아스키 아트
void SetUnicodeMode();
void SetDefaultMode();

// 다이얼로그
void RenderDialogue(const string& text, int delay);
void RenderChar(char ch, int delayTime, bool isSkip);

// 콘솔 창 관련 함수
void SetConsoleGameTitle(const wstring& title);
void SetConsoleWindowSize(int width, int height);
void SetConsoleFullScreen();
void SetConsoleWindowStyle(bool showTitleBar);
void ShakeConsoleWindow(int intensity, int duration, int interval);
void SetConsoleMouseInputDisabled();
// 커서 제어  함수
void GotoXY(int x, int y);
void SetCursorVisible(bool visible, DWORD size = 1);
BOOL IsGotoXY(int x, int y);

enum class Color
{
	BLACK, BLUE, GREEN, SKYBLUE, RED,
	VIOLET, YELLOW, LIGHT_GRAY, GRAY, LIGHT_BLUE,
	LIGHT_GREEN, CYAN, LIGHT_RED, LIGHT_VIOLET,
	LIGHT_YELLOW, WHITE, END
};

void SetColor(Color textColor = Color::WHITE, Color bgColor = Color::BLACK);
void DrawBar(int x, int y, const string& label,
			 int value, int maxValue, int barWidth,
			 const string& fillChar = "■", const string& emptyChar = "□");

// 키 입력 함수
bool GetKey(int vKey);
bool GetKeyDown(int vKey);
POINT GetMouseCellPos();
void UpdateInput();
void FrameSync(int fps);