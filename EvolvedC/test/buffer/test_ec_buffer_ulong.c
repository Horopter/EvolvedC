#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

/* Note: ec_buffer.h defines unsigned long as 'ul', not 'ulong' like ec_array.h
 * This test file uses ec_buf_ul_t to maintain consistency with the header
 * but follows the naming convention of array tests for consistency */

typedef ec_buf_ul_t ec_buf_ulong_t;
#define ec_buf_ulong_init ec_buf_ul_init
#define ec_buf_ulong_free ec_buf_ul_free
#define ec_buf_ulong_put ec_buf_ul_put
#define ec_buf_ulong_get ec_buf_ul_get
#define ec_buf_ulong_peek ec_buf_ul_peek
#define ec_buf_ulong_wrap ec_buf_ul_wrap
#define ec_buf_ulong_clear ec_buf_ul_clear
#define ec_buf_ulong_quota ec_buf_ul_quota
#define ec_buf_ulong_size ec_buf_ul_size
#define ec_buf_ulong_reserve ec_buf_ul_reserve
#define ec_buf_ulong_rbuf ec_buf_ul_rbuf
#define ec_buf_ulong_wbuf ec_buf_ul_wbuf
#define ec_buf_ulong_compact ec_buf_ul_compact
#define ec_buf_ulong_valid ec_buf_ul_valid

static void
test_init_pos(void)
{
	ec_buf_ulong_t b;
	assert(ec_buf_ulong_init(&b, 2, NULL));
	ec_buf_ulong_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_ulong_init(NULL, 2, NULL));
}

static void
test_put_growth(void)
{
	ec_buf_ulong_t b;
	assert(ec_buf_ulong_init(&b, 2, NULL));
	assert(ec_buf_ulong_put(&b, 10UL));
	assert(ec_buf_ulong_put(&b, 20UL));
	assert(ec_buf_ulong_put(&b, 30UL));
	ec_buf_ulong_free(&b);
}

static void
test_peek_get_sequence(void)
{
	ec_buf_ulong_t b;
	assert(ec_buf_ulong_init(&b, 2, NULL));
	assert(ec_buf_ulong_put(&b, 10UL));
	assert(ec_buf_ulong_put(&b, 20UL));
	unsigned long v;
	assert(ec_buf_ulong_peek(&b, &v) && v == 10UL);
	assert(ec_buf_ulong_get(&b, &v) && v == 10UL);
	assert(ec_buf_ulong_get(&b, &v) && v == 20UL);
	ec_buf_ulong_free(&b);
}

static void
test_empty_neg_and_compact(void)
{
	ec_buf_ulong_t b;
	assert(ec_buf_ulong_init(&b, 2, NULL));
	unsigned long v;
	assert(!ec_buf_ulong_peek(&b, &v));
	assert(!ec_buf_ulong_get(&b, &v));
	assert(ec_buf_ulong_put(&b, 30UL));
	ec_buf_ulong_compact(&b);
	assert(ec_buf_ulong_get(&b, &v) && v == 30UL);
	ec_buf_ulong_free(&b);
}

static void
test_wrap_pos(void)
{
	unsigned long data[2] = { 7UL, 8UL };
	ec_buf_ulong_t b;
	assert(ec_buf_ulong_init(&b, 1, NULL));
	ec_buf_ulong_wrap(&b, data, 2);
	unsigned long v;
	assert(ec_buf_ulong_get(&b, &v) && v == 7UL);
	assert(ec_buf_ulong_get(&b, &v) && v == 8UL);
	ec_buf_ulong_free(&b);
}

static void
test_clear_pos(void)
{
	ec_buf_ulong_t b;
	assert(ec_buf_ulong_init(&b, 2, NULL));
	assert(ec_buf_ulong_put(&b, 10UL));
	assert(ec_buf_ulong_put(&b, 20UL));
	assert(ec_buf_ulong_size(&b) == 2);
	ec_buf_ulong_clear(&b);
	assert(ec_buf_ulong_size(&b) == 0);
	assert(ec_buf_ulong_valid(&b));
	ec_buf_ulong_free(&b);
}

static void
test_clear_neg(void)
{
	ec_buf_ulong_clear(NULL);
}

static void
test_quota_neg(void)
{
	assert(ec_buf_ulong_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_ulong_size(NULL) == 0);
}

static void
test_reserve_neg(void)
{
	ec_buf_ulong_t b;
	assert(ec_buf_ulong_init(&b, 2, NULL));
	b.limit = 3;
	assert(ec_buf_ulong_put(&b, 10UL));
	assert(ec_buf_ulong_put(&b, 20UL));
	bool ok = ec_buf_ulong_reserve(&b, 5);
	assert(!ok);
	assert(!ec_buf_ulong_valid(&b));
	ec_buf_ulong_free(&b);
}

static void
test_put_neg_limit(void)
{
	ec_buf_ulong_t b;
	assert(ec_buf_ulong_init(&b, 1, NULL));
	b.limit = 2;
	assert(ec_buf_ulong_put(&b, 10UL));   /* wpos=1, cap=1, expands to 2 (OK) */
	assert(ec_buf_ulong_put(&b, 20UL));   /* wpos=2, cap=2, would expand to 4, but limit=2, fails */
	bool ok = ec_buf_ulong_put(&b, 30UL); /* This should fail - would need cap=4, but limit=2 */
	assert(!ok);
	assert(!ec_buf_ulong_valid(&b));
	ec_buf_ulong_free(&b);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_ulong_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_ulong_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_ulong_compact(NULL);
}

static void
test_valid_pos(void)
{
	ec_buf_ulong_t b;
	assert(ec_buf_ulong_init(&b, 2, NULL));
	assert(ec_buf_ulong_valid(&b));
	assert(ec_buf_ulong_put(&b, 10UL));
	assert(ec_buf_ulong_valid(&b));
	ec_buf_ulong_free(&b);
}

static void
test_valid_neg(void)
{
	assert(!ec_buf_ulong_valid(NULL));
}

int
main(void)
{
	printf("Starting ec_buffer_ulong tests...\n");
	test_init_pos();
	test_init_neg();
	test_put_growth();
	test_put_neg_limit();
	test_peek_get_sequence();
	test_empty_neg_and_compact();
	test_wrap_pos();
	test_clear_pos();
	test_clear_neg();
	test_quota_neg();
	test_size_neg();
	test_reserve_neg();
	test_rbuf_neg();
	test_wbuf_neg();
	test_compact_neg();
	test_valid_pos();
	test_valid_neg();
	printf("All ec_buffer_ulong tests passed.\n");
	return 0;
}
