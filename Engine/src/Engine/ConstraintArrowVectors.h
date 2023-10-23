#pragma once
#include "raylib.h"

namespace Engine
{
	void ComputeHorizontalArrow(const Vector2 cellPosition,
		const int cellSize,
		const int blockSize,
		const float tWidthPercent,
		const float tHeightPercent,
		Vector2& top, Vector2& bottom, Vector2& right,
		bool computeLeft = false);


	void ComputeVerticalArrow(const Vector2 cellPosition,
		const int cellSize,
		const int blockSize,
		const float tWidthPercent,
		const float tHeightPercent,
		Vector2& left, Vector2& bottom, Vector2& right,
		bool computeUp);
}