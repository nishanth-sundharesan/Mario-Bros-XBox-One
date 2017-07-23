#include "pch.h"
#include "XmlParseHelper.h"
#include "tinyxml2.h"

using namespace std;
using namespace Animation;
using namespace GameEntity;
using namespace tinyxml2;

namespace Core
{
	//TODO check xml files once
	const string XmlParserHelper::sMARIO_BROS_SPRITE_XML = "Content\\XmlFiles\\MarioBrosSprite.xml";
	const string XmlParserHelper::sMARIO_BROS_LEVELS_XML = "Content\\XmlFiles\\MarioBrosLevels.xml";

	void XmlParserHelper::LoadSprites(Animator& animator) const
	{
		tinyxml2::XMLDocument xmlDoc;
		xmlDoc.LoadFile(sMARIO_BROS_SPRITE_XML.c_str());
		int width, height;

		function<SpriteHandlerParameters> GetAppropriateSprites;

		/*************************************Reading Mario Sprites**************************************************/
		XMLElement* parentMetaElement = xmlDoc.FirstChildElement()->FirstChildElement();
		parentMetaElement->QueryIntAttribute("width", &width);
		parentMetaElement->QueryIntAttribute("height", &height);
		XMLElement* metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetMarioSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/***********************************************************************************************************/


		/*************************************Reading Block Sprites**************************************************/
		parentMetaElement = parentMetaElement->NextSiblingElement();
		parentMetaElement->QueryIntAttribute("width", &width);
		parentMetaElement->QueryIntAttribute("height", &height);
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetBlockSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/***********************************************************************************************************/


		/*************************************Reading Enemy Sprites**************************************************/
		/**************Reading Duck Sprites******************/
		/*********Reading Green Duck Sprites********/
		parentMetaElement = parentMetaElement->NextSiblingElement();
		parentMetaElement->QueryIntAttribute("width", &width);
		parentMetaElement->QueryIntAttribute("height", &height);
		parentMetaElement = parentMetaElement->FirstChildElement();																			//Getting inside the Enemy element	
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetGreenDuckSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/*******************************************/

		/********Reading Blue Duck Sprites***********/
		parentMetaElement = parentMetaElement->NextSiblingElement();
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetBlueDuckSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/*******************************************/

		/********Reading Red Duck Sprites***********/
		parentMetaElement = parentMetaElement->NextSiblingElement();
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetRedDuckSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/*******************************************/
		/****************************************************/


		/**************Reading Crab Sprites******************/
		/*********Reading Red Crab Sprites********/
		parentMetaElement = parentMetaElement->NextSiblingElement();
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetRedCrabSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/*****************************************/

		/*********Reading Green Crab Sprites********/
		parentMetaElement = parentMetaElement->NextSiblingElement();
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetGreenCrabSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/*******************************************/

		/*********Reading Blue Crab Sprites********/
		parentMetaElement = parentMetaElement->NextSiblingElement();
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetBlueCrabSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/*******************************************/
		/****************************************************/


		/***************Reading Bee Sprites******************/
		/*********Reading Red Bee Sprites********/
		parentMetaElement = parentMetaElement->NextSiblingElement();
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetRedBeeSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/***************************************/

		/*******Reading Yellow Bee Sprites******/
		parentMetaElement = parentMetaElement->NextSiblingElement();
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetYellowBeeSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/***************************************/

		/********Reading Blue Bee Sprites*******/
		parentMetaElement = parentMetaElement->NextSiblingElement();
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetBlueBeeSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/***************************************/
		/****************************************************/
		/************************************************************************************************************/


		/****************************************Reading Numerals****************************************************/
		parentMetaElement = parentMetaElement->Parent()->NextSiblingElement();
		parentMetaElement->QueryIntAttribute("width", &width);
		parentMetaElement->QueryIntAttribute("height", &height);
		metaElement = parentMetaElement->FirstChildElement();

		GetAppropriateSprites = bind(&XmlParserHelper::GetTextSprites, this, std::placeholders::_1, std::placeholders::_2);
		LoadIndividualSpriteData(GetAppropriateSprites, metaElement, width, height, animator);
		/************************************************************************************************************/
	}

