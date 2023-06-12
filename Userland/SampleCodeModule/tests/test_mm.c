// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscallslib.h>
#include <test_util.h>
#include <ourlib.h>


#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

void print_mem(){
    unsigned int status[3];
    char aux[50];
    sys_mem_status(status);

    intToString(status[0],aux);
    printFirst("Heap Size : ");
    printf(aux);
    putchar('\n');
    intToString(status[1],aux);
    printFirst("Heap Left : ");
    printf(aux);
    putchar('\n');
    intToString(status[2],aux);
    printFirst("Used Heap : ");
    printf(aux);
    putchar('\n');
}

uint64_t test_mm(uint64_t argc, char argv[5][20]) {

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 2){ 
    printerr("wrong amount of arguments\n");
    sys_kill(sys_get_pid());
  }
  if ((max_memory = satoi(argv[1])) <= 0){
    printerr("Wrong mem amount\n");
        sys_kill(sys_get_pid());
    }

  printf("No errors yet\n");
  while (1) {
    rq = 0;
    total = 0;
    sys_sleep(1000);
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = sys_malloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }
    sys_sleep(1000);
    printFirst("SET\n");
    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        sys_memset(mm_rqs[i].address, i, mm_rqs[i].size);
    print_mem();

    sys_sleep(1000);
    printFirst("CHECK\n");
    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("test_mm ERROR\n");
          sys_kill(sys_get_pid());
        }
    print_mem();

    sys_sleep(1000);
    printFirst("FREE\n");
    // Free
    for (i = 0; i < rq; i++){
      if (mm_rqs[i].address)
        sys_free(mm_rqs[i].address);
    }
    print_mem();
    
  }
  sys_kill(sys_get_pid());
}
