#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

int main(int argc, char* argv[])
{
    int32_t rnd = 0;
    uint32_t i = 0;
    uint32_t seconds = 0;
    uint32_t blocks = 0;
    uint32_t max_blocks = (UINT32_MAX - 36 - 1) / 4;  // 1,073,741,814.5
    uint32_t max_seconds = max_blocks / 44100;  // 24,347 seconds
    uint32_t data32 = 0;
    uint16_t data16 = 0;
    uint8_t* data = NULL;
    FILE* fp;
    const char* default_name = "out.wav";

    srand(1);

    if (argc < 2) {
        fprintf(stderr, "Usage: randwav <seconds>\n");
        exit(1);
    }

    if (sscanf(argv[1], "%u", &seconds) != 1) {
        fprintf(stderr, "Invalid number for argument <seconds>\n");
        exit(2);
    }

    if (seconds >= max_seconds) {
        fprintf(stderr, "Maximum amount of seconds cannot exceed %u\n", max_seconds);
        exit(3);
    }

    blocks = seconds * 44100;
    data = (uint8_t*)malloc(blocks * 4);
    if (data == NULL) {
        fprintf(stderr, "Failed to allocate memory for block of size %u\n", blocks);
        exit(4);
    }

    fp = fopen(default_name, "wb+");

    if (fp != NULL) {
        fwrite("RIFF", 4, 1, fp);  // Chunk ID
        data32 = 4 + 24 + 8 + (blocks * 4);
        fwrite(&data32, 4, 1, fp);  // Chunk size
        fwrite("WAVE", 4, 1, fp);  // WAVE ID
        // fmt chunk
        fwrite("fmt ", 4, 1, fp);  // Chunk ID
        data32 = 16;
        fwrite(&data32, 4, 1, fp);  // Chunk size
        data16 = 1;
        fwrite(&data16, 2, 1, fp);  // WAVE_FORMAT_PCM
        data16 = 2;
        fwrite(&data16, 2, 1, fp);  // nChannels
        data32 = 44100;
        fwrite(&data32, 4, 1, fp);  // nSamplesPerSec
        data32 *= 4;
        fwrite(&data32, 4, 1, fp);  // nAvgBytesPerSec
        data16 = 4;
        fwrite(&data16, 2, 1, fp);  // nBlockAlign
        data16 = 16;
        fwrite(&data16, 2, 1, fp);  // wBitsPerSample
        // data chunk
        fwrite("data", 4, 1, fp);  // Chunk ID
        data32 = blocks * 4;
        fwrite(&data32, 4, 1, fp);

        // Write sampled data
        for (i=0; i<blocks; i++) {
            rnd = rand()%65536 - 32768;
            rnd = (rnd << 16) | rnd;
            memcpy((void*)(data+i*4), (void*)&rnd, sizeof(int32_t));
            //memcpy((void*)(data+i*2+2), (void*)&rnd, sizeof(int16_t));
        }

        fwrite(data, blocks*4, 1, fp);
    }
    else {
        fprintf(stderr, "Failed to write to file %s\n", default_name);
        exit(3);
    }

    if (data) {
        free(data);
    }
    fclose(fp);

    return 0;
}