// q = 14 and f = 2 << 14
#define Q 14
#define F (1 << Q)
#define itof(n) (n * F)
#define ftoi(f) (f / F)
#define ff_div(x, y) ((int64_t)x * F / y)
#define ff_mul(x, y) ((int64_t)x * y / F)
