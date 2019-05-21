#ifndef H264_FILE_H
#define H264_FILE_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>

#define BUFFER_LEN (500 * 1024 * 1024)
#define INBUF_SIZE 4096

class H264File {
 public:
  H264File(const char* filename);
  virtual ~H264File();
  bool Open();
  void Close();
  uint8_t* GetData();
  int GetSize();

 private:
  const char* m_filename;
  FILE* m_fd;
  int m_filesize;
  uint8_t* m_filedata;
};

#endif
