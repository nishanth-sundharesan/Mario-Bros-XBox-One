#include "pch.h"
#include "PhysicsManager.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "Player.h"
#include "Blocks.h"

using namespace std;
using namespace DX;
using namespace GameEntity;

namespace Managers
{
	const float PhysicsManager::sCOLLISION_OFFSET_HEIGHT = 2;
	const float PhysicsManager::sFLOOR_HEIGHT = 30;

	PhysicsManager::PhysicsManager(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::weak_ptr<EnemyManager>& enemyManager, const std::weak_ptr<GameEntity::Player>& player,
		const std::weak_ptr<GameEntity::Blocks>& blocks)
		:GameComponent(deviceResources), mEnemyManager(enemyManager), mPlayer(player), mBlocks(blocks)
	{
	}

	void PhysicsManager::Update(const StepTimer& timer)
	{
		timer;
		UpdatePlayerPhysics();
		UpdateEnemyPhysics();
		UpdatePlayerEnemyCollisions();
	}

	void PhysicsManager::AddPlatformCollisionBoxes(float posX, float posY, float width, float height, GameEntity::BlockType blockType)
	{
		CollisionBoxes collisionBox;
		collisionBox.mPosX = posX;
		collisionBox.mPosY = posY;
		collisionBox.mWidth = width;
		collisionBox.mHeight = height;
		collisionBox.mBlockType = blockType;

		mPlatformBoxes.push_back(collisionBox);
	}

	void PhysicsManager::UpdatePlayerPhysics()
	{
		auto player = mPlayer.lock();
		auto blocks = mBlocks.lock();

		if (player && blocks)
		{
			if (player->mCurrentState == PlayerState::RUNNING || player->mCurrentState == PlayerState::SLIPPING)
			{
				/**************** Checking collision with the floor *******************/
				if (player->mPosY - (player->sHEIGHT + sCOLLISION_OFFSET_HEIGHT) < sFLOOR_HEIGHT)
				{
					return;
				}
				/*********************************************************************/

				/*************** Checking collision with the platform blocks ******************/
				bool isCollided = false;
				for (auto& platformBox : mPlatformBoxes)
				{
					if (platformBox.mPosX + platformBox.mWidth > player->mPosX + player->sHALFWIDTH &&
						platformBox.mPosX < player->mPosX + player->sHALFWIDTH &&
						platformBox.mPosY - platformBox.mHeight < player->mPosY &&
						platformBox.mPosY > player->mPosY - (player->sHEIGHT + sCOLLISION_OFFSET_HEIGHT))
					{
						isCollided = true;
						break;
					}
				}
				/*********************************************************************/

				if (!isCollided)
				{
					player->PlayerFalling(player->mCurrentState);
				}
			}
			else if (player->mCurrentState == PlayerState::RUNJUMPING ||
				player->mCurrentState == PlayerState::STRAIGHTJUMPING ||
				player->mCurrentState == PlayerState::FALLING ||
				player->mCurrentState == PlayerState::SLIPFALLING)
			{
				/**************** Checking collision with the floor *******************/
				if (player->mPosY - player->sHEIGHT < sFLOOR_HEIGHT)
				{
					player->CollidedWithFloor(sFLOOR_HEIGHT + player->sHEIGHT);
					return;
				}
				/*********************************************************************/

				/*************** Checking collision with the platform blocks ******************/
				for (auto& platformBox : mPlatformBoxes)
				{
					if (platformBox.mPosX + platformBox.mWidth > player->mPosX + player->sHALFWIDTH &&
						platformBox.mPosX < player->mPosX + player->sHALFWIDTH &&
						platformBox.mPosY - platformBox.mHeight < player->mPosY &&
						platformBox.mPosY > player->mPosY - player->sHEIGHT)
					{
						if (player->mYVelocity >= 0 && platformBox.mPosY > player->mPosY)
						{
							player->PlayerAttacked(platformBox.mPosY - platformBox.mHeight);
							blocks->SetFlexibleBlocks(player->mPosX + player->sHALFWIDTH, platformBox.mPosY, platformBox.mBlockType);
							AttackEnemy(player);
							return;
						}
						else if (player->mYVelocity < 0 && platformBox.mPosY - platformBox.mHeight < player->mPosY - player->sHEIGHT)
						{
							player->CollidedWithFloor(platformBox.mPosY + player->sHEIGHT);
							return;
						}
					}
				}
				/*********************************************************************/
			}
		}
	}

