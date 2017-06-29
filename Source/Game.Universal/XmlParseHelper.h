#pragma once
#include "Animator.h"
#include "Enemy.h"
#include <functional>
#include <vector>

namespace tinyxml2
{
	class XMLElement;
}

namespace Core
{
	class XmlParserHelper
	{
	public:
		XmlParserHelper() = default;
		XmlParserHelper(const XmlParserHelper&) = delete;
		XmlParserHelper& operator=(const XmlParserHelper&) = delete;
		XmlParserHelper(XmlParserHelper&&) = delete;
		XmlParserHelper& operator=(XmlParserHelper&&) = delete;
		~XmlParserHelper() = default;
				
		void LoadSprites(Animation::Animator& animator) const;
		void LoadLevels(std::vector<GameEntity::EnemySpawner>& gameEmenies) const;

	private:
		using SpriteHandlerParameters = Animation::Sprites&(const char*, Animation::Animator&);
		void LoadIndividualSpriteData(std::function<SpriteHandlerParameters> spriteHandler, tinyxml2::XMLElement* metaElement, std::int32_t width, std::int32_t height, Animation::Animator& animator) const;

		GameEntity::EnemyType GetEnemyType(const char* name) const;
		bool IsSpawnLeftOrRight(const char* position) const;

		Animation::Sprites& GetMarioSprites(const char* name, Animation::Animator& animator) const;
		Animation::Sprites& GetBlockSprites(const char* name, Animation::Animator& animator) const;

		Animation::Sprites& GetGreenDuckSprites(const char* name, Animation::Animator& animator) const;
		Animation::Sprites& GetBlueDuckSprites(const char* name, Animation::Animator& animator) const;
		Animation::Sprites& GetRedDuckSprites(const char* name, Animation::Animator& animator) const;

		Animation::Sprites& GetRedCrabSprites(const char* name, Animation::Animator& animator) const;
		Animation::Sprites& GetGreenCrabSprites(const char* name, Animation::Animator& animator) const;
		Animation::Sprites& GetBlueCrabSprites(const char* name, Animation::Animator& animator) const;

		Animation::Sprites& GetRedBeeSprites(const char* name, Animation::Animator& animator) const;
		Animation::Sprites& GetYellowBeeSprites(const char* name, Animation::Animator& animator) const;
		Animation::Sprites& GetBlueBeeSprites(const char* name, Animation::Animator& animator) const;

		Animation::Sprites& GetTextSprites(const char* name, Animation::Animator& animator) const;

		static const std::string sMARIO_BROS_SPRITE_XML;
		static const std::string sMARIO_BROS_LEVELS_XML;
	};
}