/*
 * test_ec_array.c
 *
 * Unit tests for ec_array.h. This file works whether or not
 * flags such as EC_USE_MEMPOOL and EC_THREADSAFE are defined.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> /* For timestamp functions */

#include "../../src/ec_array.h"

#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

/*---------------------------------------------------------------------------
  Helper Function: Print Timestamp
  Prints the current date/time in a formatted string.
---------------------------------------------------------------------------*/
void
print_timestamp(void)
{
	time_t now = time(NULL);
	struct tm* local = localtime(&now);
	char buffer[64];
	if (local)
		{
			strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", local);
			printf("%s ", buffer);
		}
	else
		{
			printf("[timestamp error] ");
		}
}

/*---------------------------------------------------------------------------
  Custom Data Type
---------------------------------------------------------------------------*/
typedef struct
{
	int x;
	int y;
} Point;

/* Declare an ec_array for Point objects */
EC_ARRAY_DEF(Point, point)

/*---------------------------------------------------------------------------
  Test Functions - each accepts a void *mempool argument.
  When mempool support is disabled, mempool will be NULL.
---------------------------------------------------------------------------*/

/* Test ec_array_int */
void
test_int_array(void* mempool)
{
	print_timestamp();
	printf("Running test: ec_array_int\n");
	ec_array_int_t arr;
	size_t capacity = 5;
	ec_status_t status = ec_array_int_init(&arr, capacity, mempool);
	assert(status == EC_OK);

	for (int i = 0; i < (int)capacity; i++)
		{
			status = arr.push(&arr, i + 1);
			assert(status == EC_OK);
		}
	status = arr.push(&arr, 99);
	assert(status == EC_ERR_MAX_CAPACITY);

	for (size_t i = 0; i < capacity; i++)
		{
			int value = 0;
			status = arr.get(&arr, i, &value);
			assert(status == EC_OK);
			assert(value == (int)(i + 1));
		}

	status = arr.set(&arr, 2, 42);
	assert(status == EC_OK);
	{
		int value = 0;
		status = arr.get(&arr, 2, &value);
		assert(status == EC_OK);
		assert(value == 42);
	}
	/* Out-of-bound checks */
	{
		int value = 0;
		status = arr.get(&arr, capacity, &value);
		assert(status == EC_ERR_OUT_OF_BOUNDS);
		status = arr.set(&arr, capacity, 55);
		assert(status == EC_ERR_OUT_OF_BOUNDS);
	}
	arr.free(&arr);
	printf("Passed test: ec_array_int\n\n");
}

/* Test ec_array_uint (unsigned int) */
void
test_uint_array(void* mempool)
{
	print_timestamp();
	printf("Running test: ec_array_uint\n");
	ec_array_uint_t arr;
	size_t capacity = 4;
	ec_status_t status = ec_array_uint_init(&arr, capacity, mempool);
	assert(status == EC_OK);

	for (unsigned int i = 0; i < capacity; i++)
		{
			status = arr.push(&arr, i * 10);
			assert(status == EC_OK);
		}
	status = arr.push(&arr, 999);
	assert(status == EC_ERR_MAX_CAPACITY);

	for (size_t i = 0; i < capacity; i++)
		{
			unsigned int value = 0;
			status = arr.get(&arr, i, &value);
			assert(status == EC_OK);
			assert(value == (unsigned int)(i * 10));
		}
	arr.free(&arr);
	printf("Passed test: ec_array_uint\n\n");
}

/* Test ec_array_long and ec_array_ll (long & long long) */
void
test_long_arrays(void* mempool)
{
	print_timestamp();
	printf("Running test: ec_array_long and ec_array_ll\n");

	{
		ec_array_long_t arr;
		size_t capacity = 3;
		ec_status_t status = ec_array_long_init(&arr, capacity, mempool);
		assert(status == EC_OK);
		for (long i = 0; i < (long)capacity; i++)
			{
				status = arr.push(&arr, i + 100L);
				assert(status == EC_OK);
			}
		long value = 0;
		status = arr.get(&arr, 1, &value);
		assert(status == EC_OK);
		assert(value == 101L);
		arr.free(&arr);
	}

	{
		ec_array_ll_t arr;
		size_t capacity = 3;
		ec_status_t status = ec_array_ll_init(&arr, capacity, mempool);
		assert(status == EC_OK);
		for (long long i = 0; i < (long long)capacity; i++)
			{
				status = arr.push(&arr, i + 1000LL);
				assert(status == EC_OK);
			}
		long long value = 0;
		status = arr.get(&arr, 2, &value);
		assert(status == EC_OK);
		assert(value == 1002LL);
		arr.free(&arr);
	}
	printf("Passed test: ec_array_long and ec_array_ll\n\n");
}

