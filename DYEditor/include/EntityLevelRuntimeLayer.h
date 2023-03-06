#pragma once

#include "Core/LayerBase.h"

#include "World.h"

namespace DYE::DYEntity
{
	class EntityLevelRuntimeLayer : public DYE::LayerBase
	{
	public:
		EntityLevelRuntimeLayer();
		~EntityLevelRuntimeLayer() override = default;

		void OnAttach() override;

	protected:
		virtual void onPreLevelFileLoaded() {};
		virtual void onPostLevelFileLoaded() {};

	public:
		World World;
	};
}