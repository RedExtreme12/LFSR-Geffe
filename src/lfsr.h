#pragma once

#include <stdint.h>

typedef	uint64_t lfsr_data_t;

typedef struct {
    lfsr_data_t data,
        polynomial,
        mask,
        seed,
        degree;
} lfsr_t;

void          GLFSR_init(lfsr_t*, lfsr_data_t, lfsr_data_t, lfsr_data_t);
bool GLFSR_next(lfsr_t*);
