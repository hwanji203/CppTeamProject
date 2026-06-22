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
    void DrawBox();
    void DrawItems();

private:
    void ApplySetting(int idx, int delta);   // BGM/SFX 볼륨을 delta만큼 증감

private:
    vector<SettingItem> m_items;
    int m_selectedIdx;
    int m_prevSelectedIdx;
    string m_prevSceneName;
    bool m_justOpened;
};
