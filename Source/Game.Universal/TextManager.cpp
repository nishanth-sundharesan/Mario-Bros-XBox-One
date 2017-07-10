#include "pch.h"
#include "TextManager.h"

using namespace std;
using namespace DX;
using namespace DirectX;
using namespace Animation;

namespace Managers
{
	TextManager* TextManager::sInstance = nullptr;

	const float TextManager::sPHASE_TEXT_POS_X = 195.0f;
	const float TextManager::sPHASE_TEXT_POS_Y = 172.0f;

	const float TextManager::sGAME_OVER_POS_X = 185.0f;
	const float TextManager::sGAME_OVER_POS_Y = 264.0f;

	const float TextManager::sSCORE_POS_X = 144.0f;
	const float TextManager::sSCORE_POS_Y = 414.0f;

	const float TextManager::sPHASE_NUMBER_POS_X = 48.0f;
	const float TextManager::sPHASE_NUMBER_POS_Y = 46.0f;

	const float TextManager::sNUMBER_OF_SCORE_DIGITS = 6.0f;

	const float TextManager::sNUMBER_WIDTH = 14.0f;
	const float TextManager::sNUMBER_HEIGHT = 14.0f;
	const float TextManager::sSPACE_BETWEEN_NUMBERS = 2.0f;

	const float TextManager::sLETTER_WIDTH = 14.0f;
	const float TextManager::sLETTER_HEIGHT = 14.0f;
	const float TextManager::sSPACE_BETWEEM_LETTERS = 3.0f;		

	const std::int32_t TextManager::sMAX_SINGLE_DIGIT = 9;

	const float TextManager::sPHASE_TEXT_SCALE_MULTIPLIER = 1.5f;

	const XMFLOAT2 TextManager::sNUMBER_SCALE(sNUMBER_WIDTH, sNUMBER_HEIGHT);
	const XMFLOAT2 TextManager::sLETTER_SCALE(sLETTER_WIDTH, sLETTER_HEIGHT);

