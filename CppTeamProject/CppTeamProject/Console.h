#pragma once
#include<iostream>
#include<conio.h>
#define NOMINMAX
#include<Windows.h>
#include<io.h>
#include<fcntl.h>
#include<iomanip>
#include<string>
#include <vector>
#pragma comment(lib, "winmm.lib")

#include "Vector2.h"

using std::string;
using std::wstring;
using std::cin;
using std::cout;
using std::endl;
using std::wcout;

// �ƽ�Ű ��Ʈ
void SetUnicodeMode();
void SetDefaultMode();

// ���̾�α�
void RenderDialogue(const string& text, int delay);
void RenderChar(char ch, int delayTime, bool isSkip);

// �ܼ� â ���� �Լ�
void SetConsoleGameTitle(const wstring& title);
void SetConsoleWindowSize(int width, int height);
void SetConsoleFullScreen();
void SetConsoleWindowStyle(bool showTitleBar);
void ShakeConsoleWindow(int intensity, int duration, int interval);
void SetConsoleMouseInputDisabled();
Vector2 GetConsoleResolution();
// Ŀ�� ����  �Լ�
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
			 const string& fillChar = "��", const string& emptyChar = "��");

// Ű �Է� �Լ�
bool GetKey(int vKey);
bool GetKeyDown(int vKey);
POINT GetMouseCellPos();
void UpdateInput();
void FrameSync(int fps);

int GetMouseWheelChange();