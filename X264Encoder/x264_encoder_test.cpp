#include "x264_encoder.h"
#include <iostream>
#include "../decoder/h264_file.h"

#define YUVSIZE (1280 * 720 * 3 / 2)
#define WIDTH 1280
#define HEIGHT 720
int main() {
  uint8_t *data;
  int file_size;
  FILE *fd_out;
  uint8_t *src_data[4];
  uint8_t *encoded_paylod = NULL;
  const char *outfile = "./test.264";

  H264File file("../decoder/xiaoxin5.yuv");

  file.Open();
  data = file.GetData();
  file_size = file.GetSize();

  if (!data) {
    fprintf(stderr, "GetData failed!");
    exit(-1);
  }

  fd_out = fopen(outfile, "wb+");
  if (!fd_out) {
    fprintf(stderr, "Could not open %s\n", outfile);
    exit(-1);
  }

  X264Encoder encoder(1, WIDTH, HEIGHT);
  encoder.Open();

  int size;
  while (file_size >= 0) {
    src_data[0] = data;
    src_data[1] = data + (WIDTH * HEIGHT);
    src_data[2] = data + (WIDTH * HEIGHT + WIDTH * HEIGHT / 4);
    encoded_paylod = encoder.Encode(src_data, &size);
    if (encoded_paylod != NULL) {
      fwrite(encoded_paylod, size, 1, fd_out);
    }

    file_size -= YUVSIZE;
    data += YUVSIZE;
  }

  while (encoder.EncodeDelayed()) {
    encoded_paylod = encoder.Flush(&size);
    if (encoded_paylod != NULL) {
      fwrite(encoded_paylod, size, 1, fd_out);
    }
  }
  encoder.Close();
  fclose(fd_out);
}
