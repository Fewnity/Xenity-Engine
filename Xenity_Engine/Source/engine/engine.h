// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <thread>
#endif

#include <engine/api.h>
#include <engine/event_system/event_system.h>
#include <engine/engine_args.h>

class Renderer;
class GameObject;
class Component;
class Transform;
class GameInterface;
class FileReference;
class ProjectDirectory;
class Texture;
class Shader;
class Material;

/*
* @brief Class used to manage the engine (Init, Stop...)
*/
class Engine
{
public:
	/**
	 * @brief Init Engine
	 */
	[[nodiscard]] API static int Init(int argc, char* argv[]);

	/**
	 * @brief Stop engine
	 */
	API static void Stop();

	/**
	 * @brief Engine loop
	 */
	API static void Loop();

	/**
	 * @brief Quit game
	 */
	API static void Quit();

	API static bool IsCalledFromMainThread();

	/**
	 * @brief Get if the engine is running
	 * @param checkRenderer Check if the renderer exists
	 */
	[[nodiscard]] static bool IsRunning(bool checkRenderer)
	{
		return s_isRunning && (!checkRenderer || s_renderer != nullptr);
	}

	static std::unique_ptr<GameInterface> s_game;

	static bool s_canUpdateAudio;

	/**
	 * @brief Get the renderer
	 */
	[[nodiscard]] API static inline Renderer &GetRenderer()
	{
		return *s_renderer;
	}

	/**
	 * @brief Get the OnWindowFocus event
	 */
	[[nodiscard]] static inline Event<>* GetOnWindowFocusEvent()
	{
		return s_onWindowFocusEvent;
	}

	[[nodiscard]] static const EngineArgs& GetArguments()
	{
		return s_engineArgs;
	}

private:
	static Event<>* s_onWindowFocusEvent;
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	static std::thread::id threadId;
#endif
	static void ParseEngineArguments(int argc, char* argv[]);

	/**
	 * @brief Close signal, called when the software is closing
	 */
	static void OnCloseSignal(int);

	/**
	 * @brief Check event (SDL)
	 */
	static void CheckEvents();

	static std::unique_ptr<Renderer> s_renderer;
	static EngineArgs s_engineArgs;

	static bool s_isRunning;
	static bool s_isInitialized;
	static int frameToSkip;
};