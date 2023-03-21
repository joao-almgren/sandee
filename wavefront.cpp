#include "wavefront.h"
#include "fast_float.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "timer.h"
#include <fstream>
using namespace std;

using namespace DirectX::SimpleMath;

bool readFile(const char * const filename, char*& buffer, size_t& buffersize)
{
	FILE* f = nullptr;
	if (fopen_s(&f, filename, "rb") || !f)
		return false;

	if (fseek(f, 0, SEEK_END))
	{
		fclose(f); // NOLINT(cert-err33-c)
		return false;
	}
	buffersize = ftell(f);
	rewind(f);

	buffer = new char[buffersize];
	if (!buffer)
	{
		fclose(f); // NOLINT(cert-err33-c)
		return false;
	}

	if (fread(buffer, 1, buffersize, f) != buffersize)
	{
		delete[] buffer;
		buffer = nullptr;
		buffersize = 0;
		fclose(f); // NOLINT(cert-err33-c)
		return false;
	}

	fclose(f); // NOLINT(cert-err33-c)
	return true;
}

size_t getToken(const char* buffer, const size_t size, const size_t start, char* token, const char separator = 0)
{
	size_t stop = start;
	while (stop < size)
	{
		if (buffer[stop] == separator || buffer[stop] == ' ' || buffer[stop] == '\r' || buffer[stop] == '\n')
			break;

		token[stop - start] = buffer[stop];
		stop++;
	}

	token[stop - start] = 0;
	return stop - start;
}

size_t seekEndLine(const char* buffer, const size_t size, const size_t start)
{
	size_t stop = start;
	while (stop < size)
	{
		if (buffer[stop] == '\n')
			break;

		stop++;
	}

	return stop;
}

void calcBoundingSphere(const std::vector<Vector3>& points, Vector4& sphere)
{
	Vector3 center{ 0, 0, 0 };
	for (const auto& point : points)
		center += point;
	center /= static_cast<float>(points.size());

	float radius = 0;
	for (const auto& point : points)
	{
		const Vector3 v = point - center;
		const float distSq = v.LengthSquared();
		if (distSq > radius)
			radius = distSq;
	}
	radius = sqrtf(radius);

	sphere.x = center.x;
	sphere.y = center.y;
	sphere.z = center.z;
	sphere.w = radius;
}

unsigned long fast_atoi(const char* str)
{
	unsigned long val = 0;
	while (*str)
	{
		val = (val << 1) + (val << 3) + (*str - '0');
		str++;
	}
	return val;
}

bool loadWfObject(const char* filename, std::vector<TbnVertex>& vertices, std::vector<DWORD>& indices, Vector4& sphere)
{
	std::vector<Vector3> position;
	std::vector<Vector3> normal;
	std::vector<Vector2> texcoord;

	char* buffer;
	size_t size;
	if (!readFile(filename, buffer, size))
		return false;

	Timer timer;

	for (size_t bufferIndex = 0; bufferIndex < size; bufferIndex++)
	{
		char token[32]; // max token size observed: 11
		const size_t len = getToken(buffer, size, bufferIndex, token);

		if (len == 1)
		{
			if (token[0] == 'f')
			{
				size_t offset = bufferIndex + len + 1;
				std::vector<TbnVertex> ngon;
				ngon.reserve(3);

				while (offset < size && buffer[offset] != '\r' && buffer[offset] != '\n')
				{
					size_t pointIndices[3]{ 0, 0, 0 };

					for (size_t& index : pointIndices)
					{
						const size_t toklen = getToken(buffer, size, offset, token, '/');
						assert(toklen != 0);
						offset += toklen + 1;

						const size_t i = fast_atoi(token);
						index = i - 1;
					}

					if (pointIndices[0] > position.size() || pointIndices[1] > texcoord.size() || pointIndices[2] > normal.size())
						return false;

					ngon.emplace_back(position[pointIndices[0]], normal[pointIndices[2]], Vector3::Zero, Vector3::Zero, texcoord[pointIndices[1]]);
				}

				// triangulate ngon into 0-2-1, 0-3-2, 0-4-3, ...
				for (size_t i = 1; i < ngon.size() - 1; i++)
				{
					auto index = static_cast<DWORD>(vertices.size());
					vertices.push_back(ngon[0]);
					indices.push_back(index);

					index = static_cast<DWORD>(vertices.size());
					vertices.push_back(ngon[i + 1]);
					indices.push_back(index);

					index = static_cast<DWORD>(vertices.size());
					vertices.push_back(ngon[i]);
					indices.push_back(index);
				}
			}
			else if (token[0] == 'v')
			{
				size_t offset = bufferIndex + len + 1;
				float components[3]{ 0, 0, 0 };

				for (float& component : components)
				{
					const size_t toklen = getToken(buffer, size, offset, token);
					assert(toklen != 0);
					offset += toklen + 1;

					const auto result = fast_float::from_chars(token, token + toklen, component);
					assert(result.ec == std::errc());
				}

				position.emplace_back(-components[0], components[1], components[2]);
			}
		}
		else if (len == 2 && token[0] == 'v')
		{
			if (token[1] == 't')
			{
				size_t offset = bufferIndex + len + 1;
				float components[2]{ 0, 0 };

				for (float& component : components)
				{
					const size_t toklen = getToken(buffer, size, offset, token);
					assert(toklen != 0);
					offset += toklen + 1;

					const auto result = fast_float::from_chars(token, token + toklen, component);
					assert(result.ec == std::errc());
				}

				texcoord.emplace_back(components[0], 1 - components[1]);
			}
			else if (token[1] == 'n')
			{
				size_t offset = bufferIndex + len + 1;
				float components[3]{ 0, 0, 0 };

				for (float& component : components)
				{
					const size_t toklen = getToken(buffer, size, offset, token);
					assert(toklen != 0);
					offset += toklen + 1;

					const auto result = fast_float::from_chars(token, token + toklen, component);
					assert(result.ec == std::errc());
				}

				Vector3 n =
				{
					-components[0],
					components[1],
					components[2],
				};
				n.Normalize();

				normal.push_back(n);
			}
		}

		bufferIndex = seekEndLine(buffer, size, bufferIndex);
	}

	auto stop = timer.value();
	ofstream flog("wfolog.txt", std::ios_base::app);
	flog << stop << endl;
	flog.close();

	if (vertices.empty() || indices.empty())
		return false;

	calcBoundingSphere(position, sphere);

	return true;
}

