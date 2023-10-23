#include "Engine/Application.h"

#ifdef BUILD_DEBUG

int main(int argc, char* argv[])

#elif BUILD_RELEASE

#if defined(_WIN32)           
#define NOGDI             // All GDI defines and routines
#define NOUSER            // All USER defines and routines
#endif

#include <Windows.h> // or any library that uses Windows.h

#if defined(_WIN32)           // raylib uses these names as function parameters
#undef near
#undef far
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* pCmdLine, int nCmdShow)
#endif 

{
	Engine::ApplicationProps props{
		1280,
		720,
		"Futoshiki"
	};

	Engine::Application app(props);
	app.Run();
	return 0;
}