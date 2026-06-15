#include "Core.h"
#include "Console.h"
#include "Defines.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "SettingUI.h"
#include "SceneManager.h"
#include "ColliderManager.h"
#include "EnemyManager.h"
#include <string>

Core::Core()
{
	SceneManager::GetInst()->RegisterScene("TitleScene", std::make_unique<TitleScene>());
	SceneManager::GetInst()->RegisterScene("GameScene", std::make_unique<GameScene>());
	SceneManager::GetInst()->RegisterScene("SettingUI", std::make_unique<SettingUI>());
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
	EnemyManager::GetInst()->Update();

	std::string curScene = SceneManager::GetInst()->GetCurSceneName();
	if (GetKeyDown(VK_ESCAPE) && curScene != "SettingUI")
	{
		SettingUI* setting = static_cast<SettingUI*>(SceneManager::GetInst()->GetScene("SettingUI"));
		if (setting)
			setting->SetPrevScene(curScene);
		SceneManager::GetInst()->ChangeScene("SettingUI");
	}
	else
	{
		SceneManager::GetInst()->Update();
	}

	ColliderManager::GetInst()->Update();
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


