#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

template <typename Key, size_t N = 7 >
class ADS_set {
public:
//    class /* iterator type (implementation-defined) */;
    using value_type = Key;
    using key_type = Key;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
//    using const_iterator = /* iterator type */;
//    using iterator = const_iterator;
//    using key_compare = std::less<key_type>;                         // B+-Tree
    using key_equal = std::equal_to<key_type>;                       // Hashing
    using hasher = std::hash<key_type>;   //3%7 = hashes                            // Hashing

private:
    enum class Mode{free, used};
     struct Element{  //ячейка
         key_type key; // элемен, который вставляем
         Mode mode {Mode::free}; // если в ячейку вставлено знаечение то меняется на used
         Element *next {nullptr};
         Element() = default;
         Element(key_type key, Mode mode, Element *next) : key(key), mode(mode), next(next) {}
     };
     Element *table {nullptr};
     size_type table_size {0};
     size_type current_size {0};
     void add (const key_type &key);
     Element *locate(const key_type &key) const;
     size_type h(const key_type &key) const {return hasher{}(key) % table_size;}

     void reserve (size_type i);
     void rehash (size_type i);

public:
    ADS_set():table{new Element[N]}, table_size{0}, current_size{0} {};
    ADS_set(std::initializer_list<key_type> ilist): ADS_set{} { insert(ilist);}
    template<typename InputIt> ADS_set(InputIt first, InputIt last): ADS_set(){ insert(first, last);}
    //ADS_set(const ADS_set &other);

//    ~ADS_set();

//    ADS_set &operator=(const ADS_set &other);
//    ADS_set &operator=(std::initializer_list<key_type> ilist);

    size_type size() const {return current_size;}
    bool empty() const {return current_size == 0;}

    void insert(std::initializer_list<key_type> ilist) { insert(ilist.begin(),ilist.end());}
//    std::pair<iterator,bool> insert(const key_type &key);
    template<typename InputIt> void insert(InputIt first, InputIt last); // PH1

//    void clear();
//    size_type erase(const key_type &key);

    size_type count(const key_type &key) const{return locate(key) != nullptr;}
//    iterator find(const key_type &key) const;

//    void swap(ADS_set &other);

//    const_iterator begin() const;
//    const_iterator end() const;

    void dump(std::ostream &o = std::cerr) const;

//    friend bool operator==(const ADS_set &lhs, const ADS_set &rhs);
//    friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs);
};

template <typename Key, size_t N>
 void ADS_set<Key,N>::add(const key_type &key){
     size_type idx {h(key)};
     if (table[idx].mode == Mode::used){
         auto *first = new Element {table[idx].key, Mode::free, table[idx].next};
         table[idx].key = key;
         table[idx].mode = Mode::used;
         table[idx].next = first;
     } else {
         table[idx].key = key;
         table[idx].mode = Mode::used;
         table[idx].next = nullptr;
     }
     //table[idx] =  {key, Mode::used}; // c туториума. в теории не нужно
     ++current_size;
 }

template <typename Key, size_t N>
typename ADS_set<Key, N>::Element *ADS_set<Key, N>::locate(const key_type &key) const{
     size_type idx{h(key)};
     Element *ptr = &table[idx];

     while(ptr){
         if (ptr->mode == Mode::used && key_equal{}(ptr->key, key)){
             return ptr;
         }
         ptr = ptr->next;
     }

     return nullptr;
 }

template <typename Key, size_t N>
void ADS_set<Key, N>::rehash(size_type i) {
    Element *old_table = table;
    size_type old_table_size = table_size;

    table = new Element[i];
    table_size = i;

    for (size_type n = 0; n < old_table_size; ++n) {
        Element *current = &old_table[n];
        while (current) {
            if (current->mode == Mode::used) {
                add(current->key);
            }
            Element *next = current->next;
            if (n > 0) {
                delete current;
            }
            current = next;
        }
    }

    delete[] old_table;
}

template <typename Key, size_t N>
void ADS_set<Key, N>::reserve(size_type i) {
    if (i > table_size) {
        size_type new_table_size = table_size * 2;
        while (new_table_size < i) {
            new_table_size *= 2;
        }
        rehash(new_table_size);
    }
}

template <typename Key, size_t N>
template<typename InputIt> void ADS_set<Key, N>::insert(InputIt first, InputIt last) {
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

template <typename Key, size_t N>
void ADS_set<Key, N>::dump(std::ostream &o) const {
    o << "Table size = " << table_size << ", Current size = " << current_size << "\n";
    for(size_type idx = 0; idx < table_size; ++idx) {
        o << idx << " : ";
        if (table[idx].mode == Mode::free) {
            o << "--Free\n";
        } else {
            Element *node = &table[idx];
            while(node) {
                o << node->key;
                node = node->next;
                if(node) {
                    o << " -> "; // separate keys in the same slot with "->"
                }
            }
            o << "\n";
        }
    }
}

//Tutorium versions
//template <typename Key, size_t N>
//typename ADS_set<Key, N>::Element *ADS_set<Key, N>::locate(const key_type &key) const{
//    size_type idx{h(key)};
//    for(;;){
//        switch (table[idx].mode) {
//            case Mode::free:
//                return nullptr;
//            case Mode::used:
//               if (key_equal{}(table[idx].key, key)){
//                   return table+idx;
//               }
//        }
//    }
//}
//template <typename Key, size_t N>
//void ADS_set<Key, N>::dump(std::ostream &o) const{
//    o << "table size = " << table_size << ", current size = " << current_size << "\n";
//    for(size_type idx {0}; idx < table_size; ++idx){
//        o << idx << " : ";
//        switch (table[idx].mode) {
//            case Mode::free:
//                o << "--Free";
//            case Mode::used:
//                o << table[idx].key;
//                //TODO дописать листы
//        }
//        o << "\n";
//    }
//}
//template <typename Key, size_t N>
//template<typename InputIt> void ADS_set<Key, N>::insert(InputIt first, InputIt last){
//    for (auto it{first}; it != last; ++it){
//        if(!count(*it)) {
//           // reserve(current_size +1); // TODO узнать нахуя
//            add(*it);
//        }
//    }
//}

#if 0
//template <typename Key, size_t N>
//class ADS_set<Key,N>::/* iterator type */ {
//public:
//    using value_type = Key;
//    using difference_type = std::ptrdiff_t;
//    using reference = const value_type &;
//    using pointer = const value_type *;
//    using iterator_category = std::forward_iterator_tag;
//
//    explicit /* iterator type */(/* implementation-dependent */);
//    reference operator*() const;
//    pointer operator->() const;
//    /* iterator type */ &operator++();
//    /* iterator type */ operator++(int);
//    friend bool operator==(const /* iterator type */ &lhs, const /* iterator type */ &rhs);
//    friend bool operator!=(const /* iterator type */ &lhs, const /* iterator type */ &rhs);
//};
//
//template <typename Key, size_t N>
//void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) { lhs.swap(rhs); }
#endif

#endif // ADS_SET_H