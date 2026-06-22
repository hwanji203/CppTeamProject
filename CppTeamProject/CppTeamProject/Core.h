#pragma once
#include <Windows.h>

class Core
{
public:
	Core();
	~Core();
public:
	void Run();
private:
	void Init();
	void Update();
	void Render();

private:
	bool m_isRunning = true;
	DWORD m_wheelPressStart = 0;
	bool m_wheelPressed = false;
};
