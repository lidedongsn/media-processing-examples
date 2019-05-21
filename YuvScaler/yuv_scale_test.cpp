#include "yuv_scale.h"
#include "h264_file.h"
#include "unistd.h"

#define YUVSIZE (1280 * 720 * 3 / 2)
#define WIDTH 1280
#define HEIGHT 720

int main(int argc, char **argv) {
  const char *filename;
  const char *dst_w;
  const char *dst_h;
  int file_size;
  uint8_t *data;
  FILE *f_scale;
  uint8_t *src_data[4];

  if (argc <= 3) {
    fprintf(stderr, "Usage: %s <input yuv file> <dst resolution>\n", argv[0]);
    exit(-1);
  }

  filename = argv[1];
  dst_w = argv[2];
  dst_h = argv[3];

  H264File file(filename);

  file.Open();
  data = file.GetData();
  file_size = file.GetSize();

  if (!data) {
    fprintf(stderr, "GetData failed!");
    exit(-1);
  }

  f_scale = fopen("scale_test.yuv", "wb+");
  if (!f_scale) {
    fprintf(stderr, "Could not open scale_test.yuv\n");
    exit(1);
  }

  RectRegion src(WIDTH, HEIGHT);
  int width = (atoi(dst_w));   // >> 4) << 4;
  int height = (atoi(dst_h));  // >> 4) << 4;
  RectRegion dst(width, height);

  YuvScaler scaler(src, dst);
  scaler.Init();

  printf("Starting scale, size %d (%d x %d)...\n", file_size, width, height);
  while (file_size >= 0) {
    src_data[0] = data;
    src_data[1] = data + (WIDTH * HEIGHT);
    src_data[2] = data + (WIDTH * HEIGHT + WIDTH * HEIGHT / 4);
    usleep(40000);
    scaler.Process(src_data);
    scaler.Dump(f_scale);
    file_size -= YUVSIZE;
    data += YUVSIZE;
  }
  printf("Scaling done!\n");

  fclose(f_scale);

  return 0;
}
