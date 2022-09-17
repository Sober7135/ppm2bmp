#ifndef PPM2BMP_H
#define PPM2BMP_H

#include <bits/types/FILE.h>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

using byte = unsigned char;

struct BGR {
  byte b, g, r;
};

class bmp {
public:
  bmp(std::string &&pn, std::string &&bn, unsigned int ofs = 0x8a)
      : offset(ofs), ppm_name(pn), bmp_name(bn) {
    r = fopen(ppm_name.c_str(), "rb");
    w = fopen(bmp_name.c_str(), "wb");
    header = fopen("bmp_header", "rb");

    fscanf(r, "%*s %u %u", &image_width, &image_height);
    size = image_width * image_height * 3 + offset;
  }

  ~bmp() {
    fclose(r);
    fclose(w);
    fclose(header);
  }

  void write() {
    write_header();
    write_rgb();
  }

  void write_header() {
    char *h = new char[offset + 1];
    h[offset] = '\0';

    rewind(r);
    rewind(w);
    fread(h, offset, 1, header);
    fwrite(h, offset, 1, w);

    fseek(w, 2, SEEK_SET);
    fwrite(&size, sizeof(size), 1, w);

    fseek(w, 18, SEEK_SET);
    fwrite(&image_width, sizeof(image_width), 1, w);
    fwrite(&image_height, sizeof(image_height), 1, w);

    delete[] h;
  }

  void write_rgb() {
    get_rgb_table();
    fseek(w, offset, SEEK_SET);
    auto begin = pixel.begin();
    auto it = pixel.end();
    do {
      --it;
      for (auto &x : *it) {
        fwrite(&x, sizeof(unsigned char), 1, w);
      }
    } while (it != begin);
  }

  void get_rgb_table() {
    rewind(r);
    fscanf(r, "%*s %*s %*s %*s");
    for (unsigned i = 0; i < image_height; ++i) {
      std::vector<byte> line;
      for (unsigned j = 0; j < image_width; ++j) {
        int _r, _g, _b;
        fscanf(r, "%d%d%d", &_r, &_g, &_b);
        line.push_back(_b);
        line.push_back(_g);
        line.push_back(_r);
      }
      pixel.push_back(std::move(line));
    }
  }

private:
  unsigned int size;
  unsigned int offset;
  unsigned int image_width;
  unsigned int image_height;
  std::string ppm_name;
  std::string bmp_name;
  FILE *r;
  FILE *w;
  FILE *header;
  std::vector<std::vector<byte>> pixel;
};

#endif