#ifndef HASH_I_HPP
#define HASH_I_HPP

namespace swapper {

template<class Key, class Value >
HashTable<Key, Value >::HashTable(std::size_t initialCapacity):
  size_(0),
  capacity_(initialCapacity)
{
  buckets_.reserve(capacity_);
  Entry empty;
  empty.occupied = false;
  for (std::size_t i = 0; i < capacity_; ++i) {
    buckets_.pushBack(empty);
  }
}

template<class Key, class Value >
std::size_t HashTable<Key, Value >::bucketFor(const Key & key) const
{
  std::size_t hash = std::hash<Key >()(key);
  return hash % capacity_;
}

template<class Key, class Value >
void HashTable<Key, Value >::growIfNeeded()
{
  if (size_ * 2 < capacity_) {
    return;
  }
  Vector<Entry > oldBuckets = buckets_;
  std::size_t oldCap = capacity_;
  capacity_ = capacity_ * 2;
  buckets_ = Vector<Entry >();
  buckets_.reserve(capacity_);
  Entry empty;
  empty.occupied = false;
  for (std::size_t i = 0; i < capacity_; ++i) {
    buckets_.pushBack(empty);
  }
  size_ = 0;
  for (std::size_t i = 0; i < oldCap; ++i) {
    if (oldBuckets[i].occupied) {
      insert(oldBuckets[i].key, oldBuckets[i].value);
    }
  }
}

template<class Key, class Value >
void HashTable<Key, Value >::insert(const Key & key, const Value & value)
{
  std::size_t idx = bucketFor(key);
  while (buckets_[idx].occupied && !(buckets_[idx].key == key)) {
    idx = (idx + 1) % capacity_;
  }
  if (!buckets_[idx].occupied) {
    Entry entry;
    entry.key = key;
    entry.value = value;
    entry.occupied = true;
    buckets_[idx] = entry;
    keys_.pushBack(key);
    values_.pushBack(value);
    ++size_;
    growIfNeeded();
  } else {
    buckets_[idx].value = value;
    for (std::size_t i = 0; i < keys_.size(); ++i) {
      if (keys_[i] == key) {
        values_[i] = value;
      }
    }
  }
}

template<class Key, class Value >
bool HashTable<Key, Value >::contains(const Key & key) const
{
  std::size_t idx = bucketFor(key);
  for (std::size_t step = 0; step < capacity_; ++step) {
    if (!buckets_[idx].occupied) {
      return false;
    }
    if (buckets_[idx].key == key) {
      return true;
    }
    idx = (idx + 1) % capacity_;
  }
  return false;
}

template<class Key, class Value >
Value & HashTable<Key, Value >::at(const Key & key)
{
  std::size_t idx = bucketFor(key);
  for (std::size_t step = 0; step < capacity_; ++step) {
    if (!buckets_[idx].occupied) {
      throw std::out_of_range("HashTable: key not found");
    }
    if (buckets_[idx].key == key) {
      return buckets_[idx].value;
    }
    idx = (idx + 1) % capacity_;
  }
  throw std::out_of_range("HashTable: key not found");
}

template<class Key, class Value >
const Value & HashTable<Key, Value >::at(const Key & key) const
{
  std::size_t idx = bucketFor(key);
  for (std::size_t step = 0; step < capacity_; ++step) {
    if (!buckets_[idx].occupied) {
      throw std::out_of_range("HashTable: key not found");
    }
    if (buckets_[idx].key == key) {
      return buckets_[idx].value;
    }
    idx = (idx + 1) % capacity_;
  }
  throw std::out_of_range("HashTable: key not found");
}

template<class Key, class Value >
void HashTable<Key, Value >::erase(const Key & key)
{
  std::size_t idx = bucketFor(key);
  for (std::size_t step = 0; step < capacity_; ++step) {
    if (!buckets_[idx].occupied) {
      throw std::out_of_range("HashTable: key not found");
    }
    if (buckets_[idx].key == key) {
      buckets_[idx].occupied = false;
      --size_;
      for (std::size_t i = 0; i < keys_.size(); ++i) {
        if (keys_[i] == key) {
          for (std::size_t j = i; j + 1 < keys_.size(); ++j) {
            keys_[j] = keys_[j + 1];
            values_[j] = values_[j + 1];
          }
          keys_.popBack();
          values_.popBack();
          break;
        }
      }
      return;
    }
    idx = (idx + 1) % capacity_;
  }
}

template<class Key, class Value >
std::size_t HashTable<Key, Value >::size() const
{
  return size_;
}

template<class Key, class Value >
bool HashTable<Key, Value >::empty() const
{
  return (size_ == 0);
}

template<class Key, class Value >
const Vector<Key > & HashTable<Key, Value >::keys() const
{
  return keys_;
}

template<class Key, class Value >
const Vector<Value > & HashTable<Key, Value >::values() const
{
  return values_;
}

}

#endif