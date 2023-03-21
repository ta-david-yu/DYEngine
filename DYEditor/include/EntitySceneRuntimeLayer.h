#pragma once

#include "Core/LayerBase.h"

#include "World.h"

namespace DYE::DYEntity
{
	class EntitySceneRuntimeLayer : public DYE::LayerBase
	{
	public:
		EntitySceneRuntimeLayer();
		~EntitySceneRuntimeLayer() override = default;

		void OnAttach() override;

	protected:
		virtual void onPreSceneFileLoaded() {};
		virtual void onPostSceneFileLoaded() {};

	public:
		World World;
	};
}