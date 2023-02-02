#include "Objects/LandBall.h"

namespace DYE::MiniGame
{
	void LandBall::OnBounce()
	{
		TimeToReachApex = TimeToReachApex * (1.0f - TimePercentageLossPerBounce);
		if (TimeToReachApex <= MinimumTimeToReachApex)
		{
			TimeToReachApex = MinimumTimeToReachApex;
		}
	}
}