	void XmlParserHelper::LoadLevels(vector<EnemySpawner>& gameEmenies) const
	{
		tinyxml2::XMLDocument xmlDoc;
		xmlDoc.LoadFile(sMARIO_BROS_LEVELS_XML.c_str());

		XMLElement* phaseElement = xmlDoc.FirstChildElement()->FirstChildElement();
		XMLElement* xmlElement;

		while (phaseElement != NULL)
		{
			EnemySpawner enemies;
			//Query block type here

			xmlElement = phaseElement->FirstChildElement();
			xmlElement->QueryIntAttribute("size", &enemies.mEnemySize);
			xmlElement = xmlElement->FirstChildElement();

			while (xmlElement != NULL)
			{
				IndividualEnemy individualEnemy;

				individualEnemy.mEnemyType = GetEnemyType(xmlElement->Attribute("type"));				
				xmlElement->QueryIntAttribute("spawnTime", &individualEnemy.mSpawnTime);
				individualEnemy.mIsSpawnLeft = IsSpawnLeftOrRight(xmlElement->Attribute("spawnPosition"));

				enemies.mEnemiesArray.push_back(individualEnemy);
				xmlElement = xmlElement->NextSiblingElement();
			}

			gameEmenies.push_back(enemies);
			phaseElement = phaseElement->NextSiblingElement();
		}
	}

	void XmlParserHelper::LoadIndividualSpriteData(function<SpriteHandlerParameters> spriteHandler, XMLElement* metaElement, int32_t width, int32_t height, Animator& animator) const
	{
		while (metaElement != NULL)
		{
			int32_t size;// i = 0;
			metaElement->QueryIntAttribute("size", &size);

			Sprites& commonSprites = spriteHandler(metaElement->Attribute("name"), animator);
			commonSprites.mDimensions.reserve(size);

			XMLElement* spriteElement = metaElement->FirstChildElement();
			while (spriteElement != NULL)
			{
				commonSprites.mSpriteWidth = static_cast<float>(width);
				commonSprites.mSpriteHeight = static_cast<float>(height);
				commonSprites.mLength = size;

				SpriteDimensions spriteDimensions;
				spriteElement->QueryIntAttribute("x", &(spriteDimensions.mPosX));
				spriteElement->QueryIntAttribute("y", &(spriteDimensions.mPosY));
				spriteElement->QueryIntAttribute("w", &(spriteDimensions.mWidth));
				spriteElement->QueryIntAttribute("h", &(spriteDimensions.mHeight));
				commonSprites.mDimensions.push_back(spriteDimensions);

				spriteElement = spriteElement->NextSiblingElement();
				//i++;
			}
			metaElement = metaElement->NextSiblingElement();
		}
	}

	EnemyType XmlParserHelper::GetEnemyType(const char* name) const
	{
		if (strcmp("Duck", name) == 0)
		{
			return EnemyType::DUCK;
		}
		else if (strcmp("Crab", name) == 0)
		{
			return EnemyType::CRAB;
		}
		else
		{
			//TODO fox this
			return EnemyType::BEE;							//if (strcmp("Bee", name) == 0)
		}
	}

	bool XmlParserHelper::IsSpawnLeftOrRight(const char* position) const
	{
		if (strcmp(position, "Left") == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	Sprites& XmlParserHelper::GetMarioSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Idle", name) == 0)
		{
			return animator.mMarioIdle;
		}
		else if (strcmp("Running", name) == 0)
		{
			return animator.mMarioRunning;
		}
		else if (strcmp("Death", name) == 0)
		{
			return animator.mMarioDeath;
		}
		else if (strcmp("Slip", name) == 0)
		{
			return animator.mMarioSlip;
		}
		else if (strcmp("Jump", name) == 0)
		{
			return animator.mMarioJump;
		}
		else if (strcmp("DeathByFire", name) == 0)
		{
			return animator.mMarioDeathByFire;
		}
		else if (strcmp("DeathByIce", name) == 0)
		{
			return animator.mMarioDeathByIce;
		}
		else if (strcmp("Lives", name) == 0)
		{
			return animator.mMarioLives;
		}
		else
		{
			//TODO fix this
			return animator.mPaddle;							//if (strcmp("Paddle", name) == 0)
		}
	}

	Sprites& XmlParserHelper::GetBlockSprites(const char* name, Animator& animator) const
	{
		if (strcmp("BlueBlock", name) == 0)
		{
			return animator.mBlueBlock;
		}
		else if (strcmp("RedBlock", name) == 0)
		{
			return animator.mRedBlock;
		}
		else if (strcmp("OrangeWireBlock", name) == 0)
		{
			return animator.mOrangeWireBlock;
		}
		else if (strcmp("IceBlock", name) == 0)
		{
			return animator.mIceBlock;
		}
		else if (strcmp("BlueSmoothBlock", name) == 0)
		{
			return animator.mBlueSmoothBlock;
		}
		else if (strcmp("OrangeSmoothBlock", name) == 0)
		{
			return animator.mOrangeSmoothBlock;
		}
		else if (strcmp("BlueBlockFlexible", name) == 0)
		{
			return animator.mBlueBlockFlexible;
		}
		else if (strcmp("RedBlockFlexible", name) == 0)
		{
			return animator.mRedBlockFlexible;
		}
		else if (strcmp("OrangeWireBlockFlexible", name) == 0)
		{
			return animator.mOrangeWireBlockFlexible;
		}
		else if (strcmp("IceBlockFlexible", name) == 0)
		{
			return animator.mIceBlockFlexible;
		}
		else if (strcmp("BlueSmoothBlockFlexible", name) == 0)
		{
			return animator.mBlueSmoothBlockFlexible;
		}
		else
		{
			//TODO fix this
			return animator.mOrangeSmoothBlockFlexible;					//if (strcmp("OrangeSmoothBlockFlexible", name) == 0)
		}
	}

	Sprites& XmlParserHelper::GetGreenDuckSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Moving", name) == 0)
		{
			return animator.mGreenDuckMoving;
		}
		else if (strcmp("Flipped", name) == 0)
		{
			return animator.mGreenDuckFlipped;
		}
		else
		{
			//TODO fix this
			return animator.mGreenDuckLookAround;						//if (strcmp("LookAround", name) == 0)
		}
	}

