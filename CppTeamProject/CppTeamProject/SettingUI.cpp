#include "SettingUI.h"
#include "Console.h"
#include "Defines.h"
#include "SceneManager.h"

static const int BOX_W = 36;
static const int BOX_H = 11;
static const int BOX_X = SCREEN_WIDTH / 2 - BOX_W / 2;
static const int BOX_Y = SCREEN_HEIGHT / 2 - BOX_H / 2;

void SettingUI::Init()
{
    m_selectedIdx = 0;
    m_justOpened = true;

    if (m_items.empty())
    {
        m_items.push_back(SettingItem("BGM", { "ON", "OFF" }, 0));
        m_items.push_back(SettingItem("SFX", { "ON", "OFF" }, 0));
    }
}

void SettingUI::Update()
{
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
        if (m_selectedIdx < (int)m_items.size() - 1)
            m_selectedIdx++;
    }

    if (GetKeyDown(VK_LEFT))
        m_items[m_selectedIdx].SelectPrev();

    if (GetKeyDown(VK_RIGHT))
        m_items[m_selectedIdx].SelectNext();

    if (GetKeyDown(VK_ESCAPE))
        SceneManager::GetInst()->ChangeScene(m_prevSceneName);
}

void SettingUI::Render()
{
    GotoXY(0, 0);
    DrawBox();
    DrawItems();
}

void SettingUI::Release()
{
    m_items.clear();
}

void SettingUI::DrawBox()
{
    SetColor(Color::LIGHT_YELLOW, Color::BLACK);

    GotoXY(BOX_X, BOX_Y);
    cout << '+';
    for (int i = 0; i < BOX_W - 2; ++i) cout << '-';
    cout << '+';

    for (int row = 1; row < BOX_H - 1; ++row)
    {
        GotoXY(BOX_X, BOX_Y + row);
        cout << '|';
        GotoXY(BOX_X + BOX_W - 1, BOX_Y + row);
        cout << '|';
    }

    GotoXY(BOX_X, BOX_Y + BOX_H - 1);
    cout << '+';
    for (int i = 0; i < BOX_W - 2; ++i) cout << '-';
    cout << '+';

    SetColor(Color::WHITE, Color::BLACK);
    GotoXY(BOX_X + BOX_W / 2 - 4, BOX_Y + 1);
    cout << "[ 설정 ]";
}

void SettingUI::DrawItems()
{
    int startY = BOX_Y + 3;

    for (int i = 0; i < (int)m_items.size(); ++i)
    {
        int itemY = startY + i;
        bool selected = (i == m_selectedIdx);

        GotoXY(BOX_X + 2, itemY);
        SetColor(selected ? Color::LIGHT_GREEN : Color::LIGHT_GRAY, Color::BLACK);
        cout << (selected ? "> " : "  ");
        cout << m_items[i].GetName();

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

    SetColor(Color::GRAY, Color::BLACK);
    GotoXY(BOX_X + 2, BOX_Y + BOX_H - 4);
    cout << "Up/Dn : 이동  Lt/Rt : 변경";
    GotoXY(BOX_X + 2, BOX_Y + BOX_H - 3);
    cout << "ESC : 뒤로가기";
}
