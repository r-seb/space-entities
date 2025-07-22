#include "simple_random.h"

size_t simplerandom_cong_seed_array(SimpleRandomCong_t* p_cong, const uint32_t* p_seeds,
                                    size_t num_seeds, bool mix_extras)
{
    uint32_t seed = 0;
    size_t num_seeds_used = 0;

    if (num_seeds >= 1u && p_seeds != NULL) {
        seed = p_seeds[0];
        num_seeds_used = 1u;
    }
    simplerandom_cong_seed(p_cong, seed);

    if (mix_extras && p_seeds != NULL) {
        simplerandom_cong_mix(p_cong, p_seeds + num_seeds_used, num_seeds - num_seeds_used);
        num_seeds_used = num_seeds;
    }
    return num_seeds_used;
}

void simplerandom_cong_seed(SimpleRandomCong_t* p_cong, uint32_t seed)
{
    p_cong->cong = seed;
    /* No sanitize is needed because for Cong, all state values are valid. */
}

uint32_t simplerandom_cong_next(SimpleRandomCong_t* p_cong)
{
    uint32_t cong;

    cong = UINT32_C(69069) * p_cong->cong + 12345u;
    p_cong->cong = cong;

    return cong;
}

void simplerandom_cong_mix(SimpleRandomCong_t* p_cong, const uint32_t* p_data, size_t num_data)
{
    if (p_data != NULL) {
        while (num_data) {
            --num_data;
            p_cong->cong ^= *p_data++;
            simplerandom_cong_next(p_cong);
        }
    }
}
