#include "../../src/ec_hashmap.h"
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
	printf("Starting ec_hashmap_ptr tests...\n");
	stamp();
	printf("hashmap_ptr_basic\n");

	ec_hashmap_ptr_t hm;
	ec_status_t st = ec_hashmap_ptr_init(&hm, NULL);
	assert(st == EC_OK);

	/* Pointers as keys/values: equality is pointer equality by design. */
	int a = 1, b = 2, c = 3;
	int va = 11, vb = 22, vc = 33;
	void *ka = &a, *kb = &b, *kc = &c;
	void *va_p = &va, *vb_p = &vb, *vc_p = &vc;

	assert(hm.add(&hm, &ka, &va_p) == EC_OK);
	assert(hm.add(&hm, &kb, &vb_p) == EC_OK);
	assert(hm.add(&hm, &kc, &vc_p) == EC_OK);

	void** out = NULL;
	assert(hm.get(&hm, &ka, &out) == EC_OK && *out == va_p);
	free(out);
	assert(hm.get(&hm, &kb, &out) == EC_OK && *out == vb_p);
	free(out);

	assert(hm.contains_key(&hm, &kc) == EC_OK);
	assert(hm.remove(&hm, &kc) == EC_OK);
	assert(hm.contains_key(&hm, &kc) == EC_ERR_NOT_FOUND);

	hm.remove_all(&hm);
	printf("All ec_hashmap_ptr tests passed.\n");
	return 0;
}
