#include "Notifications.h"
#include "raymath.h"

#include "Animations/Easings.h"

namespace Engine
{
	static Color GetStatusColor(TraceLogLevel status)
	{
		switch (status)
		{
		case LOG_INFO:
		{
			return Color{ 84, 168, 111 , 255 }; // Pastle Green
		}
		case LOG_WARNING:
		{
			return Color{ 255, 231, 71, 255 }; // Yellowish
		}
		case LOG_ERROR:
		{
			return Color{ 166, 60, 60 , 255 }; // Dark Red
		}
		default:
			return BLACK;
		}
	}

	void Notifications::AddNotification(TraceLogLevel status, const std::string& text, float duration)
	{
		m_Notifications.push_back(Notification{status, text, duration, Style.FadeDuration});
	}

	void Notifications::Update(const float deltaTime)
	{
		for (auto& itr = m_Notifications.begin(); itr != m_Notifications.end();)
		{
			bool deleteNotification = false;
			if (itr->slideDuration < Style.SlideDuration)
			{
				itr->slideDuration += deltaTime;
				itr->slideDuration = std::min(itr->slideDuration, Style.SlideDuration);
			}
			else if (itr->duration > 0.0f)
			{
				itr->duration -= deltaTime;
			}
			else if(itr->duration < 0.0f && itr->fadeDuration > 0.0f)
			{
				itr->fadeDuration -= deltaTime;
				itr->fadeDuration = std::max(itr->fadeDuration, 0.0f);
			}
			else
			{
				deleteNotification = true;
			}

			if (deleteNotification)
			{
				itr = m_Notifications.erase(itr);
			}
			else
			{
				itr = std::next(itr);
			}
		}
	}

	void Notifications::Draw(float offsetPercentX, float offsetPercentY, float widthPercent)
	{
		if (m_Notifications.empty())
		{
			return;
		}

		const float screenWidth = GetScreenWidth();
		const float screenHeight = GetScreenHeight();

		offsetPercentX = Clamp(offsetPercentX, 0.0f, 1.0f);

		if (offsetPercentY >= 0.0f)
		{
			offsetPercentY = Clamp(offsetPercentY, 0.0f, 1.0f);
		}
		else
		{
			offsetPercentY = 1.0;
		}

		Vector2 nextDrawingPosition{
			offsetPercentX * screenWidth,
			offsetPercentY * screenHeight - Style.ItemHeight
		};

		float width = (1.0f - offsetPercentX) * screenWidth;
		if (widthPercent > 0.0f)
		{
			widthPercent = Clamp(widthPercent, 0.0f, 1.0f);
			width = widthPercent * screenWidth;
		}

		for (int i = m_Notifications.size() - 1; i >= 0; --i)
		{
			unsigned char computedAlpha = 255 * Easings::EaseOutCubic(m_Notifications[i].fadeDuration / Style.FadeDuration);
			const float xOffset = screenWidth * Lerp(1.0f, offsetPercentX, Easings::EaseInOutCubic(m_Notifications[i].slideDuration / Style.SlideDuration));
			if (Style.FadeWithSlide && m_Notifications[i].slideDuration < Style.SlideDuration)
			{
				computedAlpha = 255 * Easings::EaseOutCubic(m_Notifications[i].slideDuration / Style.SlideDuration);
			}

			Vector2 position = { xOffset, nextDrawingPosition.y };
			Color tempColor = Style.Background;
			tempColor.a = computedAlpha;
			DrawRectangle(position.x, position.y, width, Style.ItemHeight, tempColor);

			tempColor = GetStatusColor(m_Notifications[i].status);
			tempColor.a = computedAlpha;
			DrawRectangle(position.x, position.y, Style.StatusWidth, Style.ItemHeight, tempColor);


			tempColor = Style.Text;
			tempColor.a = computedAlpha;
			DrawText(m_Notifications[i].text.c_str(), 
				position.x + Style.StatusWidth + 10,
				position.y + 0.5 * (Style.ItemHeight - Style.FontSize),
				Style.FontSize, 
				tempColor);

			nextDrawingPosition.y -= Style.ItemHeight + Style.Separation;
		}
	}
}