#include "../src/ec_hashmap.h"
#include <assert.h>
#include <stdbool.h>
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

int
main(void)
{
	printf("Starting hashmap_int stress test...\n");
	stamp();
	printf("scale add/get/remove/rehash\n");

	ec_hashmap_int_t hm;
	ec_status_t st = ec_hashmap_int_init(&hm, NULL);
	assert(st == EC_OK);

	const int N = 100000; /* insert at scale */
	for (int i = 0; i < N; i++)
		{
			int k = i * 3 + 7;
			int v = i ^ 0x5a5a5a5a;
			assert(hm.add(&hm, &k, &v) == EC_OK);
		}
	assert(hm.size_fn(&hm) == (size_t)N);

	for (int i = 0; i < N; i += 101)
		{
			int k = i * 3 + 7;
			int* out = NULL;
			assert(hm.get(&hm, &k, &out) == EC_OK);
			free(out);
		}

	for (int i = 0; i < N; i += 97)
		{
			int k = i * 3 + 7;
			hm.remove(&hm, &k);
		}

	hm.remove_all(&hm);
	printf("hashmap_int stress test passed.\n");
	return 0;
}
