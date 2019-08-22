#define _GNU_SOURCE

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

typedef unsigned long long u64;
#define USER_SEL 0x410000
#define PMC_MSR0 0x0C1
#define PMC_MSR1 0x0C2
#define PERFEVTSEL0 0x186
#define PERFEVTSEL1 0x187


void setaffinity(int coreid) {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(coreid, &mask);
  if(sched_setaffinity( 0, sizeof(mask), &mask ) == -1 ) {
    perror("couldn't set affinity");
  }
}

// note the msr binding with cpu core, I use 3 since my iso-ed core is 3 and 7
int open_msr_file() {
  FILE* f = fopen("/dev/cpu/3/msr", "r+");
  if(f == NULL) {
    perror("fopen error");
    return -1;
  }
  return fileno(f);
}

__attribute__((always_inline)) inline uint64_t read_msr(int msr_file, unsigned int msr) {
  uint64_t data = 0;
  long ret;
  ret = pread(msr_file, &data, sizeof(data), msr);
  if(ret != sizeof(data)) {
    errno = -ret;
    perror("rdmsr pread error");
  }
  return data;
}

__attribute__((always_inline)) inline void write_msr(int msr_file, unsigned int msr, uint64_t data) {
  long ret;
  ret = pwrite(msr_file, &data, sizeof(data), msr);
  if(ret != sizeof(data)) {
    errno = -ret;
    perror("wrmsr pwrite error");
    fprintf(stderr, "MSR: %x, DATA: %lx\n", msr, data);
  }
}

__attribute__((always_inline)) inline uint64_t rdpmc(int n){
  unsigned int low, high;
  
  asm volatile("rdpmc" : "=a" (low), "=d" (high) : "c" (n));
  return low | ((u64)high) << 32;
}



unsigned char enumber1, umask1, enumber2, umask2;
uint64_t begin_m0, end_m0, begin_m1, end_m1;
int pmc1, pmc2;
int dat1, dat2 = 15;


void single_round(){
  begin_m0 = rdpmc(0);
  begin_m1 = rdpmc(1);

  // intructions you want to measure with pmcs:

  dat1 = dat2 / 2;

  end_m0 = rdpmc(0); 
  end_m1 = rdpmc(1); // note pmc1 might also monitors partial pmc0 due to the order

  pmc1 = end_m0 - begin_m0;
  pmc2 = end_m1 - begin_m1;
}

int main(int argc, char ** argv) {
  if(argc != 5){
    printf("[usage] sudo ./pmc [enum1] [umask1] [enum2] [umask2]\n[example] sudo ./pmc 0xe 0x1 0xa1 0x80\n");
    exit(1);
  } else {
    enumber1 = strtol(argv[1],NULL,16);
    umask1 = strtol(argv[2],NULL,16);
    enumber2 = strtol(argv[3],NULL,16);
    umask2 = strtol(argv[4],NULL,16);
  }

  setaffinity(3);

  int msr_file1 = open_msr_file();
  write_msr(msr_file1, PMC_MSR0, 0UL);
  write_msr(msr_file1, PERFEVTSEL0, USER_SEL + (0x100 * umask1) + enumber1);

  int msr_file2 = open_msr_file();
  write_msr(msr_file2, PMC_MSR1, 0UL);
  write_msr(msr_file2, PERFEVTSEL1, USER_SEL + (0x100 * umask2) + enumber2);
  

  single_round();

  
  printf("[result] [pmc1 pmc2] = [%d %d]\n", pmc1, pmc2);

  return 0;
}




