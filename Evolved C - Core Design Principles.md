# Evolved-C Core Design Principles

---

## 1. Header-Only & Macro-Based API
- Entirely **implemented within a single header** using **macros** and/or `static inline` functions.
- No separate `.c` file required, simplifying **build** and **integration**.

## 2. Generic Data Support
- Users can define arrays (or other data structures) for **any data type**, typically through typed macros (e.g., `EC_ARRAY_DEF(TYPE, NAME)`).
- Ensures **flexibility** and **type safety** without code duplication.

## 3. Enum-Based Return Codes
- All functions return a **standardized error code** (`ec_status_t`), such as:
  - `EC_OK` for success
  - `EC_ERR_ALLOC` for allocation failures
  - `EC_ERR_OUT_OF_BOUNDS` for invalid indexes
  - `EC_ITER_END` for iterator termination
- Prevents **silent failures** and encourages **explicit error handling**.

## 4. Memory Optimization
- Optional integration with **`ec_mempool`** for **faster and more efficient** allocations.
- Fallback to **standard allocators** (`malloc`, `realloc`, `free`) if no pool is provided.
- Minimizes fragmentation and overhead, especially in performance-critical or embedded contexts.

## 5. Efficient Data Structures
- Ensures **amortized O(1)** insertion and **O(1)** direct access (e.g., arrays).
- In general, operations are kept **time-efficient** and **cache-friendly**.
- Common patterns include **push**, **pop**, **insert**, **remove**, **sort**, **search**, and more.

## 6. Iterators & Macros for Ease of Use
- Provides **simple iteration** macros, such as `FOREACH`, plus **structured iterators** (`iter_init`, `iter_next`, etc.).
- May include **zip iterators** to walk two structures in lockstep.
- Encourages **clean, readable** loops and advanced iteration patterns.

## 7. Thread-Safe & Lock-Free Considerations
- Optionally **wrap operations in mutex/spinlock** calls if `EC_ARRAY_THREADSAFE` or similar is defined.
- Basic concurrency with minimal overhead; advanced lock-free designs can be future expansions.
- Encourages safe parallel usage in multi-threaded environments.

## 8. Platform Agnostic & Lightweight
- Only uses **standard C library** and optionally **POSIX/Win32 locks**.
- Avoids OS-specific dependencies beyond optional threading constructs.
- Suitable for **Linux**, **Windows**, **embedded**, or **RTOS** with minimal changes.

## 9. Configurable Expansion & Capacity Handling
- Users can specify:
  - **Initial capacity** and **growth factor** (`EC_ARRAY_GROWTH_FACTOR`).
  - **Static size** (no expansions) or **dynamic expansions** (doubling capacity, for example).
- Adaptable to a **broad range** of memory and performance requirements.

## 10. Consistent Naming Scheme
- Functions, macros, and structures follow **`ec_`** (or **`EC_`**) prefix, e.g., `ec_array_init`, `EC_ARRAY_DEF`.
- Avoids collisions, maintains **clear naming** and **self-documentation**.

## 11. C++-Like Dot & Arrow Operator Usage
- Structures maintain an **internal function pointer table**, e.g., `arr->push(val)`.
- Allows **OOP-like syntax** in C while preserving performance and control.
- Improves readability and consistency with modern code practices.

## 12. Handle Common Exceptions
- Index out of bounds
- Out of Memory

## 13. Avoid implicit declaration errors

## 14. Use do { ... } while (0) for Safe Macro Expansion whenever absolutely needed.