#include <Windows.h>
#include "SDL3/SDL_main.h"


int bootstrap(int argc, char** argv)
{
	//return SDL_main(argc, argv);
   return 0;
}

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR argv, int argc)
{
   Sleep(1000);
   int qq = 0;

	// todo: should restore winrtrunapp in SDL3-uwp but this is a dry run
   //return SDL_main(0, 0);

	//return SDL_WinRTRunApp(bootstrap, NULL);
}