	void TextManager::CreateDeviceDependentResources()
	{
		auto loadVSTask = ReadDataAsync(L"SpriteRendererVS.cso");
		auto loadPSTask = ReadDataAsync(L"SpriteRendererPS.cso");

		// After the vertex shader file is loaded, create the shader and input layout.
		auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateVertexShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					mVertexShader.ReleaseAndGetAddressOf()
				)
			);

			// Create an input layout
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateInputLayout(
					VertexPositionTexture::InputElements,
					VertexPositionTexture::InputElementCount,
					&fileData[0],
					fileData.size(),
					mInputLayout.ReleaseAndGetAddressOf()
				)
			);

			CD3D11_BUFFER_DESC constantBufferDesc(sizeof(VSCBufferPerObject), D3D11_BIND_CONSTANT_BUFFER);
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateBuffer(
					&constantBufferDesc,
					nullptr,
					mVSCBufferPerObject.ReleaseAndGetAddressOf()
				)
			);
		});

		// After the pixel shader file is loaded, create the shader and texture sampler state.
		auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreatePixelShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					mPixelShader.ReleaseAndGetAddressOf()
				)
			);

			D3D11_SAMPLER_DESC samplerStateDesc;
			ZeroMemory(&samplerStateDesc, sizeof(samplerStateDesc));
			samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerStateDesc.MinLOD = -FLT_MAX;
			samplerStateDesc.MaxLOD = FLT_MAX;
			samplerStateDesc.MipLODBias = 0.0f;
			samplerStateDesc.MaxAnisotropy = 1;
			samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateSamplerState(&samplerStateDesc, mTextureSampler.ReleaseAndGetAddressOf()));

			D3D11_BLEND_DESC blendStateDesc = { 0 };
			blendStateDesc.RenderTarget[0].BlendEnable = true;
			blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBlendState(&blendStateDesc, mAlphaBlending.ReleaseAndGetAddressOf()));
		});

		auto loadSpriteSheetAndCreateSpritesTask = (createPSTask && createVSTask).then([this]() {
			ThrowIfFailed(CreateWICTextureFromFile(mDeviceResources->GetD3DDevice(), L"Content\\Textures\\Text.png", nullptr, mSpriteSheet.ReleaseAndGetAddressOf()));
			InitiazeIndexes();
		});

		// Once the cube is loaded, the object is ready to be rendered.
		loadSpriteSheetAndCreateSpritesTask.then([this]() {
			mLoadingComplete = true;
		});
	}

	shared_ptr<DX::DeviceResources> TextManager::DeviceResources() const
	{
		return mDeviceResources;
	}

	void TextManager::SetCameraAndDeviceDependentResources(const shared_ptr<Camera>& camera, const shared_ptr<DX::DeviceResources>& deviceResources)
	{
		mCamera = camera;
		mDeviceResources = deviceResources;
	}

	void TextManager::ReleaseDeviceDependentResources()
	{
		mLoadingComplete = false;
		mVertexShader.Reset();
		mPixelShader.Reset();
		mInputLayout.Reset();
		mVertexBuffer.Reset();
		mIndexBuffer.Reset();
		mVSCBufferPerObject.Reset();
		mSpriteSheet.Reset();
		mTextureSampler.Reset();
	}

	void TextManager::DisplayPhaseText(int32_t currentPhase)
	{
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}

		float phaseTextPosX = sPHASE_TEXT_POS_X;
		float phaseTextPosY = sPHASE_TEXT_POS_Y;

		/******************************************************* Displaying the word "PHASE" **************************************************************************************/
		for (int32_t i = 0; i < static_cast<std::int32_t>(LetterLength::FIVE_LETTER_LENGTH); i++)
		{
			float xTextureCoord = mAlphabetSprites->mDimensions[mPhaseWordIndex[i]].mPosX / mAlphabetSprites->mSpriteWidth;
			float yTextureCoord = mAlphabetSprites->mDimensions[mPhaseWordIndex[i]].mPosY / mAlphabetSprites->mSpriteHeight;
			float textureCoordWidth = (mAlphabetSprites->mDimensions[mPhaseWordIndex[i]].mPosX + mAlphabetSprites->mDimensions[mPhaseWordIndex[i]].mWidth) / mNumeralSprites->mSpriteWidth;
			float textureCoordHeight = (mAlphabetSprites->mDimensions[mPhaseWordIndex[i]].mPosY + mAlphabetSprites->mDimensions[mPhaseWordIndex[i]].mHeight) / mNumeralSprites->mSpriteHeight;
			
			SetupVertices(xTextureCoord, yTextureCoord, textureCoordWidth, textureCoordHeight);
			BindBuffers();
			DrawIndividualSprite(phaseTextPosX, phaseTextPosY, sLETTER_SCALE);

			phaseTextPosX += sLETTER_WIDTH + sSPACE_BETWEEM_LETTERS;
		}
		/**************************************************************************************************************************************************************************/


		/******************************************************* Displaying the phase number **************************************************************************************/
		phaseTextPosX += sPHASE_TEXT_SCALE_MULTIPLIER * sLETTER_WIDTH;
		while (currentPhase != 0)
		{
			int32_t numberToDisplay = currentPhase % 10;
			currentPhase = currentPhase / 10;

			float xTextureCoord = mNumeralSprites->mDimensions[numberToDisplay].mPosX / mNumeralSprites->mSpriteWidth;
			float yTextureCoord = mNumeralSprites->mDimensions[numberToDisplay].mPosY / mNumeralSprites->mSpriteHeight;
			float textureCoordWidth = (mNumeralSprites->mDimensions[numberToDisplay].mPosX + mNumeralSprites->mDimensions[numberToDisplay].mWidth) / mNumeralSprites->mSpriteWidth;
			float textureCoordHeight = (mNumeralSprites->mDimensions[numberToDisplay].mPosY + mNumeralSprites->mDimensions[numberToDisplay].mHeight) / mNumeralSprites->mSpriteHeight;

			SetupVertices(xTextureCoord, yTextureCoord, textureCoordWidth, textureCoordHeight);
			BindBuffers();
			DrawIndividualSprite(phaseTextPosX, phaseTextPosY, sLETTER_SCALE);

			phaseTextPosX -= sLETTER_WIDTH + sSPACE_BETWEEM_LETTERS;
		}
		/**************************************************************************************************************************************************************************/
	}

	void TextManager::DisplayGameOverText()
	{
		float gameOverPosX = sGAME_OVER_POS_X;
		float gameOverPosY = sGAME_OVER_POS_Y;

		/******************************************************* Displaying the word "GAME" **************************************************************************************/
		for (int32_t i = 0; i < static_cast<std::int32_t>(LetterLength::FOUR_LETTER_LENGTH); i++)
		{
			float xTextureCoord = mAlphabetSprites->mDimensions[mGameWordIndex[i]].mPosX / mAlphabetSprites->mSpriteWidth;
			float yTextureCoord = mAlphabetSprites->mDimensions[mGameWordIndex[i]].mPosY / mAlphabetSprites->mSpriteHeight;
			float textureCoordWidth = (mAlphabetSprites->mDimensions[mGameWordIndex[i]].mPosX + mAlphabetSprites->mDimensions[mGameWordIndex[i]].mWidth) / mNumeralSprites->mSpriteWidth;
			float textureCoordHeight = (mAlphabetSprites->mDimensions[mGameWordIndex[i]].mPosY + mAlphabetSprites->mDimensions[mGameWordIndex[i]].mHeight) / mNumeralSprites->mSpriteHeight;

			SetupVertices(xTextureCoord, yTextureCoord, textureCoordWidth, textureCoordHeight);
			BindBuffers();
			DrawIndividualSprite(gameOverPosX, gameOverPosY, sLETTER_SCALE);
			
			gameOverPosX += sLETTER_WIDTH + sSPACE_BETWEEM_LETTERS;
		}
		/**************************************************************************************************************************************************************************/

		gameOverPosX += sLETTER_WIDTH + sSPACE_BETWEEM_LETTERS;
		/******************************************************* Displaying the word "OVER" **************************************************************************************/
		for (int i = 0; i < static_cast<std::int32_t>(LetterLength::FOUR_LETTER_LENGTH); i++)
		{
			float xTextureCoord = mAlphabetSprites->mDimensions[mOverWordIndex[i]].mPosX / mAlphabetSprites->mSpriteWidth;
			float yTextureCoord = mAlphabetSprites->mDimensions[mOverWordIndex[i]].mPosY / mAlphabetSprites->mSpriteHeight;
			float textureCoordWidth = (mAlphabetSprites->mDimensions[mOverWordIndex[i]].mPosX + mAlphabetSprites->mDimensions[mOverWordIndex[i]].mWidth) / mNumeralSprites->mSpriteWidth;
			float textureCoordHeight = (mAlphabetSprites->mDimensions[mOverWordIndex[i]].mPosY + mAlphabetSprites->mDimensions[mOverWordIndex[i]].mHeight) / mNumeralSprites->mSpriteHeight;

			SetupVertices(xTextureCoord, yTextureCoord, textureCoordWidth, textureCoordHeight);
			BindBuffers();
			DrawIndividualSprite(gameOverPosX, gameOverPosY, sLETTER_SCALE);

			gameOverPosX += sLETTER_WIDTH + sSPACE_BETWEEM_LETTERS;
		}
		/**************************************************************************************************************************************************************************/
	}

	void TextManager::DisplayPlayerScore(int32_t score)
	{
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}

		float scorePosX = sSCORE_POS_X;
		float scorePosY = sSCORE_POS_Y;

		/******************************************************* Displaying the player score **************************************************************************************/
		for (int i = 0; i < sNUMBER_OF_SCORE_DIGITS; i++)
		{
			int32_t numberToDisplay = score % 10;
			score = score / 10;

			float xTextureCoord = mNumeralSprites->mDimensions[numberToDisplay].mPosX / mNumeralSprites->mSpriteWidth;
			float yTextureCoord = mNumeralSprites->mDimensions[numberToDisplay].mPosY / mNumeralSprites->mSpriteHeight;
			float textureCoordWidth = (mNumeralSprites->mDimensions[numberToDisplay].mPosX + mNumeralSprites->mDimensions[numberToDisplay].mWidth) / mNumeralSprites->mSpriteWidth;
			float textureCoordHeight = (mNumeralSprites->mDimensions[numberToDisplay].mPosY + mNumeralSprites->mDimensions[numberToDisplay].mHeight) / mNumeralSprites->mSpriteHeight;

			SetupVertices(xTextureCoord, yTextureCoord, textureCoordWidth, textureCoordHeight);
			BindBuffers();
			DrawIndividualSprite(scorePosX, scorePosY, sNUMBER_SCALE);

			scorePosX -= sNUMBER_WIDTH + sSPACE_BETWEEN_NUMBERS;
		}
		/**************************************************************************************************************************************************************************/
	}

	void TextManager::DisplayCurrentPhase(int32_t currentPhase)
	{
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}

		float phaseNumberPosX = currentPhase < sMAX_SINGLE_DIGIT ? sPHASE_NUMBER_POS_X : (sPHASE_NUMBER_POS_X + sNUMBER_WIDTH + sSPACE_BETWEEN_NUMBERS);
		float phaseNumberPosY = sPHASE_NUMBER_POS_Y;

		/******************************************************* Displaying the current phase number ******************************************************************************/
		while (currentPhase != 0)
		{
			int32_t numberToDisplay = currentPhase % 10;
			currentPhase = currentPhase / 10;

			float xTextureCoord = mNumeralSprites->mDimensions[numberToDisplay].mPosX / mNumeralSprites->mSpriteWidth;
			float yTextureCoord = mNumeralSprites->mDimensions[numberToDisplay].mPosY / mNumeralSprites->mSpriteHeight;
			float textureCoordWidth = (mNumeralSprites->mDimensions[numberToDisplay].mPosX + mNumeralSprites->mDimensions[numberToDisplay].mWidth) / mNumeralSprites->mSpriteWidth;
			float textureCoordHeight = (mNumeralSprites->mDimensions[numberToDisplay].mPosY + mNumeralSprites->mDimensions[numberToDisplay].mHeight) / mNumeralSprites->mSpriteHeight;

			SetupVertices(xTextureCoord, yTextureCoord, textureCoordWidth, textureCoordHeight);
			BindBuffers();
			DrawIndividualSprite(phaseNumberPosX, phaseNumberPosY, sNUMBER_SCALE);

			phaseNumberPosX -= sNUMBER_WIDTH + sSPACE_BETWEEN_NUMBERS;
		}
		/**************************************************************************************************************************************************************************/
	}

	TextManager* TextManager::GetInstance()
	{
		if (sInstance == nullptr)
		{
			sInstance = new TextManager();
		}
		return sInstance;
	}

	void TextManager::InitiazeIndexes()
	{
		// Create an index buffer
		const uint32_t indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		mIndexCount = ARRAYSIZE(indices);

		D3D11_BUFFER_DESC indexBufferDesc = { 0 };
		indexBufferDesc.ByteWidth = sizeof(uint32_t) * mIndexCount;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubResourceData = { 0 };
		indexSubResourceData.pSysMem = indices;
		ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexSubResourceData, mIndexBuffer.ReleaseAndGetAddressOf()));
	}

	void TextManager::SetupVertices(float xTextureCoord, float yTextureCoord, float textureCoordWidth, float textureCoordHeight)
	{
		//Top left pivot - right faced
		mVertices[0] = VertexPositionTexture(XMFLOAT4(0.0f, -1.0f, 0.1f, 1.0f), XMFLOAT2(xTextureCoord, textureCoordHeight));				//Bottom left
		mVertices[1] = VertexPositionTexture(XMFLOAT4(0.0f, 0.0f, 0.1f, 1.0f), XMFLOAT2(xTextureCoord, yTextureCoord));						//Top left
		mVertices[2] = VertexPositionTexture(XMFLOAT4(1.0f, 0.0f, 0.1f, 1.0f), XMFLOAT2(textureCoordWidth, yTextureCoord));					//Top right
		mVertices[3] = VertexPositionTexture(XMFLOAT4(1.0f, -1.0f, 0.1f, 1.0f), XMFLOAT2(textureCoordWidth, textureCoordHeight));			//Bottom right

		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(VertexPositionTexture) * ARRAYSIZE(mVertices);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData = { 0 };
		vertexSubResourceData.pSysMem = mVertices;
		ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, mVertexBuffer.ReleaseAndGetAddressOf()));
	}

	void TextManager::BindBuffers()
	{
		ID3D11DeviceContext* direct3DDeviceContext = mDeviceResources->GetD3DDeviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		direct3DDeviceContext->IASetInputLayout(mInputLayout.Get());

		static const UINT stride = sizeof(VertexPositionTexture);
		static const UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		direct3DDeviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
		direct3DDeviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);
		direct3DDeviceContext->VSSetConstantBuffers(0, 1, mVSCBufferPerObject.GetAddressOf());
		direct3DDeviceContext->PSSetShaderResources(0, 1, mSpriteSheet.GetAddressOf());
		direct3DDeviceContext->PSSetSamplers(0, 1, mTextureSampler.GetAddressOf());
		direct3DDeviceContext->OMSetBlendState(mAlphaBlending.Get(), 0, 0xFFFFFFFF);
	}

	void TextManager::DrawIndividualSprite(float xPosition, float yPosition, const XMFLOAT2& spriteScale)
	{
		//Drawing sprites		
		ID3D11DeviceContext* direct3DDeviceContext = mDeviceResources->GetD3DDeviceContext();
		XMFLOAT2 position(xPosition, yPosition);
		Transform2D transform(position, 0.0f, spriteScale);
		XMMATRIX wvp = XMMatrixTranspose(transform.WorldMatrix() * mCamera->ViewProjectionMatrix());
		XMStoreFloat4x4(&mVSCBufferPerObjectData.WorldViewProjection, wvp);

		XMMATRIX textureTransform = XMLoadFloat4x4(&mTextureTransform);
		XMStoreFloat4x4(&mVSCBufferPerObjectData.TextureTransform, XMMatrixTranspose(textureTransform));
		direct3DDeviceContext->UpdateSubresource(mVSCBufferPerObject.Get(), 0, nullptr, &mVSCBufferPerObjectData, 0, 0);

		direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);
	}

	TextManager::TextManager()
		:mLoadingComplete(false), mIndexCount(0), mTextureTransform(MatrixHelper::Identity)
	{		
		mNumeralSprites = &Animator::GetInstance()->mNumerals;
		mAlphabetSprites = &Animator::GetInstance()->mAlphabets;
	}
}