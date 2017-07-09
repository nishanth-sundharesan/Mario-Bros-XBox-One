#pragma once
#include "DrawableGameComponent.h"
#include "Enemy.h"

namespace Managers
{
	class LevelManager;

	class EnemyManager final : public DX::DrawableGameComponent
	{
	public:
		EnemyManager(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, std::shared_ptr<LevelManager>& levelManager);
		EnemyManager(const EnemyManager&) = delete;
		EnemyManager& operator=(const EnemyManager&) = delete;
		EnemyManager(EnemyManager&&) = delete;
		EnemyManager& operator=(EnemyManager&&) = delete;
		~EnemyManager() = default;

		virtual void CreateDeviceDependentResources() override;
		virtual void CreateWindowSizeDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;

		void ResetEnemyManager();
		void SetNewPhaseEnemies(const GameEntity::EnemySpawner& enemies);
		void SpawnNewEnemies(const DX::StepTimer& timer);
		void UpdateEnemyMovement(const DX::StepTimer& timer);		

		//TODO write a const function too
		std::vector<std::shared_ptr<GameEntity::Enemy>>& GetCurrentLevelenemies();
		void RemoveEnemy(std::int32_t enemyID);
		bool CheckIfLastEnemy();
	private:
		GameEntity::EnemySpawner mCurrentLevelEnemies;		
		bool mAreEnemiesLeft;
		bool mDeleteEnemies;

		std::int32_t mEnemyIndex;
		std::int32_t mNumberOfEnemies;
		std::int32_t mNumberOfAliveEnemies;
		float mLevelTime;
		std::shared_ptr<LevelManager> mLevelManager;		

		std::vector<std::shared_ptr<GameEntity::Enemy>> mEnemies;
		std::vector<std::int32_t> mEnemiesToDelete;
		void InstantiateEnemy(GameEntity::EnemyType enemyType, bool isSpawnLeft);
	};
}