#include "Core.h"
#include "Console.h"
#include "Defines.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"
#include "SettingUI.h"
#include "SceneManager.h"
#include "ColliderManager.h"
#include "EnemyManager.h"
#include <string>
#include "SoundManager.h"

Core::Core()
{
	// 사운드 초기화 및 효과음 로드 (실제 파일은 Sounds/ 폴더에 채워 넣을 것)
	// 첫 씬(TitleScene)의 Init()이 ChangeScene 안에서 PlayBGM을 호출하므로,
	// FMOD 초기화/로드는 반드시 ChangeScene보다 먼저 끝나 있어야 한다.
	// BGM은 씬별로 분리: TitleScene::Init / GameScene::Init 에서 각각 재생한다.
	SOUND->Init();
	SOUND->Load("enemy_death",  "Sounds/enemy_death.mp3");   // 적 사망
	SOUND->Load("player_death", "Sounds/player_death.mp3");  // 플레이어 사망
	SOUND->Load("wheel",        "Sounds/wheel.mp3");         // 마우스 휠
	SOUND->Load("hit",          "Sounds/hit.mp3");           // 적-플레이어 일반 충돌
	SOUND->Load("critical",     "Sounds/critical.mp3");      // 크리티컬 충돌

	SceneManager::GetInst()->RegisterScene("TitleScene", std::make_unique<TitleScene>());
	SceneManager::GetInst()->RegisterScene("GameScene", std::make_unique<GameScene>());
	SceneManager::GetInst()->RegisterScene("ResultScene", std::make_unique<ResultScene>());
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
	UpdateInput(true);
	
	ColliderManager::GetInst()->Update();
	EnemyManager::GetInst()->Update();

	std::string curScene = SceneManager::GetInst()->GetCurSceneName();
	if (GetKeyDown('Q') && curScene != "SettingUI")
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

	// 사운드 자원 해제
	SOUND->Release();
	SoundManager::DestroyInst();
}

void Core::Run()
{
	Init();
	while (m_isRunning)
	{
		Update();
		Render();
		SOUND->Update();   // FMOD 시스템 갱신
		FrameSync(FRAME);
	}
}


