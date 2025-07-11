#include "{FILENAME}.h"
#include <xenity.h>

/**
* Constructor
*/
{CLASSNAME}::{CLASSNAME}()
{
}

/**
* Called once when the script is enabled
*/
void {CLASSNAME}::Start()
{
}

/**
* Called every frame
*/
void {CLASSNAME}::Update()
{
}

/**
* Lists all variables to show in inspector
*/
ReflectiveData {CLASSNAME}::GetReflectiveData()
{
    BEGIN_REFLECTION();
    // ADD_VARIABLE(variableName);
    END_REFLECTION();
}
