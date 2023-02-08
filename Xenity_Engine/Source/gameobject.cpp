#include "gameobject.h"
#include "engine.h"
#include <iostream>

GameObject::GameObject()
{
	Engine::AddGameObject(this);
}

GameObject::GameObject(std::string name)
{
	this->name = name;
	Engine::AddGameObject(this);
}

GameObject::~GameObject()
{
	int componentsCount = components.size();
	for (int i = 0; i < componentsCount; i++)
	{
		delete components[i];
	}
	components.clear();
}

void MultiplyMatrix(const double* A, const double* B, double* result, int rA, int cA, int rB, int cB)
{
	if (cA != rB)
	{
		return;
	}

	double temp = 0;

	for (int i = 0; i < rA; i++)
	{
		for (int j = 0; j < cB; j++)
		{
			temp = 0;
			for (int k = 0; k < cA; k++)
			{
				temp += A[i * cA + k] * B[k * cB + j];
			}
			result[i * cB + j] = temp;
		}
	}
}

void GameObject::SetChildsWorldPositions() 
{
	int childCount = childs.size();
	for (int i = 0; i < childCount; i++)
	{
		double Deg2Rad = 0.01745329251; //M_PI / 180.0f
		Vector3 radAngles = Vector3();
		radAngles.x = Deg2Rad * -transform.GetRotation().x;
		radAngles.y = Deg2Rad * -transform.GetRotation().y;
		radAngles.z = Deg2Rad * -transform.GetRotation().z;

		double localPos[3] = { childs[i]->transform.GetLocalPosition().x, childs[i]->transform.GetLocalPosition().y, childs[i]->transform.GetLocalPosition().z};
		double rotX[9] = { 1, 0, 0, 0, cos(radAngles.x), -sin(radAngles.x), 0, sin(radAngles.x), cos(radAngles.x) };
		double rotY[9] = { cos(radAngles.y), 0, sin(radAngles.y), 0, 1, 0, -sin(radAngles.y), 0, cos(radAngles.y) };
		double rotZ[9] = { cos(radAngles.z), -sin(radAngles.z), 0, sin(radAngles.z), cos(radAngles.z), 0, 0, 0, 1 };

		double tempRotationM[9];
		double rotationM[9];
		MultiplyMatrix(rotZ, rotX, tempRotationM, 3, 3, 3, 3);
		MultiplyMatrix(tempRotationM, rotY, rotationM, 3, 3, 3, 3);

		double posAfterRotation[3];
		MultiplyMatrix(localPos, rotationM, posAfterRotation, 1,3, 3, 3);

		childs[i]->transform.SetPosition(Vector3(posAfterRotation[0] + transform.GetPosition().x, posAfterRotation[1] + transform.GetPosition().y, posAfterRotation[2] + transform.GetPosition().z));
		Vector3 newRotation;
		newRotation.x = transform.GetRotation().x + childs[i]->transform.GetLocalRotation().x;
		newRotation.y = transform.GetRotation().y + childs[i]->transform.GetLocalRotation().y;
		newRotation.z = transform.GetRotation().z + childs[i]->transform.GetLocalRotation().z;

		childs[i]->transform.SetRotation(newRotation);
	}
}



void GameObject::AddChild(GameObject* gameObject)
{
	bool add = true;
	int childCount = childs.size();
	for (int i = 0; i < childCount; i++)
	{
		if (childs[i] == gameObject)
		{
			add = false;
			break;
		}
	}

	if (add)
		childs.push_back(gameObject);
}

void GameObject::AddComponent(Component* component)
{
	bool add = true;
	int componentCount = components.size();
	for (int i = 0; i < componentCount; i++)
	{
		if (components[i] == component)
		{
			add = false;
			break;
		}
	}

	if (add) {
		components.push_back(component);
		component->gameObject = this;
	}
}

std::vector<GameObject*> GameObject::FindGameObjectsByName(std::string name)
{
	std::vector<GameObject*> foundGameObjects;

	Engine::GetGameObjects().clear();
	std::vector<GameObject*> gameObjects = Engine::GetGameObjects();

	int gameObjectCount = gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->name == name)
			foundGameObjects.push_back(gameObjects[i]);
	}
	return foundGameObjects;
}

GameObject* GameObject::FindGameObjectByName(std::string name)
{
	Engine::GetGameObjects().clear();
	std::vector<GameObject*> gameObjects = Engine::GetGameObjects();
	int gameObjectCount = gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->name == name)
			return gameObjects[i];
	}
	return nullptr;
}