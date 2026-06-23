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
#include "ItemManager.h"
#include "BulletManager.h"
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
	EnemyManager::GetInst()->Init();
	ItemManager::GetInst()->Init();
	SOUND->PlayBGM("Sounds/game_bgm.mp3");   // 게임 BGM(루프)

	m_startTick = GetTickCount64();          // 버틴 시간 측정 시작
}

void GameScene::Update()
{
	EnemyManager::GetInst()->TrySpawnEnemyInRandomPos(m_player->GetPos(), m_gameMap->GetGroundLength());
	ItemManager::GetInst()->TrySpawnItem(m_gameMap->GetGroundLength());

	m_player->Tick();
	EnemyManager::GetInst()->Update();
	ItemManager::GetInst()->Update();
	BulletManager::GetInst()->Update();
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
	ItemManager::GetInst()->Render();
	BulletManager::GetInst()->Render();
	m_player->Render();

	DrawHud();
}

void GameScene::DrawHud() const
{
	// 왼쪽 위: 플레이어 체력 바.
	if (Player* player = dynamic_cast<Player*>(m_player.get()))
		DrawBar(2, 0, "HP ", player->GetHp(), player->GetMaxHp(), player->GetMaxHp());

	// 오른쪽 가운데: 버틴(지난) 초 + 그 오른쪽에 속도 색 범례.
	// (맵은 EMPTY 타일을 그리지 않으므로 우측 빈 공간에 매 프레임 덮어쓴다.)
	const int hudY = SCREEN_HEIGHT / 3;

	unsigned long long sec = (GetTickCount64() - m_startTick) / 1000;
	char timeBuf[32];
	std::snprintf(timeBuf, sizeof(timeBuf), "TIME %llu s   ", sec);
	SetColor(Color::WHITE);
	GotoXY(SCREEN_WIDTH - 40, hudY);
	cout << timeBuf;

	// 범례: 속도 구간(밴드)별 색. 적 밴드와 동일한 중앙값으로 ColorForSpeed를 호출해
	// 사각형 색이 실제 적/플레이어 색과 항상 일치하도록 한다(라벨은 속도 범위만 표시).
	const int legendX = SCREEN_WIDTH - 22;
	const int topY    = hudY - ENEMY_SPEED_BANDS / 2;
	for (int i = 0; i < ENEMY_SPEED_BANDS; ++i)
	{
		int   band   = ENEMY_SPEED_BANDS - 1 - i;   // 위에서부터 빠른(높은) 속도
		float center = band + 0.5f;                 // 적이 스폰되는 밴드 중앙값
		char  rangeBuf[16];
		std::snprintf(rangeBuf, sizeof(rangeBuf), "%d-%d", band, band + 1);

		GotoXY(legendX, topY + i);
		SetColor(Color::WHITE);
		cout << "(";
		SetColor(Enemy::ColorForSpeed(center));
		cout << "■";
		SetColor(Color::WHITE);
		cout << ": " << rangeBuf << ")   ";
	}
}

void GameScene::Pause()
{
	m_pauseStart = GetTickCount64();
}

void GameScene::Resume()
{
	// 멈춰 있던 시간만큼 시작 시각을 뒤로 밀어, 생존 시간에서 설정창 시간을 제외한다.
	m_startTick += GetTickCount64() - m_pauseStart;
}

void GameScene::Release()
{
	EnemyManager::GetInst()->Clear();
	ItemManager::GetInst()->Clear();
	BulletManager::GetInst()->Clear();
	m_player.reset();                 
	m_gameMap.reset();
	m_generator.reset();
}