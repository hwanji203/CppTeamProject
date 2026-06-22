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

static const char* RESULT_ITEMS[] = { "Restart", "Leave" };
static const int RESULT_ITEM_COUNT = 2;

void ResultScene::Init()
{
    m_selectedIdx = 0;
    m_justOpened  = true;
}

void ResultScene::Update()
{

    // 진입 첫 프레임은 직전(게임 중) 키 입력이 흘러들어오지 않게 무시한다.
    if (m_justOpened)
    {
        m_justOpened = false;
        return;
    }

    if (GetKeyDown(VK_UP))
    {
        if (m_selectedIdx > 0)
            m_selectedIdx--;
    }
    if (GetKeyDown(VK_DOWN))
    {
        if (m_selectedIdx < RESULT_ITEM_COUNT - 1)
            m_selectedIdx++;
    }

    if (GetKeyDown(VK_RETURN))
    {
        if (m_selectedIdx == 0)
            SceneManager::GetInst()->ChangeScene("GameScene");   // Restart
        else
            SceneManager::GetInst()->ChangeScene("TitleScene");  // Leave
    }
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

    // 메뉴(Restart / Leave)
    int startY = RBOX_Y + 5;
    for (int i = 0; i < RESULT_ITEM_COUNT; ++i)
    {
        bool selected = (i == m_selectedIdx);
        GotoXY(RBOX_X + RBOX_W / 2 - 5, startY + i);
        SetColor(selected ? Color::LIGHT_GREEN : Color::LIGHT_GRAY, Color::BLACK);
        cout << (selected ? "> " : "  ") << RESULT_ITEMS[i];
    }

    SetColor(Color::GRAY, Color::BLACK);
    GotoXY(RBOX_X + 2, RBOX_Y + RBOX_H - 2);
    cout << "Up/Dn : move   Enter : select";
}
