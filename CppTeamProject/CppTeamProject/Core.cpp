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
	SetConsoleDefaultFont();   // 폰트를 기본값으로 고정한 뒤 창 크기를 맞춘다.
	SetConsoleWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetConsoleWindowStyle(true);
	SetConsoleMouseInputDisabled();
	srand((unsigned int)time(nullptr));
}

void Core::Update()
{
	UpdateInput(true);

	ColliderManager::GetInst()->Update();
	// 적 갱신은 GameScene::Update가 전담한다. (여기서 또 호출하면 적이 프레임당 두 번
	// 움직여 플레이어보다 두 배 빨라지는 버그가 생겨 제거했다.)

	std::string curScene = SceneManager::GetInst()->GetCurSceneName();

	// 휠을 길게 누르면(300ms 이상) 떼기를 기다리지 않고 그 즉시 설정창을 띄운다.
	// 설정창은 씬 전환이 아니라 일시정지 오버레이로 연다.
	// (ChangeScene을 쓰면 게임 씬이 Release/Init돼 진행 상황이 초기화되므로 사용 금지.)
	bool wheelDown = GetKey(VK_MBUTTON);

	if (wheelDown && !m_wheelPressed)
	{
		m_wheelPressed = true;
		m_wheelPressStart = GetTickCount64();
	}
	else if (!wheelDown)
	{
		m_wheelPressed = false;
	}

	if (wheelDown && m_wheelPressed
		&& GetTickCount64() - m_wheelPressStart >= 300
		&& !m_overlay && curScene != "SettingUI")
	{
		SettingUI* setting =
			static_cast<SettingUI*>(SceneManager::GetInst()->GetScene("SettingUI"));

		if (setting)
		{
			setting->SetPrevScene(curScene);
			setting->Init();        // 설정창만 초기화. 게임 씬은 살려 둔다.
			m_overlay = setting;

			// 게임 중이면 생존 시간 타이머를 멈춘다.
			if (curScene == "GameScene")
			{
				if (GameScene* game = static_cast<GameScene*>(
						SceneManager::GetInst()->GetScene("GameScene")))
					game->Pause();
			}
		}
	}

	// 오버레이가 떠 있으면 게임 Update는 멈추고(적/플레이어 정지) 설정창만 갱신한다.
	if (m_overlay)
	{
		m_overlay->Update();

		if (static_cast<SettingUI*>(m_overlay)->IsClosed())
		{
			m_overlay->Release();
			m_overlay = nullptr;        // 게임 씬은 살아있는 그대로 재개.

			// 게임 중이면 멈춰 있던 시간만큼 타이머를 보정해 재개한다.
			if (curScene == "GameScene")
			{
				if (GameScene* game = static_cast<GameScene*>(
						SceneManager::GetInst()->GetScene("GameScene")))
					game->Resume();
			}

			// 설정창 잔상을 지운다. (맵은 빈 타일을 그리지 않아 가운데 박스가 안 지워지므로
			// 화면을 한 번 비운 뒤 다음 프레임에 게임이 새로 그려지게 한다.)
			SetColor(Color::WHITE, Color::BLACK);
			for (int y = 0; y < SCREEN_HEIGHT; ++y)
			{
				GotoXY(0, y);
				for (int x = 0; x < SCREEN_WIDTH; ++x)
					cout << ' ';
			}
		}
	}
	else
	{
		SceneManager::GetInst()->Update();
	}

	ColliderManager::GetInst()->Update();
}

void Core::Render()
{
	// 오버레이 중에는 멈춘 게임 화면을 다시 그리지 않는다(화면에 그대로 남아 있음).
	// 게임을 매 프레임 다시 그린 뒤 박스를 덮어쓰면 박스 영역이 깜빡이므로 박스만 갱신한다.
	if (m_overlay)
	{
		// 바뀐 게 있을 때만 다시 그린다. 매 프레임 박스를 덮어쓰면 깜빡이므로.
		SettingUI* setting = static_cast<SettingUI*>(m_overlay);
		if (setting->IsDirty())
		{
			setting->Render();
			setting->ClearDirty();
		}
		return;
	}

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


