#include <gtest/gtest.h>

#include "stl_construct.h"

namespace forgedstl {

TEST(ConstructDestroyTest, CommonType) {
    struct Temp {
        Temp(int x) : i(x) { }
        int i;
    };

    Temp* p = (Temp*)malloc(sizeof(Temp));
    construct(p, 10);
    EXPECT_EQ(10, p->i);
    destroy(p);
    free(p);

    Temp* pa = (Temp*)malloc(10 * sizeof(Temp));
    for (int i = 0; i < 10; ++i) {
        construct(pa + i, i);
    }
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(i, (pa + i)->i);
    }
    destroy(pa, pa + 10);
    free(pa);
}

TEST(ConstructDestroyTest, PODType) {
    double* pd = (double*)malloc(10 * sizeof(double));
    for (int i = 0; i < 10; ++i) {
        construct(pd + i, (double)i);
    }
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ((double)i, *(pd + i));
    }
    destroy(pd, pd + 10);
    free(pd);
}

TEST(ConstructDestroyTest, Char) {
    char* pc = (char*)malloc(36 * sizeof(char));
    for (int i = 0; i < 36; ++i) {
        construct(pc + i, 'a' + i);
    }

    for (int i = 0; i < 36; ++i) {
        EXPECT_EQ(char('a' + i), *(pc + i));
    }
    destroy(pc, pc + 36);
    free(pc);
}

} // forgedstl