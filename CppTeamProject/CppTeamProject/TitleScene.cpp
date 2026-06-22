#include "TitleScene.h"
#include "Console.h"
#include "SceneManager.h"
#include "Defines.h"
#include "SoundManager.h"
void TitleScene::Init()
{
	m_resolution = { SCREEN_WIDTH, SCREEN_HEIGHT };
	SOUND->PlayBGM("Sounds/title_bgm.mp3");   // 타이틀 BGM(루프)
}

void TitleScene::Update()
{
	if (GetKeyDown(VK_SPACE))
	{
		EnterAnimation();
		SceneManager::GetInst()->ChangeScene("GameScene");
	}
}

void TitleScene::EnterAnimation()
{
	// �� -> �Ʒ�
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

	// �Ʒ� -> ��
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
	Vector2 startPos = { m_resolution.x / 2, m_resolution.y / 2 };
	Vector2 offset = { -5, -2 };
	startPos = startPos + offset;
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
	GotoXY(startPos.x, startPos.y + 5);
	cout << "----------";
	GotoXY(startPos.x, startPos.y + 6);
	cout << "  S: Setting";
}

void TitleScene::Release()
{
}