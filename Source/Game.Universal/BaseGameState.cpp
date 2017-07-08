#include "pch.h"
#include "BaseGameState.h"

namespace GameStates
{
	BaseGameState::BaseGameState(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera,
		const std::shared_ptr<DX::KeyboardComponent>& keyboard, const std::shared_ptr<DX::GamePadComponent>& gamePad)
		:mDeviceResources(deviceResources), mCamera(camera), mKeyboard(keyboard), mGamePad(gamePad)
	{
	}

	void BaseGameState::StateEntering()
	{
	}

	void BaseGameState::StateLeaving()
	{
	}

	void BaseGameState::CreateDeviceDependentResources()
	{
	}

	void BaseGameState::CreateWindowSizeDependentResources()
	{
	}

	void BaseGameState::ReleaseDeviceDependentResources()
	{
	}
}