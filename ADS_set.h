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
    using hasher = std::hash<key_type>;   //3%7 = hasher             // Hashing

private:
//  Each element has moods. Free - if it has no key and used if it has a key and stay on the first position
    enum class Mode {
        free, used
    };

/*
  Struct is a box. This box has:
  1. key_type - this is something is inserted (number, string, etc)
  2. Mode - it shows if the box is available
  3. Element *next - it's a pointer to the next element. if it's nullptr it means this is the last element
  4. Element() = default -- this is a default constructor without parameters.
  5. Element(key_type key, Mode mode, Element *next) : key(key), mode(mode), next(next) {} -- this is constructor with parameters.
*/
    struct Element {  //ячейка
        key_type key; // элемен, который вставляем
        Mode mode{Mode::free}; // если в ячейку вставлено знаечение то меняется на used
        Element *next{nullptr};

        Element() = default;

        Element(key_type key, Mode mode, Element *next) : key(key), mode(mode), next(next) {}
    };

//  Initialising a pointer to the object type Element
    Element *table{nullptr};

//  Table size shows vertical length of the table
    size_type table_size{0};

//  Current size show number of inserted objects in the table
    size_type current_size{0};

//  Method which adds element to the box
    void add(const key_type &key);

//  Method which finds an element in the table
    Element *locate(const key_type &key) const;

//  Method which counts a place in hach table
    size_type h(const key_type &key) const {
        return hasher{}(key) % table_size;
    }

//  Method which reserves place for elements
    void reserve(size_type i);

//  Method which rehash the table
    void rehash(size_type i);

public:
//     Old Version
//    ADS_set() : table{new Element[N]}, table_size{N}, current_size{0} {};

//     New version
//  This is a default constructor without parameters
//  rehash(N) creates a set with default table_size = 7
    ADS_set() { rehash(N); };

//  This is a constructor which initialises with ilist (special list with some elements)
//  This constrictor initialise an objekt of the class ADS_set and adds some elements in it
    ADS_set(std::initializer_list<key_type> ilist) : ADS_set{} { insert(ilist); }

//  This is a template constructor which initialise ADS_set which receives 2 iterators: first (shows first element), last (shows last element)
//  and adds all elements from first till last
    template<typename InputIt>
    ADS_set(InputIt first, InputIt last): ADS_set() { insert(first, last); }

//  This is a copy constructor. It should initialise my ADS_set by copying another ADS_set, which is passed like argument.
    ADS_set(const ADS_set &other);

//  This is destructor. It should delete my ADS_set. It should delete not only vertical, but also horizontal.
    ~ADS_set();

//  This is copy operator. It should copy from "other" to my ADS_set. It should return a reference to *this
    ADS_set &operator=(const ADS_set &other);

//  This operator should add elements from ilist to my ADS_set. It should return a reference to *this
    ADS_set &operator=(std::initializer_list<key_type> ilist);

//  Method shows a number of elements in my container
    size_type size() const { return current_size; }

//  Method returns true if there is no elements in my table and false otherwise
    bool empty() const { return current_size == 0; }

//  This method inserts element from ilist to my ADS_set in given order.
    void insert(std::initializer_list<key_type> ilist) { insert(ilist.begin(), ilist.end()); }

//  This method should insert an element to my ADS_set and return a pair of iterator and bool.
//  If element was inserted: Iterator points to inserted element, bool should be true.
//  If element was NOT inserted: Iterator points to an element which is already in the table, bool returns false
    std::pair<iterator, bool> insert(const key_type &key);

//  This method inserts elements. First and Last are iterators which shows a range of elements, that should be inserted
    template<typename InputIt>
    void insert(InputIt first, InputIt last); // PH1
//  Method deletes all elements from the ADS_set
    void clear();

//  Method deletes a chosen element. My ADS_set shouldn't be rehashed after deleting
    size_type erase(const key_type &key);

//  Method should return 1 if key in arguments located in my ADS_set and 0 otherwise.
    size_type count(const key_type &key) const {
        return locate(key) != nullptr;
    }

//  This method should return an iterator to an element in my ADS_set if element is found, otherwise it returns end-iterator
    iterator find(const key_type &key) const;

//  This method swapped the elements of my container with the elements of another container
    void swap(ADS_set &other);

