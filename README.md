# ADS_set — Hash Set with Separate Chaining (C++)

This repository contains an educational implementation of a custom set container, `ADS_set`, created for the **Algorithms and Data Structures 1** course at the University of Vienna.

`ADS_set` stores **unique keys** and is implemented as a hash table with **separate chaining** for collision handling.

## Key Characteristics

- Uses `std::hash<Key>` to compute bucket indices.
- Uses `std::equal_to<Key>` for key equality checks.
- Handles collisions with linked chains inside buckets.
- Supports common set operations similar to `std::set`/`std::unordered_set` semantics (unique elements, no duplicates).

## Implemented API

The container includes:

- Constructors:
  - default constructor,
  - initializer-list constructor,
  - range constructor (`first`, `last`),
  - copy constructor.
- Assignment:
  - copy assignment,
  - assignment from initializer list.
- Modifiers:
  - `insert(const key_type&)`,
  - `insert(first, last)`,
  - `erase(const key_type&)`,
  - `clear()`,
  - `swap(...)`.
- Lookup:
  - `count(const key_type&)`,
  - `find(const key_type&)`.
- Capacity/iteration/debug:
  - `size()`, `empty()`,
  - `begin()`, `end()`,
  - `dump()`.

## Repository Structure

- `ADS_set.h` — template implementation of the container.
- `simpletest.cpp` — interactive/basic test program.
- `btest.cpp` — more extensive test suite.

## Build and Run

A compiler with **C++17** support is required.

### Build `simpletest`

Example with `unsigned` keys:

```bash
g++ -Wall -Wextra -Werror -O3 -std=c++17 -pedantic-errors -DPH2 -DETYPE=unsigned simpletest.cpp -o simpletest
./simpletest
```

Example with `std::string` keys:

```bash
g++ -Wall -Wextra -Werror -O3 -std=c++17 -pedantic-errors -DPH2 -DETYPE=std::string simpletest.cpp -o simpletest
./simpletest
```

### Build `btest`

```bash
g++ -Wall -Wextra -Werror -O3 -std=c++17 -pedantic-errors btest.cpp -o btest
./btest
```

## Minimal Usage Example

```cpp
#include "ADS_set.h"
#include <iostream>

int main() {
    ADS_set<int> s{1, 2, 3};

    s.insert(4);
    s.insert(2); // duplicate, will not be inserted

    if (s.count(3)) {
        std::cout << "3 found\n";
    }

    s.erase(1);

    for (const auto& x : s) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
}
```

## How It Works (Short Overview)

- Bucket index is calculated as `hash(key) % table_size`.
- If a bucket is empty, the key is placed in its head element.
- If a bucket is occupied, the key is linked into that bucket’s chain.
- When needed, the table grows and keys are redistributed via rehashing.

## Notes and Limitations

- This is a course-oriented implementation focused on correctness and understanding.
- Runtime behavior depends on hash quality and key distribution.
- For custom key types, make sure both are correctly defined:
  - `std::hash<Key>`
  - `std::equal_to<Key>`
