#pragma once
#include "DeviceResources.h"
#include <string>

namespace DirectX
{
	class AudioEngine;
	class SoundEffect;
	class SoundEffectInstance;
}

namespace Audio
{
	class SoundManager
	{
	public:		
		static SoundManager* GetInstance();

		SoundManager(const SoundManager&) = delete;
		SoundManager& operator=(const SoundManager&) = delete;
		SoundManager(SoundManager&&) = delete;
		SoundManager& operator=(SoundManager&&) = delete;
		~SoundManager() = default;

		void PlayBackgroundSound();
		void StopBackgroundSound();

		void PlayGameStartSound();
		void PlayPhaseStartSound();
		void PlayPhaseEndSound();
		void PlayHurtSound();
		void PlayDeathSound();
		void PlayPlayerJumpSound();
		void PlayPlayerSpawnSound();
		void PlayPlayerKickSound();
		void PlayGameOverSound();
		void PlaySpawnShellCreeperSound();
		void PlaySpawnSideStepperSound();
		void PlayEnemyFlipSound();
		void PlayEnemyFallingSound();

	private:
		SoundManager();
		static SoundManager *sInstance;
		
		std::unique_ptr<DirectX::AudioEngine> mAudioEngine;

		std::unique_ptr<DirectX::SoundEffect> mHurtSound;
		std::unique_ptr<DirectX::SoundEffect> mDeathSound;
		std::unique_ptr<DirectX::SoundEffect> mPlayerSpawnSound;
		std::unique_ptr<DirectX::SoundEffect> mPlayerJumpSound;
		std::unique_ptr<DirectX::SoundEffect> mPlayerKickSound;

		std::unique_ptr<DirectX::SoundEffect> mPhaseStartSound;
		std::unique_ptr<DirectX::SoundEffect> mPhaseEndSound;
		std::unique_ptr<DirectX::SoundEffect> mGameStartSound;		
		std::unique_ptr<DirectX::SoundEffect> mGameOverSound;
		std::unique_ptr<DirectX::SoundEffect> mMenuScreenSound;

		std::unique_ptr<DirectX::SoundEffect> mSpawnShellCreeperSound;
		std::unique_ptr<DirectX::SoundEffect> mSpawnSideStepperSound;
		std::unique_ptr<DirectX::SoundEffect> mEnemyFlipSound;
		std::unique_ptr<DirectX::SoundEffect> mEnemyFallingSound;

		std::unique_ptr<DirectX::SoundEffectInstance> mMenuScreenInstance;

		/**************** File path to sound files **************************/		
		static const std::wstring sPHASE_START_SOUND_FILENAME;
		static const std::wstring sPHASE_END_SOUND_FILENAME;
		static const std::wstring sGAME_START_SOUND_FILENAME;
		static const std::wstring sGAME_OVER_SOUND_FILENAME;
		static const std::wstring sMENU_SCREEN_SOUND_FILENAME;

		static const std::wstring sPLAYER_HURT_SOUND_FILENAME;
		static const std::wstring sPLAYER_DEATH_SOUND_FILENAME;
		static const std::wstring sPLAYER_SPAWN_SOUND_FILENAME;
		static const std::wstring sPLAYER_JUMP_SOUND_FILENAME;
		static const std::wstring sPLAYER_KICK_SOUND_FILENAME;

		static const std::wstring sSHELL_CREEPER_SPAWN_SOUND_FILENAME;
		static const std::wstring sSIDE_STEPPER_SPAWN_SOUND_FILENAME;
		static const std::wstring sENEMY_FLIP_SOUND_FILENAME;
		static const std::wstring sENEMY_FALLING_SOUND_FILENAME;
		/********************************************************************/
	};
}