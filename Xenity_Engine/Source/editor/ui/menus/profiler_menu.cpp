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

		DrawProfilerGraph();
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

void ProfilerMenu::DrawProfilerGraph()
{
	if (ImGui::CollapsingHeader("Profiler Graph", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		// If the profiler is not running, display a message
		if(Performance::scopProfilerList.empty())
		{
			ImGui::Text("No profiler data available");
			return;
		}

		if (ImGui::Button("Pause Profiler"))
		{
			isPaused = !isPaused;
		}

		float lineHeigh = 1;

		uint64_t offsetTime = lastStartTime;
		uint64_t endTime = lastEndTime;

		// If the profiler not paused, update the timeline items
		if (!isPaused)
		{
			lastMaxLevel = 0;
			offsetTime = Performance::scopProfilerList["Engine::Loop"][0].start;
			endTime = Performance::scopProfilerList["Engine::Loop"][0].end;

			CreateTimelineItems();

			lastStartTime = offsetTime;
			lastEndTime = endTime;
		}

		// Draw graph
		ImDrawList* draw_list = ImPlot::GetPlotDrawList();
		if (ImPlot::BeginPlot("Profiler", ImVec2(-1, 500)))
		{
			// Set the axis limits
			ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, (endTime - offsetTime));
			ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, lastMaxLevel + 1);

			const ImPlotPoint mousePoint = ImPlot::GetPlotMousePos();
			ImVec2 mousePixelPos = ImPlot::PlotToPixels(mousePoint.x, mousePoint.y);

			int hoveredItemIndex = -1;
			const size_t timelineItemsCount = timelineItems.size();
			for (size_t i = 0; i < timelineItemsCount; i++)
			{
				const TimelineItem& item = timelineItems[i];
				if (ImPlot::BeginItem(item.name.c_str()))
				{
					// Get the item position in pixels and draw it
					const ImVec2 open_pos = ImPlot::PlotToPixels((item.start - offsetTime), item.level * lineHeigh);
					const ImVec2 close_pos = ImPlot::PlotToPixels((item.end - offsetTime), item.level * lineHeigh + lineHeigh);
					draw_list->AddRectFilled(open_pos, close_pos, ImGui::GetColorU32(ImPlot::GetCurrentItem()->Color));

					// Check if the mouse is over the item
					if (mousePixelPos.x >= open_pos.x && mousePixelPos.x <= close_pos.x)
					{
						if (mousePixelPos.y >= close_pos.y && mousePixelPos.y <= open_pos.y)
						{
							hoveredItemIndex = i;
						}
					}

					ImPlot::EndItem();
				}
			}

			if (hoveredItemIndex != -1)
			{
				const TimelineItem hoveredItem = timelineItems[hoveredItemIndex];
				uint64_t itemTime = hoveredItem.end - hoveredItem.start;

				const float oldMouseX = mousePixelPos.x;
				const float oldMouseY = mousePixelPos.y;

				// Draw hovered item name
				{
					std::string mouseText = hoveredItem.name;
					ImVec2 textSize = ImGui::CalcTextSize(mouseText.c_str());
					mousePixelPos.y -= textSize.y * 3;
					mousePixelPos.x -= textSize.x / 2.0f;
					draw_list->AddRectFilled(ImVec2(mousePixelPos.x, mousePixelPos.y), ImVec2(mousePixelPos.x + textSize.x, mousePixelPos.y + textSize.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 0.6f)));
					draw_list->AddText(mousePixelPos, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), mouseText.c_str());
					mousePixelPos.x = oldMouseX;
					mousePixelPos.y = oldMouseY;
					mousePixelPos.y -= textSize.y * 2;
				}

				// Draw time text
				{
					std::string mouseText = std::to_string(itemTime) + " microseconds";
					ImVec2 textSize = ImGui::CalcTextSize(mouseText.c_str());
					mousePixelPos.x -= textSize.x / 2.0f;
					draw_list->AddRectFilled(ImVec2(mousePixelPos.x, mousePixelPos.y), ImVec2(mousePixelPos.x + textSize.x, mousePixelPos.y + textSize.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 0.6f)));
					draw_list->AddText(mousePixelPos, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), mouseText.c_str());
					mousePixelPos.x = oldMouseX;
					mousePixelPos.y = oldMouseY;
					mousePixelPos.y -= textSize.y * 1;
				}

				// Draw percentage text
				{
					std::string mouseText = std::to_string((static_cast<float>(hoveredItem.end - hoveredItem.start) / static_cast<float>(endTime - offsetTime)) * 100);
					mouseText = mouseText.substr(0, mouseText.find('.') + 3) + "%";
					ImVec2 textSize = ImGui::CalcTextSize(mouseText.c_str());
					mousePixelPos.x -= textSize.x / 2.0f;
					draw_list->AddRectFilled(ImVec2(mousePixelPos.x, mousePixelPos.y), ImVec2(mousePixelPos.x + textSize.x, mousePixelPos.y + textSize.y), ImGui::GetColorU32(ImVec4(0, 0, 0, 0.6f)));
					draw_list->AddText(mousePixelPos, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), mouseText.c_str());
				}
			}

			ImPlot::EndPlot();
		}
	}
}

void ProfilerMenu::CreateTimelineItems()
{
	timelineItems.clear();
	for (const auto& valCategory : Performance::scopProfilerList)
	{
		for (const auto& value : valCategory.second)
		{
			TimelineItem item;
			item.name = valCategory.first;
			item.start = value.start;
			item.end = value.end;
			item.level = value.level;
			if (lastMaxLevel < value.level)
				lastMaxLevel = value.level;
			timelineItems.push_back(item);
		}
	}
}
