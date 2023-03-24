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

template <class T>
bool loadTbnObject(
	const char* filename,
	FastVector<TbnVertex>& vertices,
	FastVector<T>& indices,
	DirectX::SimpleMath::Vector4& sphere
);

template
bool loadTbnObject<DWORD>(
	const char* filename,
	FastVector<TbnVertex>& vertices,
	FastVector<DWORD>& indices,
	DirectX::SimpleMath::Vector4& sphere
);

template
bool loadTbnObject<WORD>(
	const char* filename,
	FastVector<TbnVertex>& vertices,
	FastVector<WORD>& indices,
	DirectX::SimpleMath::Vector4& sphere
);
