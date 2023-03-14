#pragma once
#include <vector>
#include <initializer_list>
#include <optional>
#include <map>

template <typename Type>
class Array
{
public:
	[[nodiscard]] Type* data()
	{
		return m_vector.data();
	}

	[[nodiscard]] size_t size() const
	{
		return m_vector.size();
	}

	[[nodiscard]] Type& operator[](const size_t index)
	{
		auto value = m_vector[index];
		auto iterator = m_hash.find(value);
		if (iterator != m_hash.end())
			m_hash.erase(iterator);

		return value;
	}

	void clear()
	{
		m_vector.clear();
		m_hash.clear();
	}

	size_t append(const Type& value)
	{
		const size_t index = m_vector.size();
		m_vector.push_back(value);

		return index;
	}

	size_t append(const std::initializer_list<Type>& list)
	{
		const size_t index = m_vector.size();
		for (const Type& value : list)
			m_vector.push_back(value);

		return index;
	}

	[[nodiscard]] std::optional<size_t> find(const Type& value)
	{
		auto iterator = m_hash.find(value);
		if (iterator != m_hash.end())
			return iterator->second;

		const size_t size = m_vector.size();
		for (size_t i = 0; i < size; i++)
			if (value == m_vector[i])
			{
				m_hash.insert({ value, i });
				return i;
			}

		return std::nullopt;
	}

	size_t appendAbsent(const Type& value)
	{
		const auto index = find(value);
		if (index.has_value())
			return index.value();

		return append(value);
	}

private:
	std::vector<Type> m_vector;
	std::map<Type, size_t> m_hash;
};