//  Returns an iterator to the first element. If ADS_set is empty it should return end-iterator
    const_iterator begin() const;

//  Returns an iterator to the "virtual" element after the last element of my ADS_set (end-iterator)
    const_iterator end() const;

//  Shows table in terminal
    void dump(std::ostream &o = std::cerr) const;

//  This method checks if my ADS_set is same as another ADS_set. Method should check table size, current size and each element.
//  If everything is same it returns true, otherwise it returns false.
    friend bool operator==(const ADS_set &lhs, const ADS_set &rhs) {
        if (lhs.size() != rhs.size()) {
            return false; // if number of elements in lhs and rhs tables aren't same return false
        }

        for (const auto &elem: lhs) { // iterating through the lhs
            if (rhs.count(elem) == 0) { // if one element in lhs isn't equal to the element in rhs ...
                return false; // returning fale
            }
        }
        return true; // if everything is equla returning true
    }


//  Like previous method but true and false are changed
    friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs) {
        return !(lhs == rhs); // using reversed operator==
    }
};

//template<typename Key, size_t N>
//void ADS_set<Key, N>::add(const key_type &key) {
//    size_type idx{h(key)}; // receiving hash number from key
//    if (table[idx].mode == Mode::used) { // if there is an element with same hash in the table
//        auto *first = new Element{table[idx].key, Mode::free,
//                                  table[idx].next}; // creating a buffer with old first element
//        table[idx].key = key; // new element adds to the ADS_set table
//        table[idx].mode = Mode::used; // Box of this new element has Mode - used
//        table[idx].next = first; // new first element has a reference to the old first element
//    } else { // if there is no collisions new element just adds to the ADS_set table
//        table[idx].key = key; // box with index idx now has key value
//        table[idx].mode = Mode::used; // mode = used
//        table[idx].next = nullptr; // this box has no references to the next element, because it's the only one element with this index
//    }
//    ++current_size; // increasing number of added elements
//}

template<typename Key, size_t N>
void ADS_set<Key, N>::add(const key_type &key) {
    size_type idx{h(key)}; // receiving hash number from key
    if (table[idx].mode == Mode::used) { // if there is an element with same hash in the table
        auto *new_element = new Element{key, Mode::used, table[idx].next}; // creating a new element
        table[idx].next = new_element; // new element is added to the existing list
    } else { // if there is no collisions new element just adds to the ADS_set table
        table[idx].key = key; // box with index idx now has key value
        table[idx].mode = Mode::used; // mode = used
        table[idx].next = nullptr; // this box has no references to the next element, because it's the only one element with this index
    }
    ++current_size; // increasing number of added elements
}


template<typename Key, size_t N>
typename ADS_set<Key, N>::Element *ADS_set<Key, N>::locate(const key_type &key) const {
    size_type idx{h(key)}; // receiving hash number from key
    Element *ptr = &table[idx]; // creating a pointer, which points to the first element with index = idx of my table
    if (ptr->mode == Mode::used) { // if element that pointer points to is used...
        while (ptr) { // while pointer isn't equal to nullptr
            if (key_equal{}(ptr->key,
                            key)) { // if pointer ponts to the element and this element is equal to the key which is looking for
                return ptr; // method returns this pointer
            }
            ptr = ptr->next; // if pointer points to the element which isn't equal to the key, pointer goes to the next element with same index (horizontal iteration)
        }
    }
    return nullptr; // if there is no element in my table which is equal to the key, method returns nullptr
}

template<typename Key, size_t N>
void ADS_set<Key, N>::reserve(size_type i) {
    if (i > table_size) { // if my table_size is smaller than I should make my table bigger.
        size_type new_table_size = table_size * 2; // making table size 2 times bigger.
        while (new_table_size < i) { // while table size is smaller than i
            new_table_size *= 2;     // table size doubles
        }
        rehash(new_table_size); // after finding right table size, table should be rehashed
    }
}

