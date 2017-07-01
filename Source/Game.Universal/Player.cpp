#include "pch.h"
#include "Player.h"
#include "Animator.h"

using namespace std;
using namespace DX;
using namespace DirectX;
using namespace Windows::UI::Core;
using namespace Animation;

namespace GameEntity
{
	/******************* Player Constants ******************************/
	const int32_t Player::sPLAYER_1_INITIAL_POS_X = 135;
	const int32_t Player::sPLAYER_1_INITIAL_POS_Y = 75;
	const int32_t Player::sPLAYER_HEIGHT = 45;
	const int32_t Player::sPLAYER_WIDTH = 33;
	const int32_t Player::sPLAYER_X_VELOCITY = 130;
	const int32_t Player::sPLAYER_X_JUMP_VELOCITY = 75;
	const int32_t Player::sPLAYER_X_JUMP_QUANTUM_VELOCITY = 10;
	const int32_t Player::sPLAYER_GRAVITY = -685;
	const int32_t Player::sPLAYER_Y_VELOCITY = 400;
	const int32_t Player::sPLAYER_Y_VELOCITY_CAP = 120;
	const int32_t Player::sPLAYER_Y_DEAD_VELOCITY = 200;
	const int32_t Player::sPLAYER_DEAD_GRAVITY = -850;
	const int32_t Player::sPLAYER_RESPAWN_Y_VELOCITY = -38;
	const int32_t Player::sPLAYER_RESPAWN_START_POS_X = 240;
	const int32_t Player::sPLAYER_RESPAWN_START_POS_Y = 450;
	const int32_t Player::sPLAYER_RESPAWN_STOP_POS_Y = 400;

	const int32_t Player::sKILL_SCORE = 800;
	const int32_t Player::sFLIP_SCORE = 10;

	const int32_t Player::sPLAYER_LIVES_X_POS = 126;
	const int32_t Player::sPLAYER_LIVES_Y_POS = 390;
	const int32_t Player::sPLAYER_LIVES_WIDTH = 16;
	const int32_t Player::sPLAYER_LIVES_HEIGHT = 12;
	const int32_t Player::sDISTANCE_BETWEEN_LIVES = 10;

	const int32_t Player::sNUMBER_OF_LIVES = 3;

	const int32_t Player::sATTACK_WAIT_DURATION = 300;
	const int32_t Player::sPLAYER_DEAD_WAIT_DURATION = 1000;
	const int32_t Player::sPLAYER_RESPAWN_WAIT_DURATION = 12000;
	const int32_t Player::sPLAYER_MOVE_SPRITE_SPEED = 30;
	const int32_t Player::sPADDLE_SPRITE_SPEED = sPLAYER_RESPAWN_WAIT_DURATION / 3;
	const int32_t Player::sSLIP_DURATION_CAP = 150;

	const int32_t Player::sPADDLE_WIDTH = 31;
	const int32_t Player::sPADDLE_HEIGHT = 12;

	const XMFLOAT2 Player::sSPRITE_SCALE(33.0f, 45.0f);

	const float	Player::sWIDTH(sSPRITE_SCALE.x);
	const float	Player::sHEIGHT(sSPRITE_SCALE.y);

	const float	Player::sHALFWIDTH(sSPRITE_SCALE.x / 2.0f);
	/******************************************************************/

	Player::Player(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera, const std::shared_ptr<DX::KeyboardComponent> keyboard)
		:DrawableGameComponent(deviceResources, camera), mLoadingComplete(false), mIndexCount(0), mTextureTransform(DX::MatrixHelper::Identity), CurrentPlayerState(nullptr),
		mOrthoWidth(OrthographicCamera::DefaultViewWidth), mOrthoHeight(OrthographicCamera::DefaultViewHeight), mAnimator(Animator::GetInstance()), mKeyboard(keyboard)
		//mCurrentAnimation(mAnimator->mMarioIdle), mLivesAnimation(mAnimator->mMarioLives), mPaddleAnimation(mAnimator->mPaddle)
	{
		mGamePad = make_shared<GamePadComponent>(mDeviceResources);
		//mCurrentAnimation(mAnimator->mMarioIdle), mLivesAnimation(mAnimator->mMarioLives), mPaddleAnimation(mAnimator->mPaddle)
		mCurrentAnimation = &mAnimator->mMarioIdle;
		mLivesAnimation = &mAnimator->mMarioLives;
		mPaddleAnimation = &mAnimator->mPaddle;
		ResetPlayer();
	}

