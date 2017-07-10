#pragma once
#include "Animator.h"
#include "DeviceResources.h"

namespace DX
{
	class Camera;
}

namespace Managers
{
	class TextManager
	{
	public:
		TextManager(const TextManager&) = delete;
		TextManager& operator=(const TextManager&) = delete;
		TextManager(TextManager&&) = delete;
		TextManager& operator=(TextManager&&) = delete;
		~TextManager() = default;		
		
		std::shared_ptr<DX::DeviceResources> DeviceResources() const;
		void SetCameraAndDeviceDependentResources(const std::shared_ptr<DX::Camera>& camera, const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		
		void DisplayPhaseText(std::int32_t currentPhase);
		void DisplayGameOverText();
		void DisplayPlayerScore(std::int32_t score);
		void DisplayCurrentPhase(std::int32_t currentPhase);

		static TextManager *GetInstance();
	private:
		std::shared_ptr<DX::DeviceResources> mDeviceResources;
		std::shared_ptr<DX::Camera> mCamera;

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

		//TextManager specific
		TextManager();
		static TextManager *sInstance;
		
		Animation::Sprites* mNumeralSprites;
		Animation::Sprites* mAlphabetSprites;
		
		/******************* Text Manager Constants ******************************/		
		static const float sPHASE_TEXT_POS_X;
		static const float sPHASE_TEXT_POS_Y;

		static const float sGAME_OVER_POS_X;
		static const float sGAME_OVER_POS_Y;

		static const float sSCORE_POS_X;
		static const float sSCORE_POS_Y;

		static const float sPHASE_NUMBER_POS_X;
		static const float sPHASE_NUMBER_POS_Y;

		static const float sNUMBER_OF_SCORE_DIGITS;

		static const float sNUMBER_WIDTH;
		static const float sNUMBER_HEIGHT;
		static const float sSPACE_BETWEEN_NUMBERS;

		static const float sLETTER_WIDTH;
		static const float sLETTER_HEIGHT;
		static const float sSPACE_BETWEEM_LETTERS;

		static const std::int32_t sMAX_SINGLE_DIGIT;

		static const float sPHASE_TEXT_SCALE_MULTIPLIER;

		static const DirectX::XMFLOAT2 sNUMBER_SCALE;
		static const DirectX::XMFLOAT2 sLETTER_SCALE;

		enum class LetterLength
		{
			ONE_LETTER_LENGTH = 1,
			TWO_LETTER_LENGTH = 2,
			THREE_LETTER_LENGTH = 3,
			FOUR_LETTER_LENGTH = 4,
			FIVE_LETTER_LENGTH = 5,
		};		
		/*************************************************************************/

		std::int32_t mPhaseWordIndex[static_cast<std::uint32_t>(LetterLength::FIVE_LETTER_LENGTH)] = { 15, 7, 0, 18, 4 };
		std::int32_t mGameWordIndex[static_cast<std::uint32_t>(LetterLength::FOUR_LETTER_LENGTH)] = { 6, 0, 12, 4 };
		std::int32_t mOverWordIndex[static_cast<std::uint32_t>(LetterLength::FOUR_LETTER_LENGTH)] = { 14, 21, 4, 17 };
	};

}