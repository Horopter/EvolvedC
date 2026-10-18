#include "../../src/ec_hashset.h"
#include <assert.h>
#include <stdio.h>

static void
test_init_pos(void)
{
	ec_hashset_u64_t hs;
	assert(ec_hashset_u64_init(&hs, NULL) == EC_OK);
	assert(hs.capacity_fn(&hs) == 16);
	assert(hs.size_fn(&hs) == 0);
	hs.remove_all(&hs);
}

static void
test_add_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k1 = 1000ULL, k2 = 2000ULL;
	assert(hs.add(&hs, &k1) == EC_OK);
	assert(hs.add(&hs, &k2) == EC_OK);
	assert(hs.size_fn(&hs) == 2);
	hs.remove_all(&hs);
}

static void
test_add_duplicate_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k = 1000ULL;
	assert(hs.add(&hs, &k) == EC_OK);
	assert(hs.size_fn(&hs) == 1);
	assert(hs.add(&hs, &k) == EC_OK);
	assert(hs.size_fn(&hs) == 1);
	hs.remove_all(&hs);
}

static void
test_add_neg_null(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	assert(hs.add(NULL, NULL) == EC_ERR_INVALID_ARG);
	hs.remove_all(&hs);
}

static void
test_remove_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k1 = 1000ULL, k2 = 2000ULL, k3 = 3000ULL;
	hs.add(&hs, &k1);
	hs.add(&hs, &k2);
	hs.add(&hs, &k3);
	assert(hs.size_fn(&hs) == 3);
	assert(hs.remove(&hs, &k2) == EC_OK);
	assert(hs.size_fn(&hs) == 2);
	hs.remove_all(&hs);
}

static void
test_remove_neg_not_found(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k1 = 1000ULL, k2 = 2000ULL;
	hs.add(&hs, &k1);
	assert(hs.remove(&hs, &k2) == EC_ERR_NOT_FOUND);
	hs.remove_all(&hs);
}

static void
test_remove_neg_null(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k = 1000ULL;
	hs.add(&hs, &k);
	assert(hs.remove(NULL, &k) == EC_ERR_INVALID_ARG);
	hs.remove_all(&hs);
}

static void
test_remove_all_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k1 = 1000ULL, k2 = 2000ULL, k3 = 3000ULL;
	hs.add(&hs, &k1);
	hs.add(&hs, &k2);
	hs.add(&hs, &k3);
	assert(hs.size_fn(&hs) == 3);
	assert(hs.remove_all(&hs) == EC_OK);
	assert(hs.size_fn(&hs) == 0);
	hs.remove_all(&hs);
}

static void
test_contains_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k = 1000ULL;
	hs.add(&hs, &k);
	assert(hs.contains(&hs, &k) == EC_OK);
	hs.remove_all(&hs);
}

static void
test_contains_neg_not_found(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k1 = 1000ULL, k2 = 2000ULL;
	hs.add(&hs, &k1);
	assert(hs.contains(&hs, &k2) == EC_ERR_NOT_FOUND);
	hs.remove_all(&hs);
}

static void
test_contains_neg_null(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k = 1000ULL;
	hs.add(&hs, &k);
	assert(hs.contains(NULL, &k) == EC_ERR_INVALID_ARG);
	hs.remove_all(&hs);
}

static void
test_get_keys_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k1 = 1000ULL, k2 = 2000ULL, k3 = 3000ULL;
	hs.add(&hs, &k1);
	hs.add(&hs, &k2);
	hs.add(&hs, &k3);
	uint64_t* keys = NULL;
	size_t count = 0;
	assert(hs.get_keys(&hs, &keys, &count) == EC_OK);
	assert(count == 3);
	free(keys);
	hs.remove_all(&hs);
}

static void
test_get_keys_pos_empty(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t* keys = NULL;
	size_t count = 0;
	assert(hs.get_keys(&hs, &keys, &count) == EC_OK);
	assert(count == 0);
	assert(keys == NULL);
	hs.remove_all(&hs);
}

static void
test_get_keys_neg_null(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k = 1000ULL;
	hs.add(&hs, &k);
	uint64_t* keys = NULL;
	size_t count = 0;
	assert(hs.get_keys(NULL, &keys, &count) == EC_ERR_INVALID_ARG);
	hs.remove_all(&hs);
}

static void
foreach_key_u64(uint64_t k)
{
	(void)k;
}

static void
test_foreach_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k1 = 1000ULL, k2 = 2000ULL, k3 = 3000ULL;
	hs.add(&hs, &k1);
	hs.add(&hs, &k2);
	hs.add(&hs, &k3);
	assert(hs.foreach_key(&hs, foreach_key_u64) == EC_OK);
	hs.remove_all(&hs);
}

