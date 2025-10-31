#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../src/ec_slist.h"

EC_SLIST_DEF(char, ch)

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
		printf("[timeerr] ");
}

void
test_slist_char_add_get_remove(void* mp)
{
	stamp(); printf("test_slist_char_add_get_remove\n");
	ec_slist_ch_t l; assert(ec_slist_ch_init(&l, mp) == EC_OK);
	char a='a', b='b', c='c';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add_first(&l, &c) == EC_OK);
	void* out = NULL; assert(l.get_first(&l, &out) == EC_OK && *(char*)out == 'c');
	assert(l.remove_fn(&l, &b) == EC_OK);
	assert(l.size_fn(&l) == 2);
	l.free_fn(&l);
}

int
main(void)
{
	printf("Starting ec_slist_char tests...\n");
	void* mp = NULL;
	test_slist_char_add_get_remove(mp);
	printf("All ec_slist_char tests passed.\n");
	return 0;
}
