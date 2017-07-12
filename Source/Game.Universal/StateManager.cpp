#include "pch.h"
#include "StateManager.h"
#include "BaseGameState.h"
#include "GameStart.h"
#include "GamePlayState.h"
#include "GameOver.h"

using namespace std;
using namespace DX;
using namespace GameStates;

namespace Managers
{
	StateManager* StateManager::sInstance = nullptr;

	StateManager* StateManager::GetInstance()
	{
		if (sInstance == nullptr)
		{
			sInstance = new StateManager();
		}
		return sInstance;
	}

	void StateManager::Initialize(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera, const shared_ptr<KeyboardComponent>& keyboard, const shared_ptr<GamePadComponent>& gamePad)
	{
		mGameStartState = make_shared<GameStartState>(deviceResources, camera, keyboard, gamePad);
		mGamePlayState = make_shared<GamePlayState>(deviceResources, camera, keyboard, gamePad);
		mGameOverState = make_shared<GameOverState>(deviceResources, camera, keyboard, gamePad, mGamePlayState);
	}

	void StateManager::ChangeState(GameState gameState)
	{
		//Exit condition before changing the state	
		if (mCurrentState)
		{
			mCurrentState->StateLeaving();
		}

		//Changing the state here
		switch (gameState)
		{
		case GameState::GAME_START:
			mCurrentState = mGameStartState;
			break;

		case GameState::GAME_PLAY:
			mCurrentState = mGamePlayState;
			break;

		case GameState::GAME_OVER:
			mCurrentState = mGameOverState;
			break;

		default:
			break;
		}

		//Entry condition after changing the state
		mCurrentState->StateEntering();
	}

	shared_ptr<BaseGameState>& StateManager::GetCurrentState()
	{
		return mCurrentState;
	}

	void StateManager::CreateDeviceDependentResources()
	{
		mGameStartState->CreateDeviceDependentResources();
		mGamePlayState->CreateDeviceDependentResources();
		mGameOverState->CreateDeviceDependentResources();
	}

	void StateManager::CreateWindowSizeDependentResources()
	{
		mGameStartState->CreateWindowSizeDependentResources();
		mGamePlayState->CreateWindowSizeDependentResources();
		mGameOverState->CreateWindowSizeDependentResources();
	}

	void StateManager::ReleaseDeviceDependentResources()
	{
		mGameStartState->ReleaseDeviceDependentResources();
		mGamePlayState->ReleaseDeviceDependentResources();
		mGameOverState->ReleaseDeviceDependentResources();
	}

	StateManager::StateManager()
		:mCurrentState(nullptr), mGameStartState(nullptr), mGamePlayState(nullptr), mGameOverState(nullptr)
	{
	}
}