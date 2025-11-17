#include <Windows.h>
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

int bootstrap(int, char**)
{
   // todo: This should be forced in the SDL3-UWP build
   SDL_SetHint(SDL_HINT_JOYSTICK_WGI, "true");

   char* args[] = {
	  "himom.exe",
	  "-play",
	  "E:\\vpinball\\tables\\test.vpx",
   };

   return SDL_main(3, args);
}

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR argv, int argc) {
   SDL_RunApp(0, 0, bootstrap, NULL);
}
