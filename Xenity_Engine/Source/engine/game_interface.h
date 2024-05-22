#pragma once

/**
* @brief Interface used to interactor with the compiled game class
*/
class GameInterface
{
public:
	virtual ~GameInterface() = default;

	/**
	* @brief Start game by registering game's monobehaviours
	*/
	virtual void Start() = 0;
};