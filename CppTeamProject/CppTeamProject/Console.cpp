#include "Console.h"

void SetUnicodeMode()
{
	(void)_setmode(_fileno(stdout), _O_U16TEXT);
}

void SetDefaultMode()
{
	(void)_setmode(_fileno(stdout), _O_TEXT);
}

void RenderChar(char ch, int delayTime, bool isSkip)
{
	cout << ch;

	if (strchr("@#$%^&", ch))
	{
		if (!isSkip)
			// 스킵아닐때만
			Sleep(200);
		cout << "\b \b";

		if (!isSkip)
			// 스킵아닐때만
			Sleep(100);
	}

	if (isSkip)
		return;


	int calcDelay = delayTime + (rand() % 11 - 5);
	switch (ch)
	{
		case '!':
		case '?':
		case '.':
			calcDelay += 400; // 0.4초
			break;
		case ',':
			calcDelay += 200; // 0.2초
			break;
		case '\n':
			calcDelay += 500; // 0.5초
			break;
		case ' ':
			calcDelay += 50; // 0.05초
			break;
	}
	Sleep(std::max(0, calcDelay));
}


void RenderDialogue(const string& text, int delay)
{
	bool isSkip = false;
	for (int i = 0; i < text.size(); ++i)
	{
		if (!isSkip && _kbhit())
		{
			_getch();
			isSkip = true;
		}
		RenderChar(text[i], delay, isSkip);
	}
}



void SetConsoleGameTitle(const wstring& title)
{
	SetConsoleTitle(title.c_str());
}

void SetConsoleWindowSize(int width, int height)
{
	//// 콘솔 내부: 세팅,값을 얻거나 
	//GetStdHandle(STD_OUTPUT_HANDLE);

	//// 콘솔 외부: 콘솔 창 자체를 건드릴때
	//GetConsoleWindow();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	// 뷰포트 줄이기
	SMALL_RECT rect = { 0,0,1,1 };
	SetConsoleWindowInfo(handle, true, &rect);

	// 버퍼 해상도에 맞게 설정
	COORD size = { (short)width , (short)height };
	SetConsoleScreenBufferSize(handle, size);

	// 뷰포트 맞추기
	rect.Right = width - 1;
	rect.Bottom = height - 1;
	SetConsoleWindowInfo(handle, true, &rect);

	//Sleep(1000);// 0.1초

	// 가운데 정렬
	HWND hWnd = GetConsoleWindow();

	//RECT windowRect;
	//GetWindowRect(hWnd,&windowRect);
	//int windowWidth = windowRect.right - windowRect.left;
	//int windowHeight= windowRect.bottom - windowRect.top;

	CONSOLE_FONT_INFOEX fontInfo = { sizeof(CONSOLE_FONT_INFOEX) };
	GetCurrentConsoleFontEx(handle, false, &fontInfo);

	int windowWidth = fontInfo.dwFontSize.X * width;
	int windowHeight = fontInfo.dwFontSize.Y * height;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = (screenWidth - windowWidth) / 2;
	int posY = (screenHeight - windowHeight) / 2;

	SetWindowPos(hWnd, nullptr, posX, posY, 0, 0, SWP_NOSIZE);
}


void SetConsoleFullScreen()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_FONT_INFOEX fontInfo = { sizeof(CONSOLE_FONT_INFOEX) };
	GetCurrentConsoleFontEx(handle, false, &fontInfo);

	SetConsoleDisplayMode(handle, CONSOLE_FULLSCREEN_MODE, 0);

	int width = GetSystemMetrics(SM_CXSCREEN) / fontInfo.dwFontSize.X;
	int height = GetSystemMetrics(SM_CYSCREEN) / fontInfo.dwFontSize.Y;
	SetConsoleWindowSize(width, height);
}

void SetConsoleWindowStyle(bool showTitleBar)
{
	HWND hWnd = GetConsoleWindow();
	LONG style = GetWindowLong(hWnd, GWL_STYLE);
	style &= ~WS_SIZEBOX & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX
		& ~WS_SYSMENU;
	if (!showTitleBar)
		style &= ~WS_CAPTION;
	SetWindowLong(hWnd, GWL_STYLE, style);
	// 사이즈X, 움직이지X,    지금 설정 창에 바로 적용하세요.
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
}

void ShakeConsoleWindow(int intensity, int duration, int interval)
{
	//intensity: 흔들림 강도
	//duration: 흔들 시간
	//interval: 한번 흔들고 다음 텀 
	HWND hWnd = GetConsoleWindow();
	RECT rt = {};
	GetWindowRect(hWnd, &rt);
	int originX = rt.left;
	int originY = rt.top;

	int count = duration / interval;
	// 쉐이크
	for (int i = 0; i < count; i++)
	{
		// -intensity ~ +intensity
		// -10 ~ 10 -> 0 ~ 20 
		// rand() % 21 - 10; 
		int offsetX = rand() % (2 * intensity + 1) - intensity;
		int offsetY = rand() % (2 * intensity + 1) - intensity;
		SetWindowPos(hWnd, nullptr,
					 originX + offsetX,
					 originY + offsetY, 0, 0, SWP_NOSIZE);
		Sleep(interval);
	}

	SetWindowPos(hWnd, nullptr, originX, originY, 0, 0, SWP_NOSIZE);
}

