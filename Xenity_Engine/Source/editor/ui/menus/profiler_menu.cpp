// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "profiler_menu.h"
#include <imgui/imgui.h>
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include <engine/file_system/file_reference.h>
#include <engine/time/time.h>
#include <engine/engine_settings.h>
#include <engine/debug/performance.h>
#include <engine/debug/memory_tracker.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>
#include <editor/editor.h>

void ProfilerMenu::Init()
{
}

void ProfilerMenu::Draw()
{
	UpdateFpsCounter();
	const std::string windowName = "Debug###Debug" + std::to_string(id);
	const bool visible = ImGui::Begin(windowName.c_str(), &isActive, ImGuiWindowFlags_NoCollapse);
	if (visible)
	{
		OnStartDrawing();

		const std::string fpsText = "FPS: " + std::to_string(lastFps) + "###FPS_COUNTER";
		ImGui::PlotLines(fpsText.c_str(), fpsHistory, FPS_HISTORY_SIZE, 0, "", 0);

		ImGui::Text("FPS average: %0.2f, average frame time %0.2fms", fpsAVG, (1 / fpsAVG) * 1000);

		ImGui::Text("DrawCalls Count: %d", Performance::GetDrawCallCount());
		ImGui::Text("Triangles Count: %d", Performance::GetDrawTrianglesCount());
		ImGui::Text("Materials update count: %d", Performance::GetUpdatedMaterialCount());

#if defined(DEBUG)
		DrawMemoryStats();
		DrawProfilerBenchmarks();
		DrawFilesList();
#endif

		const size_t callCount = Performance::scopProfilerList.size();
		float lineHeigh = 1;
		float lineSpace = 1;

		uint64_t offsetTime = Performance::scopProfilerList["Engine::Loop"][0].start;
		uint64_t endTime = Performance::scopProfilerList["Engine::Loop"][0].end;

		ImDrawList* draw_list = ImPlot::GetPlotDrawList();
		ImPlot::SetNextAxesLimits(0, (endTime - offsetTime) / 200.0f, 0, 7, ImPlotCond_Always);
		if (ImPlot::BeginPlot("Profiler", ImVec2(-1, 500)))
		{
			int i = 0;
			for (const auto& valCategory : Performance::scopProfilerList) 
			{
				if (ImPlot::BeginItem(valCategory.first.c_str()))
				{
					for (const auto& value : valCategory.second)
					{
						ImVec2 open_pos = ImPlot::PlotToPixels((value.start - offsetTime)/200.0f, i * lineSpace);
						ImVec2 close_pos = ImPlot::PlotToPixels((value.end - offsetTime)/200.0f, i * lineSpace + lineHeigh);
						draw_list->AddRectFilled(open_pos, close_pos, ImGui::GetColorU32(ImPlot::GetCurrentItem()->Color));
						i++;
					}
					ImPlot::EndItem();
				}
			}
			ImPlot::EndPlot();
		}


		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}

void ProfilerMenu::UpdateFpsCounter()
{
	const ImGuiIO& io = ImGui::GetIO();

	//Update timer to slowly update framerate
	nextFpsUpdate += Time::GetUnscaledDeltaTime();
	if (nextFpsUpdate >= 0.06f)
	{
		nextFpsUpdate = 0;
		lastFps = io.Framerate;
	}

	// If the counter history is empty, fill the counter with the current frame rate
	if (!counterInitialised)
	{
		counterInitialised = true;
		for (int i = 0; i < FPS_HISTORY_SIZE; i++)
		{
			fpsHistory[i] = io.Framerate;
		}
	}

	// Move history and make an average
	fpsAVG = 0;
	for (int i = 0; i < FPS_HISTORY_SIZE - 1; i++)
	{
		fpsAVG += fpsHistory[i];
		fpsHistory[i] = fpsHistory[i + 1];
	}
	fpsAVG /= FPS_HISTORY_SIZE - 1;
	fpsHistory[FPS_HISTORY_SIZE - 1] = lastFps;
}

void ProfilerMenu::DrawMemoryStats()
{
	if (ImGui::CollapsingHeader("Memory stats", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		MemoryTracker* goMem = Performance::gameObjectMemoryTracker;

		ImGui::Text("%s:", goMem->name.c_str());
		ImGui::Text("Current allocation: %d Bytes, Total: %d Bytes", goMem->allocatedMemory - goMem->deallocatedMemory, goMem->allocatedMemory);
		ImGui::Text("Current allocation: %f MegaBytes, Total: %f MegaBytes,", (goMem->allocatedMemory - goMem->deallocatedMemory) / 1000000.0f, goMem->allocatedMemory / 1000000.0f);
		ImGui::Text("Alloc count: %d, Delete count: %d", goMem->allocCount, goMem->deallocCount);

		MemoryTracker* meshDataMem = Performance::meshDataMemoryTracker;
		ImGui::Separator();
		ImGui::Text("%s:", meshDataMem->name.c_str());
		ImGui::Text("Current allocation: %d Bytes, Total: %d Bytes", meshDataMem->allocatedMemory - meshDataMem->deallocatedMemory, meshDataMem->allocatedMemory);
		ImGui::Text("Current allocation: %f MegaBytes, Total: %f MegaBytes,", (meshDataMem->allocatedMemory - meshDataMem->deallocatedMemory) / 1000000.0f, meshDataMem->allocatedMemory / 1000000.0f);
		ImGui::Text("Alloc count: %d, Delete count: %d", meshDataMem->allocCount, meshDataMem->deallocCount);

		MemoryTracker* textureMem = Performance::textureMemoryTracker;
		ImGui::Separator();
		ImGui::Text("%s:", textureMem->name.c_str());
		ImGui::Text("Current allocation: %d Bytes, Total: %d Bytes", textureMem->allocatedMemory - textureMem->deallocatedMemory, textureMem->allocatedMemory);
		ImGui::Text("Current allocation: %f MegaBytes, Total: %f MegaBytes,", (textureMem->allocatedMemory - textureMem->deallocatedMemory) / 1000000.0f, textureMem->allocatedMemory / 1000000.0f);
		ImGui::Text("Alloc count: %d, Delete count: %d", textureMem->allocCount, textureMem->deallocCount);
	}
}

void ProfilerMenu::DrawProfilerBenchmarks()
{
	if (ImGui::CollapsingHeader("Profiler", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		if (EngineSettings::values.useProfiler)
		{
			//Add profiler texts
			for (const auto& kv : Performance::profilerCategories)
			{
				std::string title = kv.first;
				if (kv.second->profilerList.count(kv.first) != 0)
				{
					title += ": " + std::to_string(kv.second->profilerList[kv.first]->GetValue()) + ", avg " + std::to_string(kv.second->profilerList[kv.first]->average);
				}
				title += "###" + kv.first;
				if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
				{
					for (const auto& kv2 : kv.second->profilerList)
					{
						if (kv2.first == kv.first)
							continue;
						ImGui::SetCursorPosX(20);
						ImGui::Text("[%s] AVG: %lld; %lld", kv2.first.c_str(), kv2.second->average, kv2.second->GetValue());
					}
				}
			}
		}
		else
		{
			ImGui::Text("Profiler disabled");
		}
	}
}

void ProfilerMenu::DrawFilesList()
{
	if (ImGui::CollapsingHeader("Files", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		for (int i = 0; i < AssetManager::GetFileReferenceCount(); i++)
		{
			const std::shared_ptr<FileReference> fileRef = AssetManager::GetFileReference(i);
			if (fileRef && !fileRef->IsLoaded())
			{
				ImGui::SetCursorPosX(20);
				if (fileRef->file)
				{
					ImGui::Text("File%lld isLoaded:%d useCount:%ld :", fileRef->fileId, fileRef->IsLoaded(), fileRef.use_count());
					ImGui::SameLine();
					ImGui::Text("%s%s", fileRef->file->GetFileName().c_str(), fileRef->file->GetFileExtension().c_str());
				}
				else
					ImGui::Text("Missing file isLoaded:%d type:%d useCount:%ld", fileRef->IsLoaded(), (int)fileRef->fileType, fileRef.use_count());
			}
		}
		ImGui::Text("-----------------------------------------------------");
		for (int i = 0; i < AssetManager::GetFileReferenceCount(); i++)
		{
			const std::shared_ptr<FileReference> fileRef = AssetManager::GetFileReference(i);
			if (fileRef && fileRef->IsLoaded())
			{
				ImGui::SetCursorPosX(20);
				if (fileRef->file)
				{
					ImGui::Text("File%lld isLoaded:%d useCount:%ld :", fileRef->fileId, fileRef->IsLoaded(), fileRef.use_count());
					ImGui::SameLine();
					ImGui::Text("%s%s", fileRef->file->GetFileName().c_str(), fileRef->file->GetFileExtension().c_str());
				}
				else
					ImGui::Text("missing file isLoaded:%d useCount:%ld", fileRef->IsLoaded(), fileRef.use_count());
			}
		}
	}
}