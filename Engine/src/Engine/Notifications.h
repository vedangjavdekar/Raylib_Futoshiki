#pragma once
#include <string>
#include <vector>
#include "raylib.h"

namespace Engine
{
	struct NotificationStyle
	{
		Color Background = DARKGRAY;
		Color Text = WHITE;

		int StatusWidth = 10;
		int ItemHeight = 80;
		int FontSize = 24;
		int Separation = 5;

		float FadeDuration = 0.5f;
		float SlideDuration = 0.2f;

		bool FadeWithSlide = true;
	};

	class Notifications
	{
		struct Notification
		{
			TraceLogLevel status;
			std::string text;
			float duration;
			float fadeDuration;
			float slideDuration;
		};

	public:
		void AddNotification(TraceLogLevel status, const std::string& text, float duration = 1.5f);

		void Update(const float deltaTime);

		void Draw(float offsetPercentX, float offsetPercentY = -1.0f, float widthPercent = -1.0f);

	public:
		NotificationStyle Style;

	private:
		std::vector<Notification> m_Notifications;
	};
}