template<typename Key, size_t N>
void ADS_set<Key, N>::rehash(size_type i) {
    Element *old_table = table; // copying my current table
    size_type old_table_size = table_size; // copying my current table size

    table = new Element[i]; // overwriting my table with  new table size i
    table_size = i; // overwriting my table size (vertical)
    current_size = 0; // after overwriting number of elements in my overwritten table = 0

    for (size_type n = 0; n < old_table_size; ++n) { // iterating through my old table vertical
        if (old_table[n].mode == Mode::used) { // if index has mode used
            Element *current = &old_table[n];  // creating a pointer to an element with index n
            while (current) {            // while current points to an element
                add(current->key);  // adding this element to the overwritten table
                current = current->next; // going to the next element in my old table (horizontal)
            }
        }
    }
    for (size_type n = 0; n < old_table_size; ++n) { // iterating again through my old table
        Element *current = old_table[n].next; // creating a pointer to an element with index n
        while (current) { // while current points to an element
            Element *next = current->next; // creating pointer to the next element in table (horizontal)
            delete current; // delete current element
            current = next; // going to the next element
        }
    }
    delete[] old_table; // deleting copy of my old table
}

template<typename Key, size_t N>
ADS_set<Key, N>::ADS_set(const ADS_set &other) {
    if (this->table_size != other.table_size) {
        rehash(other.table_size);
    }
    for (size_type i = 0; i < other.table_size; ++i) { // iterating through other table (vertical)
        if (other.table[i].mode == Mode::used) { // if index is used
            add(other.table[i].key);
            Element *current_other = other.table[i].next; // creating pointer to the next element in other table
            table[i].key = other.table[i].key; // copying key of element from other table to new table
            while (current_other) { // iterating through the other table horizontal
                add(current_other->key);
                current_other = current_other->next;
            }
        }
    }
}

template<typename Key, size_t N>
ADS_set<Key, N>::~ADS_set() {
    for (size_type i = 0; i < table_size; ++i) { // iterating through my table (vertical)
        if (table[i].mode == Mode::used) { // if index has mode used
            Element *current = table[i].next; // pointer to the next element in my table
            while (current) { // while there are some elements in my table with same index (horizontal)
                Element *temp = current; // pointer to the current element
                current = current->next; // now current points to the next element
                delete temp; // deleting element in table (horizontal)
            }
        }
    }
    delete[] table; // deleting memory used for table
}

template<typename Key, size_t N>
ADS_set<Key, N> &ADS_set<Key, N>::operator=(const ADS_set &other) {
    if (this != &other) { // check if both tables are same
        clear(); // completely delete my table
        rehash(other.table_size);  // rehashing my table to pass to other table
        for (size_type i = 0; i < other.table_size; ++i) { // iterating through other table
            if (other.table[i].mode == Mode::used) { // if index has mode used
                Element *current = &other.table[i]; // creating pointer to the element in other table
                while (current) { // while there ara any elements (horizontal)
                    add(current->key); // adding element from other table to my table
                    current = current->next; // moving pointer to the next element
                }
            }
        }
    }
    return *this; // returning pointer to my table
}

template<typename Key, size_t N>
ADS_set<Key, N> &ADS_set<Key, N>::operator=(std::initializer_list<key_type> ilist) {
    clear(); // completely delete my table
    insert(ilist); // insert ilist to my table
    return *this; // returning pointer to my table
}

template<typename Key, size_t N>
std::pair<typename ADS_set<Key, N>::iterator, bool> ADS_set<Key, N>::insert(const key_type &key) {
    Element *current_pos{locate(key)};
    if (current_pos) {
        return {iterator(current_pos, table, h(key), table_size), false};
    } else {
        float load_factor = static_cast<float>(current_size) / static_cast<float>(table_size);
        if (load_factor >= 0.7) {
            reserve(table_size * 2);
        }
        add(key);
        return {iterator(locate(key), table, h(key), table_size), true};
    }
}

template<typename Key, size_t N>
template<typename InputIt>
void ADS_set<Key, N>::insert(InputIt first, InputIt last) {
    for (auto it{first}; it != last; ++it) { // iterating from first element to the last element
        if (!count(*it)) { // check if there is no element with key *it in my table
            float load_factor = static_cast<float>(current_size) /
                                static_cast<float>(table_size); // checking load factor of my table
            if (load_factor >= 0.7) { // if load factor smaller than 0.7
                reserve(table_size * 2); // rehash table
            }
            add(*it); // adding ket = *it
        }
    }
}

