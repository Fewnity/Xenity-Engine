#include "scene.h"

Scene::Scene()
{
}

std::unordered_map<std::string, Variable> Scene::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables.insert_or_assign("fileId", fileId);
	return reflectedVariables;
}

std::unordered_map<std::string, Variable> Scene::GetMetaReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	return reflectedVariables;
}