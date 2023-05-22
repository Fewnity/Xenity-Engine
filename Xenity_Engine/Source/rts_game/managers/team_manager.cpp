#include "team_manager.h"

/// <summary>
/// Create a team
/// </summary>
/// <param name="color"></param>
/// <param name="isLocalPlayer"></param>
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

/// <summary>
/// Clear all teams
/// </summary>
void TeamManager::ClearTeams()
{
	int teamCount = teams.size();
	for (int i = 0; i < teamCount; i++)
	{
		delete teams[i];
	}
	teams.clear();
}

/// <summary>
/// Get team from color
/// </summary>
/// <param name="color"></param>
/// <returns></returns>
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

/// <summary>
/// Get team from index
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
Team* TeamManager::GetTeamFromIndex(int index)
{
	return teams[index];
}
