#include "pch.h"
#include "BaseGameState.h"

namespace GameStates
{
	BaseGameState::BaseGameState(const std::shared_ptr<DX::DeviceResources>& deviceResources)
		:DrawableGameComponent(deviceResources)
	{
	}
}