/* Test ec_array_ulong and ec_array_ull */
void
test_unsigned_long_arrays(void* mempool)
{
	print_timestamp();
	printf("Running test: ec_array_ulong and ec_array_ull\n");

	{
		ec_array_ulong_t arr;
		size_t capacity = 3;
		ec_status_t status = ec_array_ulong_init(&arr, capacity, mempool);
		assert(status == EC_OK);
		for (unsigned long i = 0; i < capacity; i++)
			{
				status = arr.push(&arr, i + 500UL);
				assert(status == EC_OK);
			}
		unsigned long value = 0;
		status = arr.get(&arr, 0, &value);
		assert(status == EC_OK);
		assert(value == 500UL);
		arr.free(&arr);
	}

	{
		ec_array_ull_t arr;
		size_t capacity = 3;
		ec_status_t status = ec_array_ull_init(&arr, capacity, mempool);
		assert(status == EC_OK);
		for (unsigned long long i = 0; i < capacity; i++)
			{
				status = arr.push(&arr, i + 1000ULL);
				assert(status == EC_OK);
			}
		unsigned long long value = 0;
		status = arr.get(&arr, 1, &value);
		assert(status == EC_OK);
		assert(value == 1001ULL);
		arr.free(&arr);
	}
	printf("Passed test: ec_array_ulong and ec_array_ull\n\n");
}

/* Test ec_array_u32 and ec_array_u64 */
void
test_fixed_width_uint_arrays(void* mempool)
{
	print_timestamp();
	printf("Running test: ec_array_u32 and ec_array_u64\n");

	{
		ec_array_u32_t arr;
		size_t capacity = 4;
		ec_status_t status = ec_array_u32_init(&arr, capacity, mempool);
		assert(status == EC_OK);
		for (uint32_t i = 0; i < capacity; i++)
			{
				status = arr.push(&arr, i + 200U);
				assert(status == EC_OK);
			}
		uint32_t value = 0;
		status = arr.get(&arr, 3, &value);
		assert(status == EC_OK);
		assert(value == 203U);
		arr.free(&arr);
	}

	{
		ec_array_u64_t arr;
		size_t capacity = 4;
		ec_status_t status = ec_array_u64_init(&arr, capacity, mempool);
		assert(status == EC_OK);
		for (uint64_t i = 0; i < capacity; i++)
			{
				status = arr.push(&arr, i + 300ULL);
				assert(status == EC_OK);
			}
		uint64_t value = 0;
		status = arr.get(&arr, 2, &value);
		assert(status == EC_OK);
		assert(value == 302ULL);
		arr.free(&arr);
	}
	printf("Passed test: ec_array_u32 and ec_array_u64\n\n");
}

/* Test ec_array_double */
void
test_double_array(void* mempool)
{
	print_timestamp();
	printf("Running test: ec_array_double\n");
	ec_array_double_t arr;
	size_t capacity = 5;
	ec_status_t status = ec_array_double_init(&arr, capacity, mempool);
	assert(status == EC_OK);
	for (int i = 0; i < (int)capacity; i++)
		{
			double val = i * 1.1;
			status = arr.push(&arr, val);
			assert(status == EC_OK);
		}
	double value = 0.0;
	status = arr.get(&arr, 3, &value);
	assert(status == EC_OK);
	assert(value == 3 * 1.1);
	arr.free(&arr);
	printf("Passed test: ec_array_double\n\n");
}

