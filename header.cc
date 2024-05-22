#include "png.h"
#include <iostream>

struct HeaderInfo {
  int width;
  int height;
  png_byte bit_depth;
  png_byte color_type;
  png_byte filter_method;
  png_byte compression_type;
  png_byte interlace_type;

  void print() {
    std::cout << "Image info:\n"
              << "width: " << width << "\n"
              << "height: " << height << "\n"
              << "bit_depth: " << (int)bit_depth << "\n"
              << "color_type: " << (int)color_type << "\n"
              << "filter_method: " << (int)filter_method << "\n"
              << "compression_type: " << (int)compression_type << "\n"
              << "interlace_type: " << (int)interlace_type << "\n";
  }
};
