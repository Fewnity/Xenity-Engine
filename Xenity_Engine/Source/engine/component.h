#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <memory>
#include "reflection/reflection.h"
#include "unique_id/unique_id.h"

class GameObject;
class Transform;

class API Component : public UniqueId, public Reflection, public std::enable_shared_from_this<Component>
{
public:
	Component();

	virtual ~Component();
	
	/**
	* Function called once before Start() at the creation of the component
	*/
	virtual void Awake() {}
	
	/**
	* Function called once after Awake() at the creation of the component
	*/
	virtual void Start() {}

	/**
	* Function called every frame
	*/
	virtual void Update() {}
	//virtual void OnParentChanged() {}

	int updatePriority = 5000; //Lower is more priotary
	
	/**
	* Get if the component is enabled
	*/
	bool GetIsEnabled();
	
	/**
	* Enable or disable the component
	*/
	void SetIsEnabled(bool isEnabled);
	bool initiated = false;

	bool waitingForDestroy = false;

	/**
	* Set component's GameObject
	*/
	void SetGameObject(std::weak_ptr<GameObject> go);

	/**
	* Get component's GameObject
	*/
	std::shared_ptr <GameObject> GetGameObject() const
	{
		return gameObject.lock();
	}

	/**
	* Get component's Transform
	*/
	std::shared_ptr <Transform> GetTransform() const
	{
		return transform.lock();
	}

	/**
	* Get component's name
	*/
	std::string GetComponentName() 
	{
		return componentName;
	}

protected:
	std::string componentName = "";

private:
	std::weak_ptr <GameObject>gameObject;
	std::weak_ptr <Transform> transform;
	bool isEnabled = true;
};