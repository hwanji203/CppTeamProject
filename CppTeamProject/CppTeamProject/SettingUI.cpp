#include "SettingUI.h"

#include "Console.h"
#include "Defines.h"
#include "SceneManager.h"
#include "SoundManager.h"

static const int BOX_W = 36;
static const int BOX_H = 11;
static const int BOX_X = SCREEN_WIDTH / 2 - BOX_W / 2;
static const int BOX_Y = SCREEN_HEIGHT / 2 - BOX_H / 2;

static const int VOL_STEP  = 10;   // 휠/방향키 한 번에 바뀌는 볼륨 폭 (step)
static const int BAR_WIDTH = 10;   // 볼륨 DrawBar 칸 수 (cells)

void SettingUI::Init()
{
    m_selectedIdx = 0;
    m_justOpened = true;

    m_items.clear();

    // BGM/SFX는 볼륨(0~100)을 DrawBar로 표시한다. 실제 값은 SoundManager가 보관한다.
    // 여기 옵션은 자리표시자만 둔다(선택 인덱스는 사용하지 않음).
    m_items.push_back(SettingItem("BGM",  { "-" }, 0));
    m_items.push_back(SettingItem("SFX",  { "-" }, 0));
    m_items.push_back(SettingItem("EXIT", { "QUIT" }, 0));
}

void SettingUI::Update()
{
    if (m_justOpened)
    {
        m_justOpened = false;
        return;
    }

    // 항목 이동: 뒤로가기(위) / 앞으로가기(아래) + 방향키. 마우스(XBUTTON)·키보드(Z/X·화살표) 모두 동작.
    if (GetKeyDown(VK_UP) || GetBackDown())
    {
        if (m_selectedIdx > 0)
            m_selectedIdx--;
    }

    if (GetKeyDown(VK_DOWN) || GetForwardDown())
    {
        if (m_selectedIdx < (int)m_items.size() - 1)
            m_selectedIdx++;
    }

    const std::string& name = m_items[m_selectedIdx].GetName();
    const bool onExit = (name == "EXIT");

    // 휠 델타는 매 프레임 비운다(EXIT에서 스크롤한 게 나중에 볼륨으로 튀지 않게).
    int wheel = GetMouseWheelChange();

    // 값 조절: 휠 스크롤 + 방향키(EXIT 제외). 휠 위/오른쪽 = 증가, 아래/왼쪽 = 감소.
    if (!onExit)
    {
        int step = 0;
        if (GetKeyDown(VK_RIGHT) || wheel > 0) step += VOL_STEP;
        if (GetKeyDown(VK_LEFT)  || wheel < 0) step -= VOL_STEP;
        if (step != 0)
            ApplySetting(m_selectedIdx, step);
    }

    // 닫기/종료. ESC는 항상 닫기, 휠 짧게 클릭은 EXIT면 종료/그 외엔 닫기, Enter는 EXIT면 종료.
    if (GetKeyDown(VK_ESCAPE))
    {
        SceneManager::GetInst()->ChangeScene(m_prevSceneName);
        return;
    }

    if (GetKeyDown(VK_MBUTTON))
    {
        if (onExit)
            exit(0);

        SceneManager::GetInst()->ChangeScene(m_prevSceneName);
        return;
    }

    if (GetKeyDown(VK_RETURN) && onExit)
        exit(0);
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

void SettingUI::ApplySetting(int idx, int delta)
{
    const std::string& name = m_items[idx].GetName();
    SoundManager* sound = SoundManager::GetInst();

    // SetBGMVolume/SetSFXVolume이 내부에서 0~100으로 클램프한다.
    if (name == "BGM")
        sound->SetBGMVolume(sound->GetBGMVolume() + delta);
    else if (name == "SFX")
        sound->SetSFXVolume(sound->GetSFXVolume() + delta);
}

void SettingUI::DrawBox()
{
    SetColor(Color::LIGHT_YELLOW, Color::BLACK);

    GotoXY(BOX_X, BOX_Y);
    std::cout << '+';

    for (int i = 0; i < BOX_W - 2; i++)
        std::cout << '-';

    std::cout << '+';

    for (int row = 1; row < BOX_H - 1; row++)
    {
        GotoXY(BOX_X, BOX_Y + row);
        std::cout << '|';

        GotoXY(BOX_X + BOX_W - 1, BOX_Y + row);
        std::cout << '|';
    }

    GotoXY(BOX_X, BOX_Y + BOX_H - 1);

    std::cout << '+';

    for (int i = 0; i < BOX_W - 2; i++)
        std::cout << '-';

    std::cout << '+';

    SetColor(Color::WHITE, Color::BLACK);

    GotoXY(BOX_X + BOX_W / 2 - 5, BOX_Y + 1);
    std::cout << "[ SETTING ]";
}

void SettingUI::DrawItems()
{
    int startY = BOX_Y + 3;

    for (int i = 0; i < (int)m_items.size(); i++)
    {
        int itemY = startY + i;
        bool selected = (i == m_selectedIdx);
        const std::string& name = m_items[i].GetName();

        // 선택 커서 (cursor)
        GotoXY(BOX_X + 2, itemY);
        SetColor(
            selected ? Color::LIGHT_GREEN : Color::LIGHT_GRAY,
            Color::BLACK
        );
        std::cout << (selected ? "> " : "  ");

        if (name == "EXIT")
        {
            std::cout << name;

            GotoXY(BOX_X + 18, itemY);
            SetColor(selected ? Color::LIGHT_RED : Color::WHITE, Color::BLACK);
            cout << "[ QUIT ]";
        }
        else
        {
            // BGM/SFX: 현재 볼륨(0~100)을 DrawBar로 표시. (라벨/바/수치는 DrawBar가 그린다.)
            int vol = (name == "BGM")
                ? SoundManager::GetInst()->GetBGMVolume()
                : SoundManager::GetInst()->GetSFXVolume();

            // 기본 ■/□는 전각(2칸)이라 모달을 넘치므로 단일 폭(ASCII) 문자로 그린다.
            DrawBar(BOX_X + 4, itemY, name + " ", vol, 100, BAR_WIDTH, "#", "-");
        }
    }

    SetColor(Color::GRAY, Color::BLACK);

    GotoXY(BOX_X + 2, BOX_Y + BOX_H - 4);
    cout << "Fwd/Back : Move   Wheel : Adjust";

    GotoXY(BOX_X + 2, BOX_Y + BOX_H - 3);
    cout << "Wheel click: Close/Quit ESC:Back";
}