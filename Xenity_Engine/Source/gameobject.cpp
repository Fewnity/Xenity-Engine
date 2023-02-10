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

/**
* TODO : Move in a math class
*/
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

/// <summary>
/// Set gameobject's children world positions
/// </summary>
void GameObject::SetChildrenWorldPositions()
{
	double Deg2Rad = 0.01745329251; //M_PI / 180.0f
	int childCount = children.size();
	Vector3 radAngles = Vector3();
	radAngles.x = Deg2Rad * -transform.GetRotation().x;
	radAngles.y = Deg2Rad * -transform.GetRotation().y;
	radAngles.z = Deg2Rad * -transform.GetRotation().z;

	double cosX = cos(radAngles.x);
	double sinX = sin(radAngles.x);
	double cosY = cos(radAngles.y);
	double sinY = sin(radAngles.y);
	double cosZ = cos(radAngles.z);
	double sinZ = sin(radAngles.z);

	//Create X, Y and Z matrices
	double rotX[9] = { 1, 0, 0, 0, cosX, -sinX, 0, sinX, cosX };
	double rotY[9] = { cosY, 0,sinY, 0, 1, 0, -sinY, 0, cosY };
	double rotZ[9] = { cosZ, -sinZ, 0, sinZ, cosZ, 0, 0, 0, 1 };

	//Multiply Z with X and with Y (there is a temp matrix because of the multiplication in two steps)
	double tempRotationM[9];
	double rotationM[9];
	MultiplyMatrix(rotZ, rotX, tempRotationM, 3, 3, 3, 3);
	MultiplyMatrix(tempRotationM, rotY, rotationM, 3, 3, 3, 3);

	//For each children
	for (int i = 0; i < childCount; i++)
	{
		GameObject* child = children[i];

		//Get child local position
		double localPos[3] = { child->transform.GetLocalPosition().x, child->transform.GetLocalPosition().y, child->transform.GetLocalPosition().z };
		//Create the matrix which store the new child's world position (wihtout parent's world position added)
		double posAfterRotation[3];
		MultiplyMatrix(localPos, rotationM, posAfterRotation, 1, 3, 3, 3);
		//Set new child position (with parent's world position added)
		child->transform.SetPosition(Vector3(posAfterRotation[0] + transform.GetPosition().x, posAfterRotation[1] + transform.GetPosition().y, posAfterRotation[2] + transform.GetPosition().z));

		Vector3 newRotation;
		newRotation.x = transform.GetRotation().x + child->transform.GetLocalRotation().x;
		newRotation.y = transform.GetRotation().y + child->transform.GetLocalRotation().y;
		newRotation.z = transform.GetRotation().z + child->transform.GetLocalRotation().z;

		child->transform.SetRotation(newRotation);

		//Update other child's children positions
		child->SetChildrenWorldPositions();
	}
}


/// <summary>
/// Add a child the the gameobject
/// </summary>
/// <param name="gameObject"></param>
void GameObject::AddChild(GameObject* newChild)
{
	//Check if the child to add is alrady a child of this gameobject
	bool add = true;
	int childCount = children.size();
	for (int i = 0; i < childCount; i++)
	{
		if (children[i] == newChild)
		{
			add = false;
			break;
		}
	}

	if (add)
	{
		children.push_back(newChild);
		newChild->parent = this;
	}
}

/// <summary>
/// Add an existing component
/// </summary>
/// <param name="component"></param>
void GameObject::AddExistingComponent(Component* componentToAdd)
{
	//Check if the component to add is alrady a component of this gameobject
	bool add = true;
	int componentCount = components.size();
	for (int i = 0; i < componentCount; i++)
	{
		if (components[i] == componentToAdd)
		{
			add = false;
			break;
		}
	}

	if (add)
	{
		components.push_back(componentToAdd);
		componentToAdd->gameObject = this;
	}
}

/// <summary>
/// Find all gameobjects with a specific name
/// </summary>
/// <param name="name">GameObjects's name</param>
/// <returns></returns>
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

/// <summary>
/// Find a gameobject with a specific name
/// </summary>
/// <param name="name">GameObject's name</param>
/// <returns>GameObject pointer or nullptr if no one is found</returns>
GameObject* GameObject::FindGameObjectByName(std::string name)
{
	std::vector<GameObject*> gameObjects = Engine::GetGameObjects();
	int gameObjectCount = gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->name == name)
			return gameObjects[i];
	}
	return nullptr;
}

bool GameObject::GetActive()
{
	return active;
}

bool GameObject::GetLocalActive()
{
	return localActive;
}

void GameObject::SetActive(bool active)
{
	if (active != this->active)
	{
		this->active = active;
		UpdateActive(this);
	}
}

/// <summary>
/// Update gameobject active state. Set the local active value depending of gameobject's parents active state
/// </summary>
/// <param name="changed"></param>
void GameObject::UpdateActive(GameObject* changed)
{
	bool lastLocalActive = localActive;
	if (!changed->GetActive()) //if the new parent's state is false, set local active to false
	{
		localActive = false;
	}
	else if(active)
	{
		bool newActive = true;
		GameObject* gmToCheck = parent;
		while (gmToCheck != nullptr)
		{
			if (!gmToCheck->GetActive()) //If a parent is disabled, set local active to false
			{
				newActive = false;
				break;
			}
			if (gmToCheck == changed)
			{
				break;
			}
			gmToCheck = gmToCheck->parent;
		}
		localActive = newActive;
	}

	//If the gameobject has changed his state
	if (lastLocalActive != localActive)
	{
		//Update children
		int childCount = children.size();
		for (int i = 0; i < childCount; i++)
		{
			children[i]->UpdateActive(changed);
		}
	}

}
