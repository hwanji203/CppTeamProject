#include "GameScene.h"
#include "TitleScene.h"
#include "Utils.h"
#include "Pawn.h"
#include "Player.h"
#include "Types.h"
#include "DefaultMapGenerator.h"
#include "Defines.h"
#include "ColliderManager.h"
#include "SceneManager.h"

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
}

void GameScene::Update()
{
	UpdateInput(true);

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
		SceneManager::GetInst()->ChangeScene("TitleScene");
		return;
	}

	EnemyManager::GetInst()->Render();
	m_player->Render();
}

void GameScene::Release()
{
	EnemyManager::GetInst()->Clear();
	m_player.reset();                 
	m_gameMap.reset();
	m_generator.reset();
}