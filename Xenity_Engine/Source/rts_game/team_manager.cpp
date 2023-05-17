#include "team_manager.h"

void TeamManager::CreateTeam(TeamColor color, bool isLocalPlayer)
{
	Team* newTeam = new Team();
	newTeam->color = color;
	teams.push_back(newTeam);
	if (isLocalPlayer)
	{
		localPlayerTeam = newTeam;
	}
}

void TeamManager::ClearTeams()
{
	int teamCount = teams.size();
	for (int i = 0; i < teamCount; i++)
	{
		delete teams[i];
	}
	teams.clear();
}

Team* TeamManager::GetTeamFromColor(TeamColor color)
{
	Team* team = nullptr;
	int teamCount = teams.size();
	for (int i = 0; i < teamCount; i++)
	{
		if (teams[i]->color == color)
		{
			team = teams[i];
			break;
		}
	}
	return team;
}

Team* TeamManager::GetTeamFromIndex(int index)
{
	return teams[index];
}
