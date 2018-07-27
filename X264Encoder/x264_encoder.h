#ifndef _X264_ENCODER_H_
#define _X264_ENCODER_H_
#include <stdint.h>
#include <stdlib.h>
#include <x264.h>

class X264Encoder {
public:
	X264Encoder(int id, uint32_t width, uint32_t height);
	virtual ~X264Encoder();

	bool Open();
	uint8_t* Encode(uint8_t* yuv[4], int *size);
	uint8_t* Flush(int *size);
	int EncodeDelayed();
	void Close();

private:
	int					m_id;
	uint32_t			m_width;
	uint32_t			m_height;
	x264_t				*m_handle;
	x264_param_t		m_param;
	x264_picture_t		m_pic_in;
	x264_picture_t		m_pic_out;
	x264_nal_t			*m_nal;
	int					m_frames;

};


#endif
