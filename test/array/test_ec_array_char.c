#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../src/ec_array.h"

EC_ARRAY_DEF(char, ch)

void
test_array_char_basics(void)
{
	ec_array_ch_t a; assert(ec_array_ch_init(&a, 3) == EC_OK);
	/* set and get */
	assert(a.set(&a, 0, 'a') == EC_OK);
	assert(a.set(&a, 1, 'b') == EC_OK);
	assert(a.set(&a, 2, 'c') == EC_OK);
	char x;
	assert(a.get(&a, 0, &x) == EC_OK && x == 'a');
	assert(a.get(&a, 2, &x) == EC_OK && x == 'c');
	/* bounds */
	assert(a.set(&a, 3, 'z') == EC_ERR_OUT_OF_BOUNDS);
	assert(a.get(&a, 3, &x) == EC_ERR_OUT_OF_BOUNDS);
	a.free_fn(&a);
}

int
main(void)
{
	printf("Starting ec_array_char tests...\n");
	test_array_char_basics();
	printf("All ec_array_char tests passed.\n");
	return 0;
}
