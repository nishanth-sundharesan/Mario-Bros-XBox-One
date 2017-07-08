#pragma once
#include "DrawableGameComponent.h"

namespace GameEntity
{
	class Background final : public DX::DrawableGameComponent
	{		
	public:
		Background(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera);
		Background(const Background&) = default;
		Background& operator=(const Background&) = default;
		Background(Background&&) = default;
		Background& operator=(Background&&) = default;
		~Background() = default;

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;		
		virtual void Render(const DX::StepTimer& timer) override;

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

		void InitializeVerticesAndIndexes();
		void InitializeSprites();

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
		DX::VertexPositionTexture mVertices[4];

		const DirectX::XMFLOAT2 SPRITE_SCALE;

		std::uint32_t mIndexCount;
		bool mLoadingComplete;

		//Sprite transforms
		DirectX::XMFLOAT4X4 mTextureTransform;
		DX::Transform2D mTransform;				
	};
}