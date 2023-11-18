# Light

## Methods
| Definition | Description |
|-|-|
void SetupPointLight(const Color& color, float _intensity, const float _range) | Setup the light as a point light
void SetupDirectionalLight(const Color& color, const float _intensity) | Setup the light as a directional light
void SetupSpotLight(const Color& color, const float _intensity, const float _range, const float _angle) | Setup the light as a spot light
void SetupSpotLight(const Color& color, const float _intensity, const float _range, const float _angle, const float _smoothness) | Setup the light as a point light
void SetRange(float value) | Set light range (Greater or equals to 0)
float GetRange() | Get light range (Greater or equals to 0)
void SetSpotAngle(float angle) | Set spot angle between [0.0 and 179.0]
void SetSpotSmoothness(float smoothness) | Set spot smoothness between [0.0 and 1.0]
float GetSpotAngle() | Get spot angle between [0.0 and 179.0]
float GetSpotSmoothness() | Get spot smoothness between [0.0 and 1.0]
void SetIntensity(float intensity) | Set light intensity
float GetIntensity() | Get light intensity

## Members
| Name | Type | Default value | Description |
|-|-|-|-|
type | LightType | Directional | Light's type
color | Color | White | Light's color
