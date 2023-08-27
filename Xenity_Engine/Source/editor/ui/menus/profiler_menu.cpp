#include "profiler_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"

void ProfilerMenu::Init()
{
}

#define FPS_HISTORY_SIZE 400
float fpsHistory[FPS_HISTORY_SIZE] = { 0 };

void ProfilerMenu::Draw()
{
	ImGuiIO& io = ImGui::GetIO();

	//Update timer to slowly update framerate
	nextFpsUpdate += Time::GetUnscaledDeltaTime();
	if (nextFpsUpdate >= 0.06f)
	{
		nextFpsUpdate = 0;
		lastFps = io.Framerate;
	}

	for (int i = 0; i < FPS_HISTORY_SIZE -1; i++)
	{
		fpsHistory[i] = fpsHistory[i + 1];
	}
	fpsHistory[FPS_HISTORY_SIZE - 1] = lastFps;
	ImGui::Begin("Debug");
	std::string fpsText = "FPS: " + std::to_string(lastFps) +"###FPS_COUNTER";
	ImGui::PlotLines(fpsText.c_str(), fpsHistory, FPS_HISTORY_SIZE, 0, "", 0);
	ImGui::Text("DrawCall Count: %d", Performance::GetDrawCallCount());
	ImGui::Text("Updated Materials: %d", Performance::GetUpdatedMaterialCount());

	if (EngineSettings::useProfiler)
	{
		//Add profiler texts
		for (const auto& kv : Performance::profilerList)
		{
			ImGui::Text("%s: %ld, avg %ld", kv.first.c_str(), kv.second->GetValue(), kv.second->average);
		}
	}
	else
	{
		ImGui::Text("Profiler disabled");
	}

	ImGui::End();
}
