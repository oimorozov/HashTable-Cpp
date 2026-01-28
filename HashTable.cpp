#include "HashTable.h"
#include <cstdint>
#include <functional>
#include <stdexcept>


// Node
template<typename KeyType, typename ValueType>
Node<KeyType, ValueType>::Node(KeyType key, ValueType value) {
    this->key = key;
    this->value = value;
    this->next = nullptr;
}


// Ctors and Dtor
template<typename KeyType, typename ValueType, typename Func>
HashTable<KeyType, ValueType, Func>::HashTable() {
    _size = 0;
    _capacity = 100;
    _overload_coeff = 0.5;
    _hash_func = Func();
    _buffer = new Node<KeyType, ValueType>*[_capacity];
    for (size_t i = 0; i < _capacity; i++) {
        _buffer[i] = nullptr;
    }
}


template<typename KeyType, typename ValueType, typename Func>
HashTable<KeyType, ValueType, Func>::HashTable(Func hash_func) {
    _size = 0;
    _capacity = 100;
    _overload_coeff = 0.5;
    _hash_func = hash_func;
    _buffer = new Node<KeyType, ValueType>*[_capacity];
    for (size_t i = 0; i < _capacity; i++) {
        _buffer[i] = nullptr;
    }
}


template<typename KeyType, typename ValueType, typename Func>
HashTable<KeyType, ValueType, Func>::HashTable(size_t size, double overload_coeff, Func hash_func) {
    _size = 0;
    _capacity = size == 0 ? 1 : size;
    _overload_coeff = normalize_coeff(overload_coeff);
    _hash_func = hash_func;
    _buffer = new Node<KeyType, ValueType>*[_capacity];
    for (size_t i = 0; i < _capacity; i++) {
        _buffer[i] = nullptr;
    }
}


template<typename KeyType, typename ValueType, typename Func>
HashTable<KeyType, ValueType, Func>::~HashTable() {
    for (size_t i = 0; i < _capacity; ++i) {
        Node<KeyType, ValueType> *current = _buffer[i];
        while (current != nullptr) {
            Node<KeyType, ValueType> *next = current->next;
            delete current;
            current = next;
        }
    }
    delete[] _buffer;
    _buffer = nullptr;
    _size = 0;
    _capacity = 0;
    _overload_coeff = 0;
}


// Insert
template<typename KeyType, typename ValueType, typename Func>
void HashTable<KeyType, ValueType, Func>::insert(KeyType key, ValueType value) {
    size_t hash = _hash_func(key);
    size_t index = hash % _capacity;
    if (_buffer[index] == nullptr) {
        _buffer[index] = new Node<KeyType, ValueType>(key, value);
    } else {
        Node<KeyType, ValueType> *current_node = _buffer[index];
        if (current_node->key == key) {
            current_node->value = value;
            return;
        }
        while (current_node->next != nullptr) {
            if (current_node->key == key) {
                current_node->value = value;
                return;
            }
            current_node = current_node->next;
        }
        if (current_node->key == key) {
            current_node->value = value;
            return;
        }
        current_node->next = new Node<KeyType, ValueType>(key, value);
    }
    ++_size;
    if (static_cast<double>(_size) / static_cast<double>(_capacity) > _overload_coeff) {
        rehash(_capacity * 2);
    }
}


// Find
template<typename KeyType, typename ValueType, typename Func>
ValueType *HashTable<KeyType, ValueType, Func>::find(KeyType key) {
    size_t hash = _hash_func(key);
    size_t index = hash % _capacity;
    if (_buffer[index] == nullptr) {
        return nullptr;
    } else {
        Node<KeyType, ValueType> *current_node = _buffer[index];
        while (current_node != nullptr && current_node->key != key) {
            current_node = current_node->next;
        }
        if (current_node == nullptr) {
            return nullptr;
        } else {
            return &(current_node->value);
        }
    }
}


// Erase
template<typename KeyType, typename ValueType, typename Func>
void HashTable<KeyType, ValueType, Func>::erase(KeyType key) {
    size_t hash = _hash_func(key);
    size_t index = hash % _capacity;
    Node<KeyType, ValueType>* current_node = _buffer[index];
    Node<KeyType, ValueType>* prev_node = nullptr;
    while (current_node != nullptr && current_node->key != key) {
        prev_node = current_node;
        current_node = current_node->next;
    }
    if (current_node == nullptr) {
        return;
    }
    if (prev_node == nullptr) {
        _buffer[index] = current_node->next;
    } else {
        prev_node->next = current_node->next;
    }
    delete current_node;
    --_size;
}


// Operators
template<typename KeyType, typename ValueType, typename Func>
Node<KeyType, ValueType> &HashTable<KeyType, ValueType, Func>::operator[](uint64_t index) {
    if (index >= _capacity) {
        throw std::out_of_range("Index out of range");
    } else if (_buffer[index] == nullptr) {
        throw std::runtime_error("Index is not occupied");
    }
    return *_buffer[index];
}


template<typename KeyType, typename ValueType, typename Func>
Node<KeyType, ValueType> HashTable<KeyType, ValueType, Func>::at(uint64_t index) {
    if (index >= _capacity) {
        throw std::out_of_range("Index out of range");
    } else if (_buffer[index] == nullptr) {
        throw std::runtime_error("Index is not occupied");
    }
    return *_buffer[index];
}


// Getters
template<typename KeyType, typename ValueType, typename Func>
size_t HashTable<KeyType, ValueType, Func>::size() const {
    return _size;
}


template<typename KeyType, typename ValueType, typename Func>
size_t HashTable<KeyType, ValueType, Func>::capacity() const {
    return _capacity;
}


// helpers
template<typename KeyType, typename ValueType, typename Func>
void HashTable<KeyType, ValueType, Func>::rehash(size_t new_capacity) {
    if (new_capacity == 0) {
        return;
    }
    Node<KeyType, ValueType> **old_buffer = _buffer;
    size_t old_capacity = _capacity;

    _capacity = new_capacity;
    _buffer = new Node<KeyType, ValueType>*[_capacity];
    for (size_t i = 0; i < _capacity; i++) {
        _buffer[i] = nullptr;
    }

    for (size_t i = 0; i < old_capacity; i++) {
        Node<KeyType, ValueType> *current = old_buffer[i];
        while (current != nullptr) {
            Node<KeyType, ValueType> *next = current->next;
            current->next = nullptr;
            size_t index = _hash_func(current->key) % _capacity;
            if (_buffer[index] == nullptr) {
                _buffer[index] = current;
            } else {
                Node<KeyType, ValueType> *tail = _buffer[index];
                while (tail->next != nullptr) {
                    tail = tail->next;
                }
                tail->next = current;
            }
            current = next;
        }
    }
    delete[] old_buffer;
}


template<typename KeyType, typename ValueType, typename Func>
double HashTable<KeyType, ValueType, Func>::normalize_coeff(double coeff) const {
    if (coeff <= 0.0 || coeff > 1.0) {
        return 0.5;
    }
    return coeff;
}


// instances
template class HashTable<int, int>;