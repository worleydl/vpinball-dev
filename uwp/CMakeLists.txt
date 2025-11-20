cmake_minimum_required(VERSION 3.25)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(AssetDeploy)
include(FetchContent)

# Setup UWP platform
set(CMAKE_SYSTEM_NAME WindowsStore)
set(CMAKE_SYSTEM_VERSION 10.0.19041.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# Define project
project(vpinball-uwp LANGUAGES CXX)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${PROJECT_NAME})

# Grab a fresh imgui (vpx project is slimmed down, we want basic SDL_renderer for launch menu)
FetchContent_Declare(imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG 23bd697f053bd5cf24601ee3f29572c0a645bd8f
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_HOME ${CMAKE_BINARY_DIR}/_deps/imgui-src/)

add_executable(${PROJECT_NAME} WIN32
	src/bootmenu.cpp
	src/bootmenu.h
	src/dx11glue.cpp
	src/dx11glue.h
	src/main.cpp

	# imgui for basic launcher
	${IMGUI_HOME}/imgui.cpp
	${IMGUI_HOME}/imgui_draw.cpp
	${IMGUI_HOME}/imgui_widgets.cpp
	${IMGUI_HOME}/imgui_tables.cpp

	${IMGUI_HOME}/backends/imgui_impl_sdl3.cpp
	${IMGUI_HOME}/backends/imgui_impl_dx11.cpp
)

set(BinDeps
	${CMAKE_SOURCE_DIR}/../build_dll/${CMAKE_BUILD_TYPE}/VPinballX_BGFX64.dll

	${CMAKE_SOURCE_DIR}/../build_dll/${CMAKE_BUILD_TYPE}/plugins/helloscript/plugin-helloscript64.dll
	${CMAKE_SOURCE_DIR}/../build_dll/${CMAKE_BUILD_TYPE}/plugins/helloworld/plugin-helloworld64.dll
	${CMAKE_SOURCE_DIR}/../build_dll/${CMAKE_BUILD_TYPE}/plugins/pinmame/plugin-pinmame64.dll
	${CMAKE_SOURCE_DIR}/../build_dll/${CMAKE_BUILD_TYPE}/plugins/remote-control/plugin-remote-control64.dll
	${CMAKE_SOURCE_DIR}/../build_dll/${CMAKE_BUILD_TYPE}/plugins/scoreview/plugin-scoreview64.dll

	${CMAKE_SOURCE_DIR}/../third-party/runtime-libs/uwp-x64/freeimage64.dll
	${CMAKE_SOURCE_DIR}/../third-party/runtime-libs/uwp-x64/pinmame64.dll
	${CMAKE_SOURCE_DIR}/../third-party/runtime-libs/uwp-x64/SDL3_image64.dll
	${CMAKE_SOURCE_DIR}/../third-party/runtime-libs/uwp-x64/SDL3_ttf64.dll

	${CMAKE_SOURCE_DIR}/../experimental/libvbscript.dll

	${CMAKE_SOURCE_DIR}/tmp-dep/dxil.dll
	${CMAKE_SOURCE_DIR}/tmp-dep/libgallium_wgl.dll
	${CMAKE_SOURCE_DIR}/tmp-dep/opengl32.dll
	${CMAKE_SOURCE_DIR}/tmp-dep/z-1.dll

	${CMAKE_SOURCE_DIR}/tmp-dep/libuwp.dll
	${CMAKE_SOURCE_DIR}/tmp-dep/SDL364.dll

	Package.appxmanifest
)
source_group("BinDeps" FILES ${BinDeps})
set_source_files_properties(${BinDeps} PROPERTIES
	VS_COPY_TO_OUT_DIR Always
	VS_DEPLOYMENT_CONTENT TRUE
	VS_DEPLOYMENT_LOCATION "."
)

set(PkgAssets
	Assets/LockScreenLogo.scale-200.png
	Assets/Square44x44Logo.targetsize-24_altform-unplated.png
	Assets/Square44x44Logo.scale-200.png
	Assets/Square150x150Logo.scale-200.png
	Assets/StoreLogo.png
)
source_group("PkgAssets" FILES ${PkgAssets})
set_source_files_properties(${PkgAssets} PROPERTIES
	#VS_COPY_TO_OUT_DIR Always
	VS_DEPLOYMENT_CONTENT TRUE
	VS_DEPLOYMENT_LOCATION "Assets"
)

# todo: restore data deploy

target_sources(${PROJECT_NAME} PRIVATE
	${BinDeps}
	${PkgAssets}
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${IMGUI_HOME}
	${CMAKE_SOURCE_DIR}/tmp-dep/include
)

set(BinLibs
	${CMAKE_SOURCE_DIR}/../build_dll/${CMAKE_BUILD_TYPE}/vpinball.lib
	${CMAKE_SOURCE_DIR}/tmp-dep/libuwp.lib
	${CMAKE_SOURCE_DIR}/tmp-dep/SDL364.lib

	WindowsApp.lib
	OneCore.lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${BinLibs})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION 10.0.19041.0)

# Data deployment
set(DEPLOY_ROOT ${CMAKE_SOURCE_DIR}/..)
uwp_pkg(${PROJECT_NAME} ${DEPLOY_ROOT}/src/assets "assets")
uwp_pkg(${PROJECT_NAME} ${DEPLOY_ROOT}/scripts "scripts")
uwp_pkg(${PROJECT_NAME} ${DEPLOY_ROOT}/tables "tables")
uwp_pkg(${PROJECT_NAME} ${DEPLOY_ROOT}/docs "docs")
uwp_pkg(${PROJECT_NAME} ${DEPLOY_ROOT}/build_dll/${CMAKE_BUILD_TYPE}/plugins "plugins")
