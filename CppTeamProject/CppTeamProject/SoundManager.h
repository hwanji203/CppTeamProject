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

    // BGM/SFX 개별 볼륨. 입력은 0~100, 내부에서 0.0~1.0으로 변환해 채널 그룹에 적용한다.
    void SetBGMVolume(int percent);
    void SetSFXVolume(int percent);
    int  GetBGMVolume() const { return m_bgmVolume; }
    int  GetSFXVolume() const { return m_sfxVolume; }

private:
    static SoundManager* m_pInst;

private:
    FMOD::System* m_system = nullptr;

    FMOD::Channel* m_bgmCh = nullptr;
    FMOD::Sound* m_bgm = nullptr;

    FMOD::ChannelGroup* m_masterGroup = nullptr;
    FMOD::ChannelGroup* m_bgmGroup = nullptr;   // BGM 전용 (마스터의 자식)
    FMOD::ChannelGroup* m_sfxGroup = nullptr;   // 효과음 전용 (마스터의 자식)

    bool m_isMuted = false;
    float m_volume = 1.0f;

    int m_bgmVolume = 100;   // 0~100
    int m_sfxVolume = 100;   // 0~100

    std::map<std::string, FMOD::Sound*> m_sounds;
    std::map<std::string, FMOD::Channel*> m_channels;
};