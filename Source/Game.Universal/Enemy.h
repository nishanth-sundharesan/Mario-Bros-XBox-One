#pragma once
#include "DrawableGameComponent.h"

namespace Managers
{
	class PhysicsManager;
	class EnemyManager;
}

namespace Animation
{
	class Animator;
}

namespace GameEntity
{
	enum class EnemyType
	{
		DUCK,
		CRAB,
		BEE
	};

	enum class EnemyEvolvedForm
	{
		FORM1,
		FORM2,
		FORM3
	};

	enum class EnemyState
	{
		SPAWNING,
		RESPAWNING,
		MOVING,
		FALLING,
		FLIPPED,
		LOOKAROUND,
		DEAD
	};

	enum class FlipDirection
	{
		LEFT,
		RIGHT,
		STRAIGHT
	};

	struct IndividualEnemy
	{
		bool mIsSpawnLeft;
		EnemyType mEnemyType;
		std::int32_t mSpawnTime;
	};

	struct EnemySpawner
	{
		std::int32_t mEnemySize;
		std::vector<IndividualEnemy> mEnemiesArray;
	};

	class Enemy : public DX::DrawableGameComponent
	{
		friend class Managers::PhysicsManager;
	public:
		Enemy(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, bool isSpawnLeft, std::int32_t enemyID,
			Managers::EnemyManager& enemyManager);
		Enemy(const Enemy&) = delete;
		Enemy& operator=(const Enemy&) = delete;
		Enemy(Enemy&&) = delete;
		Enemy& operator=(Enemy&&) = delete;
		virtual ~Enemy() = default;

		virtual void CollidedWithFloor(float newEnemyYPosition, EnemyState previousState) = 0;
		virtual void FellFromFloor() = 0;
		virtual void EscapeFromPipe(float newEnemyYPosition) = 0;
		virtual void ReInitEnemyPositionAndState() = 0;
		virtual void CollidedWithAnotherObject() = 0;
		virtual void HitTaken(FlipDirection flipDirection) = 0;
		virtual void EvolveTheEnemy() = 0;
		virtual void ResetEnemyMovement() = 0;
		virtual void GotKilled(bool isMovingRight) = 0;
		virtual void EvolveToFinalState() = 0;
		
		std::int32_t mID;					//TODO write accessors and mutators for these 2 member variables
		EnemyState mCurrentState;

	protected:
		float mPosX;
		float mPosY;
		float mXDuplicateEnemy;		
		float mXVelocity;
		float mYVelocity;
		float mGravity;		
		float mCurrentStateXVelocity;
		float mCurrentStateGravity;

		bool mCollidedWithTheBlock;
		Managers::EnemyManager& mEnemyManager;
		EnemyType mEnemyType;
		bool mIsMovingRight;
		bool mIsRenderDuplicateSprites;
		std::int32_t mSpriteIndex;

		float mTempDuration;
		float mSpriteSpeed;
		float mFlipSpriteCap;

		EnemyEvolvedForm mCurrentForm;		
		
		void RemoveEnemy();
		void ResetSpriteIndexAndSpeed();		

		const float mOrthoWidth;
		const float mOrthoHeight;

		/******************* Enemy Constants ******************************/
		static const float sENEMY_SPAWN_POS_LEFT_X;
		static const float sENEMY_SPAWN_POS_RIGHT_X;
		static const float sENEMY_SPAWN_POS_Y;

		static const float sENEMY_ESCAPE_LEFT_POS_X;
		static const float sENEMY_ESCAPE_RIGHT_POS_X;
		static const float sENEMY_ESCAPE_POS_Y;

		static const float sENEMY_SPAWN_BOX_LEFT_X;
		static const float sENEMY_SPAWN_BOX_RIGHT_X;
		static const float sENEMY_SPAWN_BOX_Y;

		static const float sPIPE_HEIGHT;
		
		static const float sENEMY_HEIGHT;				//TODO rename it to sHEIGHT
		static const float sENEMY_WIDTH;				//TODO rename it to sWIDTH
		static const float sHALF_WIDTH;		

		static const float sRESPAWN_WIDTH_PADDING;

		static const float sENEMY_RESPAWN_LEFT_POS_X;
		static const float sENEMY_RESPAWN_RIGHT_POS_X;

		static const float sENEMY_DEATH_X_VELOCITY;
		static const float sENEMY_DEATH_Y_VELOCITY;
		static const float sENEMY_DEATH_GRAVITY;

		static const DirectX::XMFLOAT2 sENEMY_SPRITE_SCALE;
		/******************************************************************/
	};
}
