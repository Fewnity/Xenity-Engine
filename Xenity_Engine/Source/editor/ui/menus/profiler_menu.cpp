#include "profiler_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include "../../../xenity_editor.h"

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

	for (int i = 0; i < FPS_HISTORY_SIZE - 1; i++)
	{
		fpsHistory[i] = fpsHistory[i + 1];
	}
	fpsHistory[FPS_HISTORY_SIZE - 1] = lastFps;
	ImGui::Begin("Debug");
	std::string fpsText = "FPS: " + std::to_string(lastFps) + "###FPS_COUNTER";
	ImGui::PlotLines(fpsText.c_str(), fpsHistory, FPS_HISTORY_SIZE, 0, "", 0);
	ImGui::Text("DrawCall Count: %d", Performance::GetDrawCallCount());
	ImGui::Text("Updated Materials: %d", Performance::GetUpdatedMaterialCount());

	if (ImGui::CollapsingHeader("Profiler", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{

		if (EngineSettings::useProfiler)
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
						ImGui::Text("%s: %ld, avg %ld", kv2.first.c_str(), kv2.second->GetValue(), kv2.second->average);
					}
				}
			}
		}
		else
		{
			ImGui::Text("Profiler disabled");
		}
	}

	if (ImGui::CollapsingHeader("Files", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		for (int i = 0; i < AssetManager::GetFileReferenceCount(); i++)
		{
			//std::string t = "";
			std::shared_ptr<FileReference> fileRef = AssetManager::GetFileReference(i);
			if (!fileRef->isLoaded)
			{
				ImGui::SetCursorPosX(20);
				if (fileRef->file)
					ImGui::Text("File%d isLoaded:%d useCount:%d : %s", fileRef->fileId, fileRef->isLoaded, fileRef.use_count(), fileRef->file->GetFileName().c_str());
				else
					ImGui::Text("missing file isLoaded:%d useCount:%d", fileRef->isLoaded, fileRef.use_count());
			}
		}
		ImGui::Text("-----------------------------------------------------");
		for (int i = 0; i < AssetManager::GetFileReferenceCount(); i++)
		{
			//std::string t = "";
			std::shared_ptr<FileReference> fileRef = AssetManager::GetFileReference(i);
			if (fileRef->isLoaded)
			{
				ImGui::SetCursorPosX(20);
				if (fileRef->file)
					ImGui::Text("File%d isLoaded:%d useCount:%d : %s", fileRef->fileId, fileRef->isLoaded, fileRef.use_count(), fileRef->file->GetFileName().c_str());
				else
					ImGui::Text("missing file isLoaded:%d useCount:%d", fileRef->isLoaded, fileRef.use_count());
			}
		}
	}
	ImGui::End();
}
