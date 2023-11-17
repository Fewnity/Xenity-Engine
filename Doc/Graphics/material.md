# Material (File)

Store and send (when rendering scene) shader values.

## Methods
| Definition | Description |
|-|-|
void SetAttribut(const char* attribut, const T& value) | Set material attribute (Type can be Vector2/3/4, float and int)


## Members
| Name | Type | Default value | Description |
|-|-|-|-|
shader | std::shared_ptr\<Shader>  | nullptr | Material's shader
