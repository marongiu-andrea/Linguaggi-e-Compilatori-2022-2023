
#include <stdbool.h>
#include <sys/mman.h>
#include <linux/perf_event.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>
#include <stdio.h>

static inline unsigned long get_rdtsc_freq(void) {
    unsigned long          tsc_freq  = 3000000000;
    bool                   fast_path = false;
    struct perf_event_attr pe        = {
        .type           = PERF_TYPE_HARDWARE,
        .size           = sizeof(struct perf_event_attr),
        .config         = PERF_COUNT_HW_INSTRUCTIONS,
        .disabled       = 1,
        .exclude_kernel = 1,
        .exclude_hv     = 1
    };
    
    int fd = syscall(298 /* __NR_perf_event_open on x86_64 */, &pe, 0, -1, -1, 0);
    if (fd != -1) {
        void *addr = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0);
        if (addr) {
            struct perf_event_mmap_page *pc = addr;
            if (pc->cap_user_time == 1) {
                tsc_freq  = ((__uint128_t)1000000000 << pc->time_shift) / pc->time_mult;
                // If you don't like 128 bit arithmetic, do this:
                // tsc_freq  = (1000000000ull << (pc->time_shift / 2)) / (pc->time_mult >> (pc->time_shift - pc->time_shift / 2));
                fast_path = true;
            }
            munmap(addr, 4096);
        }
        close(fd);
    }
    
    if (!fast_path) {
        // CLOCK_MONOTONIC_RAW is Linux-specific but better;
        // CLOCK_MONOTONIC     is POSIX-portable but slower.
        struct timespec clock = {0};
        clock_gettime(CLOCK_MONOTONIC_RAW, &clock);
        signed long   time_begin = clock.tv_sec * 1e9 + clock.tv_nsec;
        unsigned long tsc_begin  = __rdtsc();
        usleep(2000);
        clock_gettime(CLOCK_MONOTONIC_RAW, &clock);
        signed long   time_end   = clock.tv_sec * 1e9 + clock.tv_nsec;
        unsigned long tsc_end    = __rdtsc();
        tsc_freq                 = (tsc_end - tsc_begin) * 1000000000 / (time_end - time_begin);
    }
    
    return tsc_freq;
}

#define N 400000000
#define KB(bytes) 1024ULL*bytes
#define MB(bytes) 1024ULL*KB(bytes)

#define BufferSize MB(64)

extern void populate(int a[N], int b[N], int c[N], int size, int step);

int main(int argCount, char** argValue)
{
    if(argCount != 2) {
        fprintf(stderr, "Incorrect usage");
    }
    
    long freq = get_rdtsc_freq();
    int curSize = 0;
    char* buffer = (char*)malloc(BufferSize);
    buffer[0] = '\0';
    
    for (int i = 65536; i < N; i*= 2) {
        int* a = (int*)malloc(sizeof(int) * i);
        int* b = (int*)malloc(sizeof(int) * i);
        int* c = (int*)malloc(sizeof(int) * i);
        
        for (int j = 0; j < 8200; j++) {
            double averageTime = 0;
            int iterations = 0;
            
            for(int k = 0; k < 100; ++k) {
                long start = __rdtsc();
                populate(a, b, c, i, j);
                long end = __rdtsc();
                
                double elapsed = (end - start) / (double)freq;
                
                ++iterations;
                averageTime += elapsed;
            }
            
            averageTime /= (double)iterations;
            
            if(curSize >= BufferSize) {
                fprintf(stderr, "Buffer overflow\n");
                return 1;
            }
            
            curSize += snprintf(buffer+curSize, BufferSize-curSize, "%d %d %f\n", i, j, averageTime);
        }
        
        free(a);
        free(b);
        free(c);
    }
    
    FILE* writeTo = fopen(argValue[1], "w");
    fwrite(buffer, 1, curSize + 1, writeTo);
    fclose(writeTo);
}
