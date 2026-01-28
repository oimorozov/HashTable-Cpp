#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>

// Элемент хеш-таблицы - список объектов с одним хешем
template <class KeyType, class ValueType>
struct Node {
    KeyType key;
    ValueType value;
    Node *next;

    Node(KeyType, ValueType);
};

// Хеш-таблица
template <class KeyType, class ValueType, class Func = std::hash<KeyType>>
class HashTable {
public:
    HashTable();
    HashTable(Func);
    HashTable(size_t, double, Func = std::hash<KeyType>());

    ~HashTable();

    void insert(KeyType, ValueType);
    ValueType *find(KeyType);
    void erase(KeyType);

    Node<KeyType, ValueType> &operator[](uint64_t);
    Node<KeyType, ValueType> at(uint64_t);

    size_t size() const;
    size_t capacity() const;
private:
    void rehash(size_t);
    double normalize_coeff(double) const;
    // Массив указателей на списки в хеш-таблице
    Node<KeyType, ValueType> **_buffer; 
    // Хеш-функция
    Func _hash_func;
    // Коэффициент загрузки
    double _overload_coeff;
    // Количество элементов
    size_t _size;
    // Количество бакетов
    size_t _capacity;
};
