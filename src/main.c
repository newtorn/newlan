#include <stdio.h>
#include <stdint.h>

typedef union
{
  uint64_t bits64;
  uint32_t bits32[2];
  double number;
} Bits64;

int main(int argc, char **argv)
{
  Bits64 bits;
  sscanf(argv[1], "%lf", &bits.number);
  printf("%llu [%u,%u] %f", bits.bits64, bits.bits32[0], bits.bits32[1], bits.number);
  printf(" %u", bits.bits32[0] ^ bits.bits32[1]);
  return 0;
}
