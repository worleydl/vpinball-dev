#pragma once

#include <string>

namespace bootmenu
{
	static std::string g_selectedPath;

	void BootSelect(void* wnd, int w, int h);
}
