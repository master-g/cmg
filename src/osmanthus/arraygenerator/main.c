/*
 *  main.c
 *  arraygenerator
 *
 *  Created by Master.G on 13-7-25.
 *  Copyright (c) 2013 MED. All rights reserved.
 */

#include "evaluator/evalarrays.h"
#include "texas_utils.h"
#include <stdio.h>
#include <string.h>

#define DBGINFO_LEOPARDS
#define DBGINFO_STRAIGHT_FLUSHES
#define DBGINFO_FLUSHES
#define DBGINFO_STRAIGHTS
#define DBGINFO_PAIRS
#define DBGINFO_HIGHCARDS

#define ARRAY_WRITE_TO_FILE

#define FLUSHES_COUNT 7169
#define UNIQUE3_COUNT FLUSHES_COUNT
#define OTHERS_COUNT 68922

#define HASH_COUNT 256

extern void eval_init_deck(int *deck);

/*
 * ************************************************************
 * build standard evaluation array
 * ************************************************************
 */
short flushes[FLUSHES_COUNT];
short unique3[UNIQUE3_COUNT];
short others[OTHERS_COUNT];
short hash_others[HASH_COUNT];

void array_build(void) {
  int deck[52];
  int i = 0;
  int j = 0;
  int index;
  int q;
  int prime_product;
  int d1, d2, d3;
  int c1, c2, c3;
  int comb[13];

  /* init 52 deck cards */
  eval_init_deck(deck);

  memset(flushes, 0, sizeof(short) * FLUSHES_COUNT);
  memset(unique3, 0, sizeof(short) * UNIQUE3_COUNT);
  memset(others, 0, sizeof(short) * OTHERS_COUNT);

  /*
   * ************************************************************
   * Leopards
   * ************************************************************
   */
  printf("*** LEOPARDS ***\n");

  index = 1; /* entropy of leopard starts from 1 */
  for (i = 12; i >= 0; i--) {
    d1 = i;
    c1 = deck[d1];

    prime_product = eval_primes[i] * eval_primes[i] * eval_primes[i];

    others[prime_product] = index;
    index++;

#ifdef DBGINFO_LEOPARDS
    printf("[entropy: %4d | index: %5d]   ", index - 1, prime_product);
    print_rank(c1);
    printf("\n");
#endif
  }

  /*
   * ************************************************************
   * Straight flushes
   * ************************************************************
   */
  printf("*** STRAIGHT FLUSHES ***\n");

  index = 14; /* entropy of straight flushes starts from 14 */

  for (i = 13; i >= 2; i--) {
    d1 = (i + 10) % 13; /* map i to deck index */
    d2 = (i + 11) % 13;
    d3 = (i + 12) % 13;

    c1 = deck[d1]; /* get card */
    c2 = deck[d2];
    c3 = deck[d3];

    q = (c1 | c2 | c3) >> 16; /* rank OR result */
    flushes[q] = index;       /* record entropy result */
    index++;

#ifdef DBGINFO_STRAIGHT_FLUSHES
    printf("[entropy: %4d | index: %5d]   ", index - 1, q);
    print_rank(c3);
    printf(" ");
    print_rank(c2);
    printf(" ");
    print_rank(c1);
    printf("\n");
#endif
  }

  /*
   * ************************************************************
   * Flushes
   * ************************************************************
   */
  printf("*** FLUSHES ***\n");

  index = 26; /* entropy of flushes starts from 26 */

  /* setup initial combinations */
  for (i = 0; i < 13; i++)
    comb[i] = i;

  /* calculate combinations */
  do {
    d1 = (12 - comb[0]) % 13; /* map combination to deck index */
    d2 = (12 - comb[1]) % 13;
    d3 = (12 - comb[2]) % 13;

    c1 = deck[d1]; /* get card */
    c2 = deck[d2];
    c3 = deck[d3];

    q = (c1 | c2 | c3) >> 16; /* rank OR result */

    if (flushes[q] != 0) /* if the combination is straight-flush, skip */
      continue;

    flushes[q] = index;
    index++;

#ifdef DBGINFO_FLUSHES
    printf("[entropy: %4d | index: %5d]   ", index - 1, q);
    print_rank(c1);
    printf(" ");
    print_rank(c2);
    printf(" ");
    print_rank(c3);
    printf("\n");
#endif

  } while (next_comb(comb, 3, 13));

  /*
   * ************************************************************
   * Straights
   * ************************************************************
   */
  printf("*** STRAIGHTS ***\n");

  index = 300; /* entropy of flushes starts from 300 */

  for (i = 13; i >= 2; i--) {
    d1 = (i + 10) % 13; /* map i to deck index */
    d2 = (i + 11) % 13;
    d3 = (i + 12) % 13;

    c1 = deck[d1]; /* get card */
    c2 = deck[d2];
    c3 = deck[d3];

    q = (c1 | c2 | c3) >> 16; /* rank OR result */
    ;
    unique3[q] = index; /* record entropy result */
    index++;

#ifdef DBGINFO_STRAIGHTS
    printf("[entropy: %4d | index: %5d]   ", index - 1, q);
    print_rank(c3);
    printf(" ");
    print_rank(c2);
    printf(" ");
    print_rank(c1);
    printf("\n");
#endif
  }

  /*
   * ************************************************************
   * Pairs
   * ************************************************************
   */
  printf("*** PAIRS ***\n");

  index = 312; /* entropy of pairs starts from 312 */

  for (i = 12; i >= 0; i--) /* from A to 2 */
  {
    for (j = 12; j >= 0; j--) /* from A to 2 */
    {
      d1 = i;
      d2 = i;
      d3 = j;

      if (i == j) /* if the combination is leopard, skip */
        continue;

      c1 = deck[d1];
      c2 = deck[d2];
      c3 = deck[d3];

      prime_product = eval_primes[d1] * eval_primes[d2] * eval_primes[d3];

      others[prime_product] = index;
      index++;

#ifdef DBGINFO_PAIRS
      printf("[entropy: %4d | index: %5d]   ", index - 1, prime_product);
      print_rank(c1);
      printf(" ");
      print_rank(c2);
      printf(" ");
      print_rank(c3);
      printf("\n");
#endif
    }
  }

  /*
   * ************************************************************
   * High cards
   * ************************************************************
   */
  printf("*** HIGH CARDS ***\n");

  index = 468; /* entropy of pairs starts from 468 */

  /* setup initial combinations */
  for (i = 0; i < 13; i++)
    comb[i] = i;

  /* calculate combinations */
  do {
    d1 = (12 - comb[0]) % 13; /* map combination to deck index */
    d2 = (12 - comb[1]) % 13;
    d3 = (12 - comb[2]) % 13;

    c1 = deck[d1]; /* get card */
    c2 = deck[d2];
    c3 = deck[d3];

    q = (c1 | c2 | c3) >> 16; /* rank OR result */

    if (unique3[q] != 0) /* if the combination is straight, skip */
      continue;

    unique3[q] = index;
    index++;

#ifdef DBGINFO_HIGHCARDS
    printf("[entropy: %4d | index: %5d]   ", index - 1, q);
    print_rank(c1);
    printf(" ");
    print_rank(c2);
    printf(" ");
    print_rank(c3);
    printf("\n");
#endif

  } while (next_comb(comb, 3, 13));
}

