#pragma once
#include "Utils.h"
#include<map>
#include<memory>
#include<string>
#include "Scene.h"
class SceneManager
{
private:
	SceneManager() = default;
public:
	static SceneManager* GetInst()
	{
		if (nullptr == m_inst)
			m_inst = new SceneManager;
		return m_inst;
	}
	static void DestroyInst()
	{
		SAFE_DELETE(m_inst);
	}
public:
	void Update();
	void Render();
	void ChangeScene(const std::string& _sceneName);
	void RegisterScene(const std::string& _sceneName,
					std::unique_ptr<Scene> _scene);
	std::string GetCurSceneName() const { return m_curSceneName; }
	Scene* GetScene(const std::string& name);
private:
	static SceneManager* m_inst;
	std::map<std::string, std::unique_ptr<Scene>> m_mapScenes;
	Scene* m_curScene = nullptr;
	std::string m_curSceneName;
};