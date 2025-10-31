#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../src/ec_dlist.h"

EC_DLIST_DEF(char, ch)

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
test_dlist_char_add_remove(void* mp)
{
	stamp(); printf("test_dlist_char_add_remove\n");
	ec_dlist_ch_t l; assert(ec_dlist_ch_init(&l, mp) == EC_OK);
	char a='a', b='b';
	assert(l.add_first(&l, &a) == EC_OK);
	assert(l.add_last(&l, &b) == EC_OK);
	void* out = NULL; assert(l.get_first(&l, &out) == EC_OK && *(char*)out == 'a');
	assert(l.remove_first(&l) == EC_OK);
	assert(l.remove_last(&l) == EC_OK);
	assert(l.size_fn(&l) == 0);
	l.free_fn(&l);
}

int
main(void)
{
	printf("Starting ec_dlist_char tests...\n");
	void* mp = NULL;
	test_dlist_char_add_remove(mp);
	printf("All ec_dlist_char tests passed.\n");
	return 0;
}
