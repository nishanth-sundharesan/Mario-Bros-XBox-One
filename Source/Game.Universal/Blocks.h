#pragma once
#include "DrawableGameComponent.h"
#include "VertexDeclarations.h"
#include "Animator.h"

namespace Managers
{
	class PhysicsManager;
}

namespace GameEntity
{
	enum class BlockType
	{
		BOTTOMLEFTBLOCK,
		BOTTOMRIGHTBLOCK,
		MIDDLELEFTBLOCK,
		MIDDLERIGHTBLOCK,
		MIDDLEBLOCK,
		TOPLEFTBLOCK,
		TOPRIGHTBLOCK
	};

	class Blocks : public DX::DrawableGameComponent
	{
	public:
		Blocks(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera);
		Blocks(const Blocks&) = delete;
		Blocks& operator=(const Blocks&) = delete;
		Blocks(Blocks&&) = delete;
		Blocks& operator=(Blocks&&) = delete;
		~Blocks() = default;

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;		
		virtual void Render(const DX::StepTimer& timer) override;
		
		//Blocks specific
		void Init(Managers::PhysicsManager& physicsManager);
		void SetFlexibleBlocks(float hitPositionX, float hitPositionY, BlockType blockType);
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

		const float mOrthoWidth;
		const float mOrthoHeight;

		DX::VertexPositionTexture mVertices[4];
		std::shared_ptr<Animation::Animator> mAnimator;

		//Blocks specific		
		bool mRenderFlexibleBlocks;		

		float mSpriteSpeed;
		std::int32_t mSpriteIndex;

		float mFlexibleSpritePositionLeft;
		float mFlexibleSpritePositionTop;
		float mFlexibleSpritePositionRight;
		float mFlexibleSpritePositionBottom;

		DX::TextureCoordinates mStaticBlockCoordinates;

		bool mIsLastBlockAttacked;
		bool mIsFirstBlockAttacked;

		void ResetFlexibleBlocks();
		void ClearStaticBlocks();
		void UpdateFlexibleBlockSpriteData(const DX::StepTimer& timer, const Animation::Sprites& currentFlexibleBlock);
		void RenderFlexibleBlocks(const DX::StepTimer& timer);

		/******************* Blocks Constants ******************************/
		static const float sBLOCK_WIDTH;
		static const float sBLOCK_HEIGHT;

		static const float sBLOCK_HALF_WIDTH;

		static const float sTOP_BLOCKS_POS_Y;
		static const float sMIDDLE_LEFT_BLOCKS_POS_Y;
		static const float sMIDDLE_BLOCKS_POS_Y;
		static const float sBOTTOM_BLOCKS_POS_Y;

		static const std::int32_t sNUMBER_OF_TOP_BLOCKS;
		static const std::int32_t sNUMBER_OF_MIDDLE_LEFT_BLOCKS;
		static const std::int32_t sNUMBER_OF_MIDDLE_BLOCKS;
		static const std::int32_t sNUMBER_OF_BOTTOM_BLOCKS;

		static const float sMIDDLE_BLOCKS_POS_X;

		static const float sFLEXIBLE_BLOCKS_SPEED;
		static const std::int32_t sTOTAL_FLEXIBLE_BLOCKS;

		static const float sADDITIONAL_BLOCK_WIDTH;

		static const DirectX::XMFLOAT2 sBLOCK_SPRITE_SCALE;
		/*******************************************************************/
	};
}