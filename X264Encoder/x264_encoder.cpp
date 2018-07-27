#include "x264_encoder.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

X264Encoder::X264Encoder(int id, uint32_t width, uint32_t height)
			: m_id(id)
			, m_width(width)
			, m_height(height) 
			, m_frames(-1) {
//	m_nal = (x264_nal_t*)malloc(sizeof(x264_nal_t));
	if(x264_param_default_preset(&m_param, "medium", NULL) < 0 ) {
	
	}

	m_param.i_csp		= X264_CSP_I420;
	m_param.i_width		= m_width;
	m_param.i_height	= m_height;
	m_param.i_bframe	= 0;

	if(x264_param_apply_profile(&m_param, "high") < 0 ) {
	}

	x264_picture_alloc(&m_pic_in, m_param.i_csp, m_param.i_width, m_param.i_height);

}

X264Encoder::~X264Encoder() {
//	if(m_nal) {
//		free(m_nal);
//	}
	x264_picture_clean(&m_pic_in);
}

bool X264Encoder::Open() {
	m_handle = x264_encoder_open(&m_param);
	if(!m_handle) {
		return false;
	}

	return true;

}

void X264Encoder::Close() {
	if(m_handle) {
		x264_encoder_close(m_handle);
	}
}

uint8_t * X264Encoder::Encode(uint8_t* yuv[4], int *size) {
	int frame_size;
	int i_nal;

	m_frames++;
	memcpy(m_pic_in.img.plane[0], yuv[0], m_width * m_height);
	memcpy(m_pic_in.img.plane[1], yuv[1], m_width * m_height / 4);
	memcpy(m_pic_in.img.plane[2], yuv[2], m_width * m_height / 4);
//	m_pic_in.img.plane[0] = yuv[0];
//	m_pic_in.img.plane[1] = yuv[1];
//	m_pic_in.img.plane[2] = yuv[2];
	usleep(40000);
	m_pic_in.i_pts = m_frames;
	frame_size = x264_encoder_encode(m_handle, &m_nal, &i_nal, &m_pic_in, &m_pic_out);
	if(frame_size < 0) {
		return NULL;
	}else if(frame_size) {
		*size = frame_size;
		return m_nal->p_payload;
	}

	return NULL;
}

uint8_t * X264Encoder::Flush(int *size) {
	int i_nal;
	int frame_size;

	frame_size = x264_encoder_encode(m_handle, &m_nal, &i_nal, NULL, &m_pic_out);
	if(frame_size < 0) {
		return NULL;
	}else if(frame_size) {
		*size = frame_size;
		return m_nal->p_payload;
	}
	return NULL;
}

int X264Encoder::EncodeDelayed() {
		return x264_encoder_delayed_frames(m_handle);
}
