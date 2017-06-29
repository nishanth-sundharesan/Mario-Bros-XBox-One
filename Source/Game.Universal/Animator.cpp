#include "pch.h"
#include "Animator.h"
#include "XmlParseHelper.h"

using namespace std;

namespace Animation
{
	Animator* Animator::sInstance = nullptr;

	Animator::~Animator()
	{
		sInstance = nullptr;
	}

	const Sprites& Animator::GetCurrentStageBlock() const
	{
		return mBlueBlock;
	}

	const Sprites& Animator::GetCurrentStageFlexibleBlock() const
	{
		return mBlueBlockFlexible;
	}

	void Animator::GetDuckSprites(DuckSprites& duckSprites, EnemyStage currentDuckStage) const
	{
		switch (currentDuckStage)
		{
		case EnemyStage::GREEN:
			duckSprites.mMoving = mGreenDuckMoving;
			duckSprites.mFlipped = mGreenDuckFlipped;
			duckSprites.mLookAround = mGreenDuckLookAround;
			break;
		case EnemyStage::BLUE:
			duckSprites.mMoving = mBlueDuckMoving;
			duckSprites.mFlipped = mBlueDuckFlipped;
			duckSprites.mLookAround = mBlueDuckLookAround;
			break;
		case EnemyStage::RED:
			duckSprites.mMoving = mRedDuckMoving;
			duckSprites.mFlipped = mRedDuckFlipped;
			duckSprites.mLookAround = mRedDuckLookAround;
			break;
		}
	}

	void Animator::GetCrabSprites(CrabSprites& crabSprites, EnemyStage currentCrabStage) const
	{
		switch (currentCrabStage)
		{
		case EnemyStage::GREEN:
			crabSprites.mMoving = mGreenCrabMoving;
			crabSprites.mAngryMoving = mGreenCrabAngryMoving;
			crabSprites.mFlipped = mGreenCrabFlipped;
			crabSprites.mLookAround = mGreenCrabLookAround;
			break;
		case EnemyStage::BLUE:
			crabSprites.mMoving = mBlueCrabMoving;
			crabSprites.mAngryMoving = mBlueCrabAngryMoving;
			crabSprites.mFlipped = mBlueCrabFlipped;
			crabSprites.mLookAround = mBlueCrabLookAround;
			break;
		case EnemyStage::RED:
			crabSprites.mMoving = mRedCrabMoving;
			crabSprites.mAngryMoving = mRedCrabAngryMoving;
			crabSprites.mFlipped = mRedCrabFlipped;
			crabSprites.mLookAround = mRedCrabLookAround;
			break;
		}
	}

	shared_ptr<Animator> Animator::GetInstance()
	{
		if (sInstance == nullptr)													//No multi threading, so we can use expired.	
		{
			sInstance = new Animator();
		}
		//sInstance = shared_ptr<Animator>(new Animator());						//Cannot call make_shared() for a class whose constructor is private/protected.		
		return shared_ptr<Animator>(sInstance);
	}

	Animator::Animator()
	{
		Core::XmlParserHelper xmlParseHelper;
		xmlParseHelper.LoadSprites(*this);
	}
}