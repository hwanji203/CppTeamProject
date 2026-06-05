#include "SceneManager.h"
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

	m_curScene = iter->second.get();
	m_curScene->Init();
}

void SceneManager::RegisterScene(const std::string& _sceneName, std::unique_ptr<Scene> _scene)
{
	m_mapScenes[_sceneName] = std::move(_scene);
}