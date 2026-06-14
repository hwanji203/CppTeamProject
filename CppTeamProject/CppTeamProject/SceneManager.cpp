#include "SceneManager.h"
#include "Console.h"
#include "Defines.h"
SceneManager* SceneManager::m_inst = nullptr;

void SceneManager::Update()
{
	if (m_curScene)
		m_curScene->Update();
}

void SceneManager::Render()
{
	if (m_curScene)
		m_curScene->Render();
}

void SceneManager::ChangeScene(const std::string& _sceneName)
{
	auto iter = m_mapScenes.find(_sceneName);

	if (iter == m_mapScenes.end())
		return;

	if (m_curScene)
		m_curScene->Release();

	SetColor(Color::WHITE, Color::BLACK);
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		GotoXY(0, y);
		for (int x = 0; x < SCREEN_WIDTH; ++x)
			cout << ' ';
	}

	m_curSceneName = _sceneName;
	m_curScene = iter->second.get();
	m_curScene->Init();
}

Scene* SceneManager::GetScene(const std::string& name)
{
	auto iter = m_mapScenes.find(name);
	if (iter == m_mapScenes.end())
		return nullptr;
	return iter->second.get();
}

void SceneManager::RegisterScene(const std::string& _sceneName, std::unique_ptr<Scene> _scene)
{
	m_mapScenes[_sceneName] = std::move(_scene);
}