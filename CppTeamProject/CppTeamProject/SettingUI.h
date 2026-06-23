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

    // 오버레이(일시정지)로 띄울 때, 닫기 요청이 들어왔는지 Core가 확인한다.
    bool IsClosed() const { return m_closed; }

    // 내용이 바뀐 프레임에만 다시 그린다(매 프레임 재렌더 시 박스가 깜빡이므로).
    bool IsDirty() const { return m_dirty; }
    void ClearDirty() { m_dirty = false; }

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
    bool m_closed;
    bool m_dirty;
};
