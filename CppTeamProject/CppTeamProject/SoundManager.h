#pragma once
#include <string>
#include <map>
#include "Utils.h"
#include "fmod.hpp"

#define SOUND SoundManager::GetInst()

class SoundManager
{
private:
    SoundManager() = default;
    ~SoundManager() = default;

public:
    static SoundManager* GetInst()
    {
        if (m_pInst == nullptr)
            m_pInst = new SoundManager;
        return m_pInst;
    }

    static void DestroyInst()
    {
        SAFE_DELETE(m_pInst);
    }

    void Init();
    void Update();
    void Release();

    void Load(const std::string& key, const std::string& filePath);

    void Play(const std::string& key);
    void PlayRestart(const std::string& key);

    void PlayBGM(const std::string& filePath);
    void StopBGM();

    void SetMute(bool mute);
    void ToggleMute();
    bool IsMuted() const { return m_isMuted; }

private:
    static SoundManager* m_pInst;

private:
    FMOD::System* m_system = nullptr;

    FMOD::Channel* m_bgmCh = nullptr;
    FMOD::Sound* m_bgm = nullptr;

    FMOD::ChannelGroup* m_masterGroup = nullptr;

    bool m_isMuted = false;
    float m_volume = 1.0f;

    std::map<std::string, FMOD::Sound*> m_sounds;
    std::map<std::string, FMOD::Channel*> m_channels;
};