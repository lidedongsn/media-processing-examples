#include "h264_file.h"

H264File::H264File(const char* filename)
    : m_filename(filename), m_fd(NULL), m_filesize(0) {
  m_filedata = new uint8_t[BUFFER_LEN];
}

H264File::~H264File() {
  if (m_filedata) {
    delete m_filedata;
  }
}

bool H264File::Open() {
  int ret;
  uint8_t* tmp;

  m_fd = fopen(m_filename, "rb");
  if (m_fd == NULL) {
    fprintf(stderr, "open %s failed!", m_filename);
    return false;
  }

  tmp = m_filedata;

  while (!feof(m_fd)) {
    ret = fread(tmp, 1, INBUF_SIZE, m_fd);
    if (!ret) break;
    tmp += ret;
    m_filesize += ret;
  }

  return true;
}

void H264File::Close() {
  if (m_fd) fclose(m_fd);
}

uint8_t* H264File::GetData() { return m_filedata; }

int H264File::GetSize() { return m_filesize; }
