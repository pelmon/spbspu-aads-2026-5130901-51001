#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cstddef>
#include <stdexcept>

namespace swapper {

template<class T >
class Vector
{
public:
  Vector();
  Vector(const Vector & other);
  Vector & operator=(const Vector & other);
  ~Vector();

  void pushBack(const T & value);
  void popBack();
  T & operator[](std::size_t index);
  const T & operator[](std::size_t index) const;
  std::size_t size() const;
  bool empty() const;
  void clear();
  void reserve(std::size_t capacity);

private:
  void reallocate(std::size_t newCapacity);

  T * data_;
  std::size_t size_;
  std::size_t capacity_;
};

}

#include "vector_impl.h"

#endif