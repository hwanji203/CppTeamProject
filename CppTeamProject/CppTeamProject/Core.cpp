#include "Core.h"
#include "Console.h"
#include "Defines.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "SceneManager.h"

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
	SceneManager::GetInst()->Update();
}

void Core::Render()
{
	SceneManager::GetInst()->Render();
}

Core::~Core()
{
	SceneManager::GetInst()->DestroyInst();
}

void Core::Run()
{
	Init();
	while (m_isRunning)
	{
		Update();
		Render();
		FrameSync(60);
	}
}


