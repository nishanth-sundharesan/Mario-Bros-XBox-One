#pragma once
#include "DrawableGameComponent.h"

namespace GameEntity
{
	class Player;
	struct EnemySpawner;
}

namespace Managers
{
	class EnemyManager;

	class LevelManager final : public DX::DrawableGameComponent
	{	
	public:
		LevelManager(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, std::shared_ptr<GameEntity::Player>& player);
		LevelManager(const LevelManager&) = delete;
		LevelManager& operator=(const LevelManager&) = delete;
		LevelManager(LevelManager&&) = delete;
		LevelManager& operator=(LevelManager&&) = delete;
		~LevelManager() = default;

		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;
		void SetEnemyManager(const std::shared_ptr<EnemyManager>& enemyManager);

		void GetReadyForNextLevel();
		void LoadNextLevel();
		void ResetLevels();
		void ResetLevelManager();
	private:
		std::vector<GameEntity::EnemySpawner> mGameEmenies;
		std::int32_t mCurrentLevel;

		std::shared_ptr<EnemyManager> mEnemyManager;
		std::shared_ptr<GameEntity::Player> mPlayer;

		bool mIsDisplayPhaseText;
		bool mIsLoadNewLevel;
		float mLoadNewLevelDuration;
		float mPhaseDisplayDuration;
		void ResetPhaseTextVariables();
		void ResetLoadNewLevelVariables();

		//Level Manager constants
		static const float sPHASE_TEXT_DISPLAY_DURATION;
		static const float sLOAD_NEW_PHASE_DURATION;
	};
}