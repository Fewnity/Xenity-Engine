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
#include <engine/debug/memory_info.h>
#include <editor/ui/editor_ui.h>

void ProfilerMenu::Init()
{
}

void ProfilerMenu::Draw()
{
	UpdateFpsCounter();
	UpdateMemoryCounter();
	const std::string windowName = "Profiling###Profiling" + std::to_string(id);
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

		DrawMemoryStats();
#if defined(DEBUG)
		DrawProfilerBenchmarks();
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

void ProfilerMenu::UpdateMemoryCounter()
{
	for (int i = 0; i < USED_MEMORY_HISTORY_SIZE - 1; i++)
	{
		usedMemoryHistory[i] = usedMemoryHistory[i + 1];
	}
	usedMemoryHistory[USED_MEMORY_HISTORY_SIZE - 1] = MemoryInfo::GetUsedMemory() / 1000;

	for (int i = 0; i < USED_VIDE_MEMORY_HISTORY_SIZE - 1; i++)
	{
		usedVideoMemoryHistory[i] = usedVideoMemoryHistory[i + 1];
	}
	usedVideoMemoryHistory[USED_VIDE_MEMORY_HISTORY_SIZE - 1] = MemoryInfo::GetUsedVideoMemory() / 1000;
}

void ProfilerMenu::DrawMemoryStats()
{
	if (ImGui::CollapsingHeader("Memory stats", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		ImGui::Text("Used memory:");
		const std::string usedMemoryText = std::to_string((size_t)usedMemoryHistory[USED_MEMORY_HISTORY_SIZE - 1]) + " KiloBytes" + "###USED_MEMORY_COUNTER";
		ImGui::PlotLines(usedMemoryText.c_str(), usedMemoryHistory, USED_MEMORY_HISTORY_SIZE, 0, "", 0);

		ImGui::Separator();

		ImGui::Text("Used video memory:");
		const std::string usedVideoMemoryText = std::to_string((size_t)usedVideoMemoryHistory[USED_VIDE_MEMORY_HISTORY_SIZE - 1]) + " KiloBytes" + "###USED_MEMORY_COUNTER";
		ImGui::PlotLines(usedVideoMemoryText.c_str(), usedVideoMemoryHistory, USED_VIDE_MEMORY_HISTORY_SIZE, 0, "", 0);

#if defined(DEBUG)
		ImGui::Separator();

		const MemoryTracker* goMem = Performance::s_gameObjectMemoryTracker;

		ImGui::Text("%s:", goMem->m_name.c_str());
		ImGui::Text("Current allocation: %d Bytes, Total: %d Bytes", goMem->m_allocatedMemory - goMem->m_deallocatedMemory, goMem->m_allocatedMemory);
		ImGui::Text("Current allocation: %f MegaBytes, Total: %f MegaBytes,", (goMem->m_allocatedMemory - goMem->m_deallocatedMemory) / 1000000.0f, goMem->m_allocatedMemory / 1000000.0f);
		ImGui::Text("Alloc count: %d, Delete count: %d", goMem->m_allocCount, goMem->m_deallocCount);

		const MemoryTracker* meshDataMem = Performance::s_meshDataMemoryTracker;
		ImGui::Separator();
		ImGui::Text("%s:", meshDataMem->m_name.c_str());
		ImGui::Text("Current allocation: %d Bytes, Total: %d Bytes", meshDataMem->m_allocatedMemory - meshDataMem->m_deallocatedMemory, meshDataMem->m_allocatedMemory);
		ImGui::Text("Current allocation: %f MegaBytes, Total: %f MegaBytes,", (meshDataMem->m_allocatedMemory - meshDataMem->m_deallocatedMemory) / 1000000.0f, meshDataMem->m_allocatedMemory / 1000000.0f);
		ImGui::Text("Alloc count: %d, Delete count: %d", meshDataMem->m_allocCount, meshDataMem->m_deallocCount);

		const MemoryTracker* textureMem = Performance::s_textureMemoryTracker;
		ImGui::Separator();
		ImGui::Text("%s:", textureMem->m_name.c_str());
		ImGui::Text("Current allocation: %d Bytes, Total: %d Bytes", textureMem->m_allocatedMemory - textureMem->m_deallocatedMemory, textureMem->m_allocatedMemory);
		ImGui::Text("Current allocation: %f MegaBytes, Total: %f MegaBytes,", (textureMem->m_allocatedMemory - textureMem->m_deallocatedMemory) / 1000000.0f, textureMem->m_allocatedMemory / 1000000.0f);
		ImGui::Text("Alloc count: %d, Delete count: %d", textureMem->m_allocCount, textureMem->m_deallocCount);
#endif
	}
}

void ProfilerMenu::DrawProfilerBenchmarks()
{
	if (ImGui::CollapsingHeader("Profiler", ImGuiTreeNodeFlags_Framed))
	{
		if (EngineSettings::values.useProfiler)
		{
			//Add profiler texts
			for (const auto& kv : Performance::s_profilerCategories)
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

void ProfilerMenu::DrawProfilerGraph()
{
	const std::string pauseText = isPaused ? "Resume Profilers" : "Pause Profilers";
	if (ImGui::Button(pauseText.c_str()))
	{
		isPaused = !isPaused;
		Performance::s_isPaused = isPaused;
	}

	uint64_t offsetTime = lastStartTime;
	uint64_t endTime = lastEndTime;
	bool needUpdate = true;

	if (isPaused)
	{
		Performance::s_currentProfilerFrame = selectedProfilingRow;
	}
	else
	{
		selectedProfilingRow = Performance::s_currentProfilerFrame;
	}

	auto UpdateProfilers = [this, &offsetTime, &endTime, &needUpdate]()
		{
			if (needUpdate)
			{
				needUpdate = false;
				uint64_t engineLoopKey = 0;
				for (const auto& profilerNamesKV : Performance::s_scopProfilerNames)
				{
					if (profilerNamesKV.second == "Engine::Loop")
					{
						engineLoopKey = profilerNamesKV.first;
						break;
					}
				}
				offsetTime = Performance::s_scopProfilerList[Performance::s_currentProfilerFrame].timerResults[engineLoopKey][0].start;
				endTime = Performance::s_scopProfilerList[Performance::s_currentProfilerFrame].timerResults[engineLoopKey][0].end;

				CreateTimelineItems();

				lastStartTime = offsetTime;
				lastEndTime = endTime;
			}
		};

	if (ImGui::Button("Load profiler record file"))
	{
		std::string filePath = EditorUI::OpenFileDialog("Select record file", "");
		if (!filePath.empty())
		{
			Performance::LoadFromBinary(filePath);
			UpdateProfilers();
			isPaused = true;
			Performance::s_isPaused = isPaused;
		}
	}


	if (Performance::s_scopProfilerList.empty())
	{
		ImGui::Text("No profiler data available");
	}
	else
	{
		//if (!isPaused)
		{
			UpdateProfilers();
		}

		static ImGuiTableFlags profilerDumpListTableflags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersH | ImGuiTableFlags_ScrollY;
		if (ImGui::BeginTable("ProfilerDumpTable", 2, profilerDumpListTableflags, ImVec2(0, 200)))
		{
			ImGui::TableSetupColumn("id", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("duration", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupScrollFreeze(0,1);
			ImGui::TableHeadersRow();

			uint32_t i = 0;
			for (const auto& profilerLine : Performance::s_scopProfilerList)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				const std::string idStr = std::to_string(profilerLine.frameId);
				if (ImGui::Selectable(idStr.c_str(), selectedProfilingRow == i))
				{
					selectedProfilingRow = i;
					isPaused = true;
				}

				ImGui::TableSetColumnIndex(1);
				const std::string frameDurationStr = std::to_string(profilerLine.frameDuration);
				if (ImGui::Selectable(frameDurationStr.c_str(), selectedProfilingRow == i))
				{
					selectedProfilingRow = i;
					isPaused = true;
				}

				i++;
			}
			ImGui::EndTable();
		}

		if (ImGui::CollapsingHeader("Basic Profiler", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
		{
			static ImGuiTableFlags basicProfilerTableflags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersH | ImGuiTableFlags_ScrollY;
			if (ImGui::BeginTable("BasicProfilerTable", 4, basicProfilerTableflags, ImVec2(0, 300)))
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Total time", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Engine time", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Call count in frame", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();

				for (const auto& profilerLine : classicProfilerItems)
				{
					ImGui::TableNextRow();
					const uint64_t totalEngineTime = endTime - offsetTime;
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s", profilerLine.name.c_str());
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%d microseconds", profilerLine.totalTime);
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%.2f%%", ((double)profilerLine.totalTime / (double)totalEngineTime) * 100);
					ImGui::TableSetColumnIndex(3);
					ImGui::Text("%d", profilerLine.callCountInFrame);
				}

				ImGui::EndTable();
			}
		}
	}

	if (ImGui::CollapsingHeader("Profiler Graph", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		// If the profiler is not running, display a message
		if (Performance::s_scopProfilerList.empty())
		{
			ImGui::Text("No profiler data available");
		}
		else
		{
			float lineHeigh = 1;

			// If the profiler not paused, update the timeline items
			//if (!isPaused)
			{
				UpdateProfilers();
			}

			// Draw graph
			ImDrawList* draw_list = ImPlot::GetPlotDrawList();
			if (ImPlot::BeginPlot("Profiler", ImVec2(-1, 500)))
			{
				ImPlot::SetupLegend(ImPlotLocation_West, ImPlotLegendFlags_Outside);

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
						const std::string& mouseText = hoveredItem.name;
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
}

void ProfilerMenu::CreateTimelineItems()
{
	timelineItems.clear();
	classicProfilerItems.clear();
	lastMaxLevel = 0;
	for (const auto& valCategory : Performance::s_scopProfilerList[Performance::s_currentProfilerFrame].timerResults)
	{
		ClassicProfilerItem& classicProfilerItem = classicProfilerItems.emplace_back(Performance::s_scopProfilerNames[valCategory.first]);

		for (const auto& value : valCategory.second)
		{
			classicProfilerItem.totalTime += value.end - value.start;
			classicProfilerItem.callCountInFrame++;

			TimelineItem item(Performance::s_scopProfilerNames[valCategory.first]);
			item.start = value.start;
			item.end = value.end;
			item.level = value.level;
			if (lastMaxLevel < value.level)
			{
				lastMaxLevel = value.level;
			}
			timelineItems.push_back(item);
		}
	}
}
