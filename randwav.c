#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define WAVFILE_SAMPLES_PER_SECOND 44100

int main(int argc, char* argv[])
{
    int32_t rnd = 0;
    uint32_t i = 0;
    uint32_t seconds = 0;
    uint32_t blocks = 0;
    uint16_t m = 2;  // Each sample is M bytes long
    uint16_t nc = 2;  // Number of channels
    uint32_t max_blocks = (UINT32_MAX - 36 - 1) / 4;  // 1,073,741,814.5
    uint32_t max_seconds = max_blocks / WAVFILE_SAMPLES_PER_SECOND;  // 24,347 seconds
    uint32_t data32 = 0;
    uint16_t data16 = 0;
    int16_t* data = NULL;
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

    blocks = seconds * WAVFILE_SAMPLES_PER_SECOND;
    data = (int16_t*)malloc(blocks * m * nc);
    if (data == NULL) {
        fprintf(stderr, "Failed to allocate memory for block of size %u\n", blocks);
        exit(4);
    }

    fp = fopen(default_name, "wb+");

    if (fp != NULL) {
        fwrite("RIFF", 4, 1, fp);  // Chunk ID
        data32 = 4 + 24 + 8 + (blocks * m * nc);
        fwrite(&data32, 4, 1, fp);  // Chunk size
        fwrite("WAVE", 4, 1, fp);  // WAVE ID
        // fmt chunk
        fwrite("fmt ", 4, 1, fp);  // Chunk ID
        data32 = 16;
        fwrite(&data32, 4, 1, fp);  // Chunk size
        data16 = 1;
        fwrite(&data16, 2, 1, fp);  // WAVE_FORMAT_PCM
        data16 = nc;
        fwrite(&data16, 2, 1, fp);  // nChannels
        data32 = WAVFILE_SAMPLES_PER_SECOND;
        fwrite(&data32, 4, 1, fp);  // nSamplesPerSec
        data32 = data32 * m * nc;
        fwrite(&data32, 4, 1, fp);  // nAvgBytesPerSec
        data16 = m * nc;
        fwrite(&data16, 2, 1, fp);  // nBlockAlign
        data16 = 16;
        fwrite(&data16, 2, 1, fp);  // wBitsPerSample
        // data chunk
        fwrite("data", 4, 1, fp);  // Chunk ID
        data32 = blocks * m * nc;
        fwrite(&data32, 4, 1, fp);

        // Write sampled data
        for (i=0; i<blocks; i++) {
            rnd = rand()%65536 - 32768;
            data[i*2] = (int16_t)rnd;
            data[i*2+1] = (int16_t)rnd;
        }

        fwrite(data, blocks * m * nc, 1, fp);
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