#pragma once
#include "Enemy.h"
#include "Animator.h"
#include "VertexDeclarations.h"
#include "EnemyManager.h"

namespace GameEntity
{
	class Crab final : public Enemy
	{
	public:
		Crab(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, bool isSpawnLeft, std::int32_t enemyID,
			Managers::EnemyManager& enemyManager);

		Crab(const Crab&) = delete;
		Crab& operator=(const Crab&) = delete;
		Crab(Crab&&) = delete;
		Crab& operator=(Crab&&) = delete;
		~Crab() = default;

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;

		//Crab specific
		virtual void CollidedWithFloor(float newEnemyYPosition, EnemyState previousState) override;
		virtual void FellFromFloor() override;
		virtual void EscapeFromPipe(float newEnemyYPosition) override;
		virtual void ReInitEnemyPositionAndState() override;
		virtual void CollidedWithAnotherObject() override;
		virtual void HitTaken(FlipDirection flipDirection) override;
		virtual void EvolveTheEnemy() override;
		virtual void ResetEnemyMovement() override;
		virtual void GotKilled(bool isPushRight) override;
		virtual void EvolveToFinalState() override;

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

		//Sprite transforms
		DirectX::XMFLOAT4X4 mTextureTransform;
		DX::VertexPositionTexture mVertices[4];
		DX::TextureCoordinates mTextureCoordinates;

		//Crab specific
		Animation::CrabSprites mCrabSprites;
		Animation::Sprites* mCurrentAnimation;
		bool mIsAngry;
		float mCurrentStateAngryXVelocity;

		void UpdateEnemySpriteData(const DX::StepTimer& timer);

		/******************* Crab Constants ******************************/
		static const float sCRAB_Y_VELOCITY;

		static const float sCRAB_RED_X_VELOCITY;
		static const float sCRAB_RED_ANGRY_X_VELOCITY;
		static const float sCRAB_GREEN_X_VELOCITY;
		static const float sCRAB_GREEN_ANGRY_X_VELOCITY;
		static const float sCRAB_BLUE_X_VELOCITY;
		static const float sCRAB_BLUE_ANGRY_X_VELOCITY;

		static const float sCRAB_RED_GRAVITY;
		static const float sCRAB_GREEN_GRAVITY;
		static const float sCRAB_BLUE_GRAVITY;

		static const float sCRAB_MOVE_SPRITE_SPEED;
		static const float sCRAB_FLIP_INITIAL_SPRITE_SPEED;
		static const float sCRAB_FLIP_DELTA_SPRITE_SPEED;
		static const float sCRAB_LOOK_AROUND_SPRITE_SPEED;

		static const float sCRAB_LOOK_AROUND_WAIT_DURATION;
		static const float sCRAB_FLIPPED_WAIT_DURATION;
		/*****************************************************************/
	};
}