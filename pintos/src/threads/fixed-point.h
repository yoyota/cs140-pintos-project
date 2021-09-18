// p = 17, q = 14 and f = 1 << 14
#define SCALING_FACOTR (1 << 14)
#define itof(n) (n * SCALING_FACOTR)
#define ftoi(f) (f / SCALING_FACOTR)
#define ff_div(x, y) ((int64_t)x * SCALING_FACOTR / y)
#define ff_mul(x, y) ((int64_t)x * y / SCALING_FACOTR)
