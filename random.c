#include "random.h"

// https://lemire.me/blog/2019/07/03/a-fast-16-bit-random-number-generator/

#define RANDOM_KEY 0x2ab
#define RANDOM_INCREMENT 0xfc15

unsigned char wyhash = 0; 

unsigned char wyhash8() {
  wyhash += RANDOM_INCREMENT;
  unsigned int hash = wyhash * RANDOM_KEY;
  return (hash >> 8) ^ hash;
}
