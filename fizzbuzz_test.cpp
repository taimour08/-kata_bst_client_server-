#include "gtest/gtest.h"
#include "../src/fizzbuzz.h" 

TEST(FizzBuzz, ReturnsNothingFor1) {
    EXPECT_EQ(fizzbuzz(1), "");  
}

