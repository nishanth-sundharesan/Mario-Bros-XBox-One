#include "pch.h"
#include "Crab.h"

using namespace std;
using namespace DX;
using namespace DirectX;
using namespace Managers;
using namespace Animation;
using namespace Audio;

namespace GameEntity
{
	const float Crab::sCRAB_Y_VELOCITY = 195.0f;

	const float Crab::sCRAB_RED_X_VELOCITY = 65.0f;
	const float Crab::sCRAB_RED_ANGRY_X_VELOCITY = 80.0f;
	const float Crab::sCRAB_GREEN_X_VELOCITY = 105.0f;
	const float Crab::sCRAB_GREEN_ANGRY_X_VELOCITY = 129.0f;
	const float Crab::sCRAB_BLUE_X_VELOCITY = 152.0f;
	const float Crab::sCRAB_BLUE_ANGRY_X_VELOCITY = 157.0f;

	const float Crab::sCRAB_RED_GRAVITY = -700.0f;
	const float Crab::sCRAB_GREEN_GRAVITY = -800.0f;
	const float Crab::sCRAB_BLUE_GRAVITY = -900.0f;

	const float Crab::sCRAB_MOVE_SPRITE_SPEED = 100.0f;
	const float Crab::sCRAB_FLIP_INITIAL_SPRITE_SPEED = 1000.0f;
	const float Crab::sCRAB_FLIP_DELTA_SPRITE_SPEED = 61.0f;
	const float Crab::sCRAB_LOOK_AROUND_SPRITE_SPEED = 200.0f;

	const float Crab::sCRAB_LOOK_AROUND_WAIT_DURATION = 300.0f;
	const float Crab::sCRAB_FLIPPED_WAIT_DURATION = 9000.0f;

	Crab::Crab(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera, bool isSpawnLeft, int32_t enemyID, EnemyManager& enemyManager)
		:Enemy(deviceResources, camera, isSpawnLeft, enemyID, enemyManager), mLoadingComplete(false), mIndexCount(0), mTextureTransform(MatrixHelper::Identity)
	{
		//TODO pass all these variables to the constructor of Enemy class

		Animator::GetInstance()->GetCrabSprites(mCrabSprites, EnemyStage::RED);
		mCurrentForm = EnemyEvolvedForm::FORM1;
		mCurrentStateXVelocity = sCRAB_RED_X_VELOCITY;
		mCurrentStateGravity = sCRAB_RED_GRAVITY;
		mCurrentStateAngryXVelocity = sCRAB_RED_ANGRY_X_VELOCITY;

		mXVelocity = mIsMovingRight ? mCurrentStateXVelocity : -mCurrentStateXVelocity;
		mEnemyType = EnemyType::CRAB;

		mIsRenderDuplicateSprites = false;
		mGravity = 0;
		mSpriteIndex = 0;
		mYVelocity = 0;
		mTempDuration = 0;


		mPosX = mIsMovingRight ? sENEMY_SPAWN_POS_LEFT_X : sENEMY_SPAWN_POS_RIGHT_X;
		mPosY = sENEMY_SPAWN_POS_Y;
		mCurrentState = EnemyState::SPAWNING;

		mFlipSpriteCap = sCRAB_FLIP_INITIAL_SPRITE_SPEED;
		mIsAngry = false;
	}

