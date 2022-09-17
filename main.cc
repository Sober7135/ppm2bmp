#include "ppm2bmp.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    exit(1);
  }
  bmp b{argv[1], argv[2]};
  b.write();
}
