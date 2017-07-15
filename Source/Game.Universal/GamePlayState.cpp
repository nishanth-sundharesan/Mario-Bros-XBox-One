#include "pch.h"
#include "GamePlayState.h"
#include "Background.h"
#include "Player.h"
#include "Blocks.h"
#include "PhysicsManager.h"
#include "EnemyManager.h"
#include "LevelManager.h"

using namespace DX;
using namespace std;
using namespace GameEntity;
using namespace Managers;

namespace GameStates
{
	GamePlayState::GamePlayState(const shared_ptr<DeviceResources>& deviceResources, const shared_ptr<Camera>& camera, const shared_ptr<KeyboardComponent>& keyboard, const shared_ptr<GamePadComponent>& gamePad)
		:BaseGameState(deviceResources, camera, keyboard, gamePad)
	{		 
		auto background = make_shared<Background>(mDeviceResources, mCamera);

		auto blocks = make_shared<Blocks>(mDeviceResources, mCamera);

		mPlayer1 = make_shared<Player>(mDeviceResources, mCamera, mKeyboard, mGamePad);
		
		mLevelManager = make_shared<LevelManager>(mDeviceResources, mCamera, mPlayer1);

		mEnemyManager = make_shared<EnemyManager>(mDeviceResources, mCamera, mLevelManager);

		auto physicsManager = make_shared<PhysicsManager>(mDeviceResources, mEnemyManager, mPlayer1, blocks);

		blocks->Init(*physicsManager);
		mLevelManager->SetEnemyManager(mEnemyManager);

		mComponents.push_back(blocks);
		mComponents.push_back(mEnemyManager);
		mComponents.push_back(background);		
		mComponents.push_back(mPlayer1);
		mComponents.push_back(mLevelManager);
		mComponents.push_back(physicsManager);					
	}

	void GamePlayState::StateEntering()
	{
		//Add blocks,Init as well
		mPlayer1->ResetScoreAndLives();
		mLevelManager->ResetLevelManager();
		mLevelManager->LoadNextLevel();
		mEnemyManager->ResetEnemyManager();		
	}

	void GamePlayState::Update(const StepTimer& timer)
	{
		for (auto& component : mComponents)
		{
			component->Update(timer);
		}		
	}

	void GamePlayState::Render(const StepTimer& timer)
	{
		for (auto& component : mComponents)
		{
			auto drawableComponent = dynamic_pointer_cast<DrawableGameComponent>(component);
			if (drawableComponent != nullptr && drawableComponent->Visible())
			{
				drawableComponent->Render(timer);
			}
		}
	}

	void GamePlayState::CreateDeviceDependentResources()
	{
		for (auto& component : mComponents)
		{
			component->CreateDeviceDependentResources();
		}
	}

	void GamePlayState::CreateWindowSizeDependentResources()
	{
		for (auto& component : mComponents)
		{
			component->CreateWindowSizeDependentResources();
		}
	}

	void GamePlayState::ReleaseDeviceDependentResources()
	{
		for (auto& component : mComponents)
		{
			component->ReleaseDeviceDependentResources();
		}
	}
}