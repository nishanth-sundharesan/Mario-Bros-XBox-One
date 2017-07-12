#include "pch.h"
#include "LevelManager.h"
#include "XmlParseHelper.h"
#include "Player.h"
#include "EnemyManager.h"
#include "TextManager.h"

using namespace std;
using namespace DX;
using namespace Core;
using namespace GameEntity;
using namespace Managers;
using namespace Audio;

namespace Managers
{
	const float LevelManager::sPHASE_TEXT_DISPLAY_DURATION = 6500;
	const float LevelManager::sLOAD_NEW_PHASE_DURATION = 1700;

	LevelManager::LevelManager(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera, shared_ptr<Player>& player)
		:DrawableGameComponent(deviceResources, camera), mPlayer(player)
	{
		XmlParserHelper xmlParseHelper;
		xmlParseHelper.LoadLevels(mGameEmenies);
	}

	void LevelManager::Update(const StepTimer& timer)
	{
		if (mIsLoadNewLevel)
		{
			mLoadNewLevelDuration += timer.GetElapsedMilliSeconds();
			if (mLoadNewLevelDuration > sLOAD_NEW_PHASE_DURATION)
			{
				LoadNextLevel();
				ResetLoadNewLevelVariables();
			}
		}
	}

	void LevelManager::Render(const StepTimer& timer)
	{
		//Rendering Phase text
		if (mIsDisplayPhaseText)
		{
			mPhaseDisplayDuration += timer.GetElapsedMilliSeconds();
			if (mPhaseDisplayDuration < sPHASE_TEXT_DISPLAY_DURATION)
			{
				TextManager::GetInstance()->DisplayPhaseText(mCurrentLevel);
			}
			else
			{
				mIsDisplayPhaseText = false;
			}
		}
		TextManager::GetInstance()->DisplayCurrentPhase(mCurrentLevel);
	}

	void LevelManager::SetEnemyManager(const shared_ptr<EnemyManager>& enemyManager)
	{
		mEnemyManager = enemyManager;
	}

	void LevelManager::GetReadyForNextLevel()
	{
		SoundManager::GetInstance()->PlayPhaseEndSound();
		mIsLoadNewLevel = true;
	}

	void LevelManager::LoadNextLevel()
	{
		//If the player has finished all the levels, then restart the game from first level.
		if (mCurrentLevel == static_cast<int32_t>(mGameEmenies.size()))
		{
			ResetLevels();
		}

		mPlayer->ResetPlayer();
		mEnemyManager->SetNewPhaseEnemies(mGameEmenies[mCurrentLevel]);

		++mCurrentLevel;
		if (mCurrentLevel == 1)
		{
			SoundManager::GetInstance()->PlayGameStartSound();
		}
		else
		{
			SoundManager::GetInstance()->PlayPhaseStartSound();
		}
		ResetPhaseTextVariables();
	}

	void LevelManager::ResetLevels()
	{
		mCurrentLevel = 0;
	}

	void LevelManager::ResetLevelManager()
	{
		ResetLevels();
		ResetPhaseTextVariables();
		ResetLoadNewLevelVariables();
	}

	void LevelManager::ResetPhaseTextVariables()
	{
		mIsDisplayPhaseText = true;
		mPhaseDisplayDuration = 0;
	}

	void LevelManager::ResetLoadNewLevelVariables()
	{
		mIsLoadNewLevel = false;
		mLoadNewLevelDuration = 0;
	}
}