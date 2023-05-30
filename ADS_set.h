#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

template<typename Key, size_t N = 7>
class ADS_set {
public:
    class Iterator;

    using value_type = Key;
    using key_type = Key;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = Iterator;
    using iterator = const_iterator;
//    using key_compare = std::less<key_type>;                       // B+-Tree
    using key_equal = std::equal_to<key_type>;                       // Hashing
    using hasher = std::hash<key_type>;   //3%7 = hashes             // Hashing

private:
    enum class Mode {
        free, used
    };

    struct Element {  //ячейка
        key_type key; // элемен, который вставляем
        Mode mode{Mode::free}; // если в ячейку вставлено знаечение то меняется на used
        Element *next{nullptr};

        Element() = default;

        Element(key_type key, Mode mode, Element *next) : key(key), mode(mode), next(next) {}
    };

    Element *table{nullptr};
    size_type table_size{0};
    size_type current_size{0};

    void add(const key_type &key);

    Element *locate(const key_type &key) const;

    size_type h(const key_type &key) const {
        return hasher{}(key) % table_size;
    }

    void reserve(size_type i);

    void rehash(size_type i);

public:
//     Old Version
//    ADS_set() : table{new Element[N]}, table_size{N}, current_size{0} {};

//     New version
    ADS_set() { rehash(N); };

    ADS_set(std::initializer_list<key_type> ilist) : ADS_set{} { insert(ilist); }

    template<typename InputIt>
    ADS_set(InputIt first, InputIt last): ADS_set() { insert(first, last); }

    ADS_set(const ADS_set &other);

    ~ADS_set();

    ADS_set &operator=(const ADS_set &other);

    ADS_set &operator=(std::initializer_list<key_type> ilist);

    size_type size() const { return current_size; }

    bool empty() const { return current_size == 0; }

    void insert(std::initializer_list<key_type> ilist) { insert(ilist.begin(), ilist.end()); }

    std::pair<iterator, bool> insert(const key_type &key);

    template<typename InputIt>
    void insert(InputIt first, InputIt last); // PH1

    void clear();

    size_type erase(const key_type &key);

    size_type count(const key_type &key) const {
        return locate(key) != nullptr;
    }

    iterator find(const key_type &key) const;

    void swap(ADS_set &other);

    const_iterator begin() const;

    const_iterator end() const;

    void dump(std::ostream &o = std::cerr) const;

//    friend bool operator==(const ADS_set &lhs, const ADS_set &rhs);
//    friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs);
};


//---------------------------------------------------------------------------

