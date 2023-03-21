#include "wavefront.h"
#include "fast_float.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "fastvector.h"

using namespace DirectX::SimpleMath;

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

size_t getToken(const char* const buffer, const size_t size, const size_t start, char* token, const char separator = 0)
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

size_t seekEndLine(const char* const buffer, const size_t size, const size_t start)
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

void calculateBoundingSphere(const FastVector<Vector3>& points, Vector4& sphere)
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

void calculateTangents(TbnVertex& a, TbnVertex& b, TbnVertex& c)
{
	const Vector3 v = b.position - a.position;
	const Vector3 w = c.position - a.position;
	float sx = b.texcoord.x - a.texcoord.x, sy = b.texcoord.y - a.texcoord.y;
	float tx = c.texcoord.x - a.texcoord.x, ty = c.texcoord.y - a.texcoord.y;

	const float direction = sy * tx - sx * ty;
	const float dirCorrection = std::signbit(direction) ? -1.0f : 1.0f;

	if (fabs(sx * ty - sy * tx) > 0)
	{
		sx = 0.0;
		sy = 1.0;
		tx = 1.0;
		ty = 0.0;
	}

	const Vector3 tangent
	{
		(w.x * sy - v.x * ty) * dirCorrection,
		(w.y * sy - v.y * ty) * dirCorrection,
		(w.z * sy - v.z * ty) * dirCorrection
	};

	const Vector3 bitangent
	{
		(w.x * sx - v.x * tx) * dirCorrection,
		(w.y * sx - v.y * tx) * dirCorrection,
		(w.z * sx - v.z * tx) * dirCorrection
	};

	a.tangent = tangent - a.normal * tangent.Dot(a.normal);
	a.bitangent = bitangent - a.normal * bitangent.Dot(a.normal);

	a.tangent.Normalize();
	a.bitangent.Normalize();

	b.tangent = tangent - b.normal * tangent.Dot(b.normal);
	b.bitangent = bitangent - b.normal * bitangent.Dot(b.normal);

	b.tangent.Normalize();
	b.bitangent.Normalize();

	c.tangent = tangent - c.normal * tangent.Dot(c.normal);
	c.bitangent = bitangent - c.normal * bitangent.Dot(c.normal);

	c.tangent.Normalize();
	c.bitangent.Normalize();
}

bool loadTbnObject(const char* const filename, FastVector<TbnVertex>& vertices, FastVector<DWORD>& indices, Vector4& sphere)
{
	FastVector<Vector3> position;
	FastVector<Vector3> normal;
	FastVector<Vector2> texcoord;

	char* buffer;
	size_t size;
	if (!readFile(filename, buffer, size))
		return false;

	for (size_t bufferIndex = 0; bufferIndex < size; bufferIndex++)
	{
		char token[32]; // max token size observed: 11
		const size_t len = getToken(buffer, size, bufferIndex, token);

		if (len == 1)
		{
			if (token[0] == 'f')
			{
				size_t offset = bufferIndex + len + 1;
				auto vertexCount = static_cast<DWORD>(vertices.size());

				for (int n = 0; n < 3; n++)
				{
					assert(offset < size && buffer[offset] != '\r' && buffer[offset] != '\n');
					size_t pointIndices[3];

					for (size_t& index : pointIndices)
					{
						const size_t toklen = getToken(buffer, size, offset, token, '/');
						assert(toklen != 0);
						offset += toklen + 1;

						const size_t i = fast_atoi(token);
						index = i - 1;
					}

					assert(pointIndices[0] < position.size() && pointIndices[1] < texcoord.size() && pointIndices[2] < normal.size());
					vertices.emplace_back(position[pointIndices[0]], normal[pointIndices[2]], Vector3::Zero, Vector3::Zero, texcoord[pointIndices[1]]);
				}

				indices.push_back(vertexCount);
				indices.push_back(vertexCount + 2);
				indices.push_back(vertexCount + 1);
			}
			else if (token[0] == 'v')
			{
				size_t offset = bufferIndex + len + 1;
				float components[3];

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
				float components[2];

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

	calculateBoundingSphere(position, sphere);

	for (size_t i = 0; i < indices.size() - 2; i += 3)
	{
		TbnVertex& a = vertices[indices[i]];
		TbnVertex& b = vertices[indices[i + 1]];
		TbnVertex& c = vertices[indices[i + 2]];

		calculateTangents(a, b, c);
	}

	return true;
}
