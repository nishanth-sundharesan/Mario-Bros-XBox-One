#pragma once
#include <DirectXMath.h>
#include <vector>

namespace DX
{
	class DeviceResources;
	class Camera;
}

namespace GameStates
{
	enum class GameState
	{
		GAME_START,
		GAME_PLAY,
		GAME_OVER
	};

	class BaseGameState
	{
	public:
		BaseGameState(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, const std::shared_ptr<DX::KeyboardComponent>& keyboard, const std::shared_ptr<DX::GamePadComponent>& gamePad);
		BaseGameState(const BaseGameState&) = delete;
		BaseGameState& operator=(const BaseGameState&) = delete;
		BaseGameState(BaseGameState&&) = delete;
		BaseGameState& operator=(BaseGameState&&) = delete;
		virtual ~BaseGameState() = default;
		virtual void StateEntering();
		virtual void Update(const DX::StepTimer& timer) = 0;
		virtual void Render(const DX::StepTimer& timer) = 0;
		virtual void StateLeaving();

		virtual void CreateDeviceDependentResources();
		virtual void CreateWindowSizeDependentResources();
		virtual void ReleaseDeviceDependentResources();

	protected:
		std::shared_ptr<DX::DeviceResources> mDeviceResources;
		std::shared_ptr<DX::Camera> mCamera;
		std::shared_ptr<DX::KeyboardComponent> mKeyboard;
		std::shared_ptr<DX::GamePadComponent> mGamePad;
	};
}