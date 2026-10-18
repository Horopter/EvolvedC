# Evolved-C Core Design Principles

---

## 1. Header-Only & Macro-Based API
- **Implemented within a single header** using `static inline` functions and macros.
- No separate `.c` file required, simplifying **build**, **integration**, and **portability**.
- Reduces **compile-time dependencies** while maintaining **high performance**.

## 2. Generic Data Support
- Users can define **arrays, buffers, and other structures** for **any data type** via typed macros.
- Example: `EC_ARRAY_DEF(TYPE, NAME)` provides **compile-time type safety**.
- **Eliminates code duplication** while ensuring **flexibility**.

## 3. Enum-Based Return Codes
- All functions return a **standardized error code** (`ec_status_t`) to enforce strict error handling.
- Common error codes:
  - ✅ `EC_OK` → Success
  - ❌ `EC_ERR_ALLOC` → Memory allocation failure
  - 🚨 `EC_ERR_OUT_OF_BOUNDS` → Invalid index
  - 🔄 `EC_ITER_END` → Iterator reached the end
- **Prevents silent failures** and encourages **explicit error checking**.

## 4. Memory Optimization
- Uses **`ec_mempool`** (if enabled) for **faster** and **more efficient** allocations.
- **Falls back** to standard allocators (`malloc`, `realloc`, `free`) when a memory pool is **not available**.
- **Reduces fragmentation**, making it **ideal for embedded** and **performance-critical applications**.

## 5. Efficient Data Structures
- Designed for **O(1) amortized insertion** and **O(1) direct access** where possible.
- Provides **efficient push, pop, insert, remove, sort, and search** operations.
- **Cache-friendly** design improves **performance** and **predictability**.

## 6. Iterators & Macro-Based Looping
- Offers **simple iteration macros** (`FOREACH`).
- Includes **structured iterators** (`iter_init`, `iter_next`, etc.).
- Supports **zip iterators** for **walking multiple structures simultaneously**.
- **Encourages clean and readable code** while optimizing iteration efficiency.

## 7. Thread-Safe & Lock-Free Considerations
- Optionally **wraps operations in mutex/spinlock** calls when `EC_THREADSAFE` is enabled.
- **Lightweight concurrency handling** for **parallel execution**.
- Future expansions may include **lock-free designs** for even **faster multi-threading**.

## 8. Platform Agnostic & Lightweight
- **Built on the C standard library** and optional **POSIX/Win32 threading**.
- Works **across Linux, Windows, macOS, embedded systems, and RTOS**.
- **Minimizes OS-specific dependencies**, making it highly **portable**.

## 9. Configurable Expansion & Capacity Handling
- Users can configure:
  - **Initial capacity** and **growth factor** (`EC_ARRAY_GROWTH_FACTOR`).
  - **Static size** (fixed memory) or **dynamic expansion** (resizable).
- **Optimized for both low-memory environments and large-scale applications**.

## 10. Consistent Naming Scheme
- All functions, macros, and structures follow a **consistent naming pattern**:
  - **Functions** → `ec_array_init`, `ec_log_write`
  - **Macros** → `EC_ARRAY_DEF`, `EC_MUTEX_LOCK`
  - **Avoids collisions**, making integration easier.

## 11. C++-Like Dot & Arrow Operator Usage
- Structures use an **internal function pointer table**, allowing:
  ```c
  arr->push(value);
  ```
- **Encourages OOP-style usage** while preserving C's **performance and control**.
- **Improves readability** and **modernizes** the codebase.

## 12. Handle Common Exceptions
- **Prevents runtime crashes by checking:**
  - ❌ **Index out of bounds errors**
  - 🚨 **Out of memory conditions**
- **Ensures safe execution in edge cases.**

## 13. Avoid Implicit Declaration Errors
- All functions are **properly declared before use**.
- Enforces **strict compile-time checks**.
- Ensures compatibility with **modern C compilers**.

## 14. Safe Macro Expansion (`do { ... } while (0)`)
- **Encapsulates macros inside `do { ... } while (0)`** to ensure correct expansion.
- Prevents **unexpected behavior** when used in **if-else** or **loop conditions**.
- Example:
  ```c
  #define EC_SAFE_MACRO(x) do { foo(x); bar(x); } while (0)
  ```
- **Ensures macros behave as expected** in all scenarios.