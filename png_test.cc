/*
 * A few functions to play around with and understand
 * the format of png files
 */

#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct Header {
  int width;
  int height;
  int bit_depth;
  int color_type;
  int compression_method;
  int filter_method;
  int interlace_method;

  void print_info() {
    std::cout << "width: " << width << '\n';
    std::cout << "height: " << height << '\n';
    std::cout << "bit_depth: " << bit_depth << '\n';
    std::cout << "color_type: " << color_type << '\n';
    std::cout << "compression_method: " << compression_method << '\n';
    std::cout << "filter_method: " << filter_method << '\n';
    std::cout << "interlace_method: " << interlace_method << '\n';
  }
};

uint32_t bytes_to_int(unsigned char *buf, int bytes) {
  uint32_t res = 0;
  for (int i = 0; i < bytes; i++) {
    res |= (uint32_t)buf[i] << (8 * (bytes - 1 - i));
  }

  return res;
}

uint32_t read_chunk(unsigned char *buf) {
  char type[5];
  uint32_t length = bytes_to_int(buf, 4);

  memcpy(type, buf + 4, 4);
  type[4] = '\0';

  std::cout << "Chunk with type " << type << " has length " << length << "\n";

  return length + 12;
}

Header get_header_info(unsigned char *buf) {
  Header h;

  h.width = bytes_to_int(buf, 4);
  buf += 4;

  h.height = bytes_to_int(buf, 4);
  buf += 4;

  h.bit_depth = bytes_to_int(buf++, 1);
  h.color_type = bytes_to_int(buf++, 1);
  h.compression_method = bytes_to_int(buf++, 1);
  h.filter_method = bytes_to_int(buf++, 1);
  h.interlace_method = bytes_to_int(buf++, 1);

  return h;
}

int main(int argc, char **argv) {
  unsigned char buf[50000];
  char s[4];

  FILE *fd = fopen(argv[1], "rb");
  if (fd == nullptr) {
    std::cout << "File not found."
              << "\n";
    fclose(fd);
    return -1;
  }

  int bytes_read = fread(buf, 1, 50000, fd);
  std::cout << "bytes read: " << bytes_read << '\n';
  if (bytes_read == 0) {
    std::cout << "Error reading file."
              << "\n";
    fclose(fd);
    return -1;
  }

  memcpy(s, buf + 1, 3);
  s[3] = '\0';

  if (strcmp(s, "PNG")) {
    std::cout << "File is not a PNG."
              << "\n";
  }

  uint32_t offset = 8;
  while (offset < bytes_read) {
    offset += read_chunk(buf + offset);
  }

  Header h = get_header_info(buf + 16);
  h.print_info();

  fclose(fd);

  return 0;
}
