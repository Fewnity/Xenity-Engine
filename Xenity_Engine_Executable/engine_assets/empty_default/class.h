#pragma once

#include <engine/component.h>

class {CLASSNAME} : public Component
{
public:
    {CLASSNAME}();

    void Start() override;
    void Update() override;

    ReflectiveData GetReflectiveData() override;

private:
};
