// LAF Base Library
// Copyright (c) 2026-present  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_SPAN_H_INCLUDED
#define BASE_SPAN_H_INCLUDED
#pragma once

#include <iterator>
#include <stdexcept>

namespace base {

template<typename T>
class span {
public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using iterator = T*;
  using const_iterator = const T*;

  constexpr span() noexcept {}
  constexpr span(pointer p, const size_type s) noexcept : m_ptr(p), m_size(s) {}
  constexpr span& operator=(const span& other) noexcept = default;
  constexpr span(const span& other) noexcept = default;

  template<typename It>
  constexpr span(It first, It last) : m_ptr(first)
                                    , m_size(last - first)
  {
  }

  constexpr bool empty() const noexcept { return m_size == 0; }
  constexpr size_type size() const noexcept { return m_size; }
  constexpr size_type size_bytes() const noexcept { return m_size * sizeof(element_type); }

  constexpr pointer data() const noexcept { return m_ptr; }
  constexpr reference front() const { return m_ptr[0]; }
  constexpr reference back() const { return m_ptr[m_size - 1]; }

  constexpr reference operator[](size_type i) const { return m_ptr[i]; }
  constexpr reference at(size_type i) const
  {
    if (i >= size())
      throw std::out_of_range("span index out of range");
    return m_ptr[i];
  }

  constexpr iterator begin() noexcept { return m_ptr; }
  constexpr iterator end() noexcept { return m_ptr + m_size; }
  constexpr auto rbegin() const { return std::make_reverse_iterator(this->end()); }
  constexpr auto rend() const { return std::make_reverse_iterator(this->begin()); }

  constexpr const_iterator begin() const noexcept { return m_ptr; }
  constexpr const_iterator end() const noexcept { return m_ptr + m_size; }

private:
  pointer m_ptr;
  size_type m_size;
};

} // namespace base

#endif
