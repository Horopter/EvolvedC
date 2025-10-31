# ec_deque Test Coverage Analysis

## Methods Defined in ec_deque.h
1. `init` - Initialize deque
2. `push_front` - Push element at front
3. `push_back` - Push element at back
4. `pop_front` - Pop element from front
5. `pop_back` - Pop element from back
6. `peek_front` - Peek at front element (without removing)
7. `peek_back` - Peek at back element (without removing)
8. `size` - Get size of deque
9. `free` - Free the deque

## Data Types Defined in ec_deque.h
1. int
2. uint (unsigned int)
3. long
4. ll (long long)
5. ul (unsigned long)
6. ull (unsigned long long)
7. u32 (uint32_t)
8. u64 (uint64_t)
9. double (dbl)
10. str (const char*)
11. ptr (void*)

## Coverage Matrix

### int (test_ec_deque_int.c)
- ✓ init: positive
- ✓ free: positive
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive + negative (empty)
- ✓ pop_back: positive + negative (empty)
- ✓ peek_front: positive + negative (empty)
- ✓ peek_back: positive + negative (empty)
- ✓ size: positive
- ✓ invalid args: negative (NULL deque for pop/peek)

### uint (test_ec_deque_uint.c)
- ✓ init: positive (implied via test_basic_ops)
- ✓ free: positive (implied via test_basic_ops)
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive + negative (empty)
- ✓ pop_back: positive + negative (empty)
- ✓ peek_front: positive
- ✓ peek_back: positive
- ✓ size: positive
- ✓ invalid args: negative (NULL output pointer)

### long (test_ec_deque_long.c)
- ✓ init: positive (implied)
- ✓ free: positive (implied)
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive
- ✓ pop_back: positive
- ✓ peek_back: positive
- ✗ peek_front: MISSING
- ✓ size: positive
- ✗ empty deque negative tests: MISSING
- ✗ invalid args negative tests: MISSING

### ll (test_ec_deque_ll.c)
- ✓ init: positive (implied)
- ✓ free: positive (implied)
- ✓ push_back: positive
- ✓ peek_front: positive
- ✓ peek_back: positive
- ✗ push_front: MISSING
- ✗ pop_front: MISSING (only negative empty test)
- ✗ pop_back: MISSING (only negative empty test)
- ✗ size: MISSING
- ✓ empty deque negative tests: partial (pop_front, peek_back only)
- ✗ invalid args negative tests: MISSING

### ul (test_ec_deque_ul.c)
- ✓ init: positive (implied)
- ✓ free: positive (implied)
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive
- ✓ pop_back: positive + negative (empty)
- ✗ peek_front: MISSING
- ✗ peek_back: MISSING
- ✗ size: MISSING
- ✓ empty deque negative tests: partial (pop_back only)
- ✗ invalid args negative tests: MISSING

### ull (test_ec_deque_ull.c)
- ✓ init: positive (implied)
- ✓ free: positive (implied)
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive
- ✓ pop_back: positive
- ✓ peek_front: positive
- ✓ peek_back: positive
- ✗ size: MISSING
- ✗ empty deque negative tests: MISSING
- ✗ invalid args negative tests: MISSING

### u32 (test_ec_deque_u32.c)
- ✓ init: positive (implied)
- ✓ free: positive (implied)
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive
- ✗ pop_back: MISSING
- ✗ peek_front: MISSING
- ✗ peek_back: MISSING
- ✗ size: MISSING
- ✗ empty deque negative tests: MISSING
- ✗ invalid args negative tests: MISSING

### u64 (test_ec_deque_u64.c)
- ✓ init: positive (implied)
- ✓ free: positive (implied)
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive
- ✓ pop_back: positive
- ✓ peek_front: positive
- ✓ peek_back: positive
- ✗ size: MISSING
- ✓ empty deque negative tests: partial (peek_front only)
- ✗ invalid args negative tests: MISSING

### double (test_ec_deque_double.c)
- ✓ init: positive
- ✓ free: positive
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive
- ✓ pop_back: positive
- ✓ peek_front: positive
- ✓ peek_back: positive
- ✓ size: positive
- ✓ empty deque negative tests: all pop/peek methods
- ✗ invalid args negative tests: MISSING

### str (test_ec_deque_str.c)
- ✓ init: positive
- ✓ free: positive
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive
- ✓ pop_back: positive
- ✓ peek_front: positive
- ✓ peek_back: positive
- ✓ size: positive (implied in push_front_back_pos)
- ✓ empty deque negative tests: all pop/peek methods
- ✓ invalid args negative tests: all pop/peek methods

### ptr (test_ec_deque_ptr.c)
- ✓ init: positive + negative (NULL)
- ✓ free: positive
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive
- ✓ pop_back: positive
- ✓ peek_front: positive + negative (invalid args)
- ✓ peek_back: positive
- ✗ size: MISSING
- ✓ empty deque negative tests: partial (pop_front, peek_back)
- ✓ invalid args negative tests: partial (init, pop_front, peek_front)

### char (test_ec_deque_char.c) - Custom type, not in header
- ✓ init: positive
- ✓ free: positive
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive
- ✓ pop_back: positive
- ✓ peek_front: positive (via peek_pop_sequence)
- ✓ peek_back: positive (via peek_pop_sequence)
- ✓ size: positive
- ✓ empty deque negative tests: all pop/peek methods
- ✗ invalid args negative tests: MISSING

### struct (test_ec_deque_struct.c) - Custom type, not in header
- ✓ init: positive (implied)
- ✓ free: positive (implied)
- ✓ push_front: positive
- ✓ push_back: positive
- ✓ pop_front: positive
- ✓ pop_back: positive
- ✓ peek_front: positive
- ✓ peek_back: positive
- ✗ size: MISSING
- ✓ empty deque negative tests: partial (pop_front, peek_front)
- ✓ invalid args negative tests: partial (pop_back, peek_back)

## Summary of Missing Tests

### Methods Missing Positive Tests:
- **long**: peek_front
- **ll**: push_front, pop_front, pop_back, size
- **ul**: peek_front, peek_back, size
- **ull**: size
- **u32**: pop_back, peek_front, peek_back, size
- **u64**: size
- **ptr**: size
- **struct**: size

### Methods Missing Negative Tests (empty deque):
- **long**: all pop/peek methods
- **ll**: peek_front, peek_back (pop_front/back only tested empty)
- **ul**: pop_front, peek_front, peek_back
- **ull**: all pop/peek methods
- **u32**: all pop/peek methods
- **u64**: pop_front, pop_back, peek_back

### Methods Missing Negative Tests (invalid arguments):
- **long**: init (NULL), pop_front/back (NULL deque/out), peek_front/back (NULL deque/out)
- **ll**: init (NULL), pop_front/back (NULL deque/out), peek_front/back (NULL deque/out)
- **ul**: init (NULL), pop_front/back (NULL deque/out), peek_front/back (NULL deque/out)
- **ull**: init (NULL), pop_front/back (NULL deque/out), peek_front/back (NULL deque/out)
- **u32**: init (NULL), pop_front/back (NULL deque/out), peek_front/back (NULL deque/out)
- **u64**: init (NULL), pop_front/back (NULL deque/out), peek_front/back (NULL deque/out)
- **double**: init (NULL), pop_front/back (NULL deque/out), peek_front/back (NULL deque/out)
- **char**: init (NULL), pop_front/back (NULL deque/out), peek_front/back (NULL deque/out)