static void
test_foreach_neg_null(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k = 1000ULL;
	hs.add(&hs, &k);
	assert(hs.foreach_key(NULL, foreach_key_u64) == EC_ERR_INVALID_ARG);
	assert(hs.foreach_key(&hs, NULL) == EC_ERR_INVALID_ARG);
	hs.remove_all(&hs);
}

static void
test_iter_init_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	ec_hashset_u64_iter_t it;
	assert(hs.iter_init(&hs, &it) == EC_OK);
	hs.remove_all(&hs);
}

static void
test_iter_init_neg_null(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	ec_hashset_u64_iter_t it;
	assert(hs.iter_init(NULL, &it) == EC_ERR_INVALID_ARG);
	assert(hs.iter_init(&hs, NULL) == EC_ERR_INVALID_ARG);
	hs.remove_all(&hs);
}

static void
test_iter_next_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k1 = 1000ULL, k2 = 2000ULL, k3 = 3000ULL;
	hs.add(&hs, &k1);
	hs.add(&hs, &k2);
	hs.add(&hs, &k3);
	ec_hashset_u64_iter_t it;
	hs.iter_init(&hs, &it);
	uint64_t key;
	size_t count = 0;
	while (hs.iter_next(&it, &key) == EC_OK)
		count++;
	assert(count == 3);
	hs.remove_all(&hs);
}

static void
test_iter_next_neg_empty(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	ec_hashset_u64_iter_t it;
	hs.iter_init(&hs, &it);
	uint64_t key;
	assert(hs.iter_next(&it, &key) == EC_ERR_OUT_OF_BOUNDS);
	hs.remove_all(&hs);
}

static void
test_iter_next_neg_null(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k = 1000ULL;
	hs.add(&hs, &k);
	ec_hashset_u64_iter_t it;
	hs.iter_init(&hs, &it);
	uint64_t key;
	assert(hs.iter_next(NULL, &key) == EC_ERR_INVALID_ARG);
	assert(hs.iter_next(&it, NULL) == EC_ERR_INVALID_ARG);
	hs.remove_all(&hs);
}

static void
test_iter_remove_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	uint64_t k1 = 1000ULL, k2 = 2000ULL, k3 = 3000ULL;
	hs.add(&hs, &k1);
	hs.add(&hs, &k2);
	hs.add(&hs, &k3);
	ec_hashset_u64_iter_t it;
	hs.iter_init(&hs, &it);
	uint64_t key;
	hs.iter_next(&it, &key);
	assert(hs.iter_remove(&it) == EC_OK);
	assert(hs.size_fn(&hs) == 2);
	hs.remove_all(&hs);
}

static void
test_iter_remove_neg_null(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	ec_hashset_u64_iter_t it;
	hs.iter_init(&hs, &it);
	assert(hs.iter_remove(NULL) == EC_ERR_INVALID_ARG);
	assert(hs.iter_remove(&it) == EC_ERR_INVALID_ARG);
	hs.remove_all(&hs);
}

static void
test_size_capacity_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	assert(hs.size_fn(&hs) == 0);
	assert(hs.capacity_fn(&hs) == 16);
	uint64_t k = 1000ULL;
	hs.add(&hs, &k);
	assert(hs.size_fn(&hs) == 1);
	hs.remove_all(&hs);
}

static void
test_rehash_pos(void)
{
	ec_hashset_u64_t hs;
	ec_hashset_u64_init(&hs, NULL);
	size_t initial_capacity = hs.capacity_fn(&hs);
	size_t threshold = (size_t)(initial_capacity * 0.75);
	uint64_t keys[20];
	for (int i = 0; i < (int)(threshold + 2); i++)
		{
			keys[i] = (uint64_t)i;
			hs.add(&hs, &keys[i]);
		}
	size_t new_capacity = hs.capacity_fn(&hs);
	assert(new_capacity == initial_capacity * 2);
	for (int i = 0; i < (int)(threshold + 2); i++)
		assert(hs.contains(&hs, &keys[i]) == EC_OK);
	hs.remove_all(&hs);
}

int
main(void)
{
	printf("Starting ec_hashset_u64 tests...\n");
	test_init_pos();
	test_add_pos();
	test_add_duplicate_pos();
	test_add_neg_null();
	test_remove_pos();
	test_remove_neg_not_found();
	test_remove_neg_null();
	test_remove_all_pos();
	test_contains_pos();
	test_contains_neg_not_found();
	test_contains_neg_null();
	test_get_keys_pos();
	test_get_keys_pos_empty();
	test_get_keys_neg_null();
	test_foreach_pos();
	test_foreach_neg_null();
	test_iter_init_pos();
	test_iter_init_neg_null();
	test_iter_next_pos();
	test_iter_next_neg_empty();
	test_iter_next_neg_null();
	test_iter_remove_pos();
	test_iter_remove_neg_null();
	test_size_capacity_pos();
	test_rehash_pos();
	printf("All ec_hashset_u64 tests passed.\n");
	return 0;
}
