#include "GameScene.h"
#include "TitleScene.h"
#include "InputHandler.h"
#include "Utils.h"
#include "Pawn.h"
#include "Types.h"
#include "DefaultMapGenerator.h"

Position GameScene::GetSpawnPos() const
{
	return { m_resolution.x / 2, m_resolution.y / 2 };
}

void GameScene::Init()
{
	m_resolution = GetConsoleResolution();
	//m_inputHandler = new InputHandler;
	//m_player = new Pawn({ m_resolution.x / 2, m_resolution.y / 2 });
	m_inputHandler = std::make_unique<InputHandler>();
	m_generator = std::make_unique<DefaultMapGenerator>();
	m_gameMap = m_generator->Generate(50, 30);
	Position startPos = GetSpawnPos();
	m_player = std::make_unique<Pawn>(startPos);
}

void GameScene::Update()
{
	//ICommand* cmd = new MoveCommand;
	ICommand* cmd = m_inputHandler->HandleInput();
	//std::unique_ptr<ICommand> cmd = m_inputHandler->HandleInput();
	if (cmd != nullptr)
	{
		cmd->Execute(m_player.get());
		//delete cmd;
	}
	m_player->Tick();
}

void GameScene::Render()
{
	GotoXY(0, 0);
	m_gameMap->Render(m_player->GetPos());
	m_player->Render();
}

void GameScene::Release()
{
}

