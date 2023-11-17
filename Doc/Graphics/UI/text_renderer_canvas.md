# Text Renderer (Component)

Render a 2D text in a canvas.

Using normalised transform's position for placement: (0.0, 0.0) for top left, (1.0, 1.0) for bottom right.

## Methods
| Definition | Description |
|-|-|
void SetText(const std::string& text) | Set text
void SetFont(const std::shared_ptr<Font>& font) | Set font
void SetOrderInLayer(int orderInLayer) | Set order in layer (Higher value is higher priority)
void SetColor(const Color& color) | Set text color
int GetOrderInLayer() | Get order in layer (Higher value is higher priority)

## Members
| Name | Type | Default value | Description |
|-|-|-|-|
material | std::shared_ptr\<[Material](https://github.com/Fewnity/Xenity-Engine/blob/crossplatform/Doc/Graphics/material.md)> | nullptr | Text's material
horizontalAlignment | [HorizontalAlignment](https://github.com/Fewnity/Xenity-Engine/blob/crossplatform/Doc/Graphics/UI/text_alignments.md) (enum) | H_Center | Horizontal position of the text
verticalAlignment | [VerticalAlignment](https://github.com/Fewnity/Xenity-Engine/blob/crossplatform/Doc/Graphics/UI/text_alignments.md) (enum) | V_Center | Vertical position of the text
size | float | 16 | Text's size
lineSpacing | float | 0.0 | Space bewteen lines
characterSpacing | float | 0.0 | Space bewteen characters
