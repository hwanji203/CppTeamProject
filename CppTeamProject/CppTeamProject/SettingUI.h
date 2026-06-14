#pragma once
#include "Scene.h"
#include "SettingItem.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

class SettingUI : public Scene
{
public:
    void Init() override;
    void Update() override;
    void Render() override;
    void Release() override;

    void SetPrevScene(const string& sceneName) { m_prevSceneName = sceneName; }

    int GetBGMIndex() const { return m_items[0].GetCurrentIndex(); }
    int GetSFXIndex() const { return m_items[1].GetCurrentIndex(); }
    int GetDifficultyIndex() const { return m_items[2].GetCurrentIndex(); }

private:
    void ClearScreen();
    void DrawBox();
    void DrawItems();

private:
    vector<SettingItem> m_items;
    int m_selectedIdx;
    string m_prevSceneName;
    bool m_justOpened;
};
