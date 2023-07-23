#include "profiler_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"

void ProfilerMenu::Init()
{
}

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

	ImGui::Begin("Debug");
	ImGui::Text("FPS: %.1f", lastFps);
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
