#pragma once

// Originally adapted from: https://github.com/cmcqueen/simplerandom

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t cong;
} SimpleRandomCong_t;

size_t simplerandom_cong_seed_array(SimpleRandomCong_t* p_cong, const uint32_t* p_seeds,
                                    size_t num_seeds, bool mix_extras);
void simplerandom_cong_seed(SimpleRandomCong_t* p_cong, uint32_t seed);
void simplerandom_cong_mix(SimpleRandomCong_t* p_cong, const uint32_t* p_data, size_t num_data);
uint32_t simplerandom_cong_next(SimpleRandomCong_t* p_cong);
void simplerandom_cong_discard(SimpleRandomCong_t* p_cong, uintmax_t n);
