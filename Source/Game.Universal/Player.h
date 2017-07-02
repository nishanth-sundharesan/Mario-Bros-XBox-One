#pragma once
#include "DrawableGameComponent.h"
#include <functional>
#include "Animator.h"
#include "VertexDeclarations.h"

#define GOD_MODE 0

namespace Managers
{
	class PhysicsManager;
}

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
		friend class Managers::PhysicsManager;
	public:
		Player(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, const std::shared_ptr<DX::KeyboardComponent>& keyboard, const std::shared_ptr<DX::GamePadComponent>& gamePad);
		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;
		Player(Player&&) = delete;
		Player& operator=(Player&&) = delete;
		~Player() = default;

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Update(const DX::StepTimer& timer) override;
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
		
		void InitiazeIndexes();

		void SetupVertices(float xTextureCoord, float yTextureCoord, float textureCoordWidth, float textureCoordHeight);
		void BindBuffers();
		void DrawIndividualSprite(float xPosition, float yPosition, const DirectX::XMFLOAT2& spriteScale);

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

		std::uint32_t mIndexCount;
		bool mLoadingComplete;

		//Inputs
		std::shared_ptr<DX::KeyboardComponent> mKeyboard;
		std::shared_ptr<DX::GamePadComponent> mGamePad;		

		//Sprite transforms
		DirectX::XMFLOAT4X4 mTextureTransform;

		const float mOrthoWidth;
		const float mOrthoHeight;

		DX::VertexPositionTexture mVertices[4];
		std::shared_ptr<Animation::Animator> mAnimator;
		DX::TextureCoordinates mTextureCoordinates;
		
		//Player specific
		float mPosX;
		float mPosY;
		float mXDuplicatePlayer;
		float mXVelocity;
		float mYVelocity;
		float mGravity;				

		PlayerState mCurrentState;
		
		//Do not worry about deleting the sprites.
		Animation::Sprites* mCurrentAnimation;
		Animation::Sprites* mLivesAnimation;
		Animation::Sprites* mPaddleAnimation;

		bool mIsRenderDuplicateSprites;
		bool mIsMovingRight;
		float mSlipDuration;
		float mAttackDuration;
		float mDeathDuration;
		float mReSpawnDuration;
		std::int32_t mPlayerSpriteIndex;
		float mPlayerSpriteSpeed;
		std::int32_t mPaddleSpriteIndex;
		float mPaddleSpriteSpeed;

		long mPlayerScore;
		std::int32_t mLivesLeft;
		/*GLuint mSpriteBuffer;
		SpriteRenderer *renderer;		
		InputManager *inputManager;*/		

		void RenderPlayerSprites(const DX::StepTimer& timer);
		void RenderPlayerLives(const DX::StepTimer& timer);
		void RenderPlayerScore(const DX::StepTimer& timer);
		void RenderPaddle(const DX::StepTimer& timer);

		void UpdatePlayerSpriteData(const DX::StepTimer& timer);

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
		static const float sPLAYER_1_INITIAL_POS_X;
		static const float sPLAYER_1_INITIAL_POS_Y;
		static const float sPLAYER_HEIGHT;
		static const float sPLAYER_WIDTH;
		static const float sPLAYER_X_VELOCITY;
		static const float sPLAYER_X_JUMP_VELOCITY;
		static const float sPLAYER_X_JUMP_QUANTUM_VELOCITY;
		static const float sPLAYER_GRAVITY;
		static const float sPLAYER_Y_VELOCITY;
		static const float sPLAYER_Y_VELOCITY_CAP;
		static const float sPLAYER_Y_DEAD_VELOCITY;
		static const float sPLAYER_DEAD_GRAVITY;
		static const float sPLAYER_RESPAWN_Y_VELOCITY;
		static const float sPLAYER_RESPAWN_START_POS_X;
		static const float sPLAYER_RESPAWN_START_POS_Y;
		static const float sPLAYER_RESPAWN_STOP_POS_Y;

		static const std::int32_t sKILL_SCORE;
		static const std::int32_t sFLIP_SCORE;

		static const float sPLAYER_LIVES_X_POS;
		static const float sPLAYER_LIVES_Y_POS;
		static const float sPLAYER_LIVES_WIDTH;
		static const float sPLAYER_LIVES_HEIGHT;
		static const float sDISTANCE_BETWEEN_LIVES;

		static const std::int32_t sNUMBER_OF_LIVES;

		static const float sATTACK_WAIT_DURATION;
		static const float sPLAYER_DEAD_WAIT_DURATION;
		static const float sPLAYER_RESPAWN_WAIT_DURATION;
		static const float sPLAYER_MOVE_SPRITE_SPEED;
		static const float sPADDLE_SPRITE_SPEED;
		static const float sSLIP_DURATION_CAP;

		static const float sPADDLE_WIDTH;
		static const float sPADDLE_HEIGHT;

		static const float sWIDTH;
		static const float sHEIGHT;
		static const float sHALFWIDTH;
		static const DirectX::XMFLOAT2 sPLAYER_SPRITE_SCALE;
		static const DirectX::XMFLOAT2 sPLAYER_LIVES_SPRITE_SCALE;
		/******************************************************************/
	};
}