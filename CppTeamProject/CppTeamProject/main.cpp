#include "Core.h"
#include "SoundManager.h"
#include "Console.h"

int main()
{
	//Core core;
	//core.Run();

	SOUND->Init();

	SOUND->Load("jump", "Sounds/jump.wav");
	SOUND->Load("coin", "Sounds/coin.wav");
	SOUND->Load("hit", "Sounds/hit.wav");

	SOUND->Play("jump");   // 점프할 때
	SOUND->Play("coin");   // 코인 먹을 때
	
	while (true)
	{
		FrameSync(60);
		SOUND->Update();
	}

	SOUND->Release();       // 사운드 메모리 해제
	SoundManager::DestroyInst();  // 싱글톤 객체 삭제
}