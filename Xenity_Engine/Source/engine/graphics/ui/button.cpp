#include "button.h"

#include <engine/graphics/texture/texture.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/graphics/2d_graphics/sprite_manager.h>
#include <engine/graphics/graphics.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/inputs/input_system.h>
#include <engine/game_elements/rect_transform.h>
#include <engine/graphics/ui/canvas.h>
#include <engine/game_elements/transform.h>

void Button::CheckClick(Canvas& canvas)
{
	if (!IsEnabled() || !GetGameObject()->IsLocalActive())
	{
		return;
	}

	if (m_image)
	{
		const std::shared_ptr<RectTransform> rect = GetGameObject()->GetComponent<RectTransform>();
		if (rect)
		{
			const Vector3& scale = GetTransformRaw()->GetScale();
			const Vector2Int& canvasSize = canvas.GetSize();

			const float scaleCoef = (1.0f / m_image->GetPixelPerUnit());
			const float worldWidth = m_image->GetWidth() * scaleCoef;
			const float worldHeight = m_image->GetHeight() * scaleCoef;
			const float pixelsPerWorldUnitY = canvasSize.y / 10.0f;
			const float pixelWidth = worldWidth * pixelsPerWorldUnitY * scale.x;
			const float pixelHeight = worldHeight * pixelsPerWorldUnitY * scale.y;

			// Calculate the position of the image in pixels
			const Vector2 pixelImagePosition = rect->position * canvasSize;
			const Vector2 topLeft = pixelImagePosition - Vector2(pixelWidth / 2, pixelHeight / 2);
			const Vector2 bottomRight = pixelImagePosition + Vector2(pixelWidth / 2, pixelHeight / 2);

			// Detect click on the image
			const Vector2& mousePosition = InputSystem::mousePosition;
			if (InputSystem::GetKeyDown(KeyCode::MOUSE_LEFT) &&
				mousePosition.x >= topLeft.x && mousePosition.x <= bottomRight.x &&
				mousePosition.y >= topLeft.y && mousePosition.y <= bottomRight.y)
			{
				onClick.Trigger();
			}
			else if (InputSystem::GetTouchScreenCount() != 0)
			{
				const int touchScreenCount = InputSystem::GetTouchCount(0);
				for (int touchIndex = 0; touchIndex < touchScreenCount; touchIndex++)
				{
					const Touch touch = InputSystem::GetTouch(touchIndex, 0);
					if (touch.pressed &&
						touch.position.x >= topLeft.x && touch.position.x <= bottomRight.x &&
						touch.position.y >= topLeft.y && touch.position.y <= bottomRight.y)
					{
						onClick.Trigger();
						break;
					}
				}
			}
		}
	}
}
