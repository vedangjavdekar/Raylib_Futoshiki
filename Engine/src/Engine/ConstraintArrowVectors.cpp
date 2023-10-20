#include "ConstraintArrowVectors.h"
#include "raymath.h"

#include <algorithm>

namespace Engine
{
	void ComputeHorizontalArrow(
		const Vector2 cellPosition,
		const float cellSize,
		const float blockSize,
		const float tWidthPercent,
		const float tHeightPercent,
		Vector2& top, Vector2& bottom, Vector2& right,
		bool computeLeft)
	{
		const Vector2 cellCenter = Vector2Add(cellPosition, Vector2{ 0.5f * cellSize, 0.5f * cellSize });
		const float width = cellSize - blockSize;
		const float height = blockSize;
		Rectangle triangleRect = Rectangle{
			cellCenter.x + 0.5f * blockSize,
			cellCenter.y - 0.5f * blockSize,
			width,
			height
		};

		const float actualWidth = tWidthPercent * width;
		const float actualHeight = tHeightPercent * height;

		const float deltaX = 0.5f * (width - actualWidth);
		const float deltaY = 0.5f * (height - actualHeight);

		triangleRect.x += deltaX;
		triangleRect.y += deltaY;
		triangleRect.width = actualWidth;
		triangleRect.height = actualHeight;

		top = Vector2{ triangleRect.x, triangleRect.y};
		bottom = Vector2{ triangleRect.x, triangleRect.y + triangleRect.height };
		right = Vector2{ triangleRect.x + triangleRect.width, triangleRect.y + 0.5f * triangleRect.height };

		if (computeLeft)
		{
			std::swap(bottom, right);
			top.x = 2 * cellCenter.x - top.x;
			bottom.x = 2 * cellCenter.x - bottom.x;
			right.x = 2 * cellCenter.x - right.x;
		}
	}

	void ComputeVerticalArrow(const Vector2 cellPosition,
		const float cellSize,
		const float blockSize,
		const float tWidthPercent,
		const float tHeightPercent,
		Vector2& left, Vector2& bottom, Vector2& right,
		bool computeUp)
	{
		const Vector2 cellCenter = Vector2Add(cellPosition, Vector2{ 0.5f * cellSize, 0.5f * cellSize });
		const float width = blockSize;
		const float height = cellSize - blockSize;
		Rectangle triangleRect = Rectangle{
			cellCenter.x - 0.5f * blockSize,
			cellCenter.y + 0.5f * blockSize,
			width,
			height
		};

		const float actualWidth = tWidthPercent * width;
		const float actualHeight = tHeightPercent * height;

		const float deltaX = 0.5f * (width - actualWidth);
		const float deltaY = 0.5f * (height - actualHeight);

		triangleRect.x += deltaX;
		triangleRect.y += deltaY;
		triangleRect.width = actualWidth;
		triangleRect.height = actualHeight;

		left = Vector2{ triangleRect.x, triangleRect.y };
		bottom = Vector2{ triangleRect.x + 0.5f * triangleRect.width, triangleRect.y + triangleRect.height };
		right = Vector2{ triangleRect.x + triangleRect.width, triangleRect.y};

		if (computeUp)
		{
			std::swap(bottom, right);
			left.y = 2 * cellCenter.y - left.y;
			right.y = 2 * cellCenter.y - right.y;
			bottom.y = 2 * cellCenter.y - bottom.y;
		}
	}
}