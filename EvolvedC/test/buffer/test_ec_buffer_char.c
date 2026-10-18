#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdio.h>

EC_BUF_DEF(char, ch)

static void
test_init_pos(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 2, NULL));
	ec_buf_ch_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_ch_init(NULL, 2, NULL));
}

static void
test_put_pos(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 2, NULL));
	assert(ec_buf_ch_put(&b, 'a'));
	assert(ec_buf_ch_put(&b, 'b'));
	assert(ec_buf_ch_put(&b, 'c'));
	ec_buf_ch_free(&b);
}

static void
test_peek_pos(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 2, NULL));
	assert(ec_buf_ch_put(&b, 'z'));
	char v;
	assert(ec_buf_ch_peek(&b, &v) && v == 'z');
	ec_buf_ch_free(&b);
}

static void
test_peek_neg_empty(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 2, NULL));
	char v;
	assert(!ec_buf_ch_peek(&b, &v));
	ec_buf_ch_free(&b);
}

static void
test_get_pos(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 2, NULL));
	assert(ec_buf_ch_put(&b, 'x'));
	assert(ec_buf_ch_put(&b, 'y'));
	char v;
	assert(ec_buf_ch_get(&b, &v) && v == 'x');
	assert(ec_buf_ch_get(&b, &v) && v == 'y');
	ec_buf_ch_free(&b);
}

static void
test_get_neg_empty(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 2, NULL));
	char v;
	assert(!ec_buf_ch_get(&b, &v));
	ec_buf_ch_free(&b);
}

static void
test_reserve_compact_size_quota(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 1, NULL));
	for (int i = 0; i < 5; i++)
		assert(ec_buf_ch_put(&b, (char)('a' + i)));
	assert(ec_buf_ch_size(&b) == 5);
	char v;
	assert(ec_buf_ch_get(&b, &v));
	ec_buf_ch_compact(&b);
	assert(ec_buf_ch_quota(&b) >= 0);
	ec_buf_ch_free(&b);
}

static void
test_wrap_pos(void)
{
	char data[2] = { 'm', 'n' };
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 1, NULL));
	ec_buf_ch_wrap(&b, data, 2);
	char v;
	assert(ec_buf_ch_get(&b, &v) && v == 'm');
	assert(ec_buf_ch_get(&b, &v) && v == 'n');
	ec_buf_ch_free(&b);
}

static void
test_wrap_and_valid_neg(void)
{
	char data[2] = { 'm', 'n' };
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 1, NULL));
	ec_buf_ch_wrap(&b, data, 2);
	char v;
	assert(ec_buf_ch_get(&b, &v) && v == 'm');
	assert(ec_buf_ch_get(&b, &v) && v == 'n');
	assert(!ec_buf_ch_get(&b, &v));
	assert(!ec_buf_ch_valid(&b));
	ec_buf_ch_free(&b);
}

static void
test_clear_pos(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 2, NULL));
	assert(ec_buf_ch_put(&b, 'a'));
	assert(ec_buf_ch_put(&b, 'b'));
	assert(ec_buf_ch_size(&b) == 2);
	ec_buf_ch_clear(&b);
	assert(ec_buf_ch_size(&b) == 0);
	assert(ec_buf_ch_valid(&b));
	ec_buf_ch_free(&b);
}

static void
test_clear_neg(void)
{
	ec_buf_ch_clear(NULL);
}

static void
test_quota_neg(void)
{
	assert(ec_buf_ch_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_ch_size(NULL) == 0);
}

static void
test_reserve_neg(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 2, NULL));
	b.limit = 3;
	assert(ec_buf_ch_put(&b, 'a'));
	assert(ec_buf_ch_put(&b, 'b'));
	bool ok = ec_buf_ch_reserve(&b, 5);
	assert(!ok);
	assert(!ec_buf_ch_valid(&b));
	ec_buf_ch_free(&b);
}

static void
test_put_neg_limit(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 1, NULL));
	b.limit = 2;
	assert(ec_buf_ch_put(&b, 'a'));   /* wpos=1, cap=1, expands to 2 (OK) */
	assert(ec_buf_ch_put(&b, 'b'));   /* wpos=2, cap=2, would expand to 4, but limit=2, fails */
	bool ok = ec_buf_ch_put(&b, 'c'); /* This should fail - would need cap=4, but limit=2 */
	assert(!ok);
	assert(!ec_buf_ch_valid(&b));
	ec_buf_ch_free(&b);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_ch_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_ch_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_ch_compact(NULL);
}

static void
test_valid_pos(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 2, NULL));
	assert(ec_buf_ch_valid(&b));
	assert(ec_buf_ch_put(&b, 'a'));
	assert(ec_buf_ch_valid(&b));
	ec_buf_ch_free(&b);
}

static void
test_valid_neg(void)
{
	assert(!ec_buf_ch_valid(NULL));
}

static void
test_rbuf_wbuf_pos(void)
{
	ec_buf_ch_t b;
	assert(ec_buf_ch_init(&b, 2, NULL));
	assert(ec_buf_ch_put(&b, 'q'));
	assert(ec_buf_ch_rbuf(&b) != NULL);
	assert(ec_buf_ch_wbuf(&b) != NULL);
	ec_buf_ch_free(&b);
}

int
main(void)
{
	printf("Starting ec_buffer_char tests...\n");
	test_init_pos();
	test_init_neg();
	test_put_pos();
	test_put_neg_limit();
	test_peek_pos();
	test_peek_neg_empty();
	test_get_pos();
	test_get_neg_empty();
	test_reserve_compact_size_quota();
	test_reserve_neg();
	test_wrap_pos();
	test_wrap_and_valid_neg();
	test_clear_pos();
	test_clear_neg();
	test_quota_neg();
	test_size_neg();
	test_rbuf_wbuf_pos();
	test_rbuf_neg();
	test_wbuf_neg();
	test_compact_neg();
	test_valid_pos();
	test_valid_neg();
	printf("All ec_buffer_char tests passed.\n");
	return 0;
}
