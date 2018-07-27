#include "h264_decoder.h"
#include <unistd.h>

H264Decoder::H264Decoder()
		: m_parser(NULL)
		, m_context(NULL)
		, m_codec(NULL)
		, m_pkt(NULL)
		, m_frame(NULL) {


}
H264Decoder::~H264Decoder() {
	if(m_parser) {
		av_parser_close(m_parser);
	}
	if(m_context) {
		avcodec_free_context(&m_context);
	}
	if(m_frame) {
		av_frame_free(&m_frame);
	}
	if(m_pkt) {
		av_packet_free(&m_pkt);
	}
}

bool H264Decoder::Init() {
	avcodec_register_all();
	m_pkt = av_packet_alloc();
	if(!m_pkt) {
		fprintf(stderr, "av_packet_alloc err\n");
		return false;
	}

	m_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if(!m_codec) {
		fprintf(stderr, "avcodec_find_decoder AV_CODEC_ID_H264 err\n");
		return false;
	}

	m_parser = av_parser_init(m_codec->id);
	if(!m_parser) {
		fprintf(stderr, "av_parser_init err\n");
		return false;
	}

	m_context = avcodec_alloc_context3(m_codec);
	if(!m_context) {
		fprintf(stderr, "avcodec_alloc_context3 err\n");
		return false;
	}

	if (avcodec_open2(m_context, m_codec, NULL) < 0) {
		fprintf(stderr, "avcodec_open2 err\n");
		return false;
	}

	m_frame = av_frame_alloc();
	if(!m_frame) {
		fprintf(stderr, "av_frame_alloc err\n");
		return false;
	}
	
	return true;

}
AVFrame* H264Decoder::Decode(uint8_t* data, uint32_t len, uint32_t* decode_len) {

	AVFrame* out_frame;
	int ret;

	*decode_len = len;

	ret = av_parser_parse2(m_parser, m_context, &m_pkt->data, &m_pkt->size, (const uint8_t*)data, (int)len, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
	if(ret < 0) {
		fprintf(stderr, "Error while parsing\n");
		return NULL;
	}

	(*decode_len) = ret;

	if(m_pkt->size) {
	//	usleep(40000);
		ret = avcodec_send_packet(m_context, m_pkt);
		if (ret < 0) {
			fprintf(stderr, "Error sending a packet for decoding\n");
			return NULL;
		}

		while (ret >= 0) {
			ret = avcodec_receive_frame(m_context, m_frame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				return NULL;
			else if (ret < 0) {
				fprintf(stderr, "Error during decoding\n");
				return NULL;
			}

			out_frame = m_frame;

			return out_frame;
		}
	}


	return NULL;

}

void H264Decoder::Flush() {
	int ret;

	ret = avcodec_send_packet(m_context, NULL);
	if (ret < 0) {
		fprintf(stderr, "Error sending a packet for decoding\n");
		return;
	}   

	while (ret >= 0) {
		ret = avcodec_receive_frame(m_context, m_frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error during decoding\n");
			return;
		}   
	}
}

void H264Decoder::Dump(FILE *f) {
	if(m_frame) {
		int i;
		AVFrame *pFrame = m_frame;
		for(i=0;i<pFrame->height;i++){  
			fwrite(pFrame->data[0]+pFrame->linesize[0]*i,1,pFrame->width,f);  
		}   
		for(i=0;i<pFrame->height/2;i++){  
			fwrite(pFrame->data[1]+pFrame->linesize[1]*i,1,pFrame->width/2,f);  
		}   
		for(i=0;i<pFrame->height/2;i++){  
			fwrite(pFrame->data[2]+pFrame->linesize[2]*i,1,pFrame->width/2,f);  
		} 
	}
}
