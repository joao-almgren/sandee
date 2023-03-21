#pragma once
#define NOMINMAX
#include <SimpleMath.h>
#include <vector>

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
	std::vector<TbnVertex>& vertices,
	std::vector<DWORD>& indices,
	DirectX::SimpleMath::Vector4& sphere
);
