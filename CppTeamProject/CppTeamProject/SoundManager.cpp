#include "SoundManager.h"
#include "fmod.hpp"

#pragma comment(lib, "fmod_vc.lib")
SoundManager* SoundManager::m_pInst = nullptr;

void SoundManager::Init()
{
    FMOD::System_Create(&m_system);
    m_system->init(32, FMOD_INIT_NORMAL, nullptr);
}

void SoundManager::Update()
{
    if (m_system)
        m_system->update();
}

void SoundManager::Release()
{
    // 추적 중인 재생 채널 정지
    for (auto& pair : m_channels)
        if (pair.second != nullptr)
            pair.second->stop();
    m_channels.clear();

    for (auto& pair : m_sounds)
        pair.second->release();
    m_sounds.clear();

    StopBGM();

    if (m_system != nullptr)
    {
        m_system->close();
        m_system->release();
        m_system = nullptr;
    }
}

void SoundManager::Load(const std::string& key, const std::string& filePath)
{
    if (m_system == nullptr)
        return;

    FMOD::Sound* sound = nullptr;
    m_system->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
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

    m_system->playSound(it->second, nullptr, false, nullptr);
}

void SoundManager::PlayRestart(const std::string& key)
{
    if (m_system == nullptr)
        return;

    auto it = m_sounds.find(key);
    if (it == m_sounds.end())
        return;

    // 같은 키가 아직 재생 중이면 먼저 멈춰서 겹치지 않게 한다.
    auto ch = m_channels.find(key);
    if (ch != m_channels.end() && ch->second != nullptr)
    {
        bool playing = false;
        ch->second->isPlaying(&playing);
        if (playing)
            ch->second->stop();
    }

    FMOD::Channel* channel = nullptr;
    m_system->playSound(it->second, nullptr, false, &channel);
    m_channels[key] = channel;
}

void SoundManager::PlayBGM(const std::string& filePath)
{
    if (m_system == nullptr)
        return;

    StopBGM();
    m_system->createStream(filePath.c_str(), FMOD_LOOP_NORMAL | FMOD_2D, nullptr, &m_bgm);
    m_system->playSound(m_bgm, nullptr, false, &m_bgmCh);
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
