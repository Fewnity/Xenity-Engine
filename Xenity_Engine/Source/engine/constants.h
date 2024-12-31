#pragma once

//
// -------------------------------------------------- Features
//
//#define ENABLE_EXPERIMENTAL_FEATURES // Enable features that are not fully tested or implemented
//#define ENABLE_OVERDRAW_OPTIMIZATION // Enable overdraw optimization (currently not great)
#define ENABLE_SHADER_VARIANT_OPTIMIZATION // Enable shader variant optimization (currently effective only on PS3, WIP)

#if defined(__PS3__)
//#define ENABLE_OVERDRAW_OPTIMIZATION
#define ENABLE_SHADER_VARIANT_OPTIMIZATION
#endif

//
// -------------------------------------------------- Version
//
// Increase those numbers when you make a new release
#define ENGINE_VERSION "0.1"
#define ENGINE_DLL_VERSION "2" // Increase by one this number to invalidate compiled Dlls of games projects

//
// -------------------------------------------------- Files/directories names
//
// General
#define PS3_DATA_FOLDER "/dev_hdd0/xenity_engine/" // When using data from hard drive
//#define PS3_DATA_FOLDER "/dev_usb/xenity_engine/" // When using data from usb

#define PROJECT_SETTINGS_FILE_NAME "project_settings.json"
#define META_EXTENSION ".meta"
#define PROJECTS_LIST_FILE "projects.json"

// Compiler
#define ENGINE_EDITOR_FOLDER "Xenity_Editor"
#define ENGINE_GAME_FOLDER "Xenity_Engine"
#define ASSETS_FOLDER "assets/"
#define ENGINE_ASSETS_FOLDER "engine_assets/"
#define PUBLIC_ENGINE_ASSETS_FOLDER "public_engine_assets/"
#define MSVC_START_FILE_64BITS "vcvars64.bat"
#define MSVC_START_FILE_32BITS "vcvars32.bat"

// Debug
#define CRASH_DUMP_FILE "crash_dump.txt"
#define DEBUG_LOG_FILE "xenity_engine_debug.txt"
#define PSVITA_DEBUG_LOG_FOLDER "ux0:data/xenity_engine/"

//
// -------------------------------------------------- Audio
//
#if defined(__PS3__)
#define SOUND_FREQUENCY 48000
#else
#define SOUND_FREQUENCY 44100
#endif
#define AUDIO_BUFFER_SIZE 2048

//
// -------------------------------------------------- GameObjects/Components
//
#define DEFAULT_GAMEOBJECT_NAME "GameObject"

#define DEFAULT_CAMERA_FOV 60.0f

//
// -------------------------------------------------- Physics
//
#define DEFAULT_GRAVITY_Y -20

//
// -------------------------------------------------- Graphics
//
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#define MAX_LIGHT_COUNT 10
#else
#define MAX_LIGHT_COUNT 4
#endif

//
// -------------------------------------------------- World partitionner
//
#define WORLD_CHUNK_SIZE 10
#define WORLD_CHUNK_HALF_SIZE (WORLD_CHUNK_SIZE / 2.0f)

//
// -------------------------------------------------- Profiling
//

#if defined(EDITOR)
#define USE_PROFILER
#endif

//
// -------------------------------------------------- Inputs
//
#if defined(__PS3__) || defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#define MAX_CONTROLLER 8
#else
#define MAX_CONTROLLER 1
#endif