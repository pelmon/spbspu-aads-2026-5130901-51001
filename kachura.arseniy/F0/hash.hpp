#ifndef HASH_HPP
#define HASH_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include "vector.h"

namespace swapper {

template<class Key, class Value >
class HashTable
{
public:
  explicit HashTable(std::size_t initialCapacity = 16);

  void insert(const Key & key, const Value & value);
  bool contains(const Key & key) const;
  Value & at(const Key & key);
  const Value & at(const Key & key) const;
  void erase(const Key & key);
  std::size_t size() const;
  bool empty() const;

  const Vector<Key > & keys() const;
  const Vector<Value > & values() const;

private:
  struct Entry
  {
    Key key;
    Value value;
    bool occupied;
  };

  std::size_t bucketFor(const Key & key) const;
  void growIfNeeded();

  Vector<Entry > buckets_;
  Vector<Key > keys_;
  Vector<Value > values_;
  std::size_t size_;
  std::size_t capacity_;
};

}

#include "hashtable_impl.h"

#endif