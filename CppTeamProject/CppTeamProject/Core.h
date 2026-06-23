#pragma once
#include <Windows.h>

class Scene;

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
	Scene* m_overlay = nullptr;   // 떠 있는 설정창(일시정지 모달). nullptr이면 없음.
};
