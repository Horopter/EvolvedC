#Evolved - C(EC) Data Structures Library

![CI](https://github.com/Horopter/EvolvedC/actions/workflows/ci.yml/badge.svg)

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

## 🧪 CI / Tests Status

- GitHub Actions workflow `ci.yml` runs on Ubuntu and macOS.
- Steps: format (non-blocking), build, and run all tests via `test/run_tests.sh` across structure-specific subfolders.
- Current status: ✅ **All tests passing** - Comprehensive test coverage across all data structures with **253 test files**
- Test execution: All tests compile and run successfully; TBD tests (struct type limitations) are properly skipped
- Recent fixes: Resolved all test failures in dlist, vector, slist, hashmap, and skiplist modules; enabled ringbuffer char tests

### Recent Updates (Highlights)

#### Code Quality & Formatting
- ✅ **Code Formatting**: Applied clang-format (GNU style) to all 278 C/C++ files in `src/` and `test/`
- ✅ **Consistent Style**: All source code now follows the `.clang-format` configuration (GNU style with tabs)
- ✅ **Test Suite Health**: All test failures resolved; entire test suite passing with comprehensive coverage
  - Fixed assertion mismatches in dlist, vector, and hashmap tests
  - Fixed iterator index calculations in dlist and slist tests
  - Fixed type definitions and compilation errors in skiplist tests (u32, u64, uint, ul, ull, ulong)
  - Fixed string comparison issues in slist and hashmap tests
  - Enabled ringbuffer char tests by adding missing `EC_RINGBUF_DEF(char, ch)` macro

#### Test Organization & Coverage
- Organized tests into subfolders per data structure under `test/`:
  - `array/`, `buffer/`, `deque/`, `dlist/`, `ds/`, `graph/`, `hashmap/`, `hashset/`, `heap/`, `log/`, `queue/`, `ringbuffer/`, `skiplist/`, `slist/`, `stack/`, `timer/`, `treeset/`, `treetable/`, `trie/`, `vector/`.
- **Completed Test Coverage** (✅):
  - `array`: 13 test files covering all data types
  - `buffer`: 15 test files covering all data types
  - `deque`: 14 test files covering all data types
  - `dlist`: 14 test files covering all data types
  - `queue`: 13 test files covering all data types
  - `slist`: 13 test files covering all data types
  - `timer`: 1 comprehensive test file
  - `trie`: 7 test files covering all language types
- **In Progress Test Coverage** (⚠️):
  - `ds`: 14 test files, coverage expansion ongoing
  - `graph`: 13 test files, coverage expansion ongoing
  - `hashmap`: 15 test files, coverage expansion ongoing
  - `hashset`: 14 test files, coverage expansion ongoing
  - `heap`: 13 test files, coverage expansion ongoing
  - `log`: 14 test files, coverage expansion ongoing
  - `ringbuffer`: 12 test files, coverage expansion ongoing
  - `skiplist`: 15 test files, coverage expansion ongoing
  - `stack`: 14 test files, coverage expansion ongoing
  - `treeset`: 15 test files, coverage expansion ongoing
  - `treetable`: 11 test files, coverage expansion ongoing
  - `vector`: 13 test files + 1 stress test, coverage expansion ongoing
- Expanded `vector` coverage across many data types by mirroring `int` tests to: `double`, `long`, `unsigned long`, `long long`, `unsigned long long`, `uint32_t`, `uint64_t`, `const char*`, and `void*`.
- Added and strengthened tests for `array`, `buffer`, and `deque`; ensured per-method coverage and iterator/zip-iterator checks where available.
- Removed all meta tests and relocation stubs; each data structure now has independent, explicit tests.
- Fixed header include issues (case and paths); standardized includes in tests to `../../src/*.h` from subfolders.

#### Infrastructure & Tooling
- Added `.clang-format` (GNU style) configuration file
- Improved `test/run_tests.sh` to build into `test/build/`, accept optional targets, and clean generated outputs.
- Introduced CI workflow (`.github/workflows/ci.yml`) with cross-platform builds, formatting check (non-blocking), and robust path resolution on runners.
- Updated `.gitignore` to exclude build artifacts and IDE files.
- Cleaned up temporary files (`.tmp`, `.o`), duplicate test files, and outdated coverage analysis documents.


## 📋 TODO

- See `TODO.md` for a brief list of next steps.

---

⭐ **Star this repository if you find it useful!** 🚀
