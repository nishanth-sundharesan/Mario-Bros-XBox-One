#pragma once
#include "BaseGameState.h"

namespace GameStates
{
	class GamePlayState final : public BaseGameState
	{
		GamePlayState(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera);
		GamePlayState(const GamePlayState&) = default;
		GamePlayState& operator=(const GamePlayState&) = default;
		GamePlayState(GamePlayState&&) = default;
		GamePlayState& operator=(GamePlayState&&) = default;

		virtual void StateEntering() override;
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;
		virtual void StateLeaving() override;

		~GamePlayState() = default;
	};
}