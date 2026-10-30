#ifndef VECTOR_I_HPP
#define VECTOR_I_HPP

namespace swapper {

template<class T >
Vector<T >::Vector():
  data_(nullptr),
  size_(0),
  capacity_(0)
{
}

template<class T >
Vector<T >::Vector(const Vector & other):
  data_(nullptr),
  size_(0),
  capacity_(0)
{
  reserve(other.size_);
  for (std::size_t i = 0; i < other.size_; ++i) {
    pushBack(other.data_[i]);
  }
}

template<class T >
Vector<T > & Vector<T >::operator=(const Vector & other)
{
  if (this != &other) {
    Vector<T > copy(other);
    T * tmpData = data_;
    std::size_t tmpSize = size_;
    std::size_t tmpCap = capacity_;
    data_ = copy.data_;
    size_ = copy.size_;
    capacity_ = copy.capacity_;
    copy.data_ = tmpData;
    copy.size_ = tmpSize;
    copy.capacity_ = tmpCap;
  }
  return *this;
}

template<class T >
Vector<T >::~Vector()
{
  delete[] data_;
}

template<class T >
void Vector<T >::reallocate(std::size_t newCapacity)
{
  T * newData = new T[newCapacity];
  for (std::size_t i = 0; i < size_; ++i) {
    newData[i] = data_[i];
  }
  delete[] data_;
  data_ = newData;
  capacity_ = newCapacity;
}

template<class T >
void Vector<T >::reserve(std::size_t capacity)
{
  if (capacity > capacity_) {
    reallocate(capacity);
  }
}

template<class T >
void Vector<T >::pushBack(const T & value)
{
  if (size_ == capacity_) {
    std::size_t newCap = (capacity_ == 0) ? 4 : capacity_ * 2;
    reallocate(newCap);
  }
  data_[size_] = value;
  ++size_;
}

template<class T >
void Vector<T >::popBack()
{
  if (size_ == 0) {
    throw std::out_of_range("popBack on empty vector");
  }
  --size_;
}

template<class T >
T & Vector<T >::operator[](std::size_t index)
{
  if (index >= size_) {
    throw std::out_of_range("Vector index out of range");
  }
  return data_[index];
}

template<class T >
const T & Vector<T >::operator[](std::size_t index) const
{
  if (index >= size_) {
    throw std::out_of_range("Vector index out of range");
  }
  return data_[index];
}

template<class T >
std::size_t Vector<T >::size() const
{
  return size_;
}

template<class T >
bool Vector<T >::empty() const
{
  return (size_ == 0);
}

template<class T >
void Vector<T >::clear()
{
  size_ = 0;
}

}

#endif