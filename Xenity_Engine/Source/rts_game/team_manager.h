#pragma once

#include "team_enum.h"
#include <vector>
#include "../engine/monobehaviour.h"

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
	void CreateTeam(TeamColor color);
	void ClearTeams();
	Team* GetTeamFromColor(TeamColor color);
	Team* GetTeamFromIndex(int index);

private:
	std::vector<Team*> teams;
};

