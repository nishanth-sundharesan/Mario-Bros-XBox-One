#include "pch.h"
#include "Blocks.h"
#include "PhysicsManager.h"

using namespace std;
using namespace DX;
using namespace DirectX;
using namespace Windows::UI::Core;
using namespace Animation;
using namespace Managers;

namespace GameEntity
{
	const float Blocks::sBLOCK_WIDTH = 16.0f;
	const float Blocks::sBLOCK_HEIGHT = 16.0f;

	const float Blocks::sBLOCK_HALF_WIDTH = sBLOCK_WIDTH / 2.0f;

	const float Blocks::sTOP_BLOCKS_POS_Y = 318.0f;
	const float Blocks::sMIDDLE_LEFT_BLOCKS_POS_Y = 206.0f;
	const float Blocks::sMIDDLE_BLOCKS_POS_Y = sMIDDLE_LEFT_BLOCKS_POS_Y + sBLOCK_HEIGHT;
	const float Blocks::sBOTTOM_BLOCKS_POS_Y = 126.0f;

	const int32_t Blocks::sNUMBER_OF_TOP_BLOCKS = 14;
	const int32_t Blocks::sNUMBER_OF_MIDDLE_LEFT_BLOCKS = 4;
	const int32_t Blocks::sNUMBER_OF_MIDDLE_BLOCKS = 16;
	const int32_t Blocks::sNUMBER_OF_BOTTOM_BLOCKS = 12;

	const float Blocks::sMIDDLE_BLOCKS_POS_X = 128.0f;

	const float Blocks::sFLEXIBLE_BLOCKS_SPEED = 75.0f;
	const int32_t Blocks::sTOTAL_FLEXIBLE_BLOCKS = 3;

	const float Blocks::sADDITIONAL_BLOCK_WIDTH = 100.0f;

	const XMFLOAT2 Blocks::sBLOCK_SPRITE_SCALE(sBLOCK_WIDTH, sBLOCK_HEIGHT);

	Blocks::Blocks(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera)
		:DrawableGameComponent(deviceResources, camera), mLoadingComplete(false), mIndexCount(0),
		mTextureTransform(MatrixHelper::Identity), mOrthoWidth(OrthographicCamera::DefaultViewWidth),
		mOrthoHeight(OrthographicCamera::DefaultViewHeight), mAnimator(Animator::GetInstance())
	{
		ResetFlexibleBlocks();
	}

