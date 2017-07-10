#pragma once
#include "DrawableGameComponent.h"

namespace Core
{
	class ShapeHelper;
}

namespace GameEntity
{	
	class SquareShape final : public DX::DrawableGameComponent
	{
	public:
		SquareShape(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera);

		std::shared_ptr<Core::ShapeHelper> ActiveShapeHelper() const;
		void SetActiveShapeHelper(const std::shared_ptr<Core::ShapeHelper>& shapeHelper);

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Render(const DX::StepTimer& timer) override;

	private:
		void DrawShape();				

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVSCBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPSCBufferPerObject;

		bool mLoadingComplete;
		std::uint32_t mIndexCount;		
		DX::VertexPosition mVertices[4];
		std::shared_ptr<Core::ShapeHelper> mShapeHelper;
	};
}