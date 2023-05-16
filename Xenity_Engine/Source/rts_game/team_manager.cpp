#include "team_manager.h"

void TeamManager::CreateTeam(TeamColor color)
{
    Team* newTeam = new Team();
    newTeam->color = color;
    teams.push_back(newTeam);
}

void TeamManager::ClearTeams()
{
}

Team* TeamManager::GetTeamFromColor(TeamColor color)
{
    return nullptr;
}

Team* TeamManager::GetTeamFromIndex(int index)
{
    return nullptr;
}
