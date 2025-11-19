#include <Windows.h>
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include <string>
#include <vector>

#include "bootmenu.h"
#include "libuwp.h"

int bootstrap(int, char**)
{
   // todo: This should be forced in the SDL3-UWP build
   SDL_SetHint(SDL_HINT_JOYSTICK_WGI, "true");

   int x, y;
   uwp_GetScreenSize(&x, &y);
   bootmenu::BootSelect(uwp_GetWindowReference(), x, y);

   std::string exe_name = "vpinball-uwp.exe";
   std::string param = "-play";

   std::vector<char*> args;
   args.push_back(exe_name.data());
   args.push_back(param.data());
   args.push_back(bootmenu::g_selectedPath.data());

   return SDL_main(args.size(), args.data());
}

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR argv, int argc) {
   SDL_RunApp(0, 0, bootstrap, NULL);
}
