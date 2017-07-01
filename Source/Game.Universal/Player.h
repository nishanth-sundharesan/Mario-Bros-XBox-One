#pragma once
#include "DrawableGameComponent.h"
#include <functional>
#include "Animator.h"

#define GOD_MODE 0

namespace GameEntity
{
	enum class PlayerState
	{
		ONGROUND,
		RUNNING,
		STRAIGHTJUMPING,
		RUNJUMPING,
		SLIPPING,
		FALLING,
		SLIPFALLING,
		ATTACKED,
		DEAD,
		DEADJUMPING,
		RESPAWNING,
	};

	class Player final : public DX::DrawableGameComponent
	{
	public:
		Player(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, const std::shared_ptr<DX::KeyboardComponent> keyboard);
		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;
		Player(Player&&) = delete;
		Player& operator=(Player&&) = delete;
		~Player() = default;

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Update(const DX::StepTimer& timer);
		virtual void Render(const DX::StepTimer& timer) override;

		//Player specific
		void ResetPlayer();
		void CollidedWithFloor(float newPlayerYPosition);
		void PlayerAttacked(float newPlayerYPosition);
		void GotKilled();
		void Init();
	private:		

		struct VSCBufferPerObject
		{
			DirectX::XMFLOAT4X4 WorldViewProjection;
			DirectX::XMFLOAT4X4 TextureTransform;

			VSCBufferPerObject() :
				WorldViewProjection(DX::MatrixHelper::Identity), TextureTransform(DX::MatrixHelper::Identity)
			{ };

			VSCBufferPerObject(const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& textureTransform) :
				WorldViewProjection(wvp), TextureTransform(textureTransform)
			{ }
		};

		void SetupVertexAndIndexBuffer(const DX::StepTimer& timer);

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;

		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVSCBufferPerObject;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSpriteSheet;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> mTextureSampler;
		Microsoft::WRL::ComPtr<ID3D11BlendState> mAlphaBlending;

		VSCBufferPerObject mVSCBufferPerObjectData;

		//Inputs
		std::shared_ptr<DX::KeyboardComponent> mKeyboard;
		std::shared_ptr<DX::GamePadComponent> mGamePad;

		std::uint32_t mIndexCount;
		bool mLoadingComplete;

		//Sprite transforms
		DirectX::XMFLOAT4X4 mTextureTransform;
		//DX::Transform2D mTransform;		
		
		//Player specific
		float mPosX;
		float mPosY;
		float mXDuplicatePlayer;
		float mXVelocity;
		float mYVelocity;
		float mGravity;
		/*float mHheight;
		float mWidth;*/

		static const DirectX::XMFLOAT2 sSPRITE_SCALE;
		static const float sWIDTH;
		static const float sHEIGHT;
		static const float sHALFWIDTH;

		const float mOrthoWidth;
		const float mOrthoHeight;

		PlayerState currentState;

		Animation::Sprites* mCurrentAnimation;
		Animation::Sprites* mLivesAnimation;
		Animation::Sprites* mPaddleAnimation;

		bool mIsRenderDuplicateSprites;
		bool mIsMovingRight;
		std::int32_t mSlipDuration;
		std::int32_t mAttackDuration;
		std::int32_t mDeathDuration;
		std::int32_t mReSpawnDuration;
		std::int32_t mPlayerSpriteIndex;
		std::int32_t mPlayerSpriteSpeed;
		std::int32_t mPaddleSpriteIndex;
		std::int32_t mPaddleSpriteSpeed;

		long mPlayerScore;
		std::int32_t mLivesLeft;
		/*GLuint mSpriteBuffer;
		SpriteRenderer *renderer;		
		InputManager *inputManager;*/

		std::shared_ptr<Animation::Animator> mAnimator;

		void RenderPlayerScore();
		void RenderPlayerLives();
		void RenderPaddle();
		void ResetPlayerSprites();
		void ResetPaddleSprites();
		void PlayerFalling(PlayerState previousState);
		void ChangeState(PlayerState state, bool isChangeSprite = true);
		void IncrementKillScore();

		//void IncrementFlipScore();

		/******************* Player States ********************************/
		void PlayerOnGround(const DX::StepTimer& timer);
		void PlayerRunning(const DX::StepTimer& timer);
		void PlayerJumping(const DX::StepTimer& timer);
		void PlayerSlipping(const DX::StepTimer& timer);
		void PlayerAttacking(const DX::StepTimer& timer);
		void PlayerDead(const DX::StepTimer& timer);
		void PlayerDeadJumping(const DX::StepTimer& timer);
		void PlayerReSpawning(const DX::StepTimer& timer);
		
		std::function<void(const DX::StepTimer&)> CurrentPlayerState;
		/******************************************************************/


		/******************* Player Constants ******************************/
		static const int32_t sPLAYER_1_INITIAL_POS_X;
		static const int32_t sPLAYER_1_INITIAL_POS_Y;
		static const int32_t sPLAYER_HEIGHT;
		static const int32_t sPLAYER_WIDTH;
		static const int32_t sPLAYER_X_VELOCITY;
		static const int32_t sPLAYER_X_JUMP_VELOCITY;
		static const int32_t sPLAYER_X_JUMP_QUANTUM_VELOCITY;
		static const int32_t sPLAYER_GRAVITY;
		static const int32_t sPLAYER_Y_VELOCITY;
		static const int32_t sPLAYER_Y_VELOCITY_CAP;
		static const int32_t sPLAYER_Y_DEAD_VELOCITY;
		static const int32_t sPLAYER_DEAD_GRAVITY;
		static const int32_t sPLAYER_RESPAWN_Y_VELOCITY;
		static const int32_t sPLAYER_RESPAWN_START_POS_X;
		static const int32_t sPLAYER_RESPAWN_START_POS_Y;
		static const int32_t sPLAYER_RESPAWN_STOP_POS_Y;

		static const int32_t sKILL_SCORE;
		static const int32_t sFLIP_SCORE;

		static const int32_t sPLAYER_LIVES_X_POS;
		static const int32_t sPLAYER_LIVES_Y_POS;
		static const int32_t sPLAYER_LIVES_WIDTH;
		static const int32_t sPLAYER_LIVES_HEIGHT;
		static const int32_t sDISTANCE_BETWEEN_LIVES;

		static const int32_t sNUMBER_OF_LIVES;

		static const int32_t sATTACK_WAIT_DURATION;
		static const int32_t sPLAYER_DEAD_WAIT_DURATION;
		static const int32_t sPLAYER_RESPAWN_WAIT_DURATION;
		static const int32_t sPLAYER_MOVE_SPRITE_SPEED;
		static const int32_t sPADDLE_SPRITE_SPEED;
		static const int32_t sSLIP_DURATION_CAP;

		static const int32_t sPADDLE_WIDTH;
		static const int32_t sPADDLE_HEIGHT;
		/******************************************************************/
	};
}