/* Test ec_array_str (const char *) */
void
test_str_array(void* mempool)
{
	print_timestamp();
	printf("Running test: ec_array_str\n");
	ec_array_str_t arr;
	size_t capacity = 3;
	ec_status_t status = ec_array_str_init(&arr, capacity, mempool);
	assert(status == EC_OK);
	const char* strings[] = { "hello", "world", "ec_array" };
	for (size_t i = 0; i < capacity; i++)
		{
			status = arr.push(&arr, strings[i]);
			assert(status == EC_OK);
		}
	const char* value = NULL;
	status = arr.get(&arr, 1, (const char**)&value);
	assert(status == EC_OK);
	assert(strcmp(value, "world") == 0);
	arr.free(&arr);
	printf("Passed test: ec_array_str\n\n");
}

/* Test ec_array_ptr (void *) */
void
test_ptr_array(void* mempool)
{
	print_timestamp();
	printf("Running test: ec_array_ptr\n");
	ec_array_ptr_t arr;
	size_t capacity = 3;
	ec_status_t status = ec_array_ptr_init(&arr, capacity, mempool);
	assert(status == EC_OK);
	int a = 10, b = 20, c = 30;
	status = arr.push(&arr, &a);
	assert(status == EC_OK);
	status = arr.push(&arr, &b);
	assert(status == EC_OK);
	status = arr.push(&arr, &c);
	assert(status == EC_OK);
	int* ptr = NULL;
	status = arr.get(&arr, 2, (void**)&ptr);
	assert(status == EC_OK);
	assert(*ptr == 30);
	arr.free(&arr);
	printf("Passed test: ec_array_ptr\n\n");
}

/* Test ec_array_point (custom type Point) */
void
test_custom_array(void* mempool)
{
	print_timestamp();
	printf("Running test: ec_array_point (custom type Point)\n");
	ec_array_point_t arr;
	size_t capacity = 4;
	ec_status_t status = ec_array_point_init(&arr, capacity, mempool);
	assert(status == EC_OK);
	for (size_t i = 0; i < capacity; i++)
		{
			Point pt = { .x = (int)i, .y = (int)(i * 10) };
			status = arr.push(&arr, pt);
			assert(status == EC_OK);
		}
	Point p;
	status = arr.get(&arr, 2, &p);
	assert(status == EC_OK);
	assert(p.x == 2 && p.y == 20);
	Point newPt = { .x = 100, .y = 200 };
	status = arr.set(&arr, 1, newPt);
	assert(status == EC_OK);
	status = arr.get(&arr, 1, &p);
	assert(status == EC_OK);
	assert(p.x == 100 && p.y == 200);
	arr.free(&arr);
	printf("Passed test: ec_array_point (custom type Point)\n\n");
}

/*---------------------------------------------------------------------------
  Main Function
---------------------------------------------------------------------------*/
int
main(void)
{
	printf("Starting ec_array unit tests...\n\n");

#ifdef EC_USE_MEMPOOL
	/* If mempool support is enabled, initialize a mempool. */
	ec_mempool_conf_t pool_conf = {
		.block_size = 64,
		.num_blocks = 100,
		.pool_type = EC_MEMPOOL_DYNAMIC,
		.thread_mode = EC_THREAD_UNSAFE, /* or EC_THREAD_MUTEX / EC_THREAD_SPINLOCK */
		.alignment = 8,
		.max_chunks = 0,
		.expansion_factor = 2.0f,
		.mem_alloc = NULL,
		.mem_realloc = NULL,
		.mem_free = NULL
	};
	ec_mempool_t pool;
	ec_status_t status = ec_mempool_init(&pool, &pool_conf);
	assert(status == EC_OK);
	void* mptr = &pool;
#else
	void* mptr = NULL;
#endif

	/* Run tests for all array types using the mempool pointer (or NULL) */
	test_int_array(mptr);
	test_uint_array(mptr);
	test_long_arrays(mptr);
	test_unsigned_long_arrays(mptr);
	test_fixed_width_uint_arrays(mptr);
	test_double_array(mptr);
	test_str_array(mptr);
	test_ptr_array(mptr);
	test_custom_array(mptr);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy(&pool);
#endif

	printf("All ec_array tests passed successfully.\n");
	return 0;
}
