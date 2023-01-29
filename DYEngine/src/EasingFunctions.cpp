#include "Math/EasingFunctions.h"

#include <map>

#ifndef PI
#define PI 3.1415926545
#endif

namespace DYE
{
	float EaseLinear(float t)
	{
		return t;
	}

	float EaseInSine(float t)
	{
		return sin(1.5707963 * t);
	}

	float EaseOutSine(float t)
	{
		return 1 + sin(1.5707963 * (--t));
	}

	float EaseInOutSine(float t)
	{
		return 0.5 * (1 + sin(3.1415926 * (t - 0.5)));
	}

	float EaseInQuad(float t)
	{
		return t * t;
	}

	float EaseOutQuad(float t)
	{
		return t * (2 - t);
	}

	float EaseInOutQuad(float t)
	{
		return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
	}

	float EaseInCubic(float t)
	{
		return t * t * t;
	}

	float EaseOutCubic(float t)
	{
		return 1 + (--t) * t * t;
	}

	float EaseInOutCubic(float t)
	{
		return t < 0.5 ? 4 * t * t * t : 1 + (--t) * (2 * (--t)) * (2 * t);
	}

	float EaseInQuart(float t)
	{
		t *= t;
		return t * t;
	}

	float EaseOutQuart(float t)
	{
		t = (--t) * t;
		return 1 - t * t;
	}

	float EaseInOutQuart(float t)
	{
		if (t < 0.5)
		{
			t *= t;
			return 8 * t * t;
		}
		else
		{
			t = (--t) * t;
			return 1 - 8 * t * t;
		}
	}

	float EaseInQuint(float t)
	{
		float const t2 = t * t;
		return t * t2 * t2;
	}

	float EaseOutQuint(float t)
	{
		float const t2 = (--t) * t;
		return 1 + t * t2 * t2;
	}

	float EaseInOutQuint(float t)
	{
		float t2;
		if (t < 0.5)
		{
			t2 = t * t;
			return 16 * t * t2 * t2;
		}
		else
		{
			t2 = (--t) * t;
			return 1 + 16 * t * t2 * t2;
		}
	}

	float EaseInExpo(float t)
	{
		return (pow(2, 8 * t) - 1) / 255;
	}

	float EaseOutExpo(float t)
	{
		return 1 - pow(2, -8 * t);
	}

	float EaseInOutExpo(float t)
	{
		if (t < 0.5)
		{
			return (pow(2, 16 * t) - 1) / 510;
		}
		else
		{
			return 1 - 0.5 * pow(2, -16 * (t - 0.5));
		}
	}

	float EaseInCirc(float t)
	{
		return 1 - std::sqrt(1 - t);
	}

	float EaseOutCirc(float t)
	{
		return std::sqrt(t);
	}

	float EaseInOutCirc(float t)
	{
		if (t < 0.5)
		{
			return (1 - std::sqrt(1 - 2 * t)) * 0.5;
		}
		else
		{
			return (1 + std::sqrt(2 * t - 1)) * 0.5;
		}
	}

	float EaseInBack(float t)
	{
		return t * t * (2.70158 * t - 1.70158);
	}

	float EaseOutBack(float t)
	{
		return 1 + (--t) * t * (2.70158 * t + 1.70158);
	}

	float EaseInOutBack(float t)
	{
		if (t < 0.5)
		{
			return t * t * (7 * t - 2.5) * 2;
		}
		else
		{
			return 1 + (--t) * t * 2 * (7 * t + 2.5);
		}
	}

	float EaseInBackDouble(float t)
	{
		float const s = 1.70158f * 1.5f;

		float end = 1.0f;
		float const start = 0.0f;
		end -= start;
		t /= 1;
		return end * (t) * t * ((s + 1) * t - s) + start;
	}

	float EaseOutBackDouble(float t)
	{
		float const s = 1.70158f * 1.5f;

		float end = 1.0f;
		float const start = 0.0f;
		end -= start;
		t = (t) - 1;
		return end * ((t) * t * ((s + 1) * t + s) + 1) + start;
	}

