#include "pch.h"
#include "ShapeHelper.h"

using namespace DirectX;

namespace Core
{
	ShapeHelper::ShapeHelper(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color)
		:mPosition(position), mSize(size), mColor(color)
	{
	}

	const XMFLOAT2& ShapeHelper::Position() const
	{
		return mPosition;
	}

	void ShapeHelper::SetPosition(const XMFLOAT2& position)
	{
		mPosition = position;
	}

	const XMFLOAT2& ShapeHelper::Size() const
	{
		return mSize;
	}

	void ShapeHelper::SetSize(const XMFLOAT2& size)
	{
		mSize = size;
	}

	const XMFLOAT4& ShapeHelper::Color() const
	{
		return mColor;
	}

	void ShapeHelper::SetColor(const XMFLOAT4& color)
	{
		mColor = color;
	}
}