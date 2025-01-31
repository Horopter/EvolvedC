# Evolved-C (EC) Data Structures Library

🚀 **Evolved-C** is a modern, header-only, thread-safe, and optimized **C library** that provides a comprehensive set of **data structures and utilities**.

## ✅ Features
- 📦 **Header-Only**: No separate compilation required.
- 🧩 **Generic & Type-Safe**: Supports any data type.
- 🏎️ **Optimized**: Memory-efficient and cache-friendly.
- 🔒 **Thread-Safe**: Uses mutexes for safe concurrent access.
- 🛠️ **Minimal Dependencies**: Works with standard C.
- 🔗 **Modern Syntax**: Inspired by C++ STL and Rust.

## 📚 Implemented Data Structures

### **Core Collections**
- `ec_vector.h` - **Dynamic Array**
- `ec_queue.h` - **Queue**
- `ec_list.h` - **Singly Linked List**
- `ec_dlist.h` - **Doubly Linked List**
- `ec_deque.h` - **Double-Ended Queue**
- `ec_heap.h` - **Binary Heap (Min/Max Heap)**
- `ec_hashmap.h` - **Hash Map (Dictionary)**
- `ec_hashset.h` - **Hash Set (Unique Values)**
- `ec_treetable.h` - **Balanced Tree Map**
- `ec_treeset.h` - **Balanced Tree Set**
- `ec_ring_buffer.h` - **Circular Buffer**
- `ec_trie.h` - **Prefix Tree (Trie)**
- `ec_graph.h` - **Graph Data Structure**
- `ec_skiplist.h` - **Skip List**
- `ec_disjoint_set.h` - **Union-Find (Disjoint Set)**
- `ec_mempool.h` - **Memory Pool**
- `ec_buffer.h` - **Byte Buffer**
- `ec_logger.h` - **Thread-Safe Logger**
- `ec_timer.h` - **Timer Utilities**
- `ec_thread.h` - **Threading Utilities**

## 📌 Usage

Include any header file in your project and start using the data structures:

```c
#include "ec_vector.h"

int main() {
    ec_vector_int_t vec;
    ec_vector_int_init(&vec, 10, NULL);
    
    vec.push(&vec, 42);
    vec.push(&vec, 84);
    
    int value;
    vec.get(&vec, 0, &value);
    
    printf("First value: %d\n", value);
    
    vec.free_fn(&vec);
    return 0;
}
```

## 🔧 Contributions

Contributions are welcome! If you find bugs or want to improve the library:

1. **Fork** the repository.
2. **Create a new branch** for your changes.
3. **Commit** your modifications.
4. **Open a Pull Request (PR)** with details about your update.

Before submitting, please check for consistency with the **Evolved-C Core Design Principles**.

---

⭐ **Star this repository if you find it useful!**