	Sprites& XmlParserHelper::GetBlueDuckSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Moving", name) == 0)
		{
			return animator.mBlueDuckMoving;
		}
		else if (strcmp("Flipped", name) == 0)
		{
			return animator.mBlueDuckFlipped;
		}
		else
		{
			//TODO fix this
			return animator.mBlueDuckLookAround;						//if (strcmp("LookAround", name) == 0)
		}
	}

	Sprites& XmlParserHelper::GetRedDuckSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Moving", name) == 0)
		{
			return animator.mRedDuckMoving;
		}
		else if (strcmp("Flipped", name) == 0)
		{
			return animator.mRedDuckFlipped;
		}
		else
		{
			//TODO fix this
			return animator.mRedDuckLookAround;							//if (strcmp("LookAround", name) == 0)
		}
	}

	Sprites& XmlParserHelper::GetRedCrabSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Moving", name) == 0)
		{
			return animator.mRedCrabMoving;
		}
		else if (strcmp("MovingAngry", name) == 0)
		{
			return animator.mRedCrabAngryMoving;
		}
		else if (strcmp("Flipped", name) == 0)
		{
			return animator.mRedCrabFlipped;
		}
		else
		{
			//TODO fix this
			return animator.mRedCrabLookAround;							//if (strcmp("LookAround", name) == 0)
		}
	}

	Sprites& XmlParserHelper::GetGreenCrabSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Moving", name) == 0)
		{
			return animator.mGreenCrabMoving;
		}
		else if (strcmp("MovingAngry", name) == 0)
		{
			return animator.mGreenCrabAngryMoving;
		}
		else if (strcmp("Flipped", name) == 0)
		{
			return animator.mGreenCrabFlipped;
		}
		else
		{
			//TODO fix this
			return animator.mGreenCrabLookAround;						//if (strcmp("LookAround", name) == 0)
		}
	}

	Sprites& XmlParserHelper::GetBlueCrabSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Moving", name) == 0)
		{
			return animator.mBlueCrabMoving;
		}
		else if (strcmp("MovingAngry", name) == 0)
		{
			return animator.mBlueCrabAngryMoving;
		}
		else if (strcmp("Flipped", name) == 0)
		{
			return animator.mBlueCrabFlipped;
		}
		else
		{
			//TODO fix this
			return animator.mBlueCrabLookAround;						//if (strcmp("LookAround", name) == 0)
		}
	}

	Sprites & XmlParserHelper::GetRedBeeSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Moving", name) == 0)
		{
			return animator.mRedBeeMoving;
		}
		else
		{
			//TODO fix this
			return animator.mRedBeeFlipped;								//if (strcmp("Flipped", name) == 0)
		}
	}

	Sprites& XmlParserHelper::GetYellowBeeSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Moving", name) == 0)
		{
			return animator.mYellowBeeMoving;
		}
		else
		{
			//TODO fix this
			return animator.mYellowBeeFlipped;							//if (strcmp("Flipped", name) == 0)
		}
	}

	Sprites& XmlParserHelper::GetBlueBeeSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Moving", name) == 0)
		{
			return animator.mBlueBeeMoving;
		}
		else
		{
			//TODO fix this
			return animator.mBlueBeeFlipped;							//if (strcmp("Flipped", name) == 0)
		}
	}

	Sprites& XmlParserHelper::GetTextSprites(const char* name, Animator& animator) const
	{
		if (strcmp("Numbers", name) == 0)
		{
			return animator.mNumerals;
		}
		else
		{
			//TODO fix this
			return animator.mAlphabets;									//if (strcmp("Alphabets", name) == 0)
		}
	}
}