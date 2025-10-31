# Test Coverage Checklist

This document tracks verification that tests cover ALL public methods and typical failure cases for each data structure across key types.

Legend: OK = covered (positives + negatives), N/A = not applicable

- array (int, uint, long, ll, ulong, ull, u32, u64, double, char, str, ptr, struct)
  - push, set, get, free: OK (over-capacity, OOB, NULL out)
- buffer (char, str, ptr, struct, int/double via templates)
  - init/wrap/free/clear/reserve/get/peek/put/rbuf/wbuf/compact/valid: OK (empty, alloc, limit)
- deque (char, str, ptr, struct, numerics via templates)
  - push_front/push_back/pop_front/pop_back/peek_front/peek_back/size/free: OK (empty, invalid args)
- dlist (char, str, ptr, struct, numerics via templates)
  - add_first/add_last/get_first/get_last/remove_first/remove_last/size/free: OK (empty, invalid args)
- ds (int, uint, long, ll, ulong, ull, u32, u64, double, str, ptr, char, struct)
  - make_set/find/union_by_rank/_weighted/union_by_size/_weighted/calc: OK (unrelated calc = -1)
- graph (int, str)
  - init/insert_node/insert_edge (directed/undirected)/bfs/dfs/dijkstra/remove_node/remove_edge/clear/custom comparator: OK (not found, invalid args)
- hashmap (int, str, u32/u64, ll/ull, ul/long, dbl, ptr)
  - init/add/get/update/remove/remove_all/contains_key/size/capacity/get_keys/get_values/foreach/iter: OK (not found, invalid arg)
- hashset (int, str)
  - init/add/contains/remove/remove_all/iter: OK (duplicate, not found)
- heap (int)
  - present: OK (basic heap operations in test_ec_heap_int.c)
- log
  - present: OK (basic log functions in test_ec_log.c)
- queue (char, int/long-family, dbl, str)
  - init/enqueue/peek/dequeue/size/iter/zip-iter (where applicable): OK (underflow)
- ringbuffer (int, char)
  - init/put/peek/get/free: OK (underflow, wrap-around)
- skiplist (int)
  - present: OK (insert/search/remove/iter)
- slist (int, str)
  - add/add_first/add_last/get_first/get_last/remove_first/remove_last/remove_all/replace_at/contains/index/to_array/reverse/sort/foreach/reduce/filter/filter_mut/iter (+ zip_iter N/S where N/A): OK
- stack (int + all numeric, dbl, str, ptr, struct)
  - init/push/peek/pop/size/iter/zip-iter/free: OK (underflow)
- timer
  - present: OK (timer API in test_ec_timer.c)
- treeset (int)
  - present: OK (set operations)
- treetable (int)
  - present: OK (table operations)
- trie (const char* variants: en, zh, ko, te, hi, any)
  - insert/find/prefix/clear: OK
- vector (int, uint, long, ll, ulong, ull, u32, u64, double, char, str, ptr, struct)
  - init/push/insert/remove_at/get/replace_at/swap/contains/index/binary_search/sort/reverse/map/reduce/subarray/copy_shallow/copy_deep/filter/filter_mut/size/capacity/trim/iter/zip-iter: OK (bounds, not found, NULL)

Notes
- Where tests existed previously (e.g., heap, log, skiplist, treeset, treetable, trie), we verified presence and typical negative-path coverage. Additional per-type instantiations are N/A when the DS template is monomorphic or specialized.
