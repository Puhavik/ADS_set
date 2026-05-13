# ADS_set — хеш-таблица с separate chaining на C++

Реализация учебного контейнера `ADS_set` (аналог множества), выполненная в рамках курса **Algorithmen und Datenstrukturen 1 (University of Vienna)**.

Контейнер хранит **уникальные элементы** и использует:
- хеширование через `std::hash<Key>`;
- сравнение ключей через `std::equal_to<Key>`;
- разрешение коллизий методом **separate chaining** (связные списки в бакетах).

## Возможности контейнера

В `ADS_set` реализованы основные операции множества:

- конструкторы:
  - по умолчанию (`ADS_set()`),
  - из `initializer_list`,
  - из диапазона итераторов,
  - копирующий конструктор;
- присваивание:
  - копирующее,
  - из `initializer_list`;
- модификация:
  - `insert(const key_type&)`,
  - `insert(first, last)`,
  - `erase(const key_type&)`,
  - `clear()`,
  - `swap(...)`;
- поиск и доступ:
  - `count(const key_type&)`,
  - `find(const key_type&)`;
- сервисные методы:
  - `size()`, `empty()`,
  - `begin()/end()` (константные итераторы),
  - `dump()` для отладки.

## Структура проекта

- `ADS_set.h` — основная реализация шаблонного контейнера;
- `simpletest.cpp` — интерактивный/ручной тестовый стенд;
- `btest.cpp` — расширенный тест (в т.ч. стресс и проверки поведения).

## Как собрать и запустить

> Требуется компилятор с поддержкой **C++17** (`g++`/`clang++`).

### 1) Сборка `simpletest`

Пример для ключей типа `unsigned`:

```bash
g++ -Wall -Wextra -Werror -O3 -std=c++17 -pedantic-errors -DPH2 -DETYPE=unsigned simpletest.cpp -o simpletest
./simpletest
```

Пример для ключей `std::string`:

```bash
g++ -Wall -Wextra -Werror -O3 -std=c++17 -pedantic-errors -DPH2 -DETYPE=std::string simpletest.cpp -o simpletest
./simpletest
```

### 2) Сборка `btest`

```bash
g++ -Wall -Wextra -Werror -O3 -std=c++17 -pedantic-errors btest.cpp -o btest
./btest
```

## Пример использования `ADS_set`

```cpp
#include "ADS_set.h"
#include <iostream>

int main() {
    ADS_set<int> s{1, 2, 3};

    s.insert(4);
    s.insert(2); // дубликат не добавится

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

## Принципы работы (кратко)

- Индекс бакета вычисляется как `hash(key) % table_size`.
- Если бакет пуст — элемент занимает головную ячейку бакета.
- Если бакет занят — элемент добавляется в связный список бакета.
- При росте числа элементов контейнер может расширять таблицу (`reserve`/`rehash`) и перераспределять ключи.

## Ограничения и примечания

- Это учебная реализация, ориентированная на корректность и понимание структуры данных.
- Производительность зависит от качества `std::hash<Key>` и распределения ключей.
- Для пользовательских типов `Key` должны быть корректно определены:
  - `std::hash<Key>`,
  - `std::equal_to<Key>`.

---

Если хотите, могу дополнительно сделать англоязычную версию README и добавить бейджи (C++ standard, build status, license).
