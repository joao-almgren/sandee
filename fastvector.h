#pragma once
#include <cassert>
#include <cstdlib>
#include <type_traits>

template <class T>
class FastVector
{
public:
    using size_type = std::size_t;

    FastVector() = default;
    FastVector(const FastVector&) = delete;
    FastVector(FastVector&&) = delete;
    FastVector& operator=(const FastVector&) = delete;
    FastVector& operator=(FastVector&&) = delete;
    ~FastVector() { std::free(m_data); }

    // Element access

    [[nodiscard]] T& operator[](size_type pos);
    [[nodiscard]] const T& operator[](size_type pos) const;

    [[nodiscard]] T* data() noexcept { return m_data; }
    [[nodiscard]] const T* data() const noexcept { return m_data; }

    // Iterators

    [[nodiscard]] T* begin() noexcept { return m_data; }
    [[nodiscard]] const T* begin() const noexcept { return m_data; }

    [[nodiscard]] T* end() noexcept { return m_data + m_size; }
    [[nodiscard]] const T* end() const noexcept { return m_data + m_size; }

    // Capacity

    [[nodiscard]] size_type size() const noexcept { return m_size; }
    void reserve(size_type new_cap);

    // Modifiers

    void push_back(const T& value);
    void push_back(T&& value);

    template< class... Args >
    void emplace_back(Args&&... args);

    static constexpr size_type grow_factor = 2;

private:
    T* m_data = nullptr;
    size_type m_size = 0;
    size_type m_capacity = 0;
};

template <class T>
T& FastVector<T>::operator[](size_type pos)
{
    assert(pos < m_size && "Position is out of range");
    return m_data[pos];
}

template <class T>
const T& FastVector<T>::operator[](size_type pos) const
{
    assert(pos < m_size && "Position is out of range");
    return m_data[pos];
}

template <class T>
inline void copy_range(T* begin, T* end, T* dest)
{
	while (begin != end)
	{
		*dest = *begin;
		++begin;
		++dest;
	}
}

template <class T>
void FastVector<T>::reserve(const size_type new_cap)
{
    assert(new_cap > m_capacity && "Capacity is already equal to or greater than the passed value");

    if constexpr (std::is_trivial_v<T>)
    {
        m_data = reinterpret_cast<T*>(std::realloc(m_data, sizeof(T) * new_cap));
        assert(m_data != nullptr && "Reallocation failed");
    }
    else
    {
        T* new_data_location = reinterpret_cast<T*>(std::malloc(sizeof(T) * new_cap));
        assert(new_data_location != nullptr && "Allocation failed");

        copy_range(begin(), end(), new_data_location);
        std::free(m_data);
        m_data = new_data_location;
    }

    m_capacity = new_cap;
}

template <class T>
void FastVector<T>::push_back(const T& value)
{
    if (m_size == m_capacity)
        reserve(m_capacity * FastVector::grow_factor + 1);

    m_data[m_size] = value;
    m_size++;
}

template <class T>
void FastVector<T>::push_back(T&& value)
{
    if (m_size == m_capacity)
        reserve(m_capacity * FastVector::grow_factor + 1);

	m_data[m_size] = value;
	m_size++;
}

template <class T>
template <class... Args>
void FastVector<T>::emplace_back(Args&&... args)
{
    if (m_size == m_capacity)
        reserve(m_capacity * FastVector::grow_factor + 1);

    m_data[m_size] = T(std::forward<Args>(args)...);
    m_size++;
}