template<typename Key, size_t N>
void ADS_set<Key, N>::add(const key_type &key) {
    size_type idx{h(key)};
    if (table[idx].mode == Mode::used) {
        auto *first = new Element{table[idx].key, Mode::free, table[idx].next};
        table[idx].key = key;
        table[idx].mode = Mode::used;
        table[idx].next = first;
    } else {
        table[idx].key = key;
        table[idx].mode = Mode::used;
        table[idx].next = nullptr;
    }
    ++current_size;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::Element *ADS_set<Key, N>::locate(const key_type &key) const {
    size_type idx{h(key)};
    Element *ptr = &table[idx];
    if (ptr->mode == Mode::used) {
        while (ptr) {
            if (key_equal{}(ptr->key, key)) {
                return ptr;
            }
            ptr = ptr->next;
        }
    }

    return nullptr;
}

template<typename Key, size_t N>
void ADS_set<Key, N>::reserve(size_type i) {
    if (i > table_size) {
        size_type new_table_size = table_size * 2;
        while (new_table_size < i) {
            new_table_size *= 2;
        }
        rehash(new_table_size);
    }
}

template<typename Key, size_t N>
void ADS_set<Key, N>::rehash(size_type i) {
    Element *old_table = table;
    size_type old_table_size = table_size;

    table = new Element[i];
    table_size = i;
    current_size = 0;

    for (size_type n = 0; n < old_table_size; ++n) {
        if (old_table[n].mode == Mode::used) {
            Element *current = &old_table[n];
            while (current) {
                add(current->key);
                current = current->next;
            }
        }
    }

    for (size_type n = 0; n < old_table_size; ++n) {
        Element *current = old_table[n].next;
        while (current) {
            Element *next = current->next;
            delete current;
            current = next;
        }
    }

    delete[] old_table;
}

//template<typename Key, size_t N>
//ADS_set<Key, N>::ADS_set(const ADS_set &other) {
//    reserve(other.table_size);
//    for (size_type i = 0; i < other.table_size; ++i) {
//        if (other.table[i].mode == Mode::used) {
//            Element *current = &other.table[i];
//            while (current) {
//                add(current->key);
//                current = current->next;
//            }
//        }
//    }
//}

template<typename Key, size_t N>
ADS_set<Key, N>::ADS_set(const ADS_set &other) {
    table_size = other.table_size;
    table = new Element[table_size];

    for (size_type i = 0; i < other.table_size; ++i) {
        if (other.table[i].mode == Mode::used) {
            Element *current_other = other.table[i].next;
            table[i].key = other.table[i].key;
            table[i].mode = Mode::used;
            Element *current_this = &table[i];

            while (current_other) {
                Element *new_node = new Element();
                new_node->key = current_other->key;
                new_node->mode = Mode::used;
                current_this->next = new_node;

                current_other = current_other->next;
                current_this = current_this->next;
            }
        }
    }
}

template<typename Key, size_t N>
ADS_set<Key, N>::~ADS_set() {
    for (size_type i = 0; i < table_size; ++i) {
        if (table[i].mode == Mode::used) {
            Element *current = table[i].next;
            while (current) {
                Element *temp = current;
                current = current->next;
                delete temp;
            }
        }
    }
    delete[] table;
}

template<typename Key, size_t N>
ADS_set<Key, N> &ADS_set<Key, N>::operator=(const ADS_set &other) {
    if (this != &other) {
        clear();
        rehash(other.table_size);
        for (size_type i = 0; i < other.table_size; ++i) {
            if (other.table[i].mode == Mode::used) {
                Element *current = &other.table[i];
                while (current) {
                    add(current->key);
                    current = current->next;
                }
            }
        }
    }
    return *this;
}

template<typename Key, size_t N>
ADS_set<Key, N> &ADS_set<Key, N>::operator=(std::initializer_list<key_type> ilist) {
    clear();
    insert(ilist);
    return *this;
}

//template<typename Key, size_t N>
//std::pair<typename ADS_set<Key, N>::iterator, bool> ADS_set<Key, N>::insert(const key_type &key) {
//    std::cout << " Inside insert ";
//    Element *location = locate(key);
//    if (location != nullptr) {
//        // The key already exists in the set.
//        // Return an iterator to the existing element and false.
//        std::cout << "Inside IF 1 ";
//        return std::make_pair(iterator(location), false);
//    }
//
//    // Check load factor
//    if (!count(key)){
//        float load_factor = static_cast<float>(current_size) / static_cast<float>(table_size);
//        if (load_factor >= 0.7) {
//            // If load factor is too high, rehash.
//            std::cout << "Inside IF 2 ";
//            reserve(table_size * 2);
//        }
//    }
//    std::cout << "Before add";
//    // Add the key.
//    add(key);
//    std::cout << "After add";
//    // Return an iterator to the newly inserted element and true.
//    return std::make_pair(iterator(locate(key)), true);
//}

template<typename Key, size_t N>
std::pair<typename ADS_set<Key, N>::iterator, bool> ADS_set<Key, N>::insert(const key_type &key) {
    auto n = locate(key);
    if (n) {
        return {iterator{n}, false};
    }
    reserve(current_size + 1);
    float load_factor = static_cast<float>(current_size) / static_cast<float>(table_size);
        if (load_factor >= 0.7) {
            reserve(table_size * 2);
        }
    this->add(key);
    return {iterator{locate(key)}, true};
}

template<typename Key, size_t N>
template<typename InputIt>
void ADS_set<Key, N>::insert(InputIt first, InputIt last) {
    for (auto it{first}; it != last; ++it) {
        if (!count(*it)) {
            float load_factor = static_cast<float>(current_size) / static_cast<float>(table_size);
            if (load_factor >= 0.7) {
                reserve(table_size * 2);
            }
            add(*it);
        }
    }
}

template<typename Key, size_t N>
void ADS_set<Key, N>::clear() {
    ADS_set buffer;
    swap(buffer);
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::erase(const key_type &key) {
    size_type idx{h(key)};
    Element *ptr = &table[idx];
    Element *prev{nullptr};

    if (ptr->mode == Mode::free) {
        return 0; // The key was not found
    }

    // Locate the key
    while (ptr) {
        if (key_equal{}(ptr->key, key)) {
            // Found the key
            if (prev == nullptr) {
                // The key is at the head of the list
                if (ptr->next) {
                    // Move the second node to the head
                    ptr->key = ptr->next->key;
                    Element *toDelete = ptr->next;
                    ptr->next = toDelete->next;
                    delete toDelete;
                } else {
                    // The key is the only node in the list
                    ptr->mode = Mode::free;
                }
            } else {
                // The key is not at the head of the list
                prev->next = ptr->next;
                delete ptr;
            }

            --current_size;
            return 1; // The key was found and deleted
        }

        prev = ptr;
        ptr = ptr->next;
    }

    return 0; // The key was not found
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::iterator ADS_set<Key, N>::find(const key_type &key) const {
    Element *location = locate(key);
    if (location != nullptr) {
        // The key exists in the set.
        // Return an iterator to the existing element.
        return iterator(location);
    }
    // The key does not exist in the set.
    // Return the end iterator.
    // Assuming your `end()` function is implemented.
    return end();
}

template<typename Key, size_t N>
void ADS_set<Key, N>::swap(ADS_set &other) {
    std::swap(table, other.table);
    std::swap(table_size, other.table_size);
    std::swap(current_size, other.current_size);
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::begin() const {
    for (size_type idx{0}; idx < table_size; ++idx) {
        if (table[idx].mode == Mode::used) {
            return const_iterator(&table[idx]);
        }
    }
    // No element was found, return end.
    return end();
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::end() const {
    return const_iterator(&table[table_size]);
}

template<typename Key, size_t N>
void ADS_set<Key, N>::dump(std::ostream &o) const {
    o << "Table size = " << table_size << ", Current size = " << current_size << "\n";
    for (size_type idx{0}; idx < table_size; ++idx) {
        o << idx << " : ";
        if (table[idx].mode == Mode::free) {
            o << "--Free\n";
        } else {
            Element *node = &table[idx];
            while (node) {
                o << node->key;
                node = node->next;
                if (node) {
                    o << " -> ";
                }
            }
            o << "\n";
        }
    }
}

template<typename Key, size_t N>
bool operator==(const ADS_set<Key, N> &lhs, const ADS_set<Key, N> &rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (const auto &elem: lhs) {
        if (rhs.count(elem) == 0) {
            return false;
        }
    }

    return true;
}

template<typename Key, size_t N>
bool operator!=(const ADS_set<Key, N> &lhs, const ADS_set<Key, N> &rhs) {
    return !(lhs == rhs);
}

template<typename Key, size_t N>
class ADS_set<Key, N>::Iterator {
    Element *e;

    void skip() {
        while (e->mode == Mode::free) {
            e++;
        }
    };

public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type &;
    using pointer = const value_type *;
    using iterator_category = std::forward_iterator_tag;

    explicit Iterator(Element *e = nullptr) : e{e} {
        if (e) skip();
    }

    reference operator*() const {
        return e->key;
    }

    pointer operator->() const {
        return &e->key;
    }

    Iterator &operator++() {
        ++e;
        skip();
        return *this;
    }

    Iterator operator++(int) {
        auto ret_code{*this};
        ++*this;
        return ret_code;
    }

    friend bool operator==(const Iterator &lhs, const Iterator &rhs) {
        return lhs.e == rhs.e;
    }

    friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {
        return lhs.e != rhs.e;
    }
};

template<typename Key, size_t N>
void swap(ADS_set<Key, N> &lhs, ADS_set<Key, N> &rhs) {
    lhs.swap(rhs);
}

#endif // ADS_SET_H