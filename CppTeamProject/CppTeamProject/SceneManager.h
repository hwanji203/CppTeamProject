#pragma once
// 어디서든지 접근 -> include
// 객체 하나만 존재 -> static
#include "Defines.h"
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
private:
	static SceneManager* m_inst;
	std::map<std::string, std::unique_ptr<Scene>> m_mapScenes;
	Scene* m_curScene = nullptr;
};