#pragma once
#include "BaseGameState.h"

namespace GameStates
{
	class GamePlayState;

	class GameOverState final: public BaseGameState
	{
	public:
		GameOverState(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, const std::shared_ptr<DX::KeyboardComponent>& keyboard,
			const std::shared_ptr<DX::GamePadComponent>& gamePad, const std::shared_ptr<GamePlayState> gamePlayState);
		GameOverState(const GameOverState&) = delete;
		GameOverState& operator=(const GameOverState&) = delete;
		GameOverState(GameOverState&&) = delete;
		GameOverState& operator=(GameOverState&&) = delete;
		~GameOverState() = default;

		virtual void StateEntering() override;
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;		

	private:
		std::int32_t mGameOverDuration;
		std::shared_ptr<GamePlayState> mGamePlayState;

		static const float sGAME_OVER_DURATION;
	};
}