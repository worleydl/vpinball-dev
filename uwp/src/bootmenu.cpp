// Basic launcher menu heavily inspired by imgui's SDL + DX11 example: https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_directx11/main.cpp
// imgui licensed under MIT Copyright (c) 2014-2025 Omar Cornut
#include "bootmenu.h"

#include <imgui.h>
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_dx11.h"

#include "dx11glue.h"
#include "libuwp.h"
#include <SDL3/SDL.h>

namespace bootmenu
{
	void BootSelect(void* wnd, int w, int h)
	{
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

		SDL_Window* window = SDL_CreateWindow("VPX BootSelect", w, h, SDL_WINDOW_RESIZABLE);

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
		io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;

		bool running = true;
		while (running)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplSDL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Launcher");
			ImGui::Text("SDL3 + DX11 placeholder");
			ImGui::End();

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
	}
}
