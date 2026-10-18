#include "../../src/ec_vector.h"
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
	printf("Starting vector_int stress test...\n");
	stamp();
	printf("push/insert/remove/growth\n");

	ec_vector_int_t vec;
	ec_status_t st = ec_vector_int_init(&vec, 16, NULL);
	assert(st == EC_OK);

	const int N = 200000; /* reasonably large; adjust if needed */
	for (int i = 0; i < N; i++)
		{
			assert(vec.push(&vec, i) == EC_OK);
		}
	assert((int)vec.size_fn(&vec) == N);

	for (int i = 0; i < 10000; i++)
		{
			int idx = (i * 97) % (vec.size_fn(&vec));
			ec_status_t s = vec.insert(&vec, idx, i);
			assert(s == EC_OK);
		}

	for (int i = 0; i < 10000; i++)
		{
			size_t sz = vec.size_fn(&vec);
			size_t idx = (size_t)((i * 123) % sz);
			assert(vec.remove_at(&vec, idx) == EC_OK);
		}

	/* spot check values */
	int v0, vM;
	assert(vec.get(&vec, 0, &v0) == EC_OK);
	assert(vec.get(&vec, vec.size_fn(&vec) - 1, &vM) == EC_OK);
	(void)v0;
	(void)vM;

	vec.free_fn(&vec);
	printf("vector_int stress test passed.\n");
	return 0;
}
