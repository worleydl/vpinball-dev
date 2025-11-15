#include <Windows.h>
#include "SDL3/SDL_main.h"

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR argv, int argc) {
	SDL_RunApp(0, 0, SDL_main, NULL);
}
