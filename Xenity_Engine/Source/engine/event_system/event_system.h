#pragma once

#include <engine/api.h>
#include <functional>
#include <vector>
#include <stdint.h>

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
	* 
	* @param function: Pointer to the function to bind
	*/
	void Bind(void(*function)(Args...))
	{
		if (!function)
			return;

		// Get function address
		const uint64_t functionAddress = *((uint64_t*)&function);

		// Create function and add it to the list
		const std::function<void(Args...)> callableFunction = CreateBindHelper(function, std::index_sequence_for<Args...>{});
		AddFunction(functionAddress, 0, callableFunction);
	}

	/**
	* @brief Bind a function linked to an object
	* @brief |
	* @brief Example:
	* @brief Bind(&MyClass::MyFunction, ptrToMyObject)
	* 
	* @param function: Pointer to the function to bind
	* @param obj: Pointer to the object
	*/
	template<typename ObjType>
	void Bind(void(ObjType::* function)(Args...), ObjType* obj)
	{
		if (!function || !obj)
			return;

		// Get object and function addresses
		const uint64_t objectAddress = (uint64_t)obj;
		const uint64_t functionAddress = *((uint64_t*)&function);

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
		if (!function)
			return;

		// Get function address
		const uint64_t functionAddress = *((uint64_t*)&function);

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
		if (!function || !obj)
			return;

		// Get object and function addresses
		const uint64_t objectAddress = (uint64_t)obj;
		const uint64_t functionAddress = *((uint64_t*)&function);

		RemoveFunction(functionAddress, objectAddress);
	}

	/**
	* @brief Unbind all binded function
	*/
	void UnbindAll() 
	{
		functionCount = 0;
		functionsInfosList.clear();
	}

	/**
	* @brief Get the number of listener
	*/
	int GetBindedFunctionCount()
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
	void AddFunction(const uint64_t functionAddress, const uint64_t objectAddress, const std::function<void(Args...)> callableFunction)
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
	void RemoveFunction(const uint64_t functionAddress, const uint64_t objectAddress)
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
	int FindExistingFunction(const uint64_t functionAddress, const uint64_t objectAddress)
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
		uint64_t objectAddress = 0;
		uint64_t funcAddress = 0;
		std::function<void(Args...)> function;
	};

	int functionCount = 0;
	std::vector<BindedFunctionInfo> functionsInfosList;
};