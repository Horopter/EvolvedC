#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
	int id;
	char tag;
} rec_t;
EC_BUF_DEF(rec_t, rec)

static void
test_init_pos(void)
{
	ec_buf_rec_t b;
	assert(ec_buf_rec_init(&b, 2, NULL));
	ec_buf_rec_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_rec_init(NULL, 2, NULL));
}

static void
test_put_growth_and_peek_get_pos(void)
{
	ec_buf_rec_t b;
	assert(ec_buf_rec_init(&b, 2, NULL));
	rec_t r1 = { 1, 'a' }, r2 = { 2, 'b' }, r3 = { 3, 'c' };
	assert(ec_buf_rec_put(&b, r1));
	assert(ec_buf_rec_put(&b, r2));
	assert(ec_buf_rec_put(&b, r3));
	rec_t out;
	assert(ec_buf_rec_peek(&b, &out) && out.id == 1 && out.tag == 'a');
	assert(ec_buf_rec_get(&b, &out) && out.id == 1);
	assert(ec_buf_rec_get(&b, &out) && out.id == 2);
	ec_buf_rec_compact(&b);
	assert(ec_buf_rec_get(&b, &out) && out.id == 3);
	ec_buf_rec_free(&b);
}

static void
test_empty_neg(void)
{
	ec_buf_rec_t b;
	assert(ec_buf_rec_init(&b, 1, NULL));
	rec_t out;
	assert(!ec_buf_rec_get(&b, &out));
	assert(!ec_buf_rec_peek(&b, &out));
	assert(!ec_buf_rec_valid(&b));
	ec_buf_rec_free(&b);
}

static void
test_wrap_pos(void)
{
	rec_t data[2] = { { 7, 'x' }, { 8, 'y' } };
	ec_buf_rec_t b;
	assert(ec_buf_rec_init(&b, 1, NULL));
	ec_buf_rec_wrap(&b, data, 2);
	rec_t out;
	assert(ec_buf_rec_get(&b, &out) && out.id == 7 && out.tag == 'x');
	assert(ec_buf_rec_get(&b, &out) && out.id == 8 && out.tag == 'y');
	ec_buf_rec_free(&b);
}

static void
test_clear_pos(void)
{
	ec_buf_rec_t b;
	assert(ec_buf_rec_init(&b, 2, NULL));
	rec_t r = { 1, 'a' };
	assert(ec_buf_rec_put(&b, r));
	assert(ec_buf_rec_size(&b) == 1);
	ec_buf_rec_clear(&b);
	assert(ec_buf_rec_size(&b) == 0);
	assert(ec_buf_rec_valid(&b));
	ec_buf_rec_free(&b);
}

static void
test_clear_neg(void)
{
	ec_buf_rec_clear(NULL);
}

static void
test_quota_neg(void)
{
	assert(ec_buf_rec_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_rec_size(NULL) == 0);
}

static void
test_reserve_neg(void)
{
	ec_buf_rec_t b;
	assert(ec_buf_rec_init(&b, 2, NULL));
	b.limit = 3;
	rec_t r1 = { 1, 'a' }, r2 = { 2, 'b' };
	assert(ec_buf_rec_put(&b, r1));
	assert(ec_buf_rec_put(&b, r2));
	bool ok = ec_buf_rec_reserve(&b, 5);
	assert(!ok);
	assert(!ec_buf_rec_valid(&b));
	ec_buf_rec_free(&b);
}

static void
test_put_neg_limit(void)
{
	ec_buf_rec_t b;
	assert(ec_buf_rec_init(&b, 1, NULL));
	b.limit = 2;
	rec_t r1 = { 1, 'a' }, r2 = { 2, 'b' }, r3 = { 3, 'c' };
	assert(ec_buf_rec_put(&b, r1));   /* wpos=1, cap=1, expands to 2 (OK) */
	assert(ec_buf_rec_put(&b, r2));   /* wpos=2, cap=2, would expand to 4, but limit=2, fails */
	bool ok = ec_buf_rec_put(&b, r3); /* This should fail - would need cap=4, but limit=2 */
	assert(!ok);
	assert(!ec_buf_rec_valid(&b));
	ec_buf_rec_free(&b);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_rec_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_rec_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_rec_compact(NULL);
}

static void
test_valid_pos(void)
{
	ec_buf_rec_t b;
	assert(ec_buf_rec_init(&b, 2, NULL));
	assert(ec_buf_rec_valid(&b));
	rec_t r = { 1, 'a' };
	assert(ec_buf_rec_put(&b, r));
	assert(ec_buf_rec_valid(&b));
	ec_buf_rec_free(&b);
}

static void
test_valid_neg(void)
{
	assert(!ec_buf_rec_valid(NULL));
}

int
main(void)
{
	printf("Starting ec_buffer_rec tests...\n");
	test_init_pos();
	test_init_neg();
	test_put_growth_and_peek_get_pos();
	test_put_neg_limit();
	test_empty_neg();
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
	printf("All ec_buffer_rec tests passed.\n");
	return 0;
}