void SetConsoleMouseInputDisabled()
{
	HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(handle, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	mode &= ~ENABLE_INSERT_MODE;

	mode |= ENABLE_MOUSE_INPUT;

	mode |= ENABLE_EXTENDED_FLAGS;
	SetConsoleMode(handle, mode);
}

Position GetConsoleResolution()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);

	// 현재 보이는 창(뷰포트)의 칸 수
	short width = info.srWindow.Right - info.srWindow.Left + 1;
	short height = info.srWindow.Bottom - info.srWindow.Top + 1;

	return Position{ width, height };
}

void GotoXY(int x, int y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD curPos = { x,y };
	SetConsoleCursorPosition(handle, curPos);
}

void SetCursorVisible(bool visible, DWORD size)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO curInfo;
	curInfo.bVisible = visible; // on, off
	curInfo.dwSize = size;// 두께 1~100
	SetConsoleCursorInfo(handle, &curInfo);
}

BOOL IsGotoXY(int x, int y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD curPos = { x,y };
	return SetConsoleCursorPosition(handle, curPos);
}

void SetColor(Color textColor, Color bgColor)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int text = (int)textColor;
	int bg = (int)bgColor;
	SetConsoleTextAttribute(handle, (bg << 4) | text);
}

void DrawBar(int x, int y, const string& label, int value, int maxValue, int barWidth, const string& fillChar, const string& emptyChar)
{
	Color color;
	if (value > maxValue * 0.6f)
		color = Color::LIGHT_GREEN;
	else if (value > maxValue * 0.3f)
		color = Color::LIGHT_YELLOW;
	else
		color = Color::LIGHT_RED;

	GotoXY(x, y);
	SetColor();
	cout << label;

	// bar 출력
	SetColor(color);
	// value 60, maxValue 100, barWidth 10 => 6칸
	int fillValue = barWidth * value / maxValue;
	for (int i = 0; i < barWidth; ++i)
		cout << (i < fillValue ? fillChar : emptyChar);
	SetColor();
	int digits = std::to_string(maxValue).length();

	cout << " " <<
		std::setw(digits) << value << "/" <<
		std::setw(digits) << maxValue;
}

bool GetKey(int vKey)
{
	// 지금 딱 이 프레임에 눌렸냐
	return GetAsyncKeyState(vKey) & 0x8000;
}


// 전역변수 금지
// -> 전역변수 수정x -> 가능o
constexpr int KEY_COUNT = 256;
static bool prevDown[KEY_COUNT] = {};
static bool curDown[KEY_COUNT] = {};
bool GetKeyDown(int vKey)
{
	return curDown[vKey] && !prevDown[vKey];
}

void FrameSync(int fps)
{
	static ULONGLONG prevTick = GetTickCount64();

	ULONGLONG curTick = GetTickCount64();
	ULONGLONG elapsed = curTick - prevTick;
	ULONGLONG targetTick = 1000 / fps;
	//ULONGLONG targetTick =  / fps;
	if (elapsed < targetTick)
		Sleep(targetTick - elapsed);
	prevTick = GetTickCount64();
}

static int mouseWheelDelta = 0;
static bool prevSideDown = false;
static bool curSideDown = false;

int GetMouseWheelChange()
{
	return mouseWheelDelta;
}

bool GetMouseSideButtonDown()
{
	return curSideDown && !prevSideDown;
}

POINT GetMouseCellPos()
{
	POINT pt;
	GetCursorPos(&pt);
	HWND hWnd = GetConsoleWindow();
	ScreenToClient(hWnd, &pt);

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX fontInfo = { sizeof(CONSOLE_FONT_INFOEX) };
	GetCurrentConsoleFontEx(handle, false, &fontInfo);

	POINT cellPos = { pt.x / fontInfo.dwFontSize.X,
					  pt.y / fontInfo.dwFontSize.Y };
	return cellPos;
}

void UpdateInput()
{
	for (int i = 0; i < 256; ++i)
	{
		prevDown[i] = curDown[i];
		curDown[i] = GetAsyncKeyState(i) & 0x8000;
	}

	// 마우스 사이드 버튼
	prevSideDown = curSideDown;
	curSideDown = GetAsyncKeyState(VK_XBUTTON1) & 0x8000;

	// 마우스 휠
	mouseWheelDelta = 0;
	HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
	DWORD eventCount = 0;
	GetNumberOfConsoleInputEvents(handle, &eventCount);

	std::vector<INPUT_RECORD> leftover;

	for (DWORD i = 0; i < eventCount; ++i)
	{
		INPUT_RECORD record;
		DWORD read;
		ReadConsoleInput(handle, &record, 1, &read);

		if (record.EventType == MOUSE_EVENT &&
			record.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)
		{
			SHORT delta = HIWORD(record.Event.MouseEvent.dwButtonState);
			mouseWheelDelta += (delta > 0) ? 1 : -1;
		}
		else
		{
			leftover.push_back(record);
		}
	}

	if (!leftover.empty())
	{
		DWORD written;
		WriteConsoleInput(handle, leftover.data(), (DWORD)leftover.size(), &written);
	}
}