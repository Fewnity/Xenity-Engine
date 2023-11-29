#pragma once

#include "../engine/monobehaviour.h"
#include "../engine/component.h"

class {CLASSNAME} : public MonoBehaviour
{
public:
    {CLASSNAME}();

    void Start() override;
    void Update() override;

    ReflectiveData GetReflectiveData() override;

private:
};
