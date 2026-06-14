#include "SettingUI.h"
#include "Console.h"
#include "Defines.h"
#include "SceneManager.h"

// 설정 박스 크기 및 위치 (화면 중앙 기준)
static const int BOX_W = 36;
static const int BOX_H = 11;
static const int BOX_X = SCREEN_WIDTH  / 2 - BOX_W / 2;  // 62
static const int BOX_Y = SCREEN_HEIGHT / 2 - BOX_H / 2;  // 5

void SettingUI::Init()
{
    m_selectedIdx = 0;

    m_items.clear();
    m_items.push_back(SettingItem("BGM",    { "ON", "OFF" },              0));
    m_items.push_back(SettingItem("SFX",    { "ON", "OFF" },              0));
    m_items.push_back(SettingItem("난이도", { "쉬움", "보통", "어려움" }, 1));
}

void SettingUI::Update()
{
    if (GetKeyDown(VK_UP))
    {
        if (m_selectedIdx > 0)
            m_selectedIdx--;
    }
    if (GetKeyDown(VK_DOWN))
    {
        if (m_selectedIdx < (int)m_items.size() - 1)
            m_selectedIdx++;
    }

    if (GetKeyDown(VK_LEFT))
        m_items[m_selectedIdx].SelectPrev();

    if (GetKeyDown(VK_RIGHT))
        m_items[m_selectedIdx].SelectNext();

    if (GetKeyDown(VK_ESCAPE))
        SceneManager::GetInst()->ChangeScene("TitleScene");
}

void SettingUI::Render()
{
    ClearScreen();
    DrawBox();
    DrawItems();
}

void SettingUI::Release()
{
    m_items.clear();
}

void SettingUI::ClearScreen()
{
    SetColor(Color::WHITE, Color::BLACK);
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        GotoXY(0, y);
        for (int x = 0; x < SCREEN_WIDTH; ++x)
            cout << ' ';
    }
}

void SettingUI::DrawBox()
{
    SetColor(Color::LIGHT_YELLOW, Color::BLACK);

    // 상단 테두리
    GotoXY(BOX_X, BOX_Y);
    cout << '+';
    for (int i = 0; i < BOX_W - 2; ++i) cout << '-';
    cout << '+';

    // 좌우 테두리
    for (int row = 1; row < BOX_H - 1; ++row)
    {
        GotoXY(BOX_X, BOX_Y + row);
        cout << '|';
        GotoXY(BOX_X + BOX_W - 1, BOX_Y + row);
        cout << '|';
    }

    // 하단 테두리
    GotoXY(BOX_X, BOX_Y + BOX_H - 1);
    cout << '+';
    for (int i = 0; i < BOX_W - 2; ++i) cout << '-';
    cout << '+';

    // 제목 (박스 중앙 정렬, "[ 설  정 ]" = 10 표시칸)
    SetColor(Color::WHITE, Color::BLACK);
    GotoXY(BOX_X + BOX_W / 2 - 5, BOX_Y + 1);
    cout << "[ 설  정 ]";
}

void SettingUI::DrawItems()
{
    // 항목 시작 위치: 제목(+1) + 빈줄(+1) = +3
    int startY = BOX_Y + 3;

    for (int i = 0; i < (int)m_items.size(); ++i)
    {
        int  itemY    = startY + i;
        bool selected = (i == m_selectedIdx);

        // 항목 이름
        GotoXY(BOX_X + 2, itemY);
        SetColor(selected ? Color::LIGHT_GREEN : Color::LIGHT_GRAY, Color::BLACK);
        cout << (selected ? "> " : "  ");
        cout << m_items[i].GetName();

        // 옵션 값 (고정 열에 표시)
        GotoXY(BOX_X + 18, itemY);
        if (selected)
        {
            SetColor(Color::YELLOW, Color::BLACK);
            cout << "< " << m_items[i].GetCurrentOption() << " >";
        }
        else
        {
            SetColor(Color::WHITE, Color::BLACK);
            cout << "  " << m_items[i].GetCurrentOption();
        }
    }

    // 조작 안내
    SetColor(Color::GRAY, Color::BLACK);
    GotoXY(BOX_X + 2, BOX_Y + BOX_H - 4);
    cout << "Up/Dn : 이동   Lt/Rt : 변경";
    GotoXY(BOX_X + 2, BOX_Y + BOX_H - 3);
    cout << "ESC   : 뒤로가기";
}
