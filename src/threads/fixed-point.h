// p = 17, q = 14 and f = 1 << 14
#define SCALING_FACTOR (1 << 14)
#define itof(n) (n * SCALING_FACTOR)
#define ftoi(f) (f / SCALING_FACTOR)
#define ff_div(x, y) ((int64_t)x * SCALING_FACTOR / y)
#define ff_mul(x, y) ((int64_t)x * y / SCALING_FACTOR)
