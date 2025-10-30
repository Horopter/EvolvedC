#include "../../src/ec_array.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct person_a_s
{
	int id;
	char name[16];
} person_a_t;
EC_ARRAY_DEF(person_a_t, person)

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
test_array_person_basic(void)
{
	stamp();
	printf("test_array_person_basic\n");
	ec_array_person_t a;
	assert(ec_array_person_init(&a, 2, NULL) == EC_OK);
	person_a_t p1 = { 1, "alice" };
	person_a_t p2 = { 2, "bob" };
	assert(a.push(&a, p1) == EC_OK);
	assert(a.push(&a, p2) == EC_OK);
	/* full */
	assert(a.push(&a, p2) == EC_ERR_MAX_CAPACITY);

	person_a_t out;
	assert(a.get(&a, 1, &out) == EC_OK && out.id == 2);
	person_a_t p3 = { 3, "carol" };
	assert(a.set(&a, 1, p3) == EC_OK);
	assert(a.get(&a, 1, &out) == EC_OK && out.id == 3);

	a.free(&a);
	printf("pass:test_array_person_basic\n");
}

int
main(void)
{
	printf("Starting ec_array_person tests...\n");
	test_array_person_basic();
	printf("All ec_array_person tests passed.\n");
	return 0;
}
