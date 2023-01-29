#pragma once

namespace DYE::MiniGame
{
	struct Hittable
	{
		// -1 means hasn't been hit by any player.
		int LastHitByPlayerID = -1;
	};
}