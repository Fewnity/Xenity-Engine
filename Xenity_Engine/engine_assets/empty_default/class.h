#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"

class {CLASSNAME} : public MonoBehaviour
{
public:
    {CLASSNAME}();

    void Start() override;
    void Update() override;

    std::unordered_map<std::string, ReflectionEntry> GetReflection() override;

private:
};