/*
 * ************************************************************
 * compress OTHERS array via minimum perfect hashing
 * ************************************************************
 */

/* these arrays were generated by hashgenerator */
unsigned short mph_perf_tab[] = {
    0,   0,   183, 0,   220, 235, 0,  7,   0,   42, 22, 0,   113, 0,  7,   159,
    220, 125, 125, 184, 131, 231, 92, 145, 183, 0,  85, 113, 253, 22, 183, 17,
};

/* The hash function */
unsigned int mph_perf_s(unsigned int val) {
  unsigned int a, b, rsl;

  val += 0xf382e87a;
  val ^= (val >> 16);
  val += (val << 8);
  val ^= (val >> 4);
  b = (val >> 3) & 0x1f;
  a = (val + (val << 11)) >> 24;
  rsl = (a ^ mph_perf_tab[b]);

  return rsl;
}

/* generate hash key table */
void build_others_hash(void) {
  int i = 0;
  memset(hash_others, 0, sizeof(short) * HASH_COUNT);

  for (i = 0; i < OTHERS_COUNT; i++) {
    if (others[i] != 0) {
      hash_others[mph_perf_s(i)] = others[i];
    }
  }
}

/*
 * ************************************************************
 * format a array into a more human friendly form
 * ************************************************************
 */
