#include "GameScene.h"
#include "TitleScene.h"
#include "InputHandler.h"
#include "Utils.h"
#include "Pawn.h"
#include "Types.h"
#include "DefaultMapGenerator.h"
#include "Defines.h"
#include "ColliderManager.h"

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
	m_inputHandler = std::make_unique<InputHandler>();
	m_generator = std::make_unique<DefaultMapGenerator>();
	m_gameMap = m_generator->Generate(SCREEN_WIDTH, SCREEN_HEIGHT);
	Vector2 startPos = GetSpawnPos();
	m_player = std::make_unique<Pawn>(startPos);
}

void GameScene::Update()
{
	ICommand* cmd = m_inputHandler->HandleInput();
	if (cmd != nullptr)
	{
		cmd->Execute(m_player.get());
	}
	m_player->Tick();
}

void GameScene::Render()
{
	m_gameMap->Render(m_player->GetPos());
	m_player->Render();
}

void GameScene::Release()
{
}

