#include "Frojengine\System\EngineSystem.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	bool result;

	CEngineSystem* engine = new CEngineSystem;

	if (engine == nullptr)
	{
		return -1;
	}

	result = engine->Create(L"Frojengine", L"Practice DX11", 800, 600);

	if (!result)
	{
		engine->Release();
		return -1;
	}

	result = engine->LoadData();

	engine->Run();

	engine->Release();

	return 0;
}