#include <syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <dlfcn.h>

#define __NR_get_phys_addr 442

#define NUMBER_OF_STRING 7
#define MAX_STRING_SIZE 40


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct AddrInfo {
    unsigned long int virt_addr;
    unsigned long int phys_addr;
};

unsigned long get_phys_addr(unsigned long int virtual_address) {
    unsigned long int physical_address;
    syscall(__NR_get_phys_addr, &virtual_address, &physical_address);
    return physical_address;
}

unsigned long get_shr_mem_addr() {
    void *fhandle;
    //load shared library:"/usr/lib/x86_64-linux-gnu/libc-2.31.so"
    fhandle = dlopen("/usr/lib/x86_64-linux-gnu/libc-2.31.so", RTLD_LAZY);

    void (*func)();
    unsigned long addr;
    if(!fhandle){
        fprintf(stderr, "%s\n", dlerror());
        return 0;
    }
    func = (void(*)())dlsym(fhandle,"printf"); //(void(*)()) 
    addr = func;
    return addr;
}

//Data Segments//TLS(thread local storage)
__thread int var = 1;

//Data Segments(initialized)//Global
//int MyDataInData = 100;

unsigned long *MyPhysicalAddress[7];

void* worker(void *address_main) {
    printf("\n\n");
    //stack(,where automatic variables are stored, along with information that is saved each time a function is called)
    //auto variable e.g.int i = 0; 
    //static variable e.g. static int i = 0;(stored in data segment(initialized)), static int i;(stored in BSS)
    int MyDataInStack = 1000;
    //BSS(uninitialized data segment)
    int MyDataInBSS;
    //Heap(is the segment where dynamic memory allocation usually take place, managed by malloc, realloc, free, 
    //shared by all shared libraries and dynamically loaded modules in a process)
    int *MyDataInHeap = malloc(sizeof(int));
    *MyDataInHeap = 100;
    //libraries(shard library)
    void* shr_address = get_shr_mem_addr();
    //Data Segments(initialized)//local
    static int MyDataInData = 100;
    
    unsigned long MyVirtualAddress[7];
    MyVirtualAddress[0] = &var;
    MyVirtualAddress[1] = &MyDataInStack;
    MyVirtualAddress[2] = &MyDataInBSS;
    MyVirtualAddress[3] = MyDataInHeap;
    MyVirtualAddress[4] = shr_address;
    MyVirtualAddress[5] = &MyDataInData;
    MyVirtualAddress[6] = address_main;

    printf("Convert virtual addresses to physical addresses \n");
    char mesg_arr[NUMBER_OF_STRING][MAX_STRING_SIZE] =
    {"TLS address",
        "Stack address",
        "BSS address",
        "Heap address",
        "Share library address",
        "Data address",
        "Code address"
        };
    
    for(int i = 0; i < 7; i++){
        puts(mesg_arr[i]);
        printf("Virtual address : %lx ===>", MyVirtualAddress[i]);
        printf("Physical address %lx\n", get_phys_addr(MyVirtualAddress[i]));
    }
}

int main() {
    pthread_t pid1, pid2, pid3;
    pthread_create(&pid1, NULL, worker, main);
    sleep(1);
    pthread_create(&pid2, NULL, worker, main);
    sleep(1);
    pthread_create(&pid3, NULL, worker, main);
    sleep(1);
    char thread_name_1[] = "Thread1";
    char thread_name_2[] = "Thread2";
    char thread_name_3[] = "Thread3";

    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);
    pthread_join(pid3, NULL);


    return 0;
}
