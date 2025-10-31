#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../src/ec_deque.h"

EC_DEQUE_DEF(char, ch)

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
test_deque_char_push_pop(void* mp)
{
	stamp(); printf("test_deque_char_push_pop\n");
	ec_deque_ch_t d; assert(ec_deque_ch_init(&d, 4, mp) == EC_OK);
	/* push_back a,b and push_front z -> z a b */
	assert(d.push_back(&d, 'a') == EC_OK);
	assert(d.push_back(&d, 'b') == EC_OK);
	assert(d.push_front(&d, 'z') == EC_OK);
	char out;
	assert(d.pop_front(&d, &out) == EC_OK && out == 'z');
	assert(d.pop_back(&d, &out) == EC_OK && out == 'b');
	/* failures */
	for (size_t i = 0; i < 2; i++) d.pop_front(&d, &out);
	assert(d.pop_front(&d, &out) == EC_ERR_OUT_OF_BOUNDS);
	d.free_fn(&d);
	printf("pass: test_deque_char_push_pop\n");
}

int
main(void)
{
	printf("Starting ec_deque_char tests...\n");
	void* mp = NULL;
	test_deque_char_push_pop(mp);
	printf("All ec_deque_char tests passed.\n");
	return 0;
}
