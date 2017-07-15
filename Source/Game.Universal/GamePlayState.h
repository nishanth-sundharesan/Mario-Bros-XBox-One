#pragma once
#include "BaseGameState.h"

namespace GameEntity
{
	class Player;
}

namespace Managers
{
	class LevelManager;
	class EnemyManager;
}

namespace GameStates
{
	class GamePlayState final : public BaseGameState
	{
	public:
		GamePlayState(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, const std::shared_ptr<DX::KeyboardComponent>& keyboard, const std::shared_ptr<DX::GamePadComponent>& gamePad);
		GamePlayState(const GamePlayState&) = delete;
		GamePlayState& operator=(const GamePlayState&) = delete;
		GamePlayState(GamePlayState&&) = delete;
		GamePlayState& operator=(GamePlayState&&) = delete;
		~GamePlayState() = default;

		virtual void StateEntering() override;
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;		

		virtual void CreateDeviceDependentResources() override;
		virtual void CreateWindowSizeDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;

	private:
		std::shared_ptr<GameEntity::Player> mPlayer1;
		std::shared_ptr<Managers::LevelManager> mLevelManager;
		std::shared_ptr<Managers::EnemyManager> mEnemyManager;

		std::vector<std::shared_ptr<DX::GameComponent>> mComponents;
	};
}