#include "TitleScene.h"
#include "Console.h"
#include "SceneManager.h"
void TitleScene::Init()
{
	m_resolution = GetResolution();
}

void TitleScene::Update()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		EnterAnimation();
		SceneManager::GetInst()->ChangeScene("GameScene");
	}
}

void TitleScene::EnterAnimation()
{
	// 위 -> 아래
	for (int y = 0; y < m_resolution.y; ++y)
	{
		for (int x = 0; x < m_resolution.x; ++x)
		{
			GotoXY(x, y);
			SetColor(Color::WHITE, Color::GREEN);
			cout << " ";
		}
		Sleep(5);
	}

	// 아래 -> 위
	for (int y = m_resolution.y - 1; y>=0; --y)
	{
		for (int x = 0; x < m_resolution.x; ++x)
		{
			GotoXY(x, y);
			SetColor(Color::WHITE, Color::BLACK);
			cout << " ";
		}
		Sleep(2);
	}
}

void TitleScene::Render()
{
	m_resolution = GetConsoleResolution();
	Position startPos = { m_resolution.x / 2, m_resolution.y / 2 };
	GotoXY(startPos.x, startPos.y);
	cout << "----------";
	GotoXY(startPos.x, startPos.y +1);
	cout << "Start a";
	GotoXY(startPos.x, startPos.y + 2);
	cout << "New Game";
	GotoXY(startPos.x, startPos.y + 3);
	cout << "----------";
	GotoXY(startPos.x, startPos.y + 4);
	cout << "  Space     ";
}

void TitleScene::Release()
{
}


