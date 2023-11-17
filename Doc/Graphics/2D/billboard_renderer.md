# Billboard Renderer

## Methods
| Definition | Description |
|-|-|
void SetOrderInLayer(int orderInLayer) | Set order in layer (Higher value is higher priority)
void SetColor(const Color& color) | Set billboard color
int GetOrderInLayer() | Get order in layer (Higher value is higher priority)

## Members
| Name | Type | Description |
|-|-|-|
material | std::shared_ptr\<Material> | Billboard's material
texture | std::shared_ptr\<Texture> | Billboard's image
