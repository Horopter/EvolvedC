#include "../../src/ec_hashmap.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void
stamp(void)
{
	time_t t = time(NULL);
	struct tm* m = localtime(&t);
	if (m)
		{
			char b[64];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		{
			printf("[timeerr] ");
		}
}

static void
test_hashmap_u64_basic(void)
{
	stamp();
	printf("test_hashmap_u64_basic\n");
	ec_hashmap_u64_t hm;
	ec_status_t st = ec_hashmap_u64_init(&hm, NULL);
	assert(st == EC_OK);

	uint64_t k1 = 1ULL << 40, v1 = 1111ULL;
	uint64_t k2 = 1ULL << 41, v2 = 2222ULL;
	assert(hm.add(&hm, &k1, &v1) == EC_OK);
	assert(hm.add(&hm, &k2, &v2) == EC_OK);

	uint64_t* out = NULL;
	assert(hm.get(&hm, &k1, &out) == EC_OK && *out == v1);
	free(out);
	assert(hm.get(&hm, &k2, &out) == EC_OK && *out == v2);
	free(out);

	assert(hm.contains_key(&hm, &k1) == EC_OK);
	assert(hm.remove(&hm, &k1) == EC_OK);
	assert(hm.contains_key(&hm, &k1) == EC_ERR_NOT_FOUND);

	hm.remove_all(&hm);
	printf("pass:test_hashmap_u64_basic\n");
}

static void
test_hashmap_u64_rehash_heavy(void)
{
	stamp();
	printf("test_hashmap_u64_rehash_heavy\n");
	ec_hashmap_u64_t hm;
	assert(ec_hashmap_u64_init(&hm, NULL) == EC_OK);

	const int N = 50000; /* stress scale without excessive runtime */
	for (int i = 0; i < N; i++)
		{
			uint64_t k = (uint64_t)i * 2654435761ULL;
			uint64_t v = (uint64_t)i + 123456789ULL;
			assert(hm.add(&hm, &k, &v) == EC_OK);
		}
	assert(hm.size_fn(&hm) == (size_t)N);

	for (int i = 0; i < N; i += 7919)
		{
			uint64_t k = (uint64_t)i * 2654435761ULL;
			assert(hm.remove(&hm, &k) == EC_OK);
		}

	hm.remove_all(&hm);
	printf("pass:test_hashmap_u64_rehash_heavy\n");
}

int
main(void)
{
	printf("Starting ec_hashmap_u64 tests...\n");
	test_hashmap_u64_basic();
	test_hashmap_u64_rehash_heavy();
	printf("All ec_hashmap_u64 tests passed.\n");
	return 0;
}
