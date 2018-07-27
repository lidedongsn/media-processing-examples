#include "yuv_scale.h"

YuvScaler::YuvScaler(RectRegion &src_region, RectRegion &dest_region)
			:m_src_region(src_region)
			,m_dest_region(dest_region)
			,m_dst_bufsize(0) {

	m_dst_data[0] = NULL;
//	m_dst_linesize[0] = NULL;

	m_sws_ctx = sws_getContext(src_region.GetWidth(), src_region.GetHeight(), AV_PIX_FMT_YUV420P,
								dest_region.GetWidth(), dest_region.GetHeight(), AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);

	if (!m_sws_ctx) {
		fprintf(stderr, "Impossible to create scale context for the conversion fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
				av_get_pix_fmt_name(AV_PIX_FMT_YUV420P), src_region.GetWidth(), src_region.GetHeight(), av_get_pix_fmt_name(AV_PIX_FMT_YUV420P), dest_region.GetWidth(), dest_region.GetHeight());
	}

}

YuvScaler::~YuvScaler() {
	if(m_sws_ctx){
		sws_freeContext(m_sws_ctx);
	}

	if(m_dst_data) {
		av_freep(&m_dst_data[0]);
	}
}

int YuvScaler::Init() {
	int ret;

	if ((ret = av_image_alloc(m_dst_data, m_dst_linesize, m_dest_region.GetWidth(), m_dest_region.GetHeight(), AV_PIX_FMT_YUV420P, 1)) < 0) {
		fprintf(stderr, "Could not allocate destination image\n");
		return -1;
	}

	m_dst_bufsize = ret;

	return 0;

}

uint8_t ** YuvScaler::Process(uint8_t *src_data[4]) {

	if(!src_data[0]) {
		return NULL;
	}

	int src_linesize[4];

	src_linesize[0] = m_src_region.GetWidth();
	src_linesize[1] = m_src_region.GetWidth() / 2;
	src_linesize[2] = m_src_region.GetWidth() / 2;

	sws_scale(m_sws_ctx, src_data, src_linesize, 0, m_src_region.GetHeight(), m_dst_data, m_dst_linesize);

	return m_dst_data;

}

void YuvScaler::Dump(FILE *file) {

	if(file == NULL) {
		return;
	}

	fwrite(m_dst_data[0], 1, m_dst_bufsize, file);
}
