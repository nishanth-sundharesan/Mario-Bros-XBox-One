#include "pch.h"
#include "Enemy.h"
#include "Animator.h"
#include "EnemyManager.h"

using namespace std;
using namespace DX;
using namespace DirectX;
using namespace Animation;
using namespace Managers;

namespace GameEntity
{
	const float Enemy::sENEMY_SPAWN_POS_LEFT_X = 68.0f;
	const float Enemy::sENEMY_SPAWN_POS_RIGHT_X = 418.0f;
	const float Enemy::sENEMY_SPAWN_POS_Y = 391.0f;

	const float Enemy::sENEMY_ESCAPE_LEFT_POS_X = 67.0f;
	const float Enemy::sENEMY_ESCAPE_RIGHT_POS_X = 420.0f;
	const float Enemy::sENEMY_ESCAPE_POS_Y = 85.0f;

	const float Enemy::sENEMY_SPAWN_BOX_LEFT_X = 93.0f;
	const float Enemy::sENEMY_SPAWN_BOX_RIGHT_X = 395.0f;
	const float Enemy::sENEMY_SPAWN_BOX_Y = 359.0f;

	const float Enemy::sPIPE_HEIGHT = 80.0f;

	const float Enemy::sENEMY_HEIGHT = 32.0f;
	const float Enemy::sENEMY_WIDTH = 26.0f;
	const float Enemy::sHALF_WIDTH = sENEMY_WIDTH / 2.0f;

	const float Enemy::sRESPAWN_WIDTH_PADDING = 6.0f;

	const float Enemy::sENEMY_RESPAWN_LEFT_POS_X = sENEMY_ESCAPE_LEFT_POS_X - (sENEMY_WIDTH + sRESPAWN_WIDTH_PADDING);
	const float Enemy::sENEMY_RESPAWN_RIGHT_POS_X = sENEMY_ESCAPE_RIGHT_POS_X + (sENEMY_WIDTH + sRESPAWN_WIDTH_PADDING);

	const float Enemy::sENEMY_DEATH_X_VELOCITY = 125.0f;
	const float Enemy::sENEMY_DEATH_Y_VELOCITY = 200.0f;
	const float Enemy::sENEMY_DEATH_GRAVITY = -600.0f;

	const XMFLOAT2 Enemy::sENEMY_SPRITE_SCALE(sENEMY_WIDTH, sENEMY_HEIGHT);

	Enemy::Enemy(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera, bool isSpawnLeft, int32_t enemyID, EnemyManager& enemyManager)
		:DrawableGameComponent(deviceResources, camera), mIsMovingRight(isSpawnLeft), mID(enemyID), mEnemyManager(enemyManager),
		mOrthoWidth(OrthographicCamera::DefaultViewWidth), mOrthoHeight(OrthographicCamera::DefaultViewHeight), mCollidedWithTheBlock(false)
	{		
		ResetSpriteIndexAndSpeed();
	}

	void Enemy::RemoveEnemy()
	{
		mEnemyManager.RemoveEnemy(mID);
	}

	void Enemy::ResetSpriteIndexAndSpeed()
	{
		mSpriteSpeed = 0.0f;
		mSpriteIndex = 0;
	}
}