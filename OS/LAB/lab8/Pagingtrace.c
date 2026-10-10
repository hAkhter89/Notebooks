#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 256
#define NUM_PAGES 16
#define NUM_FRAMES 2
#define NUM_TRACE 100

typedef struct {
  int frameNumber;
  int valid;
} PageTableEntry;

typedef struct {
  PageTableEntry pageTable[NUM_PAGES];
  int pid;
} Process;

typedef struct {
  int va;
  int vpn;
  int offset;
  int pageFault;
} TraceEntry;

int framePool[NUM_FRAMES];
int framePage[NUM_FRAMES];
TraceEntry trace[NUM_TRACE];
int traceCount = 0;
Process *frameOwner[NUM_FRAMES];
int nextevict = 0;

void initialize(Process *p, int pid) {
  p->pid = pid;
  for (int i = 0; i < NUM_PAGES; i++) {
    p->pageTable[i].valid = 0;
  }
}
void initializeFramePool() {
  for (int i = 0; i < NUM_FRAMES; i++) {
    framePool[i] = -1;
    framePage[i] = -1;
    frameOwner[i] = NULL;
  }
}

int findFreeFrame() {
  for (int i = 0; i < NUM_FRAMES; i++) {
    if (framePool[i] == -1) {
      return i;
    }
  }
  return -1;
}

void allocatePage(Process *p, int vpn, int frame) {
  p->pageTable[vpn].frameNumber = frame;
  p->pageTable[vpn].valid = 1;
  framePool[frame] = p->pid;
  framePage[frame] = vpn;
  frameOwner[frame] = p;
}
int evictFrame() {
  // FIFO to evict frames
  int evict = nextevict;
  nextevict = (nextevict + 1) % NUM_FRAMES;
  Process *owner = frameOwner[evict];
  int oldVpn = framePage[evict];
  printf("evicting [P&d:VPN%d] from frame %d\n", owner->pid, oldVpn, evict);
  owner->pageTable[oldVpn].valid = 0;
  framePool[evict] = -1;
  framePage[evict] = -1;
  frameOwner[evict] = NULL;
  return evict;
}

int translate(Process *p, int virtualAddress, int *fault) {
  *fault = 0;
  if (virtualAddress < 0 || virtualAddress >= NUM_PAGES * PAGE_SIZE) {
    printf("Invalid virtual Address %d\n", virtualAddress);
    return -1;
  }

  int vpn = virtualAddress / PAGE_SIZE;
  int offset = virtualAddress % PAGE_SIZE;

  if (p->pageTable[vpn].valid != 1) {
    *fault = 1;
    printf(
        "Panic: Room not found at VPN %d! Scrambling to build a new frame...\n",
        vpn);
    int newFrame = findFreeFrame();
    if (newFrame == -1) {
      newFrame = evictFrame();
    }
    allocatePage(p, vpn, newFrame);
  }

  int frame = p->pageTable[vpn].frameNumber;
  int physicalAddress = (frame * PAGE_SIZE) + offset;

  printf("virtualAddress %d, VPN %d, offset %d, frame %d, physicalAddress %d\n",
         virtualAddress, vpn, offset, frame, physicalAddress);
  return physicalAddress;
}

void memoryAccess(Process *p, int va) {
  int fault;
  printf("\nProcess %d accesses VA %d\n", p->pid, va);
  int pa = translate(p, va, &fault);
  if (pa == -1) {
    return;
  }

  if (traceCount < NUM_TRACE) {
    trace[traceCount].va = va;
    trace[traceCount].vpn = va / PAGE_SIZE;
    trace[traceCount].offset = va % PAGE_SIZE;
    trace[traceCount].pageFault = fault;
    traceCount++;
  }

  printf("Trace #%d: VA=%d VPN=%d offset=%d PA=%d (%s)\n", traceCount, va,
         va / PAGE_SIZE, va % PAGE_SIZE, pa, fault ? "PAGE FAULT" : "HIT");
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

void displayPhysicalMemory() {
  printf("\n--- Physical Memory Map ---\n");
  for (int i = 0; i < NUM_FRAMES; i++) {
    if (framePool[i] == -1) {
      printf("Frame %d: [FREE]\n", i);
    } else {
      printf("Frame %d: [P%d:VPN%d]\n", i, framePool[i], framePage[i]);
    }
  }
}

int main() {
  Process p1;
  initializeFramePool();
  initialize(&p1, 1);

  for (int i = 0; i < 5; i++) {
    for (int j = 1; j <= 3; j++) {
      memoryAccess(&p1, j * PAGE_SIZE);
    }
  }

  displayPhysicalMemory();
  return 0;
}