	void Crab::CreateDeviceDependentResources()
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
			ThrowIfFailed(CreateWICTextureFromFile(mDeviceResources->GetD3DDevice(), L"Content\\Textures\\Enemies.png", nullptr, mSpriteSheet.ReleaseAndGetAddressOf()));
			InitiazeIndexes();
		});

		// Once the cube is loaded, the object is ready to be rendered.
		loadSpriteSheetAndCreateSpritesTask.then([this]() {
			mLoadingComplete = true;
		});
	}

	void Crab::ReleaseDeviceDependentResources()
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

	void Crab::Update(const StepTimer& timer)
	{
		if (mCurrentState == EnemyState::LOOKAROUND)
		{
			mTempDuration += timer.GetElapsedMilliSeconds();
			if (mTempDuration > sCRAB_LOOK_AROUND_WAIT_DURATION)
			{
				mIsMovingRight = !mIsMovingRight;
				ResetEnemyMovement();
			}
		}
		else if (mCurrentState == EnemyState::FLIPPED)
		{
			mTempDuration += timer.GetElapsedMilliSeconds();
			if (mTempDuration > sCRAB_FLIPPED_WAIT_DURATION)
			{
				EvolveTheEnemy();
				ResetEnemyMovement();
			}
		}

		mYVelocity += mGravity* (timer.GetElapsedMilliSeconds() / 1000.0f);
		mPosY += mYVelocity* (timer.GetElapsedMilliSeconds() / 1000.0f);
		mPosX += mXVelocity * (timer.GetElapsedMilliSeconds() / 1000.0f);

		//Rendering duplicate sprite images when the enemy goes out of the screen
		if (mPosX + sENEMY_WIDTH > mOrthoWidth)
		{
			mIsRenderDuplicateSprites = true;
			if (mPosX + sHALF_WIDTH >= mOrthoWidth)
			{
				mXDuplicateEnemy = mPosX;
				mPosX = -sHALF_WIDTH;
			}
			else
			{
				mXDuplicateEnemy = mPosX - mOrthoWidth;
			}
		}
		else if (mPosX < 0)
		{
			mIsRenderDuplicateSprites = true;
			if (mPosX + sHALF_WIDTH <= 0)
			{
				mXDuplicateEnemy = mPosX;
				mPosX = mOrthoWidth - sHALF_WIDTH;
			}
			else
			{
				mXDuplicateEnemy = mOrthoWidth + mPosX;
			}
		}
		else
		{
			mIsRenderDuplicateSprites = false;
		}
	}

	void Crab::Render(const StepTimer& timer)
	{
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}

		UpdateEnemySpriteData(timer);

		float xTextureCoord = mCurrentAnimation->mDimensions[mSpriteIndex].mPosX / mCurrentAnimation->mSpriteWidth;
		float yTextureCoord = mCurrentAnimation->mDimensions[mSpriteIndex].mPosY / mCurrentAnimation->mSpriteHeight;
		float textureCoordWidth = (mCurrentAnimation->mDimensions[mSpriteIndex].mPosX + mCurrentAnimation->mDimensions[mSpriteIndex].mWidth) / mCurrentAnimation->mSpriteWidth;
		float textureCoordHeight = (mCurrentAnimation->mDimensions[mSpriteIndex].mPosY + mCurrentAnimation->mDimensions[mSpriteIndex].mHeight) / mCurrentAnimation->mSpriteHeight;

		if (mIsMovingRight)
		{
			SetupVertices(textureCoordWidth, yTextureCoord, xTextureCoord, textureCoordHeight);
		}
		else
		{
			SetupVertices(xTextureCoord, yTextureCoord, textureCoordWidth, textureCoordHeight);
		}

		BindBuffers();

		//Drawing sprites
		DrawIndividualSprite(mPosX, mPosY, sENEMY_SPRITE_SCALE);
		if (mIsRenderDuplicateSprites)
		{
			DrawIndividualSprite(mXDuplicateEnemy, mPosY, sENEMY_SPRITE_SCALE);
		}
	}

	void Crab::CollidedWithFloor(float newEnemyYPosition, EnemyState previousState)
	{
		mPosY = newEnemyYPosition;
		mGravity = 0;
		mYVelocity = 0;
		if (previousState == EnemyState::FALLING)
		{
			if (mIsAngry)
			{
				mXVelocity = mIsMovingRight ? mCurrentStateAngryXVelocity : -mCurrentStateAngryXVelocity;
			}
			else
			{
				mXVelocity = mIsMovingRight ? mCurrentStateXVelocity : -mCurrentStateXVelocity;
			}

			mCurrentState = EnemyState::MOVING;
		}
		else if (previousState == EnemyState::FLIPPED)
		{
			mXVelocity = 0;																							//For the flipping state		
		}
	}

	void Crab::FellFromFloor()
	{
		mGravity = mCurrentStateGravity;
		mCurrentState = EnemyState::FALLING;
	}

	void Crab::EscapeFromPipe(float newEnemyYPosition)
	{
		mCurrentState = EnemyState::RESPAWNING;
		mPosY = newEnemyYPosition;
		mYVelocity = 0;
		mGravity = 0;
	}

	void Crab::ReInitEnemyPositionAndState()
	{
		mPosX = mIsMovingRight ? sENEMY_SPAWN_POS_RIGHT_X : sENEMY_SPAWN_POS_LEFT_X;
		mPosY = sENEMY_SPAWN_POS_Y;
		mCurrentState = EnemyState::SPAWNING;
		mXVelocity = -mXVelocity;
		mIsMovingRight = !mIsMovingRight;
	}

	void Crab::CollidedWithAnotherObject()
	{
		mCurrentState = EnemyState::LOOKAROUND;
		mXVelocity = 0;
		mYVelocity = 0;
		mGravity = 0;
		ResetSpriteIndexAndSpeed();
	}

	void Crab::HitTaken(FlipDirection flipDirection)
	{
		switch (flipDirection)
		{
		case FlipDirection::LEFT:
			mXVelocity = -mCurrentStateXVelocity;
			break;

		case FlipDirection::RIGHT:
			mXVelocity = mCurrentStateXVelocity;
			break;

		case FlipDirection::STRAIGHT:
			mXVelocity = 0;
			break;
		}

		mYVelocity = sCRAB_Y_VELOCITY;
		mGravity = mCurrentStateGravity;
		ResetSpriteIndexAndSpeed();
		mTempDuration = 0.0;
		mFlipSpriteCap = sCRAB_FLIP_INITIAL_SPRITE_SPEED;

		if (mCurrentState == EnemyState::FLIPPED)
		{
			mCurrentState = EnemyState::FALLING;
			mIsMovingRight = mXVelocity >= 0 ? true : false;
			mIsAngry = false;
		}
		else
		{
			if (mIsAngry)
			{
				mCurrentState = EnemyState::FLIPPED;
			}
			else
			{
				mIsAngry = true;
				mCurrentState = EnemyState::FALLING;
			}
		}
		SoundManager::GetInstance()->PlayEnemyFlipSound();
	}

	void Crab::EvolveTheEnemy()
	{
		mIsAngry = false;
		if (mCurrentForm == EnemyEvolvedForm::FORM1)
		{
			Animator::GetInstance()->GetCrabSprites(mCrabSprites, EnemyStage::GREEN);
			mCurrentForm = EnemyEvolvedForm::FORM2;
			mCurrentStateXVelocity = sCRAB_GREEN_X_VELOCITY;
			mCurrentStateGravity = sCRAB_GREEN_GRAVITY;
			mCurrentStateAngryXVelocity = sCRAB_GREEN_ANGRY_X_VELOCITY;
		}
		else if (mCurrentForm == EnemyEvolvedForm::FORM2)
		{
			Animator::GetInstance()->GetCrabSprites(mCrabSprites, EnemyStage::BLUE);
			mCurrentForm = EnemyEvolvedForm::FORM3;
			mCurrentStateXVelocity = sCRAB_BLUE_X_VELOCITY;
			mCurrentStateGravity = sCRAB_BLUE_GRAVITY;
			mCurrentStateAngryXVelocity = sCRAB_BLUE_ANGRY_X_VELOCITY;
		}
	}

	void Crab::ResetEnemyMovement()
	{
		mCurrentState = EnemyState::MOVING;
		mXVelocity = mIsMovingRight ? mCurrentStateXVelocity : -mCurrentStateXVelocity;
		mTempDuration = 0.0;
		mFlipSpriteCap = sCRAB_FLIP_INITIAL_SPRITE_SPEED;
		ResetSpriteIndexAndSpeed();
	}

	void Crab::GotKilled(bool isPushRight)
	{
		mXVelocity = isPushRight ? sENEMY_DEATH_X_VELOCITY : -sENEMY_DEATH_X_VELOCITY;
		mYVelocity = sENEMY_DEATH_Y_VELOCITY;
		mGravity = sENEMY_DEATH_GRAVITY;

		mCurrentState = EnemyState::DEAD;

		SoundManager::GetInstance()->PlayPlayerKickSound();

		if (!mEnemyManager.CheckIfLastEnemy())
		{
			SoundManager::GetInstance()->PlayEnemyFallingSound();
		}
	}

	void Crab::EvolveToFinalState()
	{
		Animator::GetInstance()->GetCrabSprites(mCrabSprites, EnemyStage::BLUE);
		mCurrentForm = EnemyEvolvedForm::FORM3;
		mCurrentStateXVelocity = sCRAB_BLUE_X_VELOCITY;
		mCurrentStateGravity = sCRAB_BLUE_GRAVITY;
		mCurrentStateAngryXVelocity = sCRAB_BLUE_ANGRY_X_VELOCITY;

		if (mXVelocity < 0)
		{
			mXVelocity = mIsAngry ? -mCurrentStateAngryXVelocity : -mCurrentStateXVelocity;
		}
		else if (mXVelocity > 0)
		{
			mXVelocity = mIsAngry ? mCurrentStateAngryXVelocity : mCurrentStateXVelocity;
		}
		if (mGravity != 0)
		{
			mGravity = mCurrentStateGravity;
		}
	}

	void Crab::InitiazeIndexes()
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

	void Crab::SetupVertices(float xTextureCoord, float yTextureCoord, float textureCoordWidth, float textureCoordHeight)
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

	void Crab::BindBuffers()
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

	void Crab::DrawIndividualSprite(float xPosition, float yPosition, const XMFLOAT2& spriteScale)
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

	void Crab::UpdateEnemySpriteData(const StepTimer& timer)
	{
		switch (mCurrentState)
		{
		case EnemyState::MOVING:
		case EnemyState::SPAWNING:
		case EnemyState::RESPAWNING:
		case EnemyState::FALLING:
			if (mIsAngry)
			{
				mCurrentAnimation = &mCrabSprites.mAngryMoving;
			}
			else
			{
				mCurrentAnimation = &mCrabSprites.mMoving;
			}

			mSpriteSpeed += timer.GetElapsedMilliSeconds();
			if (mSpriteSpeed > sCRAB_MOVE_SPRITE_SPEED)
			{
				mSpriteIndex++;
				if (mSpriteIndex > mCurrentAnimation->mLength - 1)
				{
					mSpriteIndex = 0;
				}
				mSpriteSpeed = 0;
			}
			break;

		case EnemyState::FLIPPED:
			mCurrentAnimation = &mCrabSprites.mFlipped;
			mSpriteSpeed += timer.GetElapsedMilliSeconds();
			if (mSpriteSpeed > mFlipSpriteCap)
			{
				mFlipSpriteCap -= sCRAB_FLIP_DELTA_SPRITE_SPEED;
				if (mFlipSpriteCap < 0)
				{
					mFlipSpriteCap = sCRAB_FLIP_DELTA_SPRITE_SPEED;
				}

				mSpriteIndex++;
				if (mSpriteIndex > mCurrentAnimation->mLength - 1)
				{
					mSpriteIndex = 0;
				}
				mSpriteSpeed = 0;
			}
			break;

		case EnemyState::DEAD:
			mCurrentAnimation = &mCrabSprites.mFlipped;
			break;

		case EnemyState::LOOKAROUND:
			mCurrentAnimation = &mCrabSprites.mLookAround;
			mSpriteSpeed += timer.GetElapsedMilliSeconds();
			if (mSpriteSpeed > sCRAB_LOOK_AROUND_SPRITE_SPEED)
			{
				mSpriteIndex++;
				if (mSpriteIndex > mCurrentAnimation->mLength - 1)
				{
					mSpriteIndex = 0;
				}
				mSpriteSpeed = 0;
			}
			break;
		}
	}
}