	void Blocks::CreateDeviceDependentResources()
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
			ThrowIfFailed(CreateWICTextureFromFile(mDeviceResources->GetD3DDevice(), L"Content\\Textures\\Blocks.png", nullptr, mSpriteSheet.ReleaseAndGetAddressOf()));
			InitiazeIndexes();
		});

		// Once the cube is loaded, the object is ready to be rendered.
		loadSpriteSheetAndCreateSpritesTask.then([this]() {
			mLoadingComplete = true;
		});
	}

	void Blocks::ReleaseDeviceDependentResources()
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

	void Blocks::Render(const StepTimer& timer)
	{
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}		

		SetupVertices(mStaticBlockCoordinates.mXTextureCoord, mStaticBlockCoordinates.mYTextureCoord, mStaticBlockCoordinates.mTextureCoordWidth, mStaticBlockCoordinates.mTextureCoordHeight);
		BindBuffers();			

		//Drawing sprites
		/*********************************Displaying Bottom Blocks**************************************/
		float xPosition = 0;		
		float yPosition = sBOTTOM_BLOCKS_POS_Y;
		
		for (int i = 0; i < sNUMBER_OF_BOTTOM_BLOCKS; i++)
		{			
			DrawIndividualSprite(xPosition, yPosition, sBLOCK_SPRITE_SCALE);
			xPosition += sBLOCK_WIDTH;			
		}

		xPosition = mOrthoWidth - (sBLOCK_WIDTH * sNUMBER_OF_BOTTOM_BLOCKS) + 1;									//Adding a hard-coded value 1 for the first texture to display properly

		for (int i = 0; i < sNUMBER_OF_BOTTOM_BLOCKS; i++)
		{			
			DrawIndividualSprite(xPosition, yPosition, sBLOCK_SPRITE_SCALE);
			xPosition += sBLOCK_WIDTH;			
		}
		/***********************************************************************************************/

		/**************************Displaying Middle Left & Right Blocks********************************/
		xPosition = 0;
		yPosition = sMIDDLE_LEFT_BLOCKS_POS_Y;		

		for (int i = 0; i < sNUMBER_OF_MIDDLE_LEFT_BLOCKS; i++)
		{			
			DrawIndividualSprite(xPosition, yPosition, sBLOCK_SPRITE_SCALE);
			xPosition += sBLOCK_WIDTH;
		}

		xPosition = mOrthoWidth - (sBLOCK_WIDTH * sNUMBER_OF_MIDDLE_LEFT_BLOCKS) + 1;								//Adding a hard-coded value 1 for the first texture to display properly

		for (int i = 0; i < sNUMBER_OF_MIDDLE_LEFT_BLOCKS; i++)
		{
			DrawIndividualSprite(xPosition, yPosition, sBLOCK_SPRITE_SCALE);
			xPosition += sBLOCK_WIDTH;			
		}
		/***********************************************************************************************/

		/******************************Displaying Middle Blocks*****************************************/
		xPosition = sMIDDLE_BLOCKS_POS_X;
		yPosition = sMIDDLE_BLOCKS_POS_Y;

		for (int i = 0; i < sNUMBER_OF_MIDDLE_BLOCKS; i++)
		{
			DrawIndividualSprite(xPosition, yPosition, sBLOCK_SPRITE_SCALE);
			xPosition += sBLOCK_WIDTH;
		}
		/***********************************************************************************************/

		/********************************Displaying Top Blocks******************************************/
		xPosition = 0;
		yPosition = sTOP_BLOCKS_POS_Y;

		for (int i = 0; i < sNUMBER_OF_TOP_BLOCKS; i++)
		{
			DrawIndividualSprite(xPosition, yPosition, sBLOCK_SPRITE_SCALE);
			xPosition += sBLOCK_WIDTH;
		}

		xPosition = mOrthoWidth - (sBLOCK_WIDTH * sNUMBER_OF_TOP_BLOCKS);

		for (int i = 0; i < sNUMBER_OF_TOP_BLOCKS; i++)
		{
			DrawIndividualSprite(xPosition, yPosition, sBLOCK_SPRITE_SCALE);
			xPosition += sBLOCK_WIDTH;
		}
		/***********************************************************************************************/

		if (mRenderFlexibleBlocks)
		{
			RenderFlexibleBlocks(timer);
		}
	}

	void Blocks::Init(PhysicsManager& physicsManager)
	{
		physicsManager.AddPlatformCollisionBoxes(-sADDITIONAL_BLOCK_WIDTH, sBOTTOM_BLOCKS_POS_Y, (sNUMBER_OF_BOTTOM_BLOCKS * sBLOCK_WIDTH) + sADDITIONAL_BLOCK_WIDTH, sBLOCK_HEIGHT, BlockType::BOTTOMLEFTBLOCK);													//Bottom left blocks
		physicsManager.AddPlatformCollisionBoxes(mOrthoWidth - (sBLOCK_WIDTH * sNUMBER_OF_BOTTOM_BLOCKS), sBOTTOM_BLOCKS_POS_Y, (sNUMBER_OF_BOTTOM_BLOCKS * sBLOCK_WIDTH) + sADDITIONAL_BLOCK_WIDTH, sBLOCK_HEIGHT, BlockType::BOTTOMRIGHTBLOCK);					//Bottom right blocks

		physicsManager.AddPlatformCollisionBoxes(-sADDITIONAL_BLOCK_WIDTH, sMIDDLE_LEFT_BLOCKS_POS_Y, (sNUMBER_OF_MIDDLE_LEFT_BLOCKS * sBLOCK_WIDTH) + sADDITIONAL_BLOCK_WIDTH, sBLOCK_HEIGHT, BlockType::MIDDLELEFTBLOCK);											//Middle left blocks
		physicsManager.AddPlatformCollisionBoxes(mOrthoWidth - (sBLOCK_WIDTH * sNUMBER_OF_MIDDLE_LEFT_BLOCKS), sMIDDLE_LEFT_BLOCKS_POS_Y, (sNUMBER_OF_MIDDLE_LEFT_BLOCKS * sBLOCK_WIDTH) + sADDITIONAL_BLOCK_WIDTH, sBLOCK_HEIGHT, BlockType::MIDDLERIGHTBLOCK);	//Middle right blocks

		physicsManager.AddPlatformCollisionBoxes(-sADDITIONAL_BLOCK_WIDTH, sTOP_BLOCKS_POS_Y, (sNUMBER_OF_TOP_BLOCKS * sBLOCK_WIDTH) + sADDITIONAL_BLOCK_WIDTH, sBLOCK_HEIGHT, BlockType::TOPLEFTBLOCK);															//Top left blocks
		physicsManager.AddPlatformCollisionBoxes(mOrthoWidth - (sBLOCK_WIDTH * sNUMBER_OF_TOP_BLOCKS), sTOP_BLOCKS_POS_Y, (sNUMBER_OF_TOP_BLOCKS * sBLOCK_WIDTH) + sADDITIONAL_BLOCK_WIDTH, sBLOCK_HEIGHT, BlockType::TOPRIGHTBLOCK);								//Top right blocks

		physicsManager.AddPlatformCollisionBoxes(sMIDDLE_BLOCKS_POS_X, sMIDDLE_BLOCKS_POS_Y, sNUMBER_OF_MIDDLE_BLOCKS * sBLOCK_WIDTH, sBLOCK_HEIGHT, BlockType::MIDDLEBLOCK);																						//Middle blocks. Note that there is no additional width padding added to the middle block
																																																																	//as the entire block is visible to the screen

		const Sprites& currentBlock = mAnimator->GetCurrentStageBlock();											//Currently blue blocks are displayed in all levels

		mStaticBlockCoordinates.mXTextureCoord = currentBlock.mDimensions[0].mPosX / currentBlock.mSpriteWidth;
		mStaticBlockCoordinates.mYTextureCoord = currentBlock.mDimensions[0].mPosY / currentBlock.mSpriteHeight;

		mStaticBlockCoordinates.mTextureCoordWidth = (currentBlock.mDimensions[0].mPosX + currentBlock.mDimensions[0].mWidth) / currentBlock.mSpriteWidth;
		mStaticBlockCoordinates.mTextureCoordHeight = (currentBlock.mDimensions[0].mPosY + currentBlock.mDimensions[0].mHeight) / currentBlock.mSpriteHeight;
	}

	void Blocks::SetFlexibleBlocks(float hitPositionX, float hitPositionY, BlockType blockType)
	{
		if (!mRenderFlexibleBlocks)
		{
			int32_t numberOfBlocks = 0;
			float flexibleBlockPositionStart = 0;
			float flexibleBlockPositionEnd = 0;

			switch (blockType)
			{
			case BlockType::BOTTOMLEFTBLOCK:
				numberOfBlocks = sNUMBER_OF_BOTTOM_BLOCKS;
				mFlexibleSpritePositionLeft = 0;
				break;

			case BlockType::BOTTOMRIGHTBLOCK:
				numberOfBlocks = sNUMBER_OF_BOTTOM_BLOCKS;
				mFlexibleSpritePositionLeft = mOrthoWidth - (sBLOCK_WIDTH * sNUMBER_OF_BOTTOM_BLOCKS);
				break;

			case BlockType::MIDDLELEFTBLOCK:
				numberOfBlocks = sNUMBER_OF_MIDDLE_LEFT_BLOCKS;
				mFlexibleSpritePositionLeft = 0;
				break;

			case BlockType::MIDDLERIGHTBLOCK:
				numberOfBlocks = sNUMBER_OF_MIDDLE_LEFT_BLOCKS;
				mFlexibleSpritePositionLeft = mOrthoWidth - (sBLOCK_WIDTH * sNUMBER_OF_MIDDLE_LEFT_BLOCKS);

				break;
			case BlockType::MIDDLEBLOCK:
				numberOfBlocks = sNUMBER_OF_MIDDLE_BLOCKS;
				mFlexibleSpritePositionLeft = sMIDDLE_BLOCKS_POS_X;
				break;

			case BlockType::TOPLEFTBLOCK:
				numberOfBlocks = sNUMBER_OF_TOP_BLOCKS;
				mFlexibleSpritePositionLeft = 0;
				break;

			case BlockType::TOPRIGHTBLOCK:
				numberOfBlocks = sNUMBER_OF_TOP_BLOCKS;
				mFlexibleSpritePositionLeft = mOrthoWidth - (sBLOCK_WIDTH * sNUMBER_OF_TOP_BLOCKS);
				break;
			}

			flexibleBlockPositionStart = mFlexibleSpritePositionLeft;															//Starting X position of the first block of the Touched blocks
			flexibleBlockPositionEnd = (flexibleBlockPositionStart + (numberOfBlocks * sBLOCK_WIDTH)) - sBLOCK_WIDTH;			//Starting X position of the last block of the Touched blocks

			for (int i = 0; i < numberOfBlocks; i++)
			{
				if (mFlexibleSpritePositionLeft > hitPositionX)
				{
					break;
				}
				mFlexibleSpritePositionLeft += sBLOCK_WIDTH;
			}
			mFlexibleSpritePositionLeft -= sBLOCK_WIDTH;

			if (mFlexibleSpritePositionLeft == flexibleBlockPositionStart)									//Checking if the X position is equal to the starting position of the first block
			{
				mFlexibleSpritePositionRight = (sTOTAL_FLEXIBLE_BLOCKS - 1) * sBLOCK_WIDTH;
				mIsFirstBlockAttacked = true;
			}
			else if (mFlexibleSpritePositionLeft == flexibleBlockPositionEnd)								//Checking if the X position is equal to the starting position of the last block
			{
				mFlexibleSpritePositionLeft -= sBLOCK_WIDTH;
				mFlexibleSpritePositionRight = (sTOTAL_FLEXIBLE_BLOCKS - 1) * sBLOCK_WIDTH;
				mIsLastBlockAttacked = true;
			}
			else																							//If the player has attacked in the middle of the blocks
			{
				mFlexibleSpritePositionLeft -= sBLOCK_WIDTH;
				mFlexibleSpritePositionRight = sTOTAL_FLEXIBLE_BLOCKS * sBLOCK_WIDTH;
			}

			mFlexibleSpritePositionRight += mFlexibleSpritePositionLeft;
			mFlexibleSpritePositionTop = hitPositionY;
			mFlexibleSpritePositionBottom = mFlexibleSpritePositionTop - sBLOCK_HEIGHT;
			mRenderFlexibleBlocks = true;
		}
	}	

	void Blocks::InitiazeIndexes()
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

	void Blocks::SetupVertices(float xTextureCoord, float yTextureCoord, float textureCoordWidth, float textureCoordHeight)
	{
		//Top left pivot - right faced
		mVertices[0] = VertexPositionTexture(XMFLOAT4(0.0f, -1.0f, 0.1f, 1.0f), XMFLOAT2(textureCoordWidth, textureCoordHeight));				//Bottom left
		mVertices[1] = VertexPositionTexture(XMFLOAT4(0.0f, 0.0f, 0.1f, 1.0f), XMFLOAT2(textureCoordWidth, yTextureCoord));						//Top left
		mVertices[2] = VertexPositionTexture(XMFLOAT4(1.0f, 0.0f, 0.1f, 1.0f), XMFLOAT2(xTextureCoord, yTextureCoord));							//Top right
		mVertices[3] = VertexPositionTexture(XMFLOAT4(1.0f, -1.0f, 0.1f, 1.0f), XMFLOAT2(xTextureCoord, textureCoordHeight));					//Bottom right

		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(VertexPositionTexture) * ARRAYSIZE(mVertices);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData = { 0 };
		vertexSubResourceData.pSysMem = mVertices;
		ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, mVertexBuffer.ReleaseAndGetAddressOf()));
	}

	void Blocks::BindBuffers()
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

	void Blocks::DrawIndividualSprite(float xPosition, float yPosition, const DirectX::XMFLOAT2& spriteScale)
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

	void Blocks::ResetFlexibleBlocks()
	{
		mSpriteIndex = 0;
		mSpriteSpeed = 0;
		mIsFirstBlockAttacked = false;
		mIsLastBlockAttacked = false;
		mRenderFlexibleBlocks = false;
	}

	void Blocks::ClearStaticBlocks()
	{
		//TODO
	}

	void Blocks::UpdateFlexibleBlockSpriteData(const StepTimer& timer, const Sprites& currentFlexibleBlock)
	{		
		mSpriteSpeed += timer.GetElapsedMilliSeconds();
		if (mSpriteSpeed > sFLEXIBLE_BLOCKS_SPEED)
		{
			mSpriteIndex++;
			if (mSpriteIndex > currentFlexibleBlock.mLength - 1)
			{
				mSpriteIndex = 0;
				ResetFlexibleBlocks();
			}
			mSpriteSpeed = 0;
		}
	}

	void Blocks::RenderFlexibleBlocks(const StepTimer& timer)
	{
		//TODO test this
		ClearStaticBlocks();

		const Sprites& currentFlexibleBlock = mAnimator->GetCurrentStageFlexibleBlock();					//Currently blue blocks are displayed in all levels
		UpdateFlexibleBlockSpriteData(timer, currentFlexibleBlock);

		float xTextureCoord = (currentFlexibleBlock.mDimensions[mSpriteIndex].mPosX + (mIsFirstBlockAttacked ? (currentFlexibleBlock.mDimensions[mSpriteIndex].mWidth / sTOTAL_FLEXIBLE_BLOCKS) : 0)) / currentFlexibleBlock.mSpriteWidth;
		float yTextureCoord = currentFlexibleBlock.mDimensions[mSpriteIndex].mPosY / currentFlexibleBlock.mSpriteHeight;
		float textureCoordWidth = (currentFlexibleBlock.mDimensions[mSpriteIndex].mPosX + (currentFlexibleBlock.mDimensions[mSpriteIndex].mWidth - (mIsLastBlockAttacked ? currentFlexibleBlock.mDimensions[mSpriteIndex].mWidth / sTOTAL_FLEXIBLE_BLOCKS : 0))) / currentFlexibleBlock.mSpriteWidth;
		float textureCoordHeight = (currentFlexibleBlock.mDimensions[mSpriteIndex].mPosY + currentFlexibleBlock.mDimensions[mSpriteIndex].mHeight) / currentFlexibleBlock.mSpriteHeight;

		SetupVertices(xTextureCoord, yTextureCoord, textureCoordWidth, textureCoordHeight);	
		BindBuffers();
		DrawIndividualSprite(mFlexibleSpritePositionLeft, mFlexibleSpritePositionTop, XMFLOAT2(abs(mFlexibleSpritePositionLeft - mFlexibleSpritePositionRight), abs((mFlexibleSpritePositionTop + sBLOCK_HEIGHT) - mFlexibleSpritePositionBottom)));
	}
}