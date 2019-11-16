#include "PathfindingApp.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720
// main that controls the creation/destruction of an application
int WinMain(/*int argc, char* argv[]*/)
{
	// explicitly control the creation of our application
	PathfindingApp* app = new PathfindingApp();
	app->run("PathfindingApp Project", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, false);

	// explicitly control the destruction of our application
	delete app;

	return 0;
}