# Camera (Component)

Visualise a scene.

## Methods
| Definition | Description |
|-|-|
Vector2 ScreenTo2DWorld(int x, int y) | Get 2D world position from pixel coordinate (values must not exceed the screen's resolution)
Vector2 MouseTo2DWorld() | Get 2D world position from mouse's position
void SetFov(const float fov) | Set field of view
void SetProjectionSize(const float value) | Set projection size (For Orthographic mode)
void SetNearClippingPlane(float value) | Set near clipping plane
void SetFarClippingPlane(float value) | Set far clipping plane
void SetProjectionType(ProjectionTypes type) | Set projection type
float GetFov() | Get field of view
float GetProjectionSize() | Get projection size (For Orthographic mode)
float GetNearClippingPlane() | Get near clipping plane
float GetFarClippingPlane() |  Get far clipping plane
ProjectionTypes GetProjectionType() | Get projection type
int GetWidth()  | Get view width in pixel
int GetHeight() | Get view height in pixel
float GetAspectRatio() | Get view aspect ratio
glm::mat4& GetProjection() | Get projection matrix

## Members
| Name | Type | Default value | Description |
|-|-|-|-|
useMultisampling | bool | true | Is the camera using Multisampling?
