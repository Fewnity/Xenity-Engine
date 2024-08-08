// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <functional>
#include <vector>
#include <stdint.h>

#include <engine/api.h>
#include <engine/assertions/assertions.h>

/**
* @brief Class used to bind functions to an event
* @brief |
* @brief Examples:
* @brief Event<> mySimpleEvent;
* @brief Event<int, float> myEventWithParams;
*/
template<typename... Args>
class Event
{
public:

	/**
	* @brief Destructor
	*/
	~Event() 
	{
		UnbindAll();
	}

	/**
	* @brief Call all binded functions
	* 
	* @param param: All parameters to send
	*/
	void Trigger(Args... param)
	{
		for (int i = 0; i < functionCount; i++)
		{
			functionsInfosList[i].function(param...);
		}
	}

	/**
	* @brief Bind a simple function
	* @brief |
	* @brief Example:
	* @brief Bind(&MyFunction);
	* @brief Bind(&MyClass::MyFunction); (static function)
	* 
	* @param function: Pointer to the function to bind
	*/
	void Bind(void(*function)(Args...))
	{
		XASSERT(function != nullptr, "[Event::Bind] function is nullptr");

		if (!function)
			return;

		// Get function address
		const size_t functionAddress = *((size_t*)&function);

		// Create function and add it to the list
		const std::function<void(Args...)> callableFunction = CreateBindHelper(function, std::index_sequence_for<Args...>{});
		AddFunction(functionAddress, 0, callableFunction);
	}

	/**
	* @brief Bind a function linked to an object
	* @brief |
	* @brief Example:
	* @brief Bind(&MyClass::MyFunction, ptrToMyObject) (non-static function)
	* 
	* @param function: Pointer to the function to bind
	* @param obj: Pointer to the object
	*/
	template<typename ObjType>
	void Bind(void(ObjType::* function)(Args...), ObjType* obj)
	{
		XASSERT(function != nullptr, "[Event::Bind] ObjType::function is nullptr");
		XASSERT(obj != nullptr, "[Event::Bind] obj is nullptr");

		if (!function || !obj)
			return;

		// Get object and function addresses
		const size_t objectAddress = (size_t)obj;
		const size_t functionAddress = *((size_t*)&function);

		// Create function and add it to the list
		const std::function<void(Args...)> callableFunction = CreateBindHelper(function, obj, std::index_sequence_for<Args...>{});
		AddFunction(functionAddress, objectAddress, callableFunction);
	}

	/**
	* @brief Unbind a simple function
	* 
	* @param function: Pointer to the function to unbind
	*/
	void Unbind(void(*function)(Args...))
	{
		XASSERT(function != nullptr, "[Event::Unbind] function is nullptr");

		if (!function)
			return;

		// Get function address
		const size_t functionAddress = *((size_t*)&function);

		RemoveFunction(functionAddress, 0);
	}

	/**
	* @brief Unbind a function linked to an object
	* 
	* @param function: Pointer to the function to unbind
	* @param obj: Pointer to the object
	*/
	template<typename ObjType>
	void Unbind(void(ObjType::* function)(Args...), ObjType* obj)
	{
		XASSERT(function != nullptr, "[Event::Unbind] ObjType::function is nullptr");
		XASSERT(obj != nullptr, "[Event::Unbind] obj is nullptr");

		if (!function || !obj)
			return;

		// Get object and function addresses
		const size_t objectAddress = (size_t)obj;
		const size_t functionAddress = *((size_t*)&function);

		RemoveFunction(functionAddress, objectAddress);
	}

	/**
	* @brief Unbind all binded function
	*/
	inline void UnbindAll()
	{
		functionCount = 0;
		functionsInfosList.clear();
	}

	/**
	* @brief Get the number of listener
	*/
	inline int GetBindedFunctionCount()
	{
		return functionCount;
	}

private:

	/**
	* @brief Add a function in the list
	* 
	* @param functionAddress: Address of the function
	* @param objectAddress: Address of the object (0 if no object is used)
	* @param callableFunction: Address of the function
	*/
	void AddFunction(const size_t functionAddress, const size_t objectAddress, const std::function<void(Args...)>& callableFunction)
	{
		const int funcIndex = FindExistingFunction(functionAddress, objectAddress);
		if (funcIndex == -1)
		{
			BindedFunctionInfo newFunc;
			newFunc.function = callableFunction;
			newFunc.funcAddress = functionAddress;
			newFunc.objectAddress = objectAddress;

			functionsInfosList.push_back(newFunc);
			functionCount++;
		}
	}

	/**
	* @brief Remove a function from the list
	* 
	* @param functionAddress: Address of the function
	* @param objectAddress: Address of the object (0 if no object is used)
	*/
	void RemoveFunction(const size_t functionAddress, const size_t objectAddress)
	{
		const int funcIndex = FindExistingFunction(functionAddress, objectAddress);
		if (funcIndex != -1)
		{
			functionCount--;
			functionsInfosList.erase(functionsInfosList.begin() + funcIndex);
		}
	}

	/**
	* @brief Find if a function is already in the list
	*
	* @param functionAddress: Address of the function
	* @param objectAddress: Address of the object (0 if no object is used)
	* 
	* @return index of the function in the list, -1 if not found
	*/
	int FindExistingFunction(const size_t functionAddress, const size_t objectAddress)
	{
		for (int i = 0; i < functionCount; i++)
		{
			const BindedFunctionInfo& info = functionsInfosList[i];
			if (functionAddress == info.funcAddress && objectAddress == info.objectAddress)
			{
				return i;
			}
		}
		return -1;
	}

	/**
	* @brief Helper to create a std::function with std::bind, this function automaticaly adds placeholder
	* 
	* @param function: Pointer to the function to bind
	* @param obj: Pointer to the object
	* 
	* @return std::function
	*/
	template<typename ObjType, std::size_t... Is>
	std::function<void(Args...)> CreateBindHelper(void(ObjType::* function)(Args...), ObjType* obj, const std::index_sequence<Is...>)
	{
		XASSERT(function != nullptr, "[Event::CreateBindHelper] ObjType::function is nullptr");
		XASSERT(obj != nullptr, "[Event::CreateBindHelper] ObjType::function is nullptr");

		// Add the right number of placeholders
#if defined(__GNUC__)
		return std::bind(function, obj, std::_Placeholder<Is + 1>{}...);
#else // For MSVC
		return std::bind(function, obj, std::_Ph<Is + 1>{}...);
#endif
	}

	/**
	* @brief Helper to create a std::function with std::bind, this function automaticaly adds placeholder
	*
	* @param function: Pointer to the function to bind
	*
	* @return std::function
	*/
	template<std::size_t... Is>
	std::function<void(Args...)> CreateBindHelper(void(*function)(Args...), const std::index_sequence<Is...>)
	{
		XASSERT(function != nullptr, "[Event::CreateBindHelper] function is nullptr");

		// Add the right number of placeholders
#if defined(__GNUC__)
		return std::bind(function, std::_Placeholder<Is + 1>{}...);
#else // For MSVC
		return std::bind(function, std::_Ph<Is + 1>{}...);
#endif
	}

	/**
	* Store data about the listener
	*/
	struct BindedFunctionInfo
	{
		size_t objectAddress = 0;
		size_t funcAddress = 0;
		std::function<void(Args...)> function;
	};

	std::vector<BindedFunctionInfo> functionsInfosList;
	int functionCount = 0;
};