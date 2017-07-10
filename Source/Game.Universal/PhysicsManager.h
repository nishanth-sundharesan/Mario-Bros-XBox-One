#pragma once
#include <vector>

namespace GameEntity
{	
	class Player;
	class Blocks;
	class Enemy;
	enum class BlockType;
}

namespace Managers
{
	class EnemyManager;

	//TODO Generalize this
	struct CollisionBoxes
	{
		float mPosX;
		float mPosY;
		float mHeight;
		float mWidth;		
		GameEntity::BlockType mBlockType;		
	};

	class PhysicsManager final: public DX::GameComponent
	{
	public:
		PhysicsManager(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::weak_ptr<EnemyManager>& enemyManager, const std::weak_ptr<GameEntity::Player>& player, const std::weak_ptr<GameEntity::Blocks>& blocks);
		PhysicsManager(const PhysicsManager&) = delete;
		PhysicsManager& operator=(const PhysicsManager&) = delete;
		PhysicsManager(PhysicsManager&&) = delete;
		PhysicsManager& operator=(PhysicsManager&&) = delete;
		~PhysicsManager() = default;		

		virtual void Update(const DX::StepTimer& timer) override;
		void AddPlatformCollisionBoxes(float posX, float posY, float width, float height, GameEntity::BlockType blockType);
		//void CheckPlayerEnemyCollisions(Player* player, std::vector<Enemy*> &enemies);
	private:
		void UpdatePlayerPhysics();
		void UpdateEnemyPhysics();
		void UpdatePlayerEnemyCollisions();
		void UpdateDuckAndCrabPhysics(GameEntity::Enemy& enemy, std::int32_t currentEnemyIndex, std::vector<std::shared_ptr<GameEntity::Enemy>>& enemies);

		std::vector<CollisionBoxes> mPlatformBoxes;
		
		std::weak_ptr<GameEntity::Player> mPlayer;
		std::weak_ptr<GameEntity::Blocks> mBlocks;
		std::weak_ptr<EnemyManager> mEnemyManager;

		static const float sCOLLISION_OFFSET_HEIGHT;
		static const float sFLOOR_HEIGHT;
		void AttackEnemy(std::shared_ptr<GameEntity::Player>& player);
	};
}