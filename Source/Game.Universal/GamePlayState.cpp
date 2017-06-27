#include "pch.h"
#include "GamePlayState.h"

using namespace DX;

namespace GameStates
{
	GamePlayState::GamePlayState(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera)
		:BaseGameState(deviceResources)
	{
		//TODO 
		camera;
	}

	void GamePlayState::StateEntering()
	{
	}

	void GamePlayState::Update(const StepTimer& timer)
	{
		//TODO
		timer;
	}

	void GamePlayState::Render(const StepTimer& timer)
	{
		//TODO
		timer;
	}

	void GamePlayState::StateLeaving()
	{
	}
}