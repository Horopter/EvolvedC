# TODO

Short, high-signal list of next steps:

## Recent Updates
- ✅ **Code Formatting**: Applied clang-format (GNU style) to all 278 C/C++ files in `src/` and `test/`
- ✅ **Test Organization**: Organized all tests into structure-specific subfolders with comprehensive coverage
- ✅ **Test Fixes**: Resolved all test failures across dlist, vector, slist, hashmap, and skiplist modules
  - Fixed assertion mismatches, iterator index calculations, type definitions, and compilation errors
  - All test suites now passing successfully with zero failures

## Test Coverage Status (Alphabetical Order)

Create comprehensive tests for all data structures, ensuring full method coverage and multiple data type support:

- [x] **array** - `test/array/` - ✅ COMPLETE: 13 test files covering all data types
- [x] **buffer** - `test/buffer/` - ✅ COMPLETE: 15 test files covering all data types
- [x] **deque** - `test/deque/` - ✅ COMPLETE: 14 test files covering all data types
- [x] **dlist** (doubly linked list) - `test/dlist/` - ✅ COMPLETE: 14 test files covering all data types
- [ ] **ds** (disjoint set) - `test/ds/` - ⚠️ IN PROGRESS: 14 test files, expand coverage
- [ ] **graph** - `test/graph/` - ⚠️ IN PROGRESS: 13 test files, expand coverage
- [ ] **hashmap** - `test/hashmap/` - ⚠️ IN PROGRESS: 15 test files, expand coverage
- [ ] **hashset** - `test/hashset/` - ⚠️ IN PROGRESS: 14 test files, expand coverage
- [ ] **heap** - `test/heap/` - ⚠️ IN PROGRESS: 13 test files, expand coverage
- [ ] **log** - `test/log/` - ⚠️ IN PROGRESS: 14 test files, expand edge cases
- [x] **queue** - `test/queue/` - ✅ COMPLETE: 13 test files covering all data types
- [ ] **ringbuffer** - `test/ringbuffer/` - ⚠️ IN PROGRESS: 12 test files, expand coverage
- [ ] **skiplist** - `test/skiplist/` - ⚠️ IN PROGRESS: 15 test files, expand coverage
- [x] **slist** (singly linked list) - `test/slist/` - ✅ COMPLETE: 13 test files covering all data types
- [ ] **stack** - `test/stack/` - ⚠️ IN PROGRESS: 14 test files, expand coverage
- [x] **timer** - `test/timer/` - ✅ COMPLETE: 1 test file with comprehensive timer API coverage
- [ ] **treeset** - `test/treeset/` - ⚠️ IN PROGRESS: 15 test files, expand coverage
- [ ] **treetable** - `test/treetable/` - ⚠️ IN PROGRESS: 11 test files, expand coverage
- [x] **trie** - `test/trie/` - ✅ COMPLETE: 7 test files covering all language types
- [ ] **vector** - `test/vector/` - ⚠️ IN PROGRESS: 14 test files + 1 stress test, expand coverage

For each data structure subfolder in `test/`, add tests for more data types (char, int, uint, long, unsigned long, long long, unsigned long long, uint32_t, uint64_t, double, strings, pointers, structs) and ensure EVERY method is covered, including iterator/zip-iterator APIs where applicable.
- Ensure negative-path coverage everywhere: out-of-bounds indices, not-found results, empty/full behavior, and error codes (`EC_ERR_*`).
- Extend stress tests to all DS (currently present for vectors and hashmap) and include randomized scenarios with assertions.
- Add CI matrix for Debug/Release and enable ASan/UBSan jobs; fail fast on sanitizer findings.
- Add optional `EC_THREADSAFE` test runs in CI to validate locking paths.
- Generate a brief coverage report (e.g., gcovr/llvm-cov) and link the artifact in CI.
- Document formatting/linting workflow in README (clang-format configuration already added).
