#pragma once

class Core
{
public:
	Core();
	~Core();
public:
	void Run();
private:
	void Init();
	void Update();
	void Render();

private:
	bool m_isRunning = true;
};
