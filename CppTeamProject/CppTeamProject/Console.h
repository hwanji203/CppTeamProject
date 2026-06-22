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

void SetUnicodeMode();
void SetDefaultMode();

void RenderDialogue(const string& text, int delay);
void RenderChar(char ch, int delayTime, bool isSkip);

void SetConsoleGameTitle(const wstring& title);
void SetConsoleWindowSize(int width, int height);
void SetConsoleFullScreen();
void SetConsoleWindowStyle(bool showTitleBar);
void ShakeConsoleWindow(int intensity, int duration, int interval);
// 비차단(프레임 단위) 카메라 쉐이킹. intensity 픽셀만큼 콘솔 창을 움직인다.
// ferris=true면 관람차처럼 작은 반지름으로 회전, false면 단순히 흔든다.
void ConsoleShake(float intensity, bool ferris);
void ConsoleShakeRestore();   // 콘솔 창을 원위치(원점)로 복귀
// 방향성 리코일: 원점에서 (offsetX, offsetY) 픽셀만큼 밀어 놓는다(흔들림 아님).
void ConsoleRecoil(float offsetX, float offsetY);
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
			 const string& fillChar = "■", const string& emptyChar = "□");

// Ű �Է� �Լ�
bool GetKey(int vKey);
bool GetKeyDown(int vKey);

// 모든 조작은 마우스 기준이지만 키보드(Z/X)로도 동작한다.
// 앞으로 가기 = 마우스 XBUTTON2 또는 'Z', 뒤로 가기 = 마우스 XBUTTON1 또는 'X'.
bool GetForward();       // 앞으로 가기 버튼이 눌려 있는지(홀드)
bool GetForwardDown();   // 앞으로 가기 버튼을 이번 프레임에 눌렀는지
bool GetBack();          // 뒤로 가기 버튼이 눌려 있는지(홀드)
bool GetBackDown();      // 뒤로 가기 버튼을 이번 프레임에 눌렀는지
POINT GetMouseCellPos();
void UpdateInput(bool lockCursor);
void FrameSync(int fps);

int GetMouseWheelChange();