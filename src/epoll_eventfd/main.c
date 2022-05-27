//
// Created by MasterG on 2020/5/19.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float randf() {
  return rand() / (float)RAND_MAX;
}

float monte_carlo_pi(int num) {
  float x, y, d, pi;
  int i, within = 0;
  for (i = 0; i < num; i++) {
    x = randf();
    y = randf();
    d = sqrtf(x * x + y * y);
    if (d <= 1.0f) {
      within++;
    }
  }

  pi = 4.0f * (float)within / (float)num;

  return pi;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  float pi = monte_carlo_pi(INT32_MAX);
  printf("pi: %f\n", pi);
  return 0;
}