template<typename Key, size_t N>
void ADS_set<Key, N>::clear() {
    ADS_set buffer; // creating new default empty table
    swap(buffer); // replacing my table with buffer table.
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::erase(const key_type &key) {
    size_type idx{h(key)}; // receiving hach of the key
    Element *ptr = &table[idx]; // creating pointer to table's index
    Element *prev{nullptr}; // creating pointer to the previous element in table (horizontal)
    if (ptr->mode == Mode::free) { // if first box has mode free ...
        return 0;
    }
    while (ptr) { // till pointer isn't equal to null. iterating horizontal
        if (key_equal{}(ptr->key, key)) { // checking if current pointer is equla to key
            if (prev ==
                nullptr) { // if prev pounter equal to null pointer - it means that current pointer points to the first element
                if (ptr->next) { // if current pointer has next element 
                    ptr->key = ptr->next->key; // copying key from next element to current element
                    Element *toDelete = ptr->next; // creating pointer to the next element;
                    ptr->next = toDelete->next; // setting current next as next after deleted element
                    delete toDelete; // deleting element
                } else {
                    ptr->mode = Mode::free; // setting par's mode to free
                }
            } else {
                prev->next = ptr->next; // previous next is now current pointer next
                delete ptr; // deleting pointer
            }
            --current_size; // reduce number of elements in the table
            return 1;
        }
        prev = ptr; // setting previous pointer as current pointer
        ptr = ptr->next; // setting current pointer as next pointer
    }
    return 0;
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::iterator ADS_set<Key, N>::find(const key_type &key) const {
    Element *location = locate(key); // setting pointer to element we're looking for
    if (location != nullptr) { // if pointer to the element we're looking isn't nullptr ..
        return iterator(location, table, h(key),
                        table_size); // returnting iterator which points to the element we're looking for
    }
    return end(); // if element isn't found returning end iterator
}

template<typename Key, size_t N>
void ADS_set<Key, N>::swap(ADS_set &other) {
    std::swap(table, other.table); // swaping my table and other table
    std::swap(table_size, other.table_size); // swaping table sizes
    std::swap(current_size, other.current_size); // swapping numbers of elements in tables
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::begin() const {
    for (size_type idx{0}; idx < table_size; ++idx) { // iterating through my table (vertical)
        if (table[idx].mode == Mode::used) { // if index has mode used ..
            return const_iterator(&table[idx], table, idx,
                                  table_size); // returning const iterator pointing to the first element with index == idx
        }
    }
    return end(); // if nothing was found returning end iterator
}

template<typename Key, size_t N>
typename ADS_set<Key, N>::const_iterator ADS_set<Key, N>::end() const {
    return const_iterator(); // returning const iterator
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
class ADS_set<Key, N>::Iterator {
    Element *current_pos;
    Element *table;
    size_type idx;
    size_type table_size;

    void skip() {
        while (table[idx].mode != Mode::used && table_size > idx) {
            ++idx;
        }
    };

public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type &;
    using pointer = const value_type *;
    using iterator_category = std::forward_iterator_tag;

    explicit Iterator(Element *current_pos = nullptr, Element *table = nullptr, size_type idx = 0,
                      size_type table_size = N) : current_pos{current_pos}, table{table}, idx{idx},
                                                  table_size{table_size} {}

    reference operator*() const {
        return current_pos->key;
    }

    pointer operator->() const {
        return &current_pos->key;
    }

    Iterator &operator++() {
        if (current_pos->next && current_pos) {
            current_pos = current_pos->next;
        } else {
            ++idx;
            skip();
            if (idx == table_size) {
                current_pos = nullptr;
                return *this;
            }
            current_pos = &table[idx];
        }
        return *this;
    }

    Iterator operator++(int) {
        auto ret_code{*this};
        ++*this;
        return ret_code;
    }

    friend bool operator==(const Iterator &lhs, const Iterator &rhs) {
        return lhs.current_pos == rhs.current_pos;
    }

    friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {
        return !(lhs.current_pos == rhs.current_pos);
    }
};

template<typename Key, size_t N>
void swap(ADS_set<Key, N> &lhs, ADS_set<Key, N> &rhs) { lhs.swap(rhs); }

#endif // ADS_SET_H