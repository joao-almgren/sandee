#pragma once
#include <cassert>
#include <cstdlib>
#include <type_traits>

// Helper functions

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
class fast_vector
{
public:
    using size_type = std::size_t;

    fast_vector() = default;
    fast_vector(const fast_vector& other) = delete;
    fast_vector(fast_vector&& other) = delete;
    fast_vector& operator=(const fast_vector& other) = delete;
    fast_vector& operator=(fast_vector&& other) = delete;
    ~fast_vector();

    // Element access

    [[nodiscard]] T& operator[](size_type pos);
    [[nodiscard]] const T& operator[](size_type pos) const;

    [[nodiscard]] T* data() noexcept;
    [[nodiscard]] const T* data() const noexcept;

    // Iterators

    [[nodiscard]] T* begin() noexcept;
    [[nodiscard]] const T* begin() const noexcept;

    [[nodiscard]] T* end() noexcept;
    [[nodiscard]] const T* end() const noexcept;

    // Capacity

    [[nodiscard]] size_type size() const noexcept;
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
fast_vector<T>::~fast_vector()
{
    std::free(m_data);
}

// Element access

template <class T>
T& fast_vector<T>::operator[](size_type pos)
{
    assert(pos < m_size && "Position is out of range");
    return m_data[pos];
}

template <class T>
const T& fast_vector<T>::operator[](size_type pos) const
{
    assert(pos < m_size && "Position is out of range");
    return m_data[pos];
}

template <class T>
T* fast_vector<T>::data() noexcept
{
    return m_data;
}

template <class T>
const T* fast_vector<T>::data() const noexcept
{
    return m_data;
}

// Iterators

template <class T>
T* fast_vector<T>::begin() noexcept
{
    return m_data;
}

template <class T>
const T* fast_vector<T>::begin() const noexcept
{
    return m_data;
}

template <class T>
T* fast_vector<T>::end() noexcept
{
    return m_data + m_size;
}

template <class T>
const T* fast_vector<T>::end() const noexcept
{
    return m_data + m_size;
}

// Capacity

template <class T>
typename fast_vector<T>::size_type fast_vector<T>::size() const noexcept
{
    return m_size;
}

template <class T>
void fast_vector<T>::reserve(const size_type new_cap)
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

// Modifiers

template <class T>
void fast_vector<T>::push_back(const T& value)
{
    if (m_size == m_capacity)
    {
        reserve(m_capacity * fast_vector::grow_factor + 1);
    }

    m_data[m_size] = value;

    m_size++;
}

template <class T>
void fast_vector<T>::push_back(T&& value)
{
    if (m_size == m_capacity)
    {
        reserve(m_capacity * fast_vector::grow_factor + 1);
    }

	m_data[m_size] = value;

	m_size++;
}

template <class T>
template< class... Args >
void fast_vector<T>::emplace_back(Args&&... args)
{
    if (m_size == m_capacity)
    {
        reserve(m_capacity * fast_vector::grow_factor + 1);
    }

    m_data[m_size] = T(std::forward<Args>(args)...);

    m_size++;
}
