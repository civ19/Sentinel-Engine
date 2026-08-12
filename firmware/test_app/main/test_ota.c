#include "unity.h"

int sum(int a, int b) {
    return a+b;
}

TEST_CASE("Sum function works with positive ints", "[math]") 
{
    int result = sum(10,5);
    TEST_ASSERT_EQUAL(15, result);
}