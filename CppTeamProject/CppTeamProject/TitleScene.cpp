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
	// 모든 조작은 마우스: 앞으로 가기 버튼(또는 Z)으로 게임 시작.
	if (GetForwardDown())
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
	// 타이틀 로고 (왼쪽 위)
	const char* title[] = {
		"  ____  _    _ __  __ _____  ______ _____  ",
		" |  _ \\| |  | |  \\/  |  __ \\|  ____|  __ \\ ",
		" | |_) | |  | | \\  / | |__) | |__  | |__) |",
		" |  _ <| |  | | |\\/| |  ___/|  __| |  _  / ",
		" | |_) | |__| | |  | | |    | |____| | \\ \\ ",
		" |____/ \\____/|_|  |_|_|    |______|_|  \\_\\",
	};

	int startY = 2;
	SetColor(Color::LIGHT_RED, Color::BLACK);
	for (int i = 0; i < (int)(sizeof(title) / sizeof(title[0])); ++i)
	{
		GotoXY(2, startY + i);
		cout << title[i];
	}

	// 마우스 아스키 아트 (오른쪽)
// 마우스 아스키 아트 (오른쪽) - 원본 전체
// 마우스 아스키 아트 (오른쪽) - 원본 그대로
	const char* mouse[] = {
		"                                                                                                    ",
		"                           .........                                                                ",
		"                       ..:^^::::::^^                         :^^:::::::.:..                         ",
		"                    ::^^:...^~7?JYYYJJJ?7!!~~!!~~~~^!!!!!!!!!?J7!!!~~^^^^::::.                      ",
		"                  .^^.. .~?YPPGGGGPPGGGGGGGGGGGGGGPPPPP555555YYYYYYYYYJJ??!^.::                     ",
		"                 .~:...!YGGGGPPPGGPGGGGPGGGGGGGGG555JJJJJJJJ???J?JJJJJJJJJJJ?^.:                    ",
		"                .^:..:JGGGPPPPPPPPPPPPPPPPPPPPPPP5Y5JJJ?????????????????J??JYY^.:.                  ",
		"               .^:...JGGGPPPPPPPPGPPPPPPPPPPPPPPP5Y5JJJ?????????????????????JY?..:.                 ",
		"              :^....7GGGGPPPPPGGPPPPPPPPPPPPPPPPP5YYJJ???????777???777?????JJJY^ .::                ",
		"             ^^:...:5GGGGGGGGGGGGGGGGGGGGGGGGGGGG5Y5JJJJJJJ?J5555?^:::~!77?JYYY~...:.               ",
		"            :~:....:5GGGGGGPPPGGGG5YJ?J5GGGGGPPPGPY5JJJJJJJ?YP55P~   .:....^7J5!:...:               ",
		"            ^^:....:?GGGGGGGGG5J7^:.   .7GGGGGGGGP55JJ???????YJJYJ!::....    :!!^:...:              ",
		"           :~^:.....~5GGGGG5J~:. ..:.. .7GGGGPGGG5Y5JJJ??JJJJJ???JJJ7~:... .   .:^^:.:.             ",
		"           ~^^::...::~JY?7~::.........^JPPPPPPPPP5YYJJJJ???????????JJYYYJ7^...   .....::            ",
		"          ^~^:::::::....  ......  .^!YGGGGGPGPGPGP5Y?JJJ???????J?JJJJJY55PPY~..   .....^.           ",
		"         :~^^::::::............:!?5PGGGGGGGGGGGGGP55YYYJJJJJJJJJJYYYYY55PPYJ?^...  .....^.          ",
		"        :~^^^^:::::..    ...:~?PGGGGGGPPP555Y55YYYYJJJJJJJJJJJJJJJJJJJJ7!^ .!^:...   ...:^          ",
		"       .~^^^^^::::.... ...^:...:::::::::.                                   !!^:..   ....:^         ",
		"      .~^^^^^:::.......:^:.                                                 ~?~^:.........:^.       ",
		"     .~^::::::........:^.                                                   !!~~^::.........:.      ",
		"    .^^^:::..........^^                                                     :!~^:::::........::     ",
		"   .^^:::..........:^^                                                       .^~~^^^^^:.......::    ",
	};

	int mouseX = SCREEN_WIDTH / 2 - 25;
	int mouseY = 1;
	SetColor(Color::GRAY, Color::BLACK);
	for (int i = 0; i < (int)(sizeof(mouse) / sizeof(mouse[0])); ++i)
	{
		GotoXY(mouseX, mouseY + i);
		cout << mouse[i];
	}

	// 메뉴 안내 (왼쪽으로 밀기)
	int menuX = 4;
	int menuY = 12;
	SetColor(Color::WHITE, Color::BLACK);
	GotoXY(menuX, menuY);
	cout << "----------";
	GotoXY(menuX, menuY + 1);
	cout << "Forward / Z";
	GotoXY(menuX, menuY + 2);
	cout << "  : New Game";
	GotoXY(menuX, menuY + 3);
	cout << "----------";
	GotoXY(menuX, menuY + 4);
	cout << "Hold Wheel / ESC";
	GotoXY(menuX, menuY + 5);
	cout << "  : Setting";
}

void TitleScene::Release()
{
}