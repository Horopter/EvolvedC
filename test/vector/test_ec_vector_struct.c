#include "../../src/ec_vector.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct person_s
{
	int id;
	char name[32];
} person_t;

/* Instantiate vector for person_t */
EC_VECTOR_DEF(person_t, person)

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

static int
cmp_person_by_id(const void* a, const void* b)
{
	const person_t* pa = (const person_t*)a;
	const person_t* pb = (const person_t*)b;
	if (pa->id < pb->id)
		return -1;
	if (pa->id > pb->id)
		return 1;
	return 0;
}

static void
uppercase_name(person_t* p)
{
	for (size_t i = 0; i < sizeof(p->name) && p->name[i]; i++)
		{
			if (p->name[i] >= 'a' && p->name[i] <= 'z')
				p->name[i] -= 32;
		}
}

static void
reduce_concat_names(person_t* acc, person_t* elem, person_t* out)
{
	(void)acc; /* concatenate into out's name, keep id */
	out->id = elem->id;
	strncat(out->name, elem->name, sizeof(out->name) - strlen(out->name) - 1);
}

static bool
is_even_id(const person_t p)
{
	return (p.id % 2) == 0;
}

static person_t
copy_person(person_t* src)
{
	person_t c = *src;
	return c;
}

static void
test_vector_person_end_to_end(void)
{
	stamp();
	printf("Running test: vector_person_end_to_end\n");

	ec_vector_person_t vec;
	ec_status_t st = ec_vector_person_init(&vec, 8, NULL);
	assert(st == EC_OK);

	person_t a = { 3, "alice" };
	person_t b = { 1, "bob" };
	person_t c = { 2, "carol" };

	assert(vec.push(&vec, a) == EC_OK);
	assert(vec.push(&vec, b) == EC_OK);
	assert(vec.insert(&vec, 1, c) == EC_OK);

	person_t out;
	assert(vec.get(&vec, 0, &out) == EC_OK && out.id == 3);
	assert(vec.get(&vec, 1, &out) == EC_OK && out.id == 2);

	vec.sort(&vec, cmp_person_by_id);
	assert(vec.get(&vec, 0, &out) == EC_OK && out.id == 1);
	assert(vec.get(&vec, 2, &out) == EC_OK && out.id == 3);

	vec.map(&vec, uppercase_name);
	assert(vec.get(&vec, 0, &out) == EC_OK && strcmp(out.name, "BOB") == 0);

	ec_vector_person_t filtered;
	st = vec.filter(&vec, is_even_id, &filtered);
	assert(st == EC_OK);
	for (size_t i = 0; i < filtered.size_fn(&filtered); i++)
		{
			person_t p;
			assert(filtered.get(&filtered, i, &p) == EC_OK);
			assert((p.id % 2) == 0);
		}

	ec_vector_person_t deep;
	st = vec.copy_deep(&vec, copy_person, &deep);
	assert(st == EC_OK && deep.size_fn(&deep) == vec.size_fn(&vec));

	filtered.free_fn(&filtered);
	deep.free_fn(&deep);
	vec.free_fn(&vec);
	printf("Passed test: vector_person_end_to_end\n");
}

int
main(void)
{
	printf("Starting ec_vector_person tests...\n");
	test_vector_person_end_to_end();
	printf("All ec_vector_person tests passed.\n");
	return 0;
}
