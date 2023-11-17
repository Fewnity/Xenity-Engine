# Sprite Renderer

## Methods
| Definition | Description |
|-|-|
void SetOrderInLayer(int orderInLayer) | Set order in layer (Higher value is higher priority)
void SetColor(const Color& color) | Set sprite color
int GetOrderInLayer() | Get order in layer (Higher value is higher priority)

## Members
| Name | Type | Description |
|-|-|-|
material | std::shared_ptr\<Material> | Sprite's material
texture | std::shared_ptr\<Texture> | Sprite's image
