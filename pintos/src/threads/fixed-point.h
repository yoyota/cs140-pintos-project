// p = 17, q = 14 and f = 1 << 14
int scaling_factor = (1 << 14);
#define itof(n) (n * scaling_factor)
#define ftoi(f) (f / scaling_factor)
#define ff_div(x, y) ((int64_t)x * scaling_factor / y)
#define ff_mul(x, y) ((int64_t)x * y / scaling_factor)