void calculateTangents(TbnVertex& a, TbnVertex& b, TbnVertex& c)
{
	const Vector3 v = b.position - a.position, w = c.position - a.position;
	float sx = b.texcoord.x - a.texcoord.x, sy = b.texcoord.y - a.texcoord.y;
	float tx = c.texcoord.x - a.texcoord.x, ty = c.texcoord.y - a.texcoord.y;

	const float dirCorrection = (tx * sy - ty * sx) < 0.0f ? -1.0f : 1.0f;

	if (fabs(sx * ty - sy * tx) > 0)
	{
		sx = 0.0;
		sy = 1.0;
		tx = 1.0;
		ty = 0.0;
	}

	Vector3 tangent, bitangent;
	tangent.x = (w.x * sy - v.x * ty) * dirCorrection;
	tangent.y = (w.y * sy - v.y * ty) * dirCorrection;
	tangent.z = (w.z * sy - v.z * ty) * dirCorrection;
	bitangent.x = (w.x * sx - v.x * tx) * dirCorrection;
	bitangent.y = (w.y * sx - v.y * tx) * dirCorrection;
	bitangent.z = (w.z * sx - v.z * tx) * dirCorrection;

	Vector3 localTangent = tangent - a.normal * tangent.Dot(a.normal);
	Vector3 localBitangent = bitangent - a.normal * bitangent.Dot(a.normal);

	localTangent.Normalize();
	localBitangent.Normalize();

	a.tangent = localTangent;
	a.bitangent = localBitangent;

	localTangent = tangent - b.normal * tangent.Dot(b.normal);
	localBitangent = bitangent - b.normal * bitangent.Dot(b.normal);

	localTangent.Normalize();
	localBitangent.Normalize();

	b.tangent = localTangent;
	b.bitangent = localBitangent;

	localTangent = tangent - c.normal * tangent.Dot(c.normal);
	localBitangent = bitangent - c.normal * bitangent.Dot(c.normal);

	localTangent.Normalize();
	localBitangent.Normalize();

	c.tangent = localTangent;
	c.bitangent = localBitangent;
}

bool loadTbnObject(const char* filename, std::vector<TbnVertex>& vertices, std::vector<DWORD>& indices, Vector4& sphere)
{
	if (!loadWfObject(filename, vertices, indices, sphere))
		return false;

	for (size_t i = 0; i < indices.size() - 2; i += 3)
	{
		TbnVertex& a = vertices[indices[i]];
		TbnVertex& b = vertices[indices[i + 1]];
		TbnVertex& c = vertices[indices[i + 2]];

		calculateTangents(a, b, c);
	}

	return true;
}
