#include "h264_decoder.h"
#include "yuv_scale.h"

#define INBUF_SIZE 4096

static void yuv_save(AVFrame *pFrame, FILE *f) 
{
	int i;

	//Y, U, V  
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
	
int main(int argc, char **argv) {

	const char *filename, *outfilename;
	FILE *f, *f_out, *f_scale;
	uint8_t *data;
	size_t   data_size;
	uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE] = {0};
	AVFrame * frame;
	uint32_t decode_len = 0;

	if (argc <= 2) {
		fprintf(stderr, "Usage: %s <input 264 file> <output yuv file>\n", argv[0]);
		exit(0);
	}
	filename    = argv[1];
	outfilename = argv[2];

	f = fopen(filename, "rb");
	if (!f) {
		fprintf(stderr, "Could not open %s\n", filename);
		exit(1);
	}

	f_out = fopen(outfilename, "wb+");
	if (!f_out) {
		fprintf(stderr, "Could not open %s\n", outfilename);
		exit(1);

	}
	f_scale = fopen("decoder_scale.yuv", "wb+");
	if (!f_scale) {
		fprintf(stderr, "Could not open scale.yuv\n");
		exit(1);

	}

	memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);


	H264Decoder *decoder = new H264Decoder();
	decoder->Init();

	RectRegion src(1280, 720);
	RectRegion dst(320, 180);

	YuvScaler scaler(src, dst);

	scaler.Init();

	while (!feof(f)) {
		/* read raw 264 data from the input file */
		data_size = fread(inbuf, 1, INBUF_SIZE, f);
		if (!data_size)
			break;
		data = inbuf;

		while (data_size > 0) { 
			frame = decoder->Decode(data, data_size, &decode_len);
			data += decode_len;
			data_size -= decode_len;
			if(frame){
				yuv_save(frame, f_out);
				scaler.Process(frame->data);
				scaler.Dump(f_scale);

			}
		}
	}


	decoder->Flush();

	fclose(f_scale);
	fclose(f_out);
	fclose(f);


	if(decoder != NULL) {
		delete decoder;
	}


}
