#include "pch.h"
#include "SoundManager.h"
#include <string>

using namespace std;
using namespace DirectX;

namespace Audio
{
	SoundManager* SoundManager::sInstance = nullptr;
	
	const wstring SoundManager::sPHASE_START_SOUND_FILENAME = L"Content\\Sounds\\PhaseStart.wav";
	const wstring SoundManager::sPHASE_END_SOUND_FILENAME = L"Content\\Sounds\\PhaseEnd.wav";
	const wstring SoundManager::sGAME_START_SOUND_FILENAME = L"Content\\Sounds\\GameStart.wav";
	const wstring SoundManager::sGAME_OVER_SOUND_FILENAME = L"Content\\Sounds\\GameOver.wav";
	const wstring SoundManager::sMENU_SCREEN_SOUND_FILENAME = L"Content\\Sounds\\MenuScreen.wav";

	const wstring SoundManager::sPLAYER_HURT_SOUND_FILENAME = L"Content\\Sounds\\Hurt.wav";
	const wstring SoundManager::sPLAYER_DEATH_SOUND_FILENAME = L"Content\\Sounds\\Die.wav";
	const wstring SoundManager::sPLAYER_SPAWN_SOUND_FILENAME = L"Content\\Sounds\\SpawnPlayer.wav";
	const wstring SoundManager::sPLAYER_JUMP_SOUND_FILENAME = L"Content\\Sounds\\Jump.wav";
	const wstring SoundManager::sPLAYER_KICK_SOUND_FILENAME = L"Content\\Sounds\\EnemyKick.wav";

	const wstring SoundManager::sSHELL_CREEPER_SPAWN_SOUND_FILENAME = L"Content\\Sounds\\SpawnShellCreeper.wav";
	const wstring SoundManager::sSIDE_STEPPER_SPAWN_SOUND_FILENAME = L"Content\\Sounds\\SpawnSideStepper.wav";
	const wstring SoundManager::sENEMY_FLIP_SOUND_FILENAME = L"Content\\Sounds\\FlipEnemy.wav";
	const wstring SoundManager::sENEMY_FALLING_SOUND_FILENAME = L"Content\\Sounds\\EnemyFall.wav";

	SoundManager* SoundManager::GetInstance()
	{
		if (sInstance == nullptr)
		{
			sInstance = new SoundManager();
		}
		return sInstance;
	}

	void SoundManager::PlayBackgroundSound()
	{		
		mMenuScreenInstance->Play(true);
	}

	void SoundManager::StopBackgroundSound()
	{
		mMenuScreenInstance->Stop();
	}

	void SoundManager::PlayGameStartSound()
	{
		mGameStartSound->Play();
	}

	void SoundManager::PlayPhaseStartSound()
	{
		mPhaseStartSound->Play();
	}

	void SoundManager::PlayPhaseEndSound()
	{
		mPhaseEndSound->Play();
	}

	void SoundManager::PlayHurtSound()
	{
		mHurtSound->Play();
	}

	void SoundManager::PlayDeathSound()
	{
		mDeathSound->Play();
	}

	void SoundManager::PlayPlayerJumpSound()
	{
		mPlayerJumpSound->Play();
	}

	void SoundManager::PlayPlayerSpawnSound()
	{
		mPlayerSpawnSound->Play();
	}

	void SoundManager::PlayPlayerKickSound()
	{
		mPlayerKickSound->Play();
	}

	void SoundManager::PlayGameOverSound()
	{
		mGameOverSound->Play();
	}

	void SoundManager::PlaySpawnShellCreeperSound()
	{
		mSpawnShellCreeperSound->Play();
	}

	void SoundManager::PlaySpawnSideStepperSound()
	{
		mSpawnSideStepperSound->Play();
	}

	void SoundManager::PlayEnemyFlipSound()
	{
		mEnemyFlipSound->Play();
	}

	void SoundManager::PlayEnemyFallingSound()
	{
		mEnemyFallingSound->Play();
	}

	SoundManager::SoundManager()
	{
		AUDIO_ENGINE_FLAGS eFlags = AudioEngine_Default;
		mAudioEngine = make_unique<AudioEngine>(eFlags);
		
		mHurtSound = make_unique<SoundEffect>(mAudioEngine.get(), sPLAYER_HURT_SOUND_FILENAME.c_str());
		mDeathSound = make_unique<SoundEffect>(mAudioEngine.get(), sPLAYER_DEATH_SOUND_FILENAME.c_str());
		mPlayerSpawnSound = make_unique<SoundEffect>(mAudioEngine.get(), sPLAYER_SPAWN_SOUND_FILENAME.c_str());
		mPlayerJumpSound = make_unique<SoundEffect>(mAudioEngine.get(), sPLAYER_JUMP_SOUND_FILENAME.c_str());
		mPlayerKickSound = make_unique<SoundEffect>(mAudioEngine.get(), sPLAYER_KICK_SOUND_FILENAME.c_str());

		mPhaseStartSound = make_unique<SoundEffect>(mAudioEngine.get(), sPHASE_START_SOUND_FILENAME.c_str());
		mPhaseEndSound = make_unique<SoundEffect>(mAudioEngine.get(), sPHASE_END_SOUND_FILENAME.c_str());
		mGameStartSound = make_unique<SoundEffect>(mAudioEngine.get(), sGAME_START_SOUND_FILENAME.c_str());		
		mGameOverSound = make_unique<SoundEffect>(mAudioEngine.get(), sGAME_OVER_SOUND_FILENAME.c_str());
		mMenuScreenSound = make_unique<SoundEffect>(mAudioEngine.get(), sMENU_SCREEN_SOUND_FILENAME.c_str());
		mMenuScreenInstance = mMenuScreenSound->CreateInstance();

		mSpawnShellCreeperSound = make_unique<SoundEffect>(mAudioEngine.get(), sSHELL_CREEPER_SPAWN_SOUND_FILENAME.c_str());
		mSpawnSideStepperSound = make_unique<SoundEffect>(mAudioEngine.get(), sSIDE_STEPPER_SPAWN_SOUND_FILENAME.c_str());
		mEnemyFlipSound = make_unique<SoundEffect>(mAudioEngine.get(), sENEMY_FLIP_SOUND_FILENAME.c_str());
		mEnemyFallingSound = make_unique<SoundEffect>(mAudioEngine.get(), sENEMY_FALLING_SOUND_FILENAME.c_str());
	}
}