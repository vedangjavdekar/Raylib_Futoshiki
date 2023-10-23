#pragma once
// Reference: https://github.com/nicolausYes/easing-functions/blob/master/src/easing.cpp
#include <cmath>

#ifndef PI
#define PI 3.1415926545f
#endif

namespace Engine::Easings
{
	inline float EaseInSine(float t)
	{
		return sin(1.5707963f * t);
	}

	inline float EaseOutSine(float t)
	{
		return 1.0f + sin(1.5707963f * (--t));
	}

	inline float EaseInOutSine(float t)
	{
		return 0.5f * (1 + sin(3.1415926f * (t - 0.5f)));
	}

	inline float EaseInQuad(float t)
	{
		return t * t;
	}

	inline float EaseOutQuad(float t)
	{
		return t * (2.0f - t);
	}

	inline float EaseInOutQuad(float t)
	{
		return t < 0.5f ? 2.0f * t * t : t * (4.0f - 2.0f * t) - 1.0f;
	}

	inline float EaseInCubic(float t)
	{
		return t * t * t;
	}

	inline float EaseOutCubic(float t)
	{
		return 1.0f + (--t) * t * t;
	}

	inline float EaseInOutCubic(float t)
	{
		if (t < 0.5f)
		{
			return 4.0f * t * t * t;
		}
		else
		{
			float f = ((2 * t) - 2);
			return 0.5 * f * f * f + 1;
		}
	}

	inline float EaseInQuart(float t)
	{
		t *= t;
		return t * t;
	}

	inline float EaseOutQuart(float t)
	{
		t = (--t) * t;
		return 1 - t * t;
	}

	inline float EaseInOutQuart(float t)
	{
		if (t < 0.5f)
		{
			t *= t;
			return 8.0f * t * t;
		}
		else
		{
			t = (--t) * t;
			return 1.0f - 8.0f * t * t;
		}
	}

	inline float EaseInQuint(float t)
	{
		float t2 = t * t;
		return t * t2 * t2;
	}

	inline float EaseOutQuint(float t)
	{
		float t2 = (--t) * t;
		return 1.0f + t * t2 * t2;
	}

	inline float EaseInOutQuint(float t)
	{
		float t2;
		if (t < 0.5f)
		{
			t2 = t * t;
			return 16.0f * t * t2 * t2;
		}
		else
		{
			t2 = (--t) * t;
			return 1.0f + 16.0f * t * t2 * t2;
		}
	}

	inline float EaseInExpo(float t)
	{
		return (pow(2.0f, 8.0f * t) - 1.0f) / 255.0f;
	}

	inline float EaseOutExpo(float t)
	{
		return 1.0f - pow(2.0f, -8.0f * t);
	}

	inline float EaseInOutExpo(float t)
	{
		if (t < 0.5f)
		{
			return (pow(2.0f, 16.0f * t) - 1.0f) / 510.0f;
		}
		else
		{
			return 1.0f - 0.5f * pow(2.0f, -16.0f * (t - 0.5f));
		}
	}

	inline float EaseInCirc(float t)
	{
		return 1.0f - sqrt(1.0f - t);
	}

	inline float EaseOutCirc(float t)
	{
		return sqrt(t);
	}

	inline float EaseInOutCirc(float t)
	{
		if (t < 0.5f)
		{
			return (1.0f - sqrt(1.0f - 2.0f * t)) * 0.5f;
		}
		else
		{
			return (1.0f + sqrt(2.0f * t - 1.0f)) * 0.5f;
		}
	}

	inline float EaseInBack(float t)
	{
		return t * t * (2.70158f * t - 1.70158f);
	}

	inline float EaseOutBack(float t)
	{
		return 1.0f + (--t) * t * (2.70158f * t + 1.70158f);
	}

	inline float EaseInOutBack(float t)
	{
		if (t < 0.5f)
		{
			return t * t * (7 * t - 2.5f) * 2.0f;
		}
		else
		{
			return 1.0f + (--t) * t * 2.0f * (7.0f * t + 2.5f);
		}
	}

	inline float EaseInElastic(float t)
	{
		float t2 = t * t;
		return t2 * t2 * sin(t * PI * 4.5f);
	}

	inline float EaseOutElastic(float t)
	{
		float t2 = (t - 1.0f) * (t - 1.0f);
		return 1 - t2 * t2 * cos(t * PI * 4.5f);
	}

	inline float EaseInOutElastic(float t)
	{
		float t2;
		if (t < 0.45f)
		{
			t2 = t * t;
			return 8.0f * t2 * t2 * sin(t * PI * 9.0f);
		}
		else if (t < 0.55f)
		{
			return 0.5f + 0.75f * sin(t * PI * 4.0f);
		}
		else
		{
			t2 = (t - 1.0f) * (t - 1.0f);
			return 1.0f - 8.0f * t2 * t2 * sin(t * PI * 9.0f);
		}
	}

	inline float EaseInBounce(float t)
	{
		return pow(2.0f, 6.0f * (t - 1.0f)) * abs(sin(t * PI * 3.5f));
	}

	inline float EaseOutBounce(float t)
	{
		return 1.0f - pow(2.0f, -6.0f * t) * abs(cos(t * PI * 3.5f));
	}

	inline float EaseInOutBounce(float t)
	{
		if (t < 0.5f)
		{
			return 8.0f * pow(2.0f, 8.0f * (t - 1.0f)) * abs(sin(t * PI * 7.0f));
		}
		else
		{
			return 1.0f - 8.0f * pow(2.0f, -8.0f * t) * abs(sin(t * PI * 7.0f));
		}
	}
}