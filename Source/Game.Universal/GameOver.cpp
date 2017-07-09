#include "pch.h"
#include "GameOver.h"
#include "TextManager.h"
#include "GamePlayState.h"
#include "StateManager.h"

using namespace std;
using namespace DX;
using namespace Managers;
using namespace Audio;

namespace GameStates
{
	const float GameOverState::sGAME_OVER_DURATION = 6000;

	GameOverState::GameOverState(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera, const shared_ptr<KeyboardComponent>& keyboard,
		const shared_ptr<GamePadComponent>& gamePad, const std::shared_ptr<GamePlayState> gamePlayState)
		:BaseGameState(deviceResources, camera, keyboard, gamePad), mGameOverDuration(0), mGamePlayState(gamePlayState)
	{
	}

	void GameOverState::StateEntering()
	{
		SoundManager::GetInstance()->PlayGameOverSound();
	}

	void GameOverState::Update(const StepTimer& timer)
	{
		mGameOverDuration += timer.GetElapsedMilliSeconds();
		if (mGameOverDuration > sGAME_OVER_DURATION)
		{
			StateManager::GetInstance()->ChangeState(GameState::GAME_START);
			mGameOverDuration = 0;
		}
	}

	void GameOverState::Render(const StepTimer& timer)
	{
		TextManager::GetInstance()->DisplayGameOverText();
		mGamePlayState->Render(timer);
	}
}
