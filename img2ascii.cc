#include "assert.h"
#include "error.cc"
#include "header.cc"
#include "png.h"
#include "pngconf.h"
#include "stdio.h"
#include "stdlib.h"
#include <iostream>

#define PNG_BYTES_TO_CHECK 8
#define ASCII_MAP_SIZE 11

png_structp read_ptr;
png_structp write_ptr;
png_bytep *row_pointers = nullptr;
HeaderInfo header_info;
// png_colorp plte;
// int num_plte;
char ascii_map[ASCII_MAP_SIZE] = {'@', '#', 'S', '%', '?', '*',
                                  '+', ';', ':', ',', '.'};

void downsample(int height_factor, int width_factor) {
  int new_height = (header_info.height + height_factor - 1) / height_factor;
  int new_width = (header_info.width + width_factor - 1) / width_factor;

  // create temp_row_pointers to hold new image
  png_bytep *temp_row_pointers =
      (png_bytep *)malloc(new_height * sizeof(png_bytep));
  for (int i = 0; i < new_height; i++) {
    temp_row_pointers[i] = (png_byte *)malloc(new_width * sizeof(png_byte));
    int p_size = i == new_height - 1 ? header_info.height - height_factor * i
                                     : height_factor;
    for (int j = 0; j < new_width; j++) {
      int new_value = 0;
      int q_size = j == new_width - 1 ? header_info.width - width_factor * j
                                      : width_factor;

      for (int p = 0; p < p_size; p++) {
        for (int q = 0; q < q_size; q++) {
          new_value +=
              row_pointers[height_factor * i + p][width_factor * j + q];
        }
      }
      temp_row_pointers[i][j] = new_value / (p_size * q_size);
    }
  }

  // free old row_pointers
  for (int i = 0; i < header_info.height; i++) {
    free(row_pointers[i]);
  }
  free(row_pointers);

  // point row_pointers to the new array
  row_pointers = temp_row_pointers;

  header_info.height = new_height;
  header_info.width = new_width;
}

Error read_file(char *in_file) {
  FILE *fp = fopen(in_file, "rb");
  if (!fp) {
    return ERR_INVALID_FILE_NAME;
  }

  // check header to ensure file is valid png
  // unsigned char header[PNG_BYTES_TO_CHECK];
  // fread(header, 1, PNG_BYTES_TO_CHECK, fp);
  // if (png_sig_cmp(header, 0, PNG_BYTES_TO_CHECK)) {
  //   fclose(fp);
  //   return ERR_NOT_A_PNG;
  // }

  read_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!read_ptr) {
    fclose(fp);
    return ERR_CREATE_READ_STRUCT_FAIL;
  }

  png_infop info_ptr = png_create_info_struct(read_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&read_ptr, nullptr, nullptr);
    fclose(fp);
    return ERR_CREATE_INFO_STRUCT_FAIL;
  }

  if (setjmp(png_jmpbuf(read_ptr))) {
    png_destroy_read_struct(&read_ptr, nullptr, nullptr);
    png_destroy_write_struct(&read_ptr, &info_ptr);
    fclose(fp);
    return ERR_JMP_FAIL;
  }

  png_init_io(read_ptr, fp);
  // png_set_sig_bytes(read_ptr, PNG_BYTES_TO_CHECK);

  png_read_info(read_ptr, info_ptr);

  header_info.width = png_get_image_width(read_ptr, info_ptr);
  header_info.height = png_get_image_height(read_ptr, info_ptr);
  header_info.bit_depth = png_get_bit_depth(read_ptr, info_ptr);
  header_info.color_type = png_get_color_type(read_ptr, info_ptr);
  header_info.filter_method = png_get_filter_type(read_ptr, info_ptr);
  header_info.compression_type = png_get_compression_type(read_ptr, info_ptr);
  header_info.interlace_type = png_get_interlace_type(read_ptr, info_ptr);
  // header_info.print();

  // png_get_PLTE(read_ptr, info_ptr, &plte, &num_plte);

  // turn paletted image to rgb
  if (header_info.color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(read_ptr);
    header_info.color_type = PNG_COLOR_TYPE_RGB;
  }

  // turn rgb image to grayscale
  if (header_info.color_type == PNG_COLOR_TYPE_RGB ||
      header_info.color_type == PNG_COLOR_TYPE_RGBA) {
    png_set_rgb_to_gray_fixed(read_ptr, 1, -1, -1);
    header_info.color_type = PNG_COLOR_TYPE_GRAY;
  }

  png_read_update_info(read_ptr, info_ptr);

  // read image data into buffer
  row_pointers = (png_bytep *)malloc(header_info.height * sizeof(png_bytep));
  for (int i = 0; i < header_info.height; i++) {
    row_pointers[i] = (png_byte *)malloc(png_get_rowbytes(read_ptr, info_ptr));
  }

  png_read_image(read_ptr, row_pointers);

  png_destroy_read_struct(&read_ptr, &info_ptr, nullptr);
  fclose(fp);

  return SUCCESS;
}

Error write_file(char *out_file) {
  FILE *fp = fopen(out_file, "wb");
  if (!fp) {
    return ERR_INVALID_FILE_NAME;
  }

  write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!write_ptr) {
    fclose(fp);
    return ERR_CREATE_WRITE_STRUCT_FAIL;
  }

  png_infop info_ptr = png_create_info_struct(write_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&write_ptr, nullptr);
    fclose(fp);
    return ERR_CREATE_INFO_STRUCT_FAIL;
  }

  if (setjmp(png_jmpbuf(write_ptr))) {
    png_destroy_read_struct(&write_ptr, nullptr, nullptr);
    png_destroy_write_struct(&write_ptr, &info_ptr);
    fclose(fp);
    return ERR_JMP_FAIL;
  }

  png_init_io(write_ptr, fp);

  png_set_IHDR(write_ptr, info_ptr, header_info.width, header_info.height,
               header_info.bit_depth, header_info.color_type,
               header_info.filter_method, header_info.compression_type,
               header_info.interlace_type);
  // png_set_PLTE(write_ptr, info_ptr, plte, num_plte);

  png_write_info(write_ptr, info_ptr);
  png_write_image(write_ptr, row_pointers);
  png_write_end(write_ptr, nullptr);

  for (int i = 0; i < header_info.height; i++) {
    free(row_pointers[i]);
  }
  free(row_pointers);

  png_destroy_write_struct(&write_ptr, &info_ptr);

  fclose(fp);

  return SUCCESS;
}

void write_ascii(char *file) {
  FILE *fd = fopen(file, "w");
  for (int i = 0; i < header_info.height; i++) {
    for (int j = 0; j < header_info.width; j++) {
      int index = (int)row_pointers[i][j] /
                  ((256 + ASCII_MAP_SIZE - 1) / ASCII_MAP_SIZE);
      // printf("writing character %c\n", ascii_map[index]);
      fputc(ascii_map[index], fd);
    }
    fputc('\n', fd);
  }

  fclose(fd);
}

int main(int argc, char **argv) {
  if (argc != 6) {
    std::cout << "Usage: img2ascii <in_file> <out_file> <ascii_txt_file> "
                 "<downsample_height> <downsample_width>"
              << "\n";
    return 0;
  }

  int downsample_height = atoi(argv[4]);
  int downsample_width = atoi(argv[5]);
  char *in_file, *out_file;

  in_file = argv[1];
  out_file = argv[2];

  Error e = read_file(in_file);
  if (e) {
    printf("Read had an error: %s\n", get_error_message(e));
    return 1;
  }

  // print something out for each sample
  // for (int i = 0; i < header_info.height; i++) {
  //   png_bytep row = row_pointers[i];
  //   for (int j = 0; j < header_info.width; j++) {
  //     printf("%4d, %4d = grayscale value %3d\n", i, j, row[j]);
  //   }
  // }

  printf("Old image size: %dpx x %dpx\n", header_info.height,
         header_info.width);
  downsample(downsample_height, downsample_width);
  printf("New image size: %dpx x %dpx\n", header_info.height,
         header_info.width);

  write_ascii(argv[3]);

  e = write_file(out_file);
  if (e) {
    printf("Write had an error: %s\n", get_error_message(e));
    return 1;
  }

  return 0;
}
