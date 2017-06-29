#pragma once
#include <cstdint>
#include <vector>

namespace Animation
{
	//TODO rename enum class
	enum class EnemyStage
	{
		GREEN,
		BLUE,
		RED
	};

	struct TextureDimensions
	{
		std::int32_t mLength;
		float mSpriteWidth;
		float mSpriteHeight;
	};

	struct SpriteDimensions
	{
		std::int32_t mPosX;
		std::int32_t mPosY;
		std::int32_t mWidth;
		std::int32_t mHeight;
	};

	typedef struct SpriteStruct :TextureDimensions
	{
		std::vector<struct SpriteDimensions> mDimensions;
	}Sprites;

	typedef struct BeeSpriteStruct
	{
		Sprites mMoving;
		Sprites mFlipped;
	}BeeSprites;

	typedef struct DuckSpriteStruct :BeeSpriteStruct
	{
		Sprites mLookAround;
	}DuckSprites;

	typedef struct CrabSpriteStruct :DuckSprites
	{
		Sprites mAngryMoving;
	}CrabSprites;


	class Animator final
	{
	public:				
		Animator(const Animator&) = delete;
		Animator& operator=(const Animator&) = delete;
		Animator(Animator&&) = delete;
		Animator& operator=(Animator&&) = delete;
		~Animator();

		Sprites mMarioIdle;
		Sprites mMarioRunning;
		Sprites mMarioDeath;
		Sprites mMarioSlip;
		Sprites mMarioJump;
		Sprites mMarioDeathByFire;
		Sprites mMarioDeathByIce;
		Sprites mMarioLives;

		Sprites mPaddle;

		Sprites mBlueBlock;
		Sprites mRedBlock;
		Sprites mOrangeWireBlock;
		Sprites mIceBlock;
		Sprites mBlueSmoothBlock;
		Sprites mOrangeSmoothBlock;

		Sprites mBlueBlockFlexible;
		Sprites mRedBlockFlexible;
		Sprites mOrangeWireBlockFlexible;
		Sprites mIceBlockFlexible;
		Sprites mBlueSmoothBlockFlexible;
		Sprites mOrangeSmoothBlockFlexible;

		Sprites mGreenDuckMoving;
		Sprites mGreenDuckFlipped;
		Sprites mGreenDuckLookAround;

		Sprites mBlueDuckMoving;
		Sprites mBlueDuckFlipped;
		Sprites mBlueDuckLookAround;

		Sprites mRedDuckMoving;
		Sprites mRedDuckFlipped;
		Sprites mRedDuckLookAround;

		Sprites mRedCrabMoving;
		Sprites mRedCrabAngryMoving;
		Sprites mRedCrabFlipped;
		Sprites mRedCrabLookAround;

		Sprites mGreenCrabMoving;
		Sprites mGreenCrabAngryMoving;
		Sprites mGreenCrabFlipped;
		Sprites mGreenCrabLookAround;

		Sprites mBlueCrabMoving;
		Sprites mBlueCrabAngryMoving;
		Sprites mBlueCrabFlipped;
		Sprites mBlueCrabLookAround;

		Sprites mRedBeeMoving;
		Sprites mRedBeeFlipped;

		Sprites mYellowBeeMoving;
		Sprites mYellowBeeFlipped;

		Sprites mBlueBeeMoving;
		Sprites mBlueBeeFlipped;

		Sprites mNumerals;
		Sprites mAlphabets;

		const Sprites& GetCurrentStageBlock() const;
		const Sprites& GetCurrentStageFlexibleBlock() const;
		void GetDuckSprites(DuckSprites& duckSprites, EnemyStage currentDuckStage) const;
		void GetCrabSprites(CrabSprites& crabSprites, EnemyStage currentCrabStage) const;
		
		static std::shared_ptr<Animator> GetInstance();
		
	private:
		Animator();
		static Animator* sInstance;
	};
}