	float EaseInElastic(float t)
	{
		float const t2 = t * t;
		return t2 * t2 * sin(t * PI * 4.5);
	}

	float EaseOutElastic(float t)
	{
		float const t2 = (t - 1) * (t - 1);
		return 1 - t2 * t2 * cos(t * PI * 4.5);
	}

	float EaseInOutElastic(float t)
	{
		float t2;
		if (t < 0.45)
		{
			t2 = t * t;
			return 8 * t2 * t2 * sin(t * PI * 9);
		}
		else if (t < 0.55)
		{
			return 0.5 + 0.75 * sin(t * PI * 4);
		}
		else
		{
			t2 = (t - 1) * (t - 1);
			return 1 - 8 * t2 * t2 * sin(t * PI * 9);
		}
	}

	float EaseInBounce(float t)
	{
		return pow(2, 6 * (t - 1)) * std::abs(sin(t * PI * 3.5));
	}

	float EaseOutBounce(float t)
	{
		return 1 - pow(2, -6 * t) * std::abs(cos(t * PI * 3.5));
	}

	float EaseInOutBounce(float t)
	{
		if (t < 0.5)
		{
			return 8 * pow(2, 8 * (t - 1)) * std::abs(sin(t * PI * 7));
		}
		else
		{
			return 1 - 8 * pow(2, -8 * t) * std::abs(sin(t * PI * 7));
		}
	}

	EasingFunction GetEasingFunction(Ease ease)
	{
		switch (ease)
		{
			case Ease::Linear:
				return EaseLinear;
			case Ease::EaseInSine:
				return EaseInSine;
			case Ease::EaseOutSine:
				return EaseOutSine;
			case Ease::EaseInOutSine:
				return EaseInOutSine;
			case Ease::EaseInQuad:
				return EaseInQuad;
			case Ease::EaseOutQuad:
				return EaseOutQuad;
			case Ease::EaseInOutQuad:
				return EaseInOutQuad;
			case Ease::EaseInCubic:
				return EaseInCubic;
			case Ease::EaseOutCubic:
				return EaseOutCubic;
			case Ease::EaseInOutCubic:
				return EaseInOutCubic;
			case Ease::EaseInQuart:
				return EaseInQuart;
			case Ease::EaseOutQuart:
				return EaseOutQuart;
			case Ease::EaseInOutQuart:
				return EaseInOutQuart;
			case Ease::EaseInQuint:
				return EaseInQuint;
			case Ease::EaseOutQuint:
				return EaseOutQuint;
			case Ease::EaseInOutQuint:
				return EaseInOutQuint;
			case Ease::EaseInExpo:
				return EaseInExpo;
			case Ease::EaseOutExpo:
				return EaseOutExpo;
			case Ease::EaseInOutExpo:
				return EaseInOutExpo;
			case Ease::EaseInCirc:
				return EaseInCirc;
			case Ease::EaseOutCirc:
				return EaseOutCirc;
			case Ease::EaseInOutCirc:
				return EaseInOutCirc;
			case Ease::EaseInBack:
				return EaseInBack;
			case Ease::EaseOutBack:
				return EaseOutBack;
			case Ease::EaseInOutBack:
				return EaseInOutBack;
			case Ease::EaseInBackDouble:
				return EaseInBackDouble;
			case Ease::EaseOutBackDouble:
				return EaseOutBackDouble;
			case Ease::EaseInElastic:
				return EaseInElastic;
			case Ease::EaseOutElastic:
				return EaseOutElastic;
			case Ease::EaseInOutElastic:
				return EaseInOutElastic;
			case Ease::EaseInBounce:
				return EaseInBounce;
			case Ease::EaseOutBounce:
				return EaseOutBounce;
			case Ease::EaseInOutBounce:
				return EaseInOutBounce;
		}

		return EaseLinear;
	}
}