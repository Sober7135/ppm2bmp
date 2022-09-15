#include "ppm2bmp.h"
#include <cstdio>

int main() {
  // printf("hello");
  bmp b{"test.ppm", "test.bmp"};
  // printf("%d", __LINE__);
  b.write();
}