	void Player::CreateDeviceDependentResources()
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
			ThrowIfFailed(CreateWICTextureFromFile(mDeviceResources->GetD3DDevice(), L"Content\\Textures\\Mario.png", nullptr, mSpriteSheet.ReleaseAndGetAddressOf()));
		});

		// Once the cube is loaded, the object is ready to be rendered.
		loadSpriteSheetAndCreateSpritesTask.then([this]() {
			mLoadingComplete = true;
		});
	}

	void Player::ReleaseDeviceDependentResources()
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

	void Player::Update(const StepTimer& timer)
	{
		//TODO
		timer;

		//inputManager->UpdateDeBounceKeys();

		if (CurrentPlayerState != nullptr)
		{
			CurrentPlayerState(timer);
		}

		/*mYVelocity += mGravity* ((float_t)milliseconds / 1000);
		mPosY += mYVelocity* ((float_t)milliseconds / 1000);
		mPosX += mXVelocity * ((float_t)milliseconds / 1000);*/

		mYVelocity += mGravity * static_cast<float>(timer.GetElapsedSeconds());
		mPosY += mYVelocity * static_cast<float>(timer.GetElapsedSeconds());
		mPosX += mXVelocity * static_cast<float>(timer.GetElapsedSeconds());

		//Rendering duplicate sprite images when the player goes out of the screen
		if (mPosX + sWIDTH > mOrthoWidth)
		{
			mIsRenderDuplicateSprites = true;
			if (mPosX + sHALFWIDTH >= mOrthoWidth)
			{
				mXDuplicatePlayer = mPosX;
				mPosX = -sHALFWIDTH;
			}
			else
			{
				mXDuplicatePlayer = mPosX - mOrthoWidth;
			}
		}
		else if (mPosX < 0)
		{
			mIsRenderDuplicateSprites = true;
			if (mPosX + sHALFWIDTH <= 0)
			{
				mXDuplicatePlayer = mPosX;
				mPosX = mOrthoWidth - sHALFWIDTH;
			}
			else
			{
				mXDuplicatePlayer = mOrthoWidth + mPosX;
			}
		}
		else
		{
			mIsRenderDuplicateSprites = false;
		}
	}

	void Player::Render(const StepTimer& timer)
	{
		//TODO
		timer;
		SetupVertexAndIndexBuffer(timer);

		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}

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

		//Drawing sprite
		XMFLOAT2 position(mPosX, mPosY);
		Transform2D transform(position, 0.0f, sSPRITE_SCALE);
		const XMMATRIX wvp = XMMatrixTranspose(transform.WorldMatrix() * mCamera->ViewProjectionMatrix());
		XMStoreFloat4x4(&mVSCBufferPerObjectData.WorldViewProjection, wvp);

		XMMATRIX textureTransform = XMLoadFloat4x4(&mTextureTransform);
		XMStoreFloat4x4(&mVSCBufferPerObjectData.TextureTransform, XMMatrixTranspose(textureTransform));
		direct3DDeviceContext->UpdateSubresource(mVSCBufferPerObject.Get(), 0, nullptr, &mVSCBufferPerObjectData, 0, 0);

		direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);
	}

	void Player::ResetPlayer()
	{
		mPosX = sPLAYER_1_INITIAL_POS_X;
		mPosY = sPLAYER_1_INITIAL_POS_Y;
		ChangeState(PlayerState::ONGROUND);
		mIsMovingRight = true;
		mIsRenderDuplicateSprites = false;
		mXVelocity = 0;
		mYVelocity = 0;
		mGravity = 0;
		mSlipDuration = 0;
		mAttackDuration = 0;
		mDeathDuration = 0;
		ResetPlayerSprites();
		ResetPaddleSprites();
		mReSpawnDuration = 0;
	}

	void Player::CollidedWithFloor(float newPlayerYPosition)
	{
		if ((mIsMovingRight && mKeyboard->IsKeyDown(Keys::Right)) || (!mIsMovingRight && mKeyboard->IsKeyDown(Keys::Left)))
		{
			ChangeState(PlayerState::ONGROUND);
		}
		else
		{
			ChangeState(PlayerState::SLIPPING);
		}

		ResetPlayerSprites();
		mPosY = newPlayerYPosition;
		mYVelocity = 0;
		mGravity = 0;
	}

	void Player::PlayerAttacked(float newPlayerYPosition)
	{
		ChangeState(PlayerState::ATTACKED);
		mPosY = newPlayerYPosition;
	}

	void Player::GotKilled()
	{
#if GOD_MODE == 0
		{
			mLivesLeft--;

			ChangeState(PlayerState::DEAD);
			//SoundManager::GetInstance()->PlayHurtSound();
		}
#endif
	}

	void Player::Init()
	{
		mLivesLeft = sNUMBER_OF_LIVES;
		mPlayerScore = 0;
	}

	void Player::SetupVertexAndIndexBuffer(const StepTimer& timer)
	{
		switch (currentState)
		{
		case PlayerState::RESPAWNING:
			mPaddleSpriteSpeed += timer.GetElapsedMilliSeconds();
			if (mPaddleSpriteSpeed > sPADDLE_SPRITE_SPEED)
			{
				mPaddleSpriteIndex++;
				mPaddleSpriteSpeed = 0;
			}
			RenderPaddle();
			break;
		case PlayerState::RUNNING:
		case PlayerState::FALLING:
			mPlayerSpriteSpeed += timer.GetElapsedMilliSeconds();
			if (mPlayerSpriteSpeed > sPLAYER_MOVE_SPRITE_SPEED)
			{
				mPlayerSpriteIndex++;
				if (mPlayerSpriteIndex > mCurrentAnimation->mLength - 1)
				{
					mPlayerSpriteIndex = 0;
				}
				mPlayerSpriteSpeed = 0;
			}
			break;
		default:
			break;
		}

		float xTextureCoord = mCurrentAnimation->mDimensions[mPlayerSpriteIndex].mPosX / mCurrentAnimation->mSpriteWidth;
		float yTextureCoord = mCurrentAnimation->mDimensions[mPlayerSpriteIndex].mPosY / mCurrentAnimation->mSpriteHeight;
		float textureCoordWidth = (mCurrentAnimation->mDimensions[mPlayerSpriteIndex].mPosX + mCurrentAnimation->mDimensions[mPlayerSpriteIndex].mWidth) / mCurrentAnimation->mSpriteWidth;
		float textureCoordHeight = (mCurrentAnimation->mDimensions[mPlayerSpriteIndex].mPosY + mCurrentAnimation->mDimensions[mPlayerSpriteIndex].mHeight) / mCurrentAnimation->mSpriteHeight;

		VertexPositionTexture vertices[4];
		if (mIsMovingRight)
		{
			//Top left pivot - right faced
			vertices[0] = VertexPositionTexture(XMFLOAT4(0.0f, -1.0f, 0.1f, 1.0f), XMFLOAT2(textureCoordWidth, textureCoordHeight));				//Bottom left
			vertices[1] = VertexPositionTexture(XMFLOAT4(0.0f, 0.0f, 0.1f, 1.0f), XMFLOAT2(textureCoordWidth, yTextureCoord));						//Top left
			vertices[2] = VertexPositionTexture(XMFLOAT4(1.0f, 0.0f, 0.1f, 1.0f), XMFLOAT2(xTextureCoord, yTextureCoord));							//Top right
			vertices[3] = VertexPositionTexture(XMFLOAT4(1.0f, -1.0f, 0.1f, 1.0f), XMFLOAT2(xTextureCoord, textureCoordHeight));					//Bottom right
		}
		else
		{

			//Top left pivot - left faced
			vertices[0] = VertexPositionTexture(XMFLOAT4(0.0f, -1.0f, 0.1f, 1.0f), XMFLOAT2(xTextureCoord, textureCoordHeight));					//Bottom left
			vertices[1] = VertexPositionTexture(XMFLOAT4(0.0f, 0.0f, 0.1f, 1.0f), XMFLOAT2(xTextureCoord, yTextureCoord));							//Top left
			vertices[2] = VertexPositionTexture(XMFLOAT4(1.0f, 0.0f, 0.1f, 1.0f), XMFLOAT2(textureCoordWidth, yTextureCoord));						//Top right
			vertices[3] = VertexPositionTexture(XMFLOAT4(1.0f, -1.0f, 0.1f, 1.0f), XMFLOAT2(textureCoordWidth, textureCoordHeight));				//Bottom right

			//Bottom left pivot - left faced
			//VertexPositionTexture(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)),						//Bottom left
			//VertexPositionTexture(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)),						//Top left
			//VertexPositionTexture(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)),						//Top right
			//VertexPositionTexture(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)),						//Bottom right

			//Bottom left pivot - right faced
			//VertexPositionTexture(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)),						
			//VertexPositionTexture(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)),						
			//VertexPositionTexture(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)),						
			//VertexPositionTexture(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)),						

		}
		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(VertexPositionTexture) * ARRAYSIZE(vertices);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData = { 0 };
		vertexSubResourceData.pSysMem = vertices;
		ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, mVertexBuffer.ReleaseAndGetAddressOf()));

		// Create and index buffer
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

	void Player::RenderPlayerScore()
	{
	}

	void Player::RenderPlayerLives()
	{
		//TODO
	}

	void Player::RenderPaddle()
	{
		//TODO
	}

	void Player::ResetPlayerSprites()
	{
		mPlayerSpriteSpeed = 0;
		mPlayerSpriteIndex = 0;
	}

	void Player::ResetPaddleSprites()
	{
		mPaddleSpriteSpeed = 0;
		mPaddleSpriteIndex = 0;
	}

	void Player::PlayerFalling(PlayerState previousState)
	{
		ChangeState(previousState == PlayerState::SLIPPING ? PlayerState::SLIPFALLING : PlayerState::FALLING);
	}

	void Player::ChangeState(PlayerState state, bool isChangeSprite)
	{
		currentState = state;

		switch (currentState)
		{
		case PlayerState::ONGROUND:
			if (isChangeSprite)
			{
				mCurrentAnimation = &mAnimator->mMarioIdle;
			}

			mXVelocity = 0;
			mSlipDuration = 0;

			CurrentPlayerState = bind(&Player::PlayerOnGround, this, std::placeholders::_1);
			break;

		case PlayerState::RUNNING:
			if (isChangeSprite)
			{
				mCurrentAnimation = &mAnimator->mMarioRunning;
			}

			CurrentPlayerState = bind(&Player::PlayerRunning, this, std::placeholders::_1);
			break;

		case PlayerState::STRAIGHTJUMPING:
		case PlayerState::RUNJUMPING:
			if (isChangeSprite)
			{
				mCurrentAnimation = &mAnimator->mMarioJump;
			}

			mYVelocity = sPLAYER_Y_VELOCITY;
			mGravity = sPLAYER_GRAVITY;

			CurrentPlayerState = bind(&Player::PlayerJumping, this, std::placeholders::_1);
			break;

		case PlayerState::SLIPPING:
			if (isChangeSprite)
			{
				mCurrentAnimation = &mAnimator->mMarioSlip;
			}

			ResetPlayerSprites();

			CurrentPlayerState = bind(&Player::PlayerSlipping, this, std::placeholders::_1);
			break;

		case PlayerState::FALLING:
			if (isChangeSprite)
			{
				mCurrentAnimation = &mAnimator->mMarioRunning;
			}

			if (mGravity == 0)
			{
				mGravity = sPLAYER_GRAVITY;
			}

			CurrentPlayerState = nullptr;
			break;

		case PlayerState::SLIPFALLING:
			if (isChangeSprite)
			{
				mCurrentAnimation = &mAnimator->mMarioSlip;
			}

			if (mGravity == 0)
			{
				mGravity = sPLAYER_GRAVITY;
			}

			CurrentPlayerState = nullptr;
			break;

		case PlayerState::ATTACKED:
			if (mXVelocity != 0)
			{
				if (mIsMovingRight)
				{
					mXVelocity = sPLAYER_X_VELOCITY;
				}
				else
				{
					mXVelocity = -sPLAYER_X_VELOCITY;
				}
			}

			mYVelocity = 0;
			mGravity = 0;

			CurrentPlayerState = bind(&Player::PlayerAttacking, this, std::placeholders::_1);
			break;

		case PlayerState::DEAD:
			if (isChangeSprite)
			{
				mCurrentAnimation = &mAnimator->mMarioDeath;
			}

			mSlipDuration = 0;
			mXVelocity = 0;
			mYVelocity = 0;
			mGravity = 0;
			ResetPlayerSprites();

			CurrentPlayerState = bind(&Player::PlayerDead, this, std::placeholders::_1);
			break;

		case PlayerState::DEADJUMPING:
			mYVelocity = sPLAYER_Y_DEAD_VELOCITY;
			mGravity = sPLAYER_DEAD_GRAVITY;
			mPlayerSpriteIndex = 1;																//Show the second sprite dead animation after time has passed the "PLAYER_DEAD_WAIT_DURATION". This is the only place where spriteIndex is hard coded.

			CurrentPlayerState = bind(&Player::PlayerDeadJumping, this, std::placeholders::_1);
			break;

		case PlayerState::RESPAWNING:
			if (isChangeSprite)
			{
				mCurrentAnimation = &mAnimator->mMarioIdle;
			}

			mXVelocity = 0;
			mYVelocity = sPLAYER_RESPAWN_Y_VELOCITY;
			mGravity = 0;

			mPosX = sPLAYER_RESPAWN_START_POS_X;
			mPosY = sPLAYER_RESPAWN_START_POS_Y;

			ResetPlayerSprites();

			CurrentPlayerState = bind(&Player::PlayerReSpawning, this, std::placeholders::_1);
			break;

		default:
			break;
		}
	}

	void Player::IncrementKillScore()
	{
		mPlayerScore += sKILL_SCORE;
	}

	/******************* Player States ********************************/
	void Player::PlayerOnGround(const StepTimer& timer)
	{
		timer;
		//TODO Add gamepad support
		if (mKeyboard->IsKeyDown(Keys::Space))
		{
			ChangeState(PlayerState::STRAIGHTJUMPING);
			//SoundManager::GetInstance()->PlayPlayerJumpSound();
		}
		else if (mKeyboard->IsKeyDown(Keys::Right))
		{
			mXVelocity = sPLAYER_X_VELOCITY;
			mIsMovingRight = true;
			ChangeState(PlayerState::RUNNING);
		}
		else if (mKeyboard->IsKeyDown(Keys::Left))
		{
			mXVelocity = -sPLAYER_X_VELOCITY;
			mIsMovingRight = false;
			ChangeState(PlayerState::RUNNING);
		}
	}

	void Player::PlayerRunning(const StepTimer& timer)
	{
		//TODO Add gamepad support
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Space))
		{
			mSlipDuration = sSLIP_DURATION_CAP;
			ResetPlayerSprites();
			ChangeState(PlayerState::RUNJUMPING);
			//SoundManager::GetInstance()->PlayPlayerJumpSound();
		}
		else if (mKeyboard->IsKeyDown(Keys::Right))
		{
			if (!mIsMovingRight)
			{
				ChangeState(PlayerState::SLIPPING);
			}
			else
			{
				mSlipDuration = mSlipDuration > sSLIP_DURATION_CAP ? sSLIP_DURATION_CAP : mSlipDuration + timer.GetElapsedMilliSeconds();
				mXVelocity = sPLAYER_X_VELOCITY;
				mIsMovingRight = true;
			}
		}
		else if (mKeyboard->IsKeyDown(Keys::Left))
		{
			if (mIsMovingRight)
			{
				ChangeState(PlayerState::SLIPPING);
			}
			else
			{
				mSlipDuration = mSlipDuration > sSLIP_DURATION_CAP ? sSLIP_DURATION_CAP : mSlipDuration + timer.GetElapsedMilliSeconds();
				mXVelocity = -sPLAYER_X_VELOCITY;
				mIsMovingRight = false;
			}
		}
		else
		{
			ChangeState(PlayerState::SLIPPING);
		}
	}

	void Player::PlayerJumping(const StepTimer& timer)
	{
		timer;
		//TODO Add gamepad support
		if (mKeyboard->IsKeyDown(Keys::Right))
		{
			mXVelocity = sPLAYER_X_JUMP_VELOCITY;
			mPosX += sPLAYER_X_JUMP_QUANTUM_VELOCITY * static_cast<float>(timer.GetElapsedSeconds());
			mIsMovingRight = true;
		}
		else if (mKeyboard->IsKeyDown(Keys::Left))
		{
			mXVelocity = -sPLAYER_X_JUMP_VELOCITY;
			mPosX -= sPLAYER_X_JUMP_QUANTUM_VELOCITY * static_cast<float>(timer.GetElapsedSeconds());
			mIsMovingRight = false;
		}
	}

	void Player::PlayerSlipping(const StepTimer& timer)
	{
		//TODO Add gamepad support
		if (mSlipDuration > 0)
		{
			mSlipDuration -= timer.GetElapsedMilliSeconds();

			if (mKeyboard->WasKeyPressedThisFrame(Keys::Space))
			{
				ChangeState(PlayerState::STRAIGHTJUMPING);
				//SoundManager::GetInstance()->PlayPlayerJumpSound();
				mXVelocity = 0;
				mSlipDuration = 0;
			}
		}
		else
		{
			ChangeState(PlayerState::ONGROUND);
		}
	}

	void Player::PlayerAttacking(const StepTimer& timer)
	{
		mAttackDuration += timer.GetElapsedMilliSeconds();
		if (mAttackDuration > sATTACK_WAIT_DURATION)
		{
			mAttackDuration = 0;
			ChangeState(PlayerState::FALLING, false);
		}
	}

	void Player::PlayerDead(const StepTimer& timer)
	{
		if (mDeathDuration < sPLAYER_DEAD_WAIT_DURATION)
		{
			mDeathDuration += timer.GetElapsedMilliSeconds();
		}
		else
		{
			ChangeState(PlayerState::DEADJUMPING);

			mDeathDuration = 0;
			//SoundManager::GetInstance()->PlayDeathSound();
		}
	}

	void Player::PlayerDeadJumping(const StepTimer& timer)
	{
		timer;
		if (mPosY < 0)
		{
			if (mLivesLeft == 0)
			{
				//TODO Implement game over state
				//StateManager::GetInstance()->ChangeState(GameState::GAME_OVER);
			}
			else
			{
				ChangeState(PlayerState::RESPAWNING);
				mIsMovingRight = false;
				//SoundManager::GetInstance()->PlayPlayerSpawnSound();
			}
		}
	}

	void Player::PlayerReSpawning(const StepTimer& timer)
	{
		//TODO Add gamepad support
		mReSpawnDuration += timer.GetElapsedMilliSeconds();
		if (mPosY < sPLAYER_RESPAWN_STOP_POS_Y)
		{
			mGravity = 0;
			mYVelocity = 0;
			mPosY = sPLAYER_RESPAWN_STOP_POS_Y - 1;

			if (mKeyboard->WasKeyPressedThisFrame(Keys::Space))
			{
				ChangeState(PlayerState::STRAIGHTJUMPING);
				ResetPaddleSprites();
				//SoundManager::GetInstance()->PlayPlayerJumpSound();
			}
			else if (mKeyboard->IsKeyDown(Keys::Right))
			{
				mXVelocity = sPLAYER_X_VELOCITY;
				mIsMovingRight = true;
				ChangeState(PlayerState::RUNNING);
				ResetPaddleSprites();
			}
			else if (mKeyboard->IsKeyDown(Keys::Left))
			{
				mXVelocity = -sPLAYER_X_VELOCITY;
				mIsMovingRight = false;
				ChangeState(PlayerState::RUNNING);
				ResetPaddleSprites();
			}

			if (mReSpawnDuration > sPLAYER_RESPAWN_WAIT_DURATION)
			{
				mReSpawnDuration = 0;
				ResetPaddleSprites();
				ChangeState(PlayerState::FALLING, false);
			}
		}
	}
	/******************************************************************/
}