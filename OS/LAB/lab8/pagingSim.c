#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 256
#define NUM_PAGES 16
#define NUM_FRAMES 8

typedef struct {
  int frameNumber;
  int valid;
} PageTableEntry;

typedef struct {
  PageTableEntry pageTable[NUM_PAGES];
  int pid;
} Process;

int framePool[NUM_FRAMES];

void initialize(Process *p, int pid) {
  p->pid = pid;
  for (int i = 0; i < NUM_PAGES; i++) {
    p->pageTable[i].valid = 0;
  }
}
void initializeFramePool() {
  for (int i = 0; i < NUM_FRAMES; i++) {
    framePool[i] = -1;
  }
}

int translate(Process *p, int virtualAddress) {
  if (virtualAddress < 0 || virtualAddress >= NUM_PAGES * PAGE_SIZE) {
    printf("Invalid virtual Address %d\n", virtualAddress);
    return -1;
  }

  int vpn = virtualAddress / PAGE_SIZE;
  int offset = virtualAddress % PAGE_SIZE;

  if (p->pageTable[vpn].valid != 1) {
    printf("VPN is invalid %d\n", vpn);
    return -1;
  }
  int frame = p->pageTable[vpn].frameNumber;
  int physicalAddress = (frame * PAGE_SIZE) + offset;

  printf("virtualAddress %d, VPN %d, offset %d, frame %d, physicalAddress %d\n",
         virtualAddress, vpn, offset, frame, physicalAddress);
  return physicalAddress;
}

void allocatePage(Process *p, int vpn, int frame) {
  p->pageTable[vpn].frameNumber = frame;
  p->pageTable[vpn].valid = 1;
  framePool[frame] = p->pid;
}

void displayPageTable(Process *p) {
  printf("Page Table for Procces %d\n", p->pid);
  for (int i = 0; i < NUM_PAGES; i++) {
    if (p->pageTable[i].valid) {
      printf("VPN %d -> Frame %d\n", i, p->pageTable[i].frameNumber);
    } else {
      printf("VPN %d -> INVALID\n", i);
    }
  }
}

int main() {
  Process p1;
  initializeFramePool();
  initialize(&p1, 1);

  allocatePage(&p1, 1, 4);
  allocatePage(&p1, 2, 6);

  displayPageTable(&p1);

  int pa = translate(&p1, 512);
  printf("physicalAddress = %d\n", pa);

  return 0;
}
