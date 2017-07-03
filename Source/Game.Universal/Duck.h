#pragma once
#include "Enemy.h"
#include "Animator.h"
#include "VertexDeclarations.h"
#include "EnemyManager.h"

namespace GameEntity
{
	class Duck final : public Enemy
	{
	public:
		Duck(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, bool isSpawnLeft, std::int32_t enemyID,
			Managers::EnemyManager& enemyManager);

		Duck(const Duck&) = delete;
		Duck& operator=(const Duck&) = delete;
		Duck(Duck&&) = delete;
		Duck& operator=(Duck&&) = delete;
		~Duck() = default;

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;				
		
		//Duck specific
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

		//Duck specific
		Animation::DuckSprites mDuckSprites;
		Animation::Sprites* mCurrentAnimation;		

		void UpdateEnemySpriteData(const DX::StepTimer& timer);

		/******************* Duck Constants ******************************/
		static const float sDUCK_Y_VELOCITY;
		static const float sDUCK_GREEN_X_VELOCITY;
		static const float sDUCK_BLUE_X_VELOCITY;
		static const float sDUCK_RED_X_VELOCITY;

		static const float sDUCK_GREEN_GRAVITY;
		static const float sDUCK_BLUE_GRAVITY;
		static const float sDUCK_RED_GRAVITY;

		static const float sDUCK_MOVE_SPRITE_SPEED;
		static const float sDUCK_FLIP_INITIAL_SPRITE_SPEED;
		static const float sDUCK_FLIP_DELTA_SPRITE_SPEED;
		static const float sDUCK_LOOK_AROUND_SPRITE_SPEED;

		static const float sDUCK_LOOK_AROUND_WAIT_DURATION;
		static const float sDUCK_FLIPPED_WAIT_DURATION;
		/*****************************************************************/
	};
}