// Basic launcher menu heavily inspired by imgui's SDL + DX11 example: https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_directx11/main.cpp
// imgui licensed under MIT Copyright (c) 2014-2025 Omar Cornut
#include "bootmenu.h"

#include <imgui.h>
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_dx11.h"

#include "dx11glue.h"
#include "libuwp.h"
#include <SDL3/SDL.h>
#include <filesystem>
#include <ranges>
#include <string>
#include <vector>

namespace bootutil
{
	struct TableSelection
	{
		std::string displayText;
		std::string fullPath;
	};

	std::vector<TableSelection> DetectTables()
	{
		std::vector<std::string> active_paths = {};
		std::vector<std::string> search_paths = { "E:\\vpinball\\tables" }; // todo: local search
		std::vector<TableSelection> detected_tables = {};

		// Figure out which paths exist
		for (std::string path : search_paths)
		{
			std::filesystem::path fspath { path };

			if (std::filesystem::is_directory(fspath))
			{
				active_paths.push_back(path);
			}
		}

		// Check folders in active_paths
		for (std::string path : active_paths)
		{
			for (auto& p : std::filesystem::recursive_directory_iterator(path))
			{
				if (!p.is_regular_file())
					continue;

				std::string ext = p.path().extension().string();
				std::ranges::transform(ext, ext.begin(), ::tolower);

				if (ext == ".vpx")
				{
					std::string path = p.path().string();
					size_t pos = path.find_last_of("\\/");
					std::string display = (pos != std::string::npos) ? path.substr(pos + 1) : path;
					detected_tables.push_back(TableSelection { display, path });
				}
			}
		}

		return detected_tables;
	}
}

namespace bootmenu
{
	std::string g_selectedPath;

	void BootSelect(void* wnd, int w, int h)
	{
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

		SDL_Window* window = SDL_CreateWindow("VPX BootSelect", w, h, SDL_WINDOW_RESIZABLE);
		SDL_ShowWindow(window);

		if (!dx11glue::CreateDeviceD3D(wnd, w, h))
		{
			dx11glue::CleanupDeviceD3D();
			return;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplSDL3_InitForD3D(window);
		ImGui_ImplDX11_Init(dx11glue::g_pd3dDevice, dx11glue::g_pd3dDeviceContext);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = w;
		io.DisplaySize.y = h;

		io.FontGlobalScale = h / 720.0f;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NavEnableKeyboard;

		bool running = true;
		while (running)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				ImGui_ImplSDL3_ProcessEvent(&event);
			}

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			io.DisplaySize.x = w; // sdl3 seems to have legacy uwp bug of always reporting 1080, bandaid fix
			io.DisplaySize.y = h;

			ImGui::NewFrame();

			// Make window take up entire screen, no styles
			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			{
				ImGui::Begin("Table Selection", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

				ImGui::Text("VPX Standalone UWP Edition");
				ImGui::NewLine();
				ImGui::Text("Detected tables:");

				// Show detected tables
				ImGui::BeginListBox("Detected tables:", ImVec2(w, h * (2 / 3.0)));

				auto detected_tables = bootutil::DetectTables();
				for (auto entry : detected_tables)
				{
					if (ImGui::Selectable(entry.displayText.c_str(), false))
					{
						g_selectedPath = entry.fullPath;
						running = false;
					}
				}
				ImGui::EndListBox();


				ImGui::Text("Place VPX files under E:\\vpinball\\tables for detection.");
				ImGui::End();
			}

			ImGui::PopStyleVar(1);

			ImGui::Render();

			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			dx11glue::g_pd3dDeviceContext->OMSetRenderTargets(1, &dx11glue::g_mainRenderTargetView, nullptr);
			dx11glue::g_pd3dDeviceContext->ClearRenderTargetView(dx11glue::g_mainRenderTargetView, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			dx11glue::g_pSwapChain->Present(1, 0);
			uwp_ProcessEvents();
		}

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();

		dx11glue::CleanupDeviceD3D();
		SDL_DestroyWindow(window);
		SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
	}
}
