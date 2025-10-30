# TODO

Short, high-signal list of next steps:

## Test Coverage Status (Alphabetical Order)

Create comprehensive tests for all data structures, ensuring full method coverage and multiple data type support:

- [ ] **array** - `test/array/` - Expand data type coverage and add tests for all methods
- [ ] **buffer** - `test/buffer/` - Expand data type coverage and add tests for all methods
- [ ] **deque** - `test/deque/` - Expand data type coverage and add tests for all methods
- [ ] **dlist** (doubly linked list) - `test/dlist/` - Expand data type coverage and add tests for all methods
- [ ] **ds** (disjoint set) - `test/ds/` - Expand data type coverage and add tests for all methods
- [ ] **graph** - `test/graph/` - Expand data type coverage and add tests for all methods
- [ ] **hashmap** - `test/hashmap/` - Expand data type coverage and add tests for all methods
- [ ] **hashset** - `test/hashset/` - Expand data type coverage and add tests for all methods
- [ ] **heap** - `test/heap/` - Expand data type coverage and add tests for all methods
- [ ] **log** - `test/log/` - Expand test scenarios and edge cases
- [ ] **queue** - `test/queue/` - Expand data type coverage and add tests for all methods
- [ ] **ringbuffer** - `test/ringbuffer/` - Expand data type coverage and add tests for all methods
- [ ] **skiplist** - `test/skiplist/` - Expand data type coverage and add tests for all methods
- [ ] **slist** (singly linked list) - `test/slist/` - Expand data type coverage and add tests for all methods
- [ ] **stack** - `test/stack/` - Expand data type coverage and add tests for all methods
- [ ] **timer** - `test/timer/` - Expand test scenarios and edge cases
- [ ] **treeset** - `test/treeset/` - Expand data type coverage and add tests for all methods
- [ ] **treetable** - `test/treetable/` - Expand data type coverage and add tests for all methods
- [ ] **trie** - `test/trie/` - Expand test scenarios and edge cases
- [ ] **vector** - `test/vector/` - Expand data type coverage and add tests for all methods

For each data structure subfolder in `test/`, add tests for more data types (char, int, uint, long, unsigned long, long long, unsigned long long, uint32_t, uint64_t, double, strings, pointers, structs) and ensure EVERY method is covered, including iterator/zip-iterator APIs where applicable.
- Ensure negative-path coverage everywhere: out-of-bounds indices, not-found results, empty/full behavior, and error codes (`EC_ERR_*`).
- Extend stress tests to all DS (currently present for vectors and hashmap) and include randomized scenarios with assertions.
- Add CI matrix for Debug/Release and enable ASan/UBSan jobs; fail fast on sanitizer findings.
- Add optional `EC_THREADSAFE` test runs in CI to validate locking paths.
- Generate a brief coverage report (e.g., gcovr/llvm-cov) and link the artifact in CI.
- Document formatting/linting workflow in README and add a local helper script (optional).
