#include "ResultScene.h"
#include "Console.h"
#include "Defines.h"
#include "SceneManager.h"
#include <cstdio>
#include <cstring>

static const int RBOX_W = 40;
static const int RBOX_H = 11;
static const int RBOX_X = SCREEN_WIDTH / 2 - RBOX_W / 2;
static const int RBOX_Y = SCREEN_HEIGHT / 2 - RBOX_H / 2;

void ResultScene::Init()
{
    m_justOpened = true;
}

void ResultScene::Update()
{

    // 진입 첫 프레임은 직전(게임 중) 키 입력이 흘러들어오지 않게 무시한다.
    if (m_justOpened)
    {
        m_justOpened = false;
        return;
    }

    // 모든 조작은 마우스: 앞으로 가기 = 재시작, 뒤로 가기 = 게임 나가기.
    if (GetForwardDown())
        SceneManager::GetInst()->ChangeScene("GameScene");   // Restart
    else if (GetBackDown())
        SceneManager::GetInst()->ChangeScene("TitleScene");  // Leave
}

void ResultScene::Render()
{
    GotoXY(0, 0);
    DrawBox();
    DrawContents();
}

void ResultScene::Release()
{
}

void ResultScene::DrawBox()
{
    SetColor(Color::LIGHT_RED, Color::BLACK);

    GotoXY(RBOX_X, RBOX_Y);
    cout << '+';
    for (int i = 0; i < RBOX_W - 2; ++i) cout << '-';
    cout << '+';

    for (int row = 1; row < RBOX_H - 1; ++row)
    {
        GotoXY(RBOX_X, RBOX_Y + row);
        cout << '|';
        GotoXY(RBOX_X + RBOX_W - 1, RBOX_Y + row);
        cout << '|';
    }

    GotoXY(RBOX_X, RBOX_Y + RBOX_H - 1);
    cout << '+';
    for (int i = 0; i < RBOX_W - 2; ++i) cout << '-';
    cout << '+';
}

void ResultScene::DrawContents()
{
    // 제목
    SetColor(Color::LIGHT_RED, Color::BLACK);
    GotoXY(RBOX_X + RBOX_W / 2 - 5, RBOX_Y + 1);
    cout << "GAME OVER";

    // 버틴 시간
    char buf[32];
    snprintf(buf, sizeof(buf), "You survived: %.1fs", m_surviveTime);
    SetColor(Color::WHITE, Color::BLACK);
    GotoXY(RBOX_X + RBOX_W / 2 - (int)(strlen(buf) / 2), RBOX_Y + 3);
    cout << buf;

    // 마우스 앞/뒤 버튼에 고정 매핑된 동작 안내.
    int startY = RBOX_Y + 5;
    SetColor(Color::LIGHT_GREEN, Color::BLACK);
    GotoXY(RBOX_X + RBOX_W / 2 - 8, startY);
    cout << "Forward / Z : Restart";
    SetColor(Color::LIGHT_RED, Color::BLACK);
    GotoXY(RBOX_X + RBOX_W / 2 - 8, startY + 1);
    cout << "Back / X    : Leave";

    SetColor(Color::GRAY, Color::BLACK);
    GotoXY(RBOX_X + 2, RBOX_Y + RBOX_H - 2);
    cout << "Mouse Fwd/Back buttons";
}
