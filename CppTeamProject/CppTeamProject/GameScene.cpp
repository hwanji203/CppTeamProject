#include "GameScene.h"
#include "TitleScene.h"
#include "ResultScene.h"
#include "Utils.h"
#include "Pawn.h"
#include "Player.h"
#include "Types.h"
#include "DefaultMapGenerator.h"
#include "Defines.h"
#include "ColliderManager.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "Enemy.h"
#include "Console.h"
#include <cstdio>

Vector2 GameScene::GetSpawnPos() const
{
	Vector2 pos = { 0,0 };
	DefaultMapGenerator* gen = dynamic_cast<DefaultMapGenerator*>(m_generator.get());
	if (!gen)
		return pos;

	return { m_resolution.x / 2, gen->GetGroundY() - 2};
}

void GameScene::Init()
{
	m_resolution = GetConsoleResolution();
	m_generator = std::make_unique<DefaultMapGenerator>();
	m_gameMap = m_generator->Generate(SCREEN_WIDTH, SCREEN_HEIGHT);
	Vector2 startPos = GetSpawnPos();
	m_player = std::make_unique<Player>(startPos);
	EnemyManager::GetInst()->Init(2);
	SOUND->PlayBGM("Sounds/game_bgm.mp3");   // 게임 BGM(루프)

	m_startTick = GetTickCount64();          // 버틴 시간 측정 시작
}

void GameScene::Update()
{
	EnemyManager::GetInst()->TrySpawnEnemyInRandomPos(m_player->GetPos(), m_gameMap->GetGroundLength());

	m_player->Tick();
	EnemyManager::GetInst()->Update();
}

void GameScene::Render()
{
	m_gameMap->Render();

	Pawn& player = dynamic_cast<Pawn&>(*m_player);
	if (player.IsDead() || player.IsLeaveDeadZone())
	{
		// 버틴 시간(초)을 계산해 ResultScene에 전달한 뒤 전환한다.
		float survived = (GetTickCount64() - m_startTick) / 1000.0f;
		ResultScene* result = static_cast<ResultScene*>(
			SceneManager::GetInst()->GetScene("ResultScene"));
		if (result)
			result->SetSurviveTime(survived);

		SceneManager::GetInst()->ChangeScene("ResultScene");
		return;
	}

	EnemyManager::GetInst()->Render();
	m_player->Render();

	DrawHud();
}

void GameScene::DrawHud() const
{
	// 오른쪽 가운데: 버틴(지난) 초 + 그 오른쪽에 속도 색 범례.
	// (맵은 EMPTY 타일을 그리지 않으므로 우측 빈 공간에 매 프레임 덮어쓴다.)
	const int hudY = SCREEN_HEIGHT / 2;

	unsigned long long sec = (GetTickCount64() - m_startTick) / 1000;
	char timeBuf[32];
	std::snprintf(timeBuf, sizeof(timeBuf), "TIME %llu s   ", sec);
	SetColor(Color::WHITE);
	GotoXY(SCREEN_WIDTH - 40, hudY);
	cout << timeBuf;

	// 범례: 속도 구간별 색(빠름→느림). 같은 색을 맞추면 크리티컬.
	struct LegendItem { float speed; const char* label; };
	const LegendItem legend[] = {
		{ 3.5f, "3+  RED" },
		{ 2.5f, "2-3 ORG" },
		{ 1.5f, "1-2 YEL" },
		{ 0.5f, "0-1 GRN" },
	};
	const int legendX = SCREEN_WIDTH - 22;
	const int topY    = hudY - 2;
	for (int i = 0; i < 4; ++i)
	{
		GotoXY(legendX, topY + i);
		SetColor(Color::WHITE);
		cout << "(";
		SetColor(Enemy::ColorForSpeed(legend[i].speed));
		cout << "■";
		SetColor(Color::WHITE);
		cout << ": " << legend[i].label << ")";
	}
}

void GameScene::Release()
{
	EnemyManager::GetInst()->Clear();
	m_player.reset();                 
	m_gameMap.reset();
	m_generator.reset();
}