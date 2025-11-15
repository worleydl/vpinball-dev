cmake_minimum_required(VERSION 3.25)

# Setup UWP platform
set(CMAKE_SYSTEM_NAME WindowsStore)
set(CMAKE_SYSTEM_VERSION 10.0.19041.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# Define project
project(vpinball-uwp LANGUAGES CXX)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${PROJECT_NAME})

add_executable(${PROJECT_NAME} WIN32
	main.cpp
)

set(BinDeps
	${CMAKE_SOURCE_DIR}/../build_dll/${CMAKE_BUILD_TYPE}/VPinballX_BGFX64.dll

	${CMAKE_SOURCE_DIR}/../third-party/runtime-libs/uwp-x64/freeimage64.dll
	${CMAKE_SOURCE_DIR}/../third-party/runtime-libs/uwp-x64/SDL3_image64.dll
	${CMAKE_SOURCE_DIR}/../third-party/runtime-libs/uwp-x64/SDL3_ttf64.dll

	${CMAKE_SOURCE_DIR}/../experimental/libvbscript.dll

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
	${CMAKE_SOURCE_DIR}/tmp-dep/include
)

set(BinLibs
	${CMAKE_SOURCE_DIR}/../build_dll/${CMAKE_BUILD_TYPE}/vpinball.lib
	${CMAKE_SOURCE_DIR}/tmp-dep/SDL364.lib

	dxguid.lib
	WindowsApp.lib
	OneCore.lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${BinLibs})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION 10.0.19041.0)

# Data deployment
add_custom_command(TARGET vpinball-uwp POST_BUILD
   COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:vpinball-uwp>/user"
   COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/../src/assets" "$<TARGET_FILE_DIR:vpinball-uwp>/assets"
   COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/../scripts" "$<TARGET_FILE_DIR:vpinball-uwp>/scripts"
   COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/../tables" "$<TARGET_FILE_DIR:vpinball-uwp>/tables"
   COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/../docs" "$<TARGET_FILE_DIR:vpinball-uwp>/docs"

   COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/../build_dll/${CMAKE_BUILD_TYPE}/plugins" "$<TARGET_FILE_DIR:vpinball-uwp>/plugins"
)
