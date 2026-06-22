#include "SoundManager.h"
#include "fmod.hpp"

#pragma comment(lib, "fmod_vc.lib")

SoundManager* SoundManager::m_pInst = nullptr;

void SoundManager::Init()
{
    FMOD::System_Create(&m_system);

    if (m_system == nullptr)
        return;

    m_system->init(32, FMOD_INIT_NORMAL, nullptr);

    m_system->getMasterChannelGroup(&m_masterGroup);

    // BGM/SFX 개별 볼륨 제어용 채널 그룹. 기본적으로 마스터 그룹의 자식이라
    // 마스터 음소거/볼륨도 두 그룹에 함께 적용된다.
    m_system->createChannelGroup("bgm", &m_bgmGroup);
    m_system->createChannelGroup("sfx", &m_sfxGroup);
}

void SoundManager::Update()
{
    if (m_system)
        m_system->update();
}

void SoundManager::Release()
{
    for (auto& pair : m_channels)
    {
        if (pair.second != nullptr)
            pair.second->stop();
    }
    m_channels.clear();

    for (auto& pair : m_sounds)
    {
        if (pair.second != nullptr)
            pair.second->release();
    }
    m_sounds.clear();

    StopBGM();

    // 직접 만든 채널 그룹만 해제한다(마스터 그룹은 시스템 소유라 해제하지 않음).
    if (m_bgmGroup != nullptr)
    {
        m_bgmGroup->release();
        m_bgmGroup = nullptr;
    }

    if (m_sfxGroup != nullptr)
    {
        m_sfxGroup->release();
        m_sfxGroup = nullptr;
    }

    if (m_system != nullptr)
    {
        m_system->close();
        m_system->release();
        m_system = nullptr;
    }

    m_masterGroup = nullptr;
}

void SoundManager::Load(const std::string& key, const std::string& filePath)
{
    if (m_system == nullptr)
        return;

    FMOD::Sound* sound = nullptr;

    m_system->createSound(
        filePath.c_str(),
        FMOD_DEFAULT,
        nullptr,
        &sound
    );

    if (sound != nullptr)
        m_sounds[key] = sound;
}

void SoundManager::Play(const std::string& key)
{
    if (m_system == nullptr)
        return;

    auto it = m_sounds.find(key);

    if (it == m_sounds.end())
        return;

    m_system->playSound(
        it->second,
        m_sfxGroup,   // 효과음 그룹으로 재생 (SFX 볼륨 적용)
        false,
        nullptr
    );
}

void SoundManager::PlayRestart(const std::string& key)
{
    if (m_system == nullptr)
        return;

    auto it = m_sounds.find(key);

    if (it == m_sounds.end())
        return;

    auto ch = m_channels.find(key);

    if (ch != m_channels.end() && ch->second != nullptr)
    {
        bool playing = false;

        ch->second->isPlaying(&playing);

        if (playing)
            ch->second->stop();
    }

    FMOD::Channel* channel = nullptr;

    m_system->playSound(
        it->second,
        m_sfxGroup,   // 효과음 그룹으로 재생 (SFX 볼륨 적용)
        false,
        &channel
    );

    m_channels[key] = channel;
}

void SoundManager::PlayBGM(const std::string& filePath)
{
    if (m_system == nullptr)
        return;

    StopBGM();

    m_system->createStream(
        filePath.c_str(),
        FMOD_LOOP_NORMAL | FMOD_2D,
        nullptr,
        &m_bgm
    );

    m_system->playSound(
        m_bgm,
        m_bgmGroup,   // BGM 그룹으로 재생 (BGM 볼륨 적용)
        false,
        &m_bgmCh
    );
}

void SoundManager::StopBGM()
{
    if (m_bgmCh != nullptr)
    {
        m_bgmCh->stop();
        m_bgmCh = nullptr;
    }

    if (m_bgm != nullptr)
    {
        m_bgm->release();
        m_bgm = nullptr;
    }
}

void SoundManager::SetMute(bool mute)
{
    if (m_masterGroup == nullptr)
        return;

    m_masterGroup->setMute(mute);
    m_isMuted = mute;
}

void SoundManager::ToggleMute()
{
    SetMute(!m_isMuted);
}

void SoundManager::SetBGMVolume(int percent)
{
    if (percent < 0)   percent = 0;
    if (percent > 100) percent = 100;
    m_bgmVolume = percent;

    if (m_bgmGroup != nullptr)
        m_bgmGroup->setVolume(percent / 100.0f);
}

void SoundManager::SetSFXVolume(int percent)
{
    if (percent < 0)   percent = 0;
    if (percent > 100) percent = 100;
    m_sfxVolume = percent;

    if (m_sfxGroup != nullptr)
        m_sfxGroup->setVolume(percent / 100.0f);
}