void array_format(short array[], int length, FILE *fp) {
  int count = 0;
  int i = 0;
  int j = 0;

#ifdef ARRAY_WRITE_TO_FILE
  char buffer[1024];
#endif

  for (i = 0; i <= length / 16; i++) {
    for (j = 0; j < 16; j++) {
      if (array[i * 16 + j] != 0) {
        count++;
      }
      if (i * 16 + j < length) {
#ifdef ARRAY_WRITE_TO_FILE
        sprintf(buffer, "%4d,", array[i * 16 + j]);
        fputs(buffer, fp);
#else
        printf("%4d,", array[i * 16 + j]);
#endif
      }
    }
#ifdef ARRAY_WRITE_TO_FILE
    fputs("\n", fp);
#else
    printf("\n");
#endif
  }
}

/*
 * ************************************************************
 * dump array keys to text file for hashing
 * ************************************************************
 */
void array_dump(short array[], int length, const char *filename) {
#ifdef ARRAY_WRITE_TO_FILE

  int i = 0;
  char buffer[1024];
  FILE *fp;
  fp = fopen(filename, "w");

  for (i = 0; i < length; i++) {
    if (array[i] != 0) {
      sprintf(buffer, "%08x\n", i);
      fputs(buffer, fp);
    }
  }

  fclose(fp);

#endif
}

const char *flushes_array_desc = "unsigned short eval_flushes[] = {\n";
const char *unique3_array_desc = "unsigned short eval_unique3[] = {\n";
const char *others_array_desc = "unsigned short eval_others[] = {\n";
const char *hash_others_desc = "unsigned short eval_hash_others[] = {\n";
const char *array_end = "};\n";

int main(int argc, const char *argv[]) {
#ifdef ARRAY_WRITE_TO_FILE
  FILE *fp;
  fp = fopen("evalarrays.c", "w+");
#endif

  array_build();

#ifdef ARRAY_WRITE_TO_FILE
  fputs(flushes_array_desc, fp);
  array_format(flushes, FLUSHES_COUNT, fp);
  fputs(array_end, fp);
#else
  printf("%s", flushes_array_desc);
  array_format(flushes, FLUSHES_COUNT, NULL);
  printf(array_end, NULL);
#endif

#ifdef ARRAY_WRITE_TO_FILE
  fputs(unique3_array_desc, fp);
  array_format(unique3, UNIQUE3_COUNT, fp);
  fputs(array_end, fp);
#else
  printf("%s", unique3_array_desc);
  array_format(unique3, UNIQUE3_COUNT, NULL);
  printf(array_end, NULL);
#endif

#ifdef ARRAY_WRITE_TO_FILE
  fputs(others_array_desc, fp);
  array_format(others, OTHERS_COUNT, fp);
  fputs(array_end, fp);
#else
  printf("%s", others_array_desc);
  array_format(others, OTHERS_COUNT, NULL);
  printf(array_end, NULL);
#endif

  array_dump(others, OTHERS_COUNT, "others.txt");

  build_others_hash();

#ifdef ARRAY_WRITE_TO_FILE
  fputs(hash_others_desc, fp);
  array_format(hash_others, HASH_COUNT, fp);
  fputs(array_end, fp);
#else
  printf("%s", hash_others_desc);
  array_format(hash_others, HASH_COUNT, NULL);
  printf(array_end);
#endif

#ifdef ARRAY_WRITE_TO_FILE
  fclose(fp);
#endif

  return 0;
}
