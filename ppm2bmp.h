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
    size = image_height * image_width * 3 + offset;
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
    // printf("%d", __LINE__);
    write_rgb();
    printf("image_width=%u, image_height=%u, size=%u\n", image_width,
           image_height, size);
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
    // printf("%d", __LINE__);
    get_rgb_table();
    printf("here");
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
        BGR bgr = get_rgb();
        line.push_back(bgr.b);
        line.push_back(bgr.g);
        line.push_back(bgr.r);
      }
      pixel.push_back(std::move(line));
      // printf("%d\n", __LINE__);
    }
  }

  BGR get_rgb() {
    char temp;
    BGR bgr;
    byte rgb;

    for (int i = 0; i < 3; ++i) {
      while (1) {
        temp = fgetc(r);
        if (temp >= 0x30 && temp <= 0x39)
          break;
      }
      while (temp >= 0x30 && temp <= 0x39) {
        rgb = rgb * 10 + temp - 0x30;
        temp = fgetc(r);
      }
      printf("%d ",rgb);
      switch (i) {
      case 0:
        bgr.r = rgb;
        rgb = 0;
        break;
      case 1:
        bgr.g = rgb;
        rgb = 0;
        break;
      case 2:
        bgr.b = rgb;
        rgb = 0;
        printf("\n");
      }
    }
    return bgr;
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