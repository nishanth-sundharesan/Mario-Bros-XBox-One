#pragma once
namespace GameStates
{
	class BaseGameState;
	class GameStartState;
	class GamePlayState;
	class GameOverState;

	enum class GameState;
}

namespace Managers
{
	class StateManager final
	{
	public:
		StateManager(const StateManager&) = delete;
		StateManager& operator=(const StateManager&) = delete;
		StateManager(StateManager&&) = delete;
		StateManager& operator=(StateManager&&) = delete;
		~StateManager() = default;

		static StateManager	*GetInstance();
		void Initialize(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, const std::shared_ptr<DX::KeyboardComponent>& keyboard, const std::shared_ptr<DX::GamePadComponent>& gamePad);
		void ChangeState(GameStates::GameState gameState);
		std::shared_ptr<GameStates::BaseGameState>& GetCurrentState();

		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
	private:
		StateManager();
		static StateManager *sInstance;

		std::shared_ptr<GameStates::GameStartState> mGameStartState;
		std::shared_ptr<GameStates::GamePlayState> mGamePlayState;
		std::shared_ptr<GameStates::GameOverState> mGameOverState;

		std::shared_ptr<GameStates::BaseGameState> mCurrentState;
	};
}