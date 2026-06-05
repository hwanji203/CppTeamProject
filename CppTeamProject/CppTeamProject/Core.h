#pragma once
//#include "Pawn.h"
//void test(); // 함수 전방선언
//class Actor; // 이 클래스가 있습니다. - 클래스 전방선언
//#include "Actor.h"


//class Actor; 
//class InputHandler;

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

	//Actor* m_player;
	//InputHandler* m_inputHandler;

};
