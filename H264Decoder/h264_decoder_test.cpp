#include "h264_decoder.h"
#include "h264_file.h"
#include <time.h>
#include<sys/time.h>
#include <unistd.h>

int main(int argc, char **argv) {

	uint8_t *data;
	int file_size;
	uint32_t decode_len = 0;
	int data_size = INBUF_SIZE;
	int frame_count = 0;
	AVFrame *frame;
	const char* filename;

	struct  timeval start;
	struct  timeval end;

	if (argc <= 1) {
		fprintf(stderr, "Usage: %s <input 264 file>\n", argv[0]);
		exit(-1);
	}

	filename = argv[1];

	H264File file(filename);

	file.Open();
	data = file.GetData();
	file_size = file.GetSize();

	if(!data) {
		fprintf(stderr, "GetData failed!");
		exit(-1);
	}

	H264Decoder *decoder = new H264Decoder();

	decoder->Init();

	printf("%s size: %d\n", filename, file_size);
	printf("Starting decode ...\n");
	gettimeofday(&start, NULL);
	while(file_size >= 0) {
		if(file_size < INBUF_SIZE) {
			data_size = file_size;
		}else{
			data_size = INBUF_SIZE;
		}
		while(data_size > 0) {
			frame = decoder->Decode(data, data_size, &decode_len);
			data += decode_len;
			data_size -= decode_len;
//			printf("data_size: %d, decode_len: %d\n", data_size, decode_len);
			if(frame) {
				frame_count++;
//				printf("frame_count %d\n", frame_count);
			}

		}
		file_size -= INBUF_SIZE;
//		printf("-->%s size: %d\n", filename, file_size);
	}	
	decoder->Flush();
	gettimeofday(&end, NULL);



	printf("%ld %ld %ld, %ld\n", end.tv_sec , start.tv_sec, end.tv_usec , start.tv_usec);
	long duration_sec = end.tv_sec - start.tv_sec;
	long duration_usec = 0;
	if(duration_sec > 0 && end.tv_usec < start.tv_usec) {
		duration_sec -= 1;
		duration_usec = 1000000 - start.tv_usec;
		duration_usec = duration_usec + end.tv_usec;
	}else{
		duration_usec = end.tv_usec - start.tv_usec;
	}
	printf("Decode done! frame_count: %d, %ld s %ld us\n", frame_count, duration_sec, duration_usec);

	file.Close();
	if(decoder != NULL) {
		delete decoder;
	}
}
