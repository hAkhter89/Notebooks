#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_POOL_SIZE 4096
#define CHECK_BOUNDS 0 // flip to 0 to see the "virus" succeed

typedef struct FreeBlock {
  size_t size;
  struct FreeBlock *next;
} FreeBlock;

char memoryPool[MEMORY_POOL_SIZE];
FreeBlock *freeListHead = NULL;

void initializeMemoryPool() {
  freeListHead = (FreeBlock *)memoryPool;
  freeListHead->size = MEMORY_POOL_SIZE - sizeof(FreeBlock);
  freeListHead->next = NULL;
}

/* Minimal Worst Fit allocator (Task 1d): splits the largest block that fits */
void *worstFitAlloc(size_t requestSize) {
  FreeBlock *prev = NULL, *curr = freeListHead;
  FreeBlock *worstPrev = NULL, *worst = NULL;

  while (curr != NULL) {
    if (curr->size >= requestSize &&
        (worst == NULL || curr->size > worst->size)) {
      worst = curr;
      worstPrev = prev;
    }
    prev = curr;
    curr = curr->next;
  }

  if (worst == NULL)
    return NULL;

  if (worst->size > requestSize + sizeof(FreeBlock)) {
    FreeBlock *remainder =
        (FreeBlock *)((char *)worst + sizeof(FreeBlock) + requestSize);
    remainder->size = worst->size - requestSize - sizeof(FreeBlock);
    remainder->next = worst->next;
    if (worstPrev)
      worstPrev->next = remainder;
    else
      freeListHead = remainder;
    worst->size = requestSize;
  } else {
    if (worstPrev)
      worstPrev->next = worst->next;
    else
      freeListHead = worst->next;
  }
  return (void *)((char *)worst + sizeof(FreeBlock));
}

/* Task 1e: walk back to the block's header, then push it onto the free list */
void freeMemory(void *ptr) {
  FreeBlock *block = (FreeBlock *)((char *)ptr - sizeof(FreeBlock));
  block->next = freeListHead;
  freeListHead = block;
}

void displayFreeList() {
  FreeBlock *curr = freeListHead;
  printf(" Free list: ");
  while (curr) {
    printf("[size=%zu @%p] -> ", curr->size, (void *)curr);
    curr = curr->next;
  }
  printf("NULL\n");
}

/* A write that is supposed to stay inside its own allocation */
void safeWrite(void *userPtr, size_t allocatedSize, const char *data,
               size_t len) {
#if CHECK_BOUNDS
  if (len > allocatedSize) {
    printf(" [BLOCKED] write of %zu bytes exceeds %zu-byte allocation\n", len,
           allocatedSize);
    return;
  }
#endif
  memcpy(userPtr, data, len);
  printf(" [ALLOWED] wrote %zu bytes into a %zu-byte allocation\n", len,
         allocatedSize);
}

int main() {
  initializeMemoryPool();
  printf("=== Heap Overflow Virus Demo (bounds=%s) ===\n",
         CHECK_BOUNDS ? "ON" : "OFF");

  void *blockA = worstFitAlloc(32);
  void *blockB = worstFitAlloc(64);

  // the attacker's own, legitimate buffer
  // the victim: its header sits right after A's payload
  printf("\nblockA payload = %p (32 bytes) \n", blockA);
  printf("blockB header = %p (right after blockA's payload) \n",
         (void *)((char *)blockA + 32));

  printf("\n--- Free list before overflow ---\n");
  displayFreeList();

  /* ATTACK: write into blockA, but ask for more bytes than it was ever
     given. The overrun lands squarely on blockB's FreeBlock header --
     its size and next fields without ever touching blockB's pointer. */
  char payload[100];
  memset(payload, 'A', sizeof(payload));

  printf("\n[ATTACK] Overflowing blockA (32 bytes) with a 100-byte write\n");
  safeWrite(blockA, 32, payload, sizeof(payload));

  /* The victim frees blockB like ordinary code would. It has no idea
     its header was just overwritten by its neighbor. */
  freeMemory(blockB);

  printf("\n--- Free list after blockB is freed ---\n");
  displayFreeList();

  return 0;
}
