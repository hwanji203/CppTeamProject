#pragma once
class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;
public:
	virtual void Init()   abstract;
	virtual void Update() abstract;
	virtual void Render() abstract;
	virtual void Release()abstract;
};


// gamescene
// titlescene
