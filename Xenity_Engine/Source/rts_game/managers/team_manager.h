#pragma once

#include "../team_enum.h"
#include <vector>
#include "../../engine/monobehaviour.h"

class Unit;

class Team 
{
public:
	TeamColor color;
	std::vector<Unit*> units;
};

class TeamManager : public MonoBehaviour
{
public:
	void CreateTeam(TeamColor color, bool isLocalPlayer);
	void ClearTeams();
	Team* GetTeamFromColor(TeamColor color);
	Team* GetTeamFromIndex(int index);
	Team* localPlayerTeam = nullptr;

private:
	std::vector<Team*> teams;
};

