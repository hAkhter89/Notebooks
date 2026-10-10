#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PAGE_SIZE 256
#define NUM_PAGES 16
#define NUM_FRAMES 8    // frames the OS is actually allowed to hand out
#define TOTAL_FRAMES 32 // the physical RAM chip has more capacity than that

#define CHECK_PTE 0 // flip to 0 to see the "virus" succeed

typedef struct {
  int frameNumber;
  int valid;
} PageTableEntry;

typedef struct {
  PageTableEntry pageTable[NUM_PAGES];
  int pid;
} Process;

int framePool[NUM_FRAMES]; // -1 = free, else owning pid
uint8_t physicalMemory[TOTAL_FRAMES * PAGE_SIZE];

void initialize(Process *p, int pid) {
  p->pid = pid;
  for (int i = 0; i < NUM_PAGES; i++)
    p->pageTable[i].valid = 0;
}

void initializeFramePool() {
  for (int i = 0; i < NUM_FRAMES; i++)
    framePool[i] = -1;
}

/* Only path a well-behaved process is supposed to use to get a frame */
void allocatePage(Process *p, int vpn, int frame) {
  p->pageTable[vpn].frameNumber = frame;
  p->pageTable[vpn].valid = 1;
  framePool[frame] = p->pid;
}

/* translate(): the lab's version only checks "valid". Here we add the
 * ownership + bounds checks a real MMU/OS has to enforce, guarded so you
 * can switch them off and see exactly what that buys an attacker. */
int translate(Process *p, int vpn, int *physicalAddress) {
  if (!p->pageTable[vpn].valid)
    return -1; // page fault

  int frame = p->pageTable[vpn].frameNumber;

#if CHECK_PTE
  if (frame < 0 || frame >= NUM_FRAMES) {
    printf("  [BLOCKED] frame %d is outside the %d frames the OS ever handed "
           "out\n",
           frame, NUM_FRAMES);
    return -2; // bounds check
  }
  if (framePool[frame] != p->pid) {
    printf("  [BLOCKED] frame %d belongs to PID %d, not PID %d\n", frame,
           framePool[frame], p->pid);
    return -3; // ownership check
  }
#endif

  *physicalAddress = frame * PAGE_SIZE;
  return 0;
}

int main() {
  initializeFramePool();
  printf("=== Rogue Page Table Entry Demo (PTE checks=%s) ===\n",
         CHECK_PTE ? "ON" : "OFF");

  Process victim, attacker;
  initialize(&victim, 1);
  initialize(&attacker, 2);

  /* Victim legitimately owns frame 2 and keeps a secret there */
  allocatePage(&victim, 0, 2);
  memcpy(&physicalMemory[2 * PAGE_SIZE], "VICTIM_SECRET", 14);

  /* Attacker legitimately owns frame 5 -- nothing wrong so far */
  allocatePage(&attacker, 0, 5);

  printf("\n[ATTACK 1] Attacker forges its own PTE to point at the victim's "
         "frame\n");
  attacker.pageTable[1].frameNumber =
      2; // victim's frame, never allocated to attacker!
  attacker.pageTable[1].valid = 1; // bypasses allocatePage() entirely

  int pa;
  if (translate(&attacker, 1, &pa) == 0) {
    printf("  [ALLOWED] PA=%d -> \"%s\"\n", pa, (char *)&physicalMemory[pa]);
  }

  printf("\n[ATTACK 2] Attacker forges a frame number the OS never handed to "
         "anyone\n");
  attacker.pageTable[2].frameNumber =
      20; // >= NUM_FRAMES, but still inside physicalMemory[]
  attacker.pageTable[2].valid = 1;

  if (translate(&attacker, 2, &pa) == 0) {
    printf("  [ALLOWED] PA=%d (reaching into never-allocated physical "
           "memory)\n",
           pa);
  }

  return 0;
}
