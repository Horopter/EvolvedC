#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../src/ec_deque.h"

#define TEST_ASSERT(cond)                                                            \
    do                                                                               \
    {                                                                                \
        if (!(cond))                                                                 \
        {                                                                            \
            fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            exit(3);                                                                 \
        }                                                                            \
    } while (0)

void test_deque_push_pop(void) {
    printf("Running test_deque_push_pop...\n");
    ec_deque_int_t deque;
    ec_status_t st = ec_deque_int_init(&deque, NULL);
    TEST_ASSERT(st == EC_OK);
    
    int a = 10, b = 20;
    deque.push_front(&deque, &a);
    deque.push_back(&deque, &b);
    TEST_ASSERT(deque.size_fn(&deque) == 2);
    
    int *front_val;
    deque.pop_front(&deque, (void **)&front_val);
    TEST_ASSERT(*front_val == 10);
    
    int *back_val;
    deque.pop_back(&deque, (void **)&back_val);
    TEST_ASSERT(*back_val == 20);
    
    TEST_ASSERT(deque.size_fn(&deque) == 0);
    deque.free_fn(&deque);
    printf("test_deque_push_pop passed!\n");
}

void test_deque_peek(void) {
    printf("Running test_deque_peek...\n");
    ec_deque_int_t deque;
    ec_status_t st = ec_deque_int_init(&deque, NULL);
    TEST_ASSERT(st == EC_OK);
    
    int a = 42;
    deque.push_front(&deque, &a);
    
    int *peek_val;
    deque.peek_front(&deque, (void **)&peek_val);
    TEST_ASSERT(*peek_val == 42);
    deque.peek_back(&deque, (void **)&peek_val);
    TEST_ASSERT(*peek_val == 42);
    
    deque.free_fn(&deque);
    printf("test_deque_peek passed!\n");
}

void test_deque_size(void) {
    printf("Running test_deque_size...\n");
    ec_deque_int_t deque;
    ec_status_t st = ec_deque_int_init(&deque, NULL);
    TEST_ASSERT(st == EC_OK);
    
    int a = 1, b = 2;
    TEST_ASSERT(deque.size_fn(&deque) == 0);
    
    deque.push_back(&deque, &a);
    TEST_ASSERT(deque.size_fn(&deque) == 1);
    deque.push_front(&deque, &b);
    TEST_ASSERT(deque.size_fn(&deque) == 2);
    
    deque.free_fn(&deque);
    printf("test_deque_size passed!\n");
}

int main(void) {
    printf("Starting EC_Deque tests...\n");
    test_deque_push_pop();
    test_deque_peek();
    test_deque_size();
    printf("All tests passed!\n");
    return 0;
}
