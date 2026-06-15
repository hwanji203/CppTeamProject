#include "Core.h"
#include "Console.h"
#include "Defines.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "SceneManager.h"
#include "ColliderManager.h"
#include "EnemyManager.h"

Core::Core()
{
	SceneManager::GetInst()->RegisterScene("TitleScene", std::make_unique<TitleScene>());
	SceneManager::GetInst()->RegisterScene("GameScene", std::make_unique<GameScene>());
	SceneManager::GetInst()->ChangeScene("TitleScene");
}

void Core::Init()
{
	SetCursorVisible(false);
	SetConsoleWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetConsoleWindowStyle(true);
	SetConsoleMouseInputDisabled();
	srand((unsigned int)time(nullptr));
}

void Core::Update()
{
	ColliderManager::GetInst()->Update();

	SceneManager::GetInst()->Update();
}

void Core::Render()
{
	SceneManager::GetInst()->Render();
}

Core::~Core()
{
	// 씬/맵이 소멸하면서 ColliderManager에서 콜라이더를 해제하므로
	// SceneManager를 먼저 파괴한 뒤 ColliderManager를 파괴한다.
	SceneManager::GetInst()->DestroyInst();
	EnemyManager::GetInst()->DestroyInst();
	ColliderManager::GetInst()->DestroyInst();
}

void Core::Run()
{
	Init();
	while (m_isRunning)
	{
		Update();
		Render();
		FrameSync(FRAME);
	}
}


