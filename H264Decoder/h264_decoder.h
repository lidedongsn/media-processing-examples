#ifndef H264_DECODER_H
#define H264_DECODER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern "C" {
#include <libavcodec/avcodec.h>
}

class H264Decoder{
public:
	H264Decoder();
	virtual ~H264Decoder();

	bool Init();
	AVFrame* Decode(uint8_t* data, uint32_t len, uint32_t* decode_len);
	void Flush();
	void Dump(FILE *f);

private:
	AVCodecParserContext	*m_parser;
	AVCodecContext			*m_context;
	AVCodec					*m_codec;
	AVPacket				*m_pkt;
	AVFrame					*m_frame;
};


#endif 
