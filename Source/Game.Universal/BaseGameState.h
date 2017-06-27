#pragma once
#include "DrawableGameComponent.h"
#include <DirectXMath.h>
#include <vector>

namespace GameStates
{
	class BaseGameState : public DX::DrawableGameComponent
	{
	public:
		BaseGameState(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		BaseGameState(const BaseGameState&) = default;
		BaseGameState& operator=(const BaseGameState&) = default;
		BaseGameState(BaseGameState&&) = default;
		BaseGameState& operator=(BaseGameState&&) = default;
		virtual ~BaseGameState() = default;
		virtual void StateEntering() = 0;
		virtual void StateLeaving() = 0;
	};
}