	void PhysicsManager::UpdateEnemyPhysics()
	{
		if (auto enemyManager = mEnemyManager.lock())
		{
			vector<shared_ptr<Enemy>>& enemies = enemyManager->GetCurrentLevelenemies();

			int32_t enemyIndex = 0;
			for (auto& enemy : enemies)
			{
				switch (enemy->mEnemyType)
				{
				case EnemyType::DUCK:
					UpdateDuckAndCrabPhysics(*enemy, enemyIndex, enemies);
					break;
				case  EnemyType::CRAB:
					UpdateDuckAndCrabPhysics(*enemy, enemyIndex, enemies);
					break;
				case  EnemyType::BEE:
					break;
				}
				++enemyIndex;
			}
		}
	}

	void PhysicsManager::UpdatePlayerEnemyCollisions()
	{
		auto player = mPlayer.lock();
		auto enemyManager = mEnemyManager.lock();

		if (player && enemyManager)
		{
			vector<shared_ptr<Enemy>>& enemies = enemyManager->GetCurrentLevelenemies();

			if (player->mCurrentState != PlayerState::DEAD && player->mCurrentState != PlayerState::DEADJUMPING)
			{
				for (auto& enemy : enemies)
				{
					if (player->mPosX + player->sHALFWIDTH >= enemy->mPosX &&
						player->mPosX <= enemy->mPosX + enemy->sHALF_WIDTH &&
						player->mPosY - player->sHEIGHT <= enemy->mPosY &&
						player->mPosY >= enemy->mPosY - enemy->sENEMY_HEIGHT)
					{
						if (enemy->mCurrentState == EnemyState::FLIPPED)
						{
							enemy->GotKilled(player->mPosX > enemy->mPosX ? false : true);
							player->IncrementKillScore();
						}
						else if (enemy->mCurrentState != EnemyState::DEAD)
						{
							player->GotKilled();
						}
					}
				}
			}
		}
	}

	void PhysicsManager::UpdateDuckAndCrabPhysics(Enemy& enemy, int32_t currentEnemyIndex, vector<shared_ptr<Enemy>>& enemies)
	{		
		bool isCollided = false;
		int32_t enemyIndex = 0;

		switch (enemy.mCurrentState)
		{
		case EnemyState::MOVING:

			
			for (auto& otherEnemy : enemies)
			{
				if (enemyIndex != currentEnemyIndex && otherEnemy->mCurrentState != EnemyState::DEAD &&
					enemy.mPosX + enemy.sENEMY_WIDTH >= otherEnemy->mPosX &&
					enemy.mPosX <= otherEnemy->mPosX + otherEnemy->sENEMY_WIDTH &&
					enemy.mPosY - enemy.sENEMY_HEIGHT <= otherEnemy->mPosY &&
					enemy.mPosY >= otherEnemy->mPosY - otherEnemy->sENEMY_HEIGHT)
				{
					if (enemy.mIsMovingRight)
					{
						if (enemy.mPosX < otherEnemy->mPosX)
						{
							enemy.CollidedWithAnotherObject();
						}
					}
					else
					{
						if (enemy.mPosX > otherEnemy->mPosX)
						{
							enemy.CollidedWithAnotherObject();
						}
					}
				}
				++enemyIndex;
			}

			if (enemy.mIsMovingRight)
			{
				if (enemy.mPosY < Enemy::sENEMY_ESCAPE_POS_Y && enemy.mPosX > Enemy::sENEMY_ESCAPE_RIGHT_POS_X)
				{
					enemy.EscapeFromPipe(Enemy::sPIPE_HEIGHT);
					return;
				}
			}
			else
			{
				if (enemy.mPosY < Enemy::sENEMY_ESCAPE_POS_Y && enemy.mPosX < Enemy::sENEMY_ESCAPE_LEFT_POS_X)
				{
					enemy.EscapeFromPipe(Enemy::sPIPE_HEIGHT);
					return;
				}
			}

			/**************** Checking collision with the floor *******************/
			if (enemy.mPosY - (enemy.sENEMY_HEIGHT + sCOLLISION_OFFSET_HEIGHT) < sFLOOR_HEIGHT)
			{
				return;
			}
			/*********************************************************************/

			for (auto& platformBox : mPlatformBoxes)
			{
				if (platformBox.mPosX + platformBox.mWidth > enemy.mPosX + enemy.sHALF_WIDTH &&
					platformBox.mPosX < enemy.mPosX + enemy.sHALF_WIDTH &&
					platformBox.mPosY - platformBox.mHeight < enemy.mPosY &&
					platformBox.mPosY > enemy.mPosY - (enemy.sENEMY_HEIGHT + sCOLLISION_OFFSET_HEIGHT))
				{
					isCollided = true;
					break;
				}				
			}

			if (!isCollided)
			{
				enemy.FellFromFloor();
				return;
			}

			break;

		case EnemyState::FALLING:
			/**************** Checking collision with the floor *******************/
			if (enemy.mPosY - enemy.sENEMY_HEIGHT < sFLOOR_HEIGHT)
			{
				enemy.CollidedWithFloor(sFLOOR_HEIGHT + enemy.sENEMY_HEIGHT, enemy.mCurrentState);
				return;
			}
			/*********************************************************************/


			/*************** Checking collision with the platform boxes ******************/
			for (auto& platformBox : mPlatformBoxes)
			{
				if (platformBox.mPosX + platformBox.mWidth > enemy.mPosX + enemy.sHALF_WIDTH &&
					platformBox.mPosX < enemy.mPosX + enemy.sHALF_WIDTH &&
					platformBox.mPosY - platformBox.mHeight < enemy.mPosY &&
					platformBox.mPosY > enemy.mPosY - enemy.sENEMY_HEIGHT)
				{
					enemy.CollidedWithFloor(platformBox.mPosY + enemy.sENEMY_HEIGHT, enemy.mCurrentState);
					return;
				}				
			}
			/*********************************************************************/
			break;
		case EnemyState::FLIPPED:

			/**************** Checking collision with the floor *******************/
			if (enemy.mPosY - enemy.sENEMY_HEIGHT < sFLOOR_HEIGHT)
			{
				enemy.CollidedWithFloor(sFLOOR_HEIGHT + enemy.sENEMY_HEIGHT, enemy.mCurrentState);
				return;
			}
			/*********************************************************************/

			//If no collision happens between the blocks and the duck, then the "enemy.mCollidedWithTheBlock" is reset to false.
			if (enemy.mCollidedWithTheBlock)
			{
				/*************** Checking collision with the platform boxes ******************/
				for (auto& platformBox : mPlatformBoxes)
				{
					if (platformBox.mPosX + platformBox.mWidth > enemy.mPosX &&
						platformBox.mPosX < enemy.mPosX + enemy.sENEMY_WIDTH &&
						platformBox.mPosY - platformBox.mHeight < enemy.mPosY &&
						platformBox.mPosY > enemy.mPosY - enemy.sENEMY_HEIGHT)
					{
						return;
					}					
				}

				enemy.mCollidedWithTheBlock = false;
				/*********************************************************************/
			}
			else
			{
				/*************** Checking collision with the platform boxes ******************/
				for (auto& platformBox : mPlatformBoxes)
				{
					if (platformBox.mPosX + platformBox.mWidth > enemy.mPosX &&
						platformBox.mPosX < enemy.mPosX + enemy.sENEMY_WIDTH &&
						platformBox.mPosY - platformBox.mHeight < enemy.mPosY &&
						platformBox.mPosY > enemy.mPosY - enemy.sENEMY_HEIGHT)
					{
						//This condition is added to make sure that the collision doesn't work when a fast moving enemy is flipped and the flipped enemy enters the collision box of a block from the side angle.
						//To check for this condition, on the first collision of the duck with the block, we check if the enemy's (x + half width) is greater then the x of the block for the right blocks (or x + it's width for the left blocks).
						//If it is greater, then the duck has collided with the block or else, the enemy just passes through the blocks
						if (platformBox.mPosX + platformBox.mWidth > enemy.mPosX + enemy.sHALF_WIDTH && platformBox.mPosX < enemy.mPosX + enemy.sHALF_WIDTH)
						{
							enemy.CollidedWithFloor(platformBox.mPosY + enemy.sENEMY_HEIGHT, enemy.mCurrentState);
							return;
						}
						else
						{
							enemy.mCollidedWithTheBlock = true;
							return;
						}
					}					
				}
				/*********************************************************************/
			}

			break;
		case EnemyState::DEAD:
			if (enemy.mPosY < 0)
			{
				enemy.RemoveEnemy();
			}
			break;
		case EnemyState::SPAWNING:
			if (enemy.mIsMovingRight)
			{
				if (enemy.mPosX > Enemy::sENEMY_SPAWN_BOX_LEFT_X)
				{
					enemy.FellFromFloor();
				}
			}
			else
			{
				if (enemy.mPosX < Enemy::sENEMY_SPAWN_BOX_RIGHT_X)
				{
					enemy.FellFromFloor();
				}
			}
			break;
		case EnemyState::RESPAWNING:
			if (enemy.mIsMovingRight)
			{
				if (enemy.mPosX > Enemy::sENEMY_RESPAWN_RIGHT_POS_X)
				{
					enemy.ReInitEnemyPositionAndState();
				}
			}
			else
			{
				if (enemy.mPosX < Enemy::sENEMY_RESPAWN_LEFT_POS_X)
				{
					enemy.ReInitEnemyPositionAndState();
				}
			}
			break;
		}
	}

	void PhysicsManager::AttackEnemy(shared_ptr<Player>& player)
	{
		if (auto enemyManager = mEnemyManager.lock())
		{
			vector<shared_ptr<Enemy>>& enemies = enemyManager->GetCurrentLevelenemies();

			float attackPositionStartX = player->mPosX - Blocks::sBLOCK_HALF_WIDTH;
			float attackPositionEndX = attackPositionStartX + (Blocks::sTOTAL_FLEXIBLE_BLOCKS  * Blocks::sBLOCK_WIDTH);

			float attackPositionStartY = player->mPosY + Blocks::sBLOCK_HEIGHT + sCOLLISION_OFFSET_HEIGHT;
			float attackPositionEndY = attackPositionStartY - Blocks::sBLOCK_HEIGHT;

			for (uint32_t i = 0; i < enemies.size(); i++)
			{
				if (enemies[i]->mCurrentState != EnemyState::DEAD)
				{
					if (attackPositionEndX >= enemies[i]->mPosX &&
						attackPositionStartX <= enemies[i]->mPosX + enemies[i]->sENEMY_WIDTH &&
						attackPositionEndY <= enemies[i]->mPosY &&
						attackPositionStartY >= enemies[i]->mPosY - enemies[i]->sENEMY_HEIGHT)
					{
						if (enemies[i]->mPosX + enemies[i]->sHALF_WIDTH < attackPositionStartX + Blocks::sBLOCK_HALF_WIDTH)
						{
							enemies[i]->HitTaken(FlipDirection::LEFT);
						}
						else if (enemies[i]->mPosX + enemies[i]->sHALF_WIDTH > attackPositionEndX - Blocks::sBLOCK_HALF_WIDTH)
						{
							enemies[i]->HitTaken(FlipDirection::RIGHT);
						}
						else
						{
							enemies[i]->HitTaken(FlipDirection::STRAIGHT);
						}
					}
				}
			}
		}
	}
}