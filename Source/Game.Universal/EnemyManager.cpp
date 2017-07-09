#include "pch.h"
#include "EnemyManager.h"
#include "LevelManager.h"
#include "Duck.h"
#include "Crab.h"

using namespace std;
using namespace GameEntity;
using namespace DX;
using namespace Audio;

namespace Managers
{
	EnemyManager::EnemyManager(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera, shared_ptr<LevelManager>& levelManager)
		:DrawableGameComponent(deviceResources, camera), mLevelManager(levelManager), mDeleteEnemies(false)
	{
	}

	void EnemyManager::RemoveEnemy(int32_t enemyID)
	{
		mEnemiesToDelete.push_back(enemyID);
		mDeleteEnemies = true;
	}

	bool EnemyManager::CheckIfLastEnemy()
	{
		bool isLastEnemy = false;
		mNumberOfAliveEnemies--;

		if (!mAreEnemiesLeft)
		{
			if (mNumberOfAliveEnemies == 0)
			{
				isLastEnemy = true;
				mLevelManager->GetReadyForNextLevel();
			}
			else if (mNumberOfAliveEnemies == 1)
			{
				for (auto& enemy : mEnemies)
				{
					if (enemy->mCurrentState != EnemyState::DEAD)
					{
						enemy->EvolveToFinalState();
					}
				}
			}
		}

		return isLastEnemy;
	}

	void EnemyManager::InstantiateEnemy(EnemyType enemyType, bool isSpawnLeft)
	{
		isSpawnLeft;
		if (enemyType == EnemyType::DUCK)
		{
			auto duck = make_shared<Duck>(mDeviceResources, mCamera, isSpawnLeft, mEnemyIndex, *this);
			duck->CreateDeviceDependentResources();
			mEnemies.push_back(duck);
			SoundManager::GetInstance()->PlaySpawnShellCreeperSound();
		}
		else if (enemyType == EnemyType::CRAB)
		{
			auto crab = make_shared<Crab>(mDeviceResources, mCamera, isSpawnLeft, mEnemyIndex, *this);
			crab->CreateDeviceDependentResources();
			mEnemies.push_back(crab);
			SoundManager::GetInstance()->PlaySpawnSideStepperSound();
		}
		else if (enemyType == EnemyType::BEE)
		{
		}
	}

	vector<shared_ptr<Enemy>>& EnemyManager::GetCurrentLevelenemies()
	{
		return mEnemies;
	}

	void EnemyManager::ResetEnemyManager()
	{
		mEnemies.clear();
		mEnemiesToDelete.clear();
	}

	void EnemyManager::SetNewPhaseEnemies(const EnemySpawner& enemies)
	{
		mCurrentLevelEnemies = enemies;
		mLevelTime = 0;
		mNumberOfEnemies = 0;
		mEnemyIndex = 0;
		mNumberOfAliveEnemies = 0;
		mAreEnemiesLeft = true;
	}

	void EnemyManager::CreateDeviceDependentResources()
	{
		for (auto& enemy : mEnemies)
		{
			enemy->CreateDeviceDependentResources();
		}
	}

	void EnemyManager::CreateWindowSizeDependentResources()
	{
		for (auto& enemy : mEnemies)
		{
			enemy->CreateWindowSizeDependentResources();
		}
	}

	void EnemyManager::ReleaseDeviceDependentResources()
	{
		for (auto& enemy : mEnemies)
		{
			enemy->ReleaseDeviceDependentResources();
		}
	}

	void EnemyManager::Update(const StepTimer& timer)
	{
		SpawnNewEnemies(timer);
		UpdateEnemyMovement(timer);
	}

	void EnemyManager::SpawnNewEnemies(const StepTimer& timer)
	{
		if (mAreEnemiesLeft)
		{
			mLevelTime += (timer.GetElapsedMilliSeconds() / 1000.0f);
			if (mLevelTime > mCurrentLevelEnemies.mEnemiesArray[mEnemyIndex].mSpawnTime)
			{
				InstantiateEnemy(mCurrentLevelEnemies.mEnemiesArray[mEnemyIndex].mEnemyType, mCurrentLevelEnemies.mEnemiesArray[mEnemyIndex].mIsSpawnLeft);
				mEnemyIndex++;
				mNumberOfEnemies++;
				mNumberOfAliveEnemies++;

				if (mEnemyIndex > mCurrentLevelEnemies.mEnemySize - 1)
				{
					mAreEnemiesLeft = false;
				}
			}
		}

		if (mDeleteEnemies)
		{
			for (int32_t enemyToDelete : mEnemiesToDelete)
			{
				for (int j = 0; j < mNumberOfEnemies; j++)
				{
					if (enemyToDelete == mEnemies[j]->mID)
					{
						mEnemies.erase(mEnemies.begin() + j);
						--mNumberOfEnemies;
						break;
					}
				}
			}
			mDeleteEnemies = false;
			mEnemiesToDelete.clear();
		}
	}

	void EnemyManager::UpdateEnemyMovement(const StepTimer& timer)
	{
		for (auto& enemy : mEnemies)
		{
			enemy->Update(timer);
		}
	}

	void EnemyManager::Render(const StepTimer& timer)
	{
		for (auto& enemy : mEnemies)
		{
			enemy->Render(timer);
		}
	}
}