// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 Suyash Shirsat

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  pin_t ablepin;
  pin_t simulpin;
  pin_t outpin;
} chip_state_t;

static void pin_state_allocation(void* data, pin_t pin, uint32_t value) {
  chip_state_t* chip = (chip_state_t*) data;
  int able_val = pin_read(chip->ablepin);
  int simul_val = pin_read(chip->simulpin);

  if (able_val == 1) {
    pin_write(chip->outpin, simul_val);
  } else {
    pin_write(chip->outpin, 0); // disable output when not enabled
  }
}

void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));
  chip->ablepin  = pin_init("ABLE", INPUT);
  chip->simulpin = pin_init("SIMUL", INPUT);
  chip->outpin   = pin_init("OUTPIN", OUTPUT);

  // Watch both input pins for changes
  const pin_watch_config_t config = {
    .edge = BOTH,
    .pin_change = pin_state_allocation,
    .user_data = chip
  };

  pin_watch(chip->ablepin, &config);
  pin_watch(chip->simulpin, &config);

  printf("Hello from custom chip!\n");
}
