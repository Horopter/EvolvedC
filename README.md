# Evolved-C (EC) Data Structures Library

🚀 **Evolved-C** is a modern, header-only, thread-safe, and optimized **C library** providing a comprehensive set of **data structures and utilities**.

## ✅ Features
- 📦 **Header-Only**: No separate compilation required.
- 🧩 **Generic & Type-Safe**: Supports any data type with minimal boilerplate.
- 🏎️ **Optimized**: Efficient memory usage and cache-friendly algorithms.
- 🔒 **Thread-Safe**: Optional locking mechanisms for concurrency.
- 🛠️ **Minimal Dependencies**: Works with standard C (C11+ recommended).
- 🔗 **Modern Syntax**: Inspired by C++ STL and Rust.

---

## 📚 Implemented Data Structures

### **Core Collections**
- [`ec_vector.h`](./ec_vector.h) - **Dynamic Array**
- [`ec_stack.h`](./ec_stack.h) - **Stack (LIFO)**
- [`ec_queue.h`](./ec_queue.h) - **Queue (FIFO)**
- [`ec_deque.h`](./ec_deque.h) - **Double-Ended Queue**
- [`ec_slist.h`](./ec_slist.h) - **Singly Linked List**
- [`ec_dlist.h`](./ec_dlist.h) - **Doubly Linked List**
- [`ec_skiplist.h`](./ec_skiplist.h) - **Skip List**
- [`ec_heap.h`](./ec_heap.h) - **Binary Heap (Min/Max Heap)**

### **Hash-Based Data Structures**
- [`ec_hashmap.h`](./ec_hashmap.h) - **Hash Map (Dictionary)**
- [`ec_hashset.h`](./ec_hashset.h) - **Hash Set (Unique Elements)**

### **Tree-Based Data Structures**
- [`ec_treetable.h`](./ec_treetable.h) - **Balanced Tree Map**
- [`ec_treeset.h`](./ec_treeset.h) - **Balanced Tree Set**
- [`ec_trie.h`](./ec_trie.h) - **Prefix Tree (Trie)**

### **Graph Data Structures**
- [`ec_graph.h`](./ec_graph.h) - **Graph Representation (Adjacency List)**

### **Utility & Specialized Structures**
- [`ec_array.h`](./ec_array.h) - **Fixed-Size Array**
- [`ec_buffer.h`](./ec_buffer.h) - **Resizable Byte Buffer**
- [`ec_mempool.h`](./ec_mempool.h) - **Memory Pool for Efficient Allocations**
- [`ec_status.h`](./ec_status.h) - **Standardized Error Codes**
- [`ec_common.h`](./ec_common.h) - **Common Utilities & Macros**
- [`ec_ds.h`](./ec_ds.h) - **Disjoint Set**

### **Concurrency & Timing Utilities**
- [`ec_log.h`](./ec_log.h) - **Thread-Safe Logging System**
- [`ec_timer.h`](./ec_timer.h) - **Timer Utilities (One-Shot & Periodic)**
- [`ec_ringbuffer.h`](./ec_ringbuffer.h) - **Concurrent Ring Buffer**

---

## 📌 Quick Start

Include any header file in your project and start using the data structures:

### Example: Using `ec_vector.h`
```c
#include "ec_vector.h"
#include <stdio.h>

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

---

## 🔧 Contributions

Contributions are **highly encouraged**! To contribute:

1. **Fork** this repository.
2. **Create a new branch** for your changes.
3. **Commit** your modifications with meaningful messages.
4. **Submit a Pull Request (PR)** with a detailed description.

Before submitting, please review the **Evolved-C Core Design Principles**.

---

⭐ **Star this repository if you find it useful!** 🚀
