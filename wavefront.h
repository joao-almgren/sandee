#pragma once
#define NOMINMAX
#include <SimpleMath.h>
#include "fastvector.h"

struct TbnVertex
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 normal;
	DirectX::SimpleMath::Vector3 tangent;
	DirectX::SimpleMath::Vector3 bitangent;
	DirectX::SimpleMath::Vector2 texcoord;
};

bool loadTbnObject(
	const char* filename,
	FastVector<TbnVertex>& vertices,
	FastVector<DWORD>& indices,
	DirectX::SimpleMath::Vector4& sphere
);
