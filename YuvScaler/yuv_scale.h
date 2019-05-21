#ifndef YUV_SCALE_H
#define YUV_SCALE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>
}

class RectRegion {
 public:
  RectRegion(uint32_t width, uint32_t height)
      : m_width(width), m_height(height) {}
  virtual ~RectRegion() {}

  uint32_t GetWidth() { return m_width; }
  uint32_t GetHeight() { return m_height; }

 private:
  uint32_t m_width;
  uint32_t m_height;
};

class YuvScaler {
 public:
  YuvScaler(RectRegion &src_region, RectRegion &dest_region);
  virtual ~YuvScaler();

  int Init();
  uint8_t **Process(uint8_t *src_data[4]);
  void Dump(FILE *file);

 private:
  RectRegion &m_src_region;
  RectRegion &m_dest_region;
  struct SwsContext *m_sws_ctx;
  uint8_t *m_dst_data[4];
  int m_dst_linesize[4];
  int m_dst_bufsize;
};

#endif
