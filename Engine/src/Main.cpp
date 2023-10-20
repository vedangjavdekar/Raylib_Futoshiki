#include "Engine/Application.h"

int main(int argc, char* argv[])
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