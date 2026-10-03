#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct FreeBlock {
  size_t size;
  struct FreeBlock *next;
} FreeBlock;

FreeBlock *freeListHead = NULL;

#define MEMORY_POOL_SIZE 4096
char memoryPool[MEMORY_POOL_SIZE];

void initializeMemoryPool() {
  freeListHead = (FreeBlock *)memoryPool;
  freeListHead->size = MEMORY_POOL_SIZE - sizeof(FreeBlock);
  freeListHead->next = NULL;
}

void *BestFit(size_t requestSize) {
  FreeBlock *prev = NULL;
  FreeBlock *curr = freeListHead;

  FreeBlock *bestprev = NULL;
  FreeBlock *bestcurr = NULL;

  // Loop the entire free_list to find the smallest suitable block
  while (curr != NULL) {
    if (curr->size >= requestSize) {
      // Update if this is the first block found that fits, or if it is smaller
      // than the current best fit
      if (bestcurr == NULL || curr->size < bestcurr->size) {
        bestcurr = curr;
        bestprev = prev;
      }
    }
    prev = curr;
    curr = curr->next;
  }

  if (bestcurr == NULL) {
    printf("Allocation Failed of %zu\n", requestSize);
    return NULL;
  }

  // Splitting
  if (bestcurr->size >= requestSize + sizeof(FreeBlock) + 1) {
    FreeBlock *newFreeBlock =
        (FreeBlock *)((char *)bestcurr + sizeof(FreeBlock) + requestSize);
    newFreeBlock->size = bestcurr->size - requestSize - sizeof(FreeBlock);
    newFreeBlock->next = bestcurr->next;

    if (bestprev == NULL) {
      freeListHead = newFreeBlock;
    } else {
      bestprev->next = newFreeBlock;
    }
    bestcurr->size = requestSize;
  } else {
    // Unlink the block entirely if it matches exactly (or close enough)
    if (bestprev == NULL) {
      freeListHead = bestcurr->next;
    } else {
      bestprev->next = bestcurr->next;
    }
  }
  //
  return (void *)((char *)bestcurr + sizeof(FreeBlock));
}

void freeMemory(void *ptr) {
  if (ptr == NULL)
    return;
  // locate header of block to free
  FreeBlock *blockToFree = (FreeBlock *)((char *)ptr - sizeof(FreeBlock));
  // insert freed block at start of list
  blockToFree->next = freeListHead;
  freeListHead = blockToFree;
}

void coalesceFreeBlocks() {
  int merged = 1;
  while (merged) {
    merged = 0;
    FreeBlock *curr = freeListHead;
    while (curr != NULL) {
      FreeBlock *check = freeListHead;
      FreeBlock *prevCheck = NULL;

      while (check != NULL) {
        // Check if the memory of 'curr' ends exactly where 'check' begins
        if ((char *)curr + sizeof(FreeBlock) + curr->size == (char *)check) {
          // Expand 'curr' to absorb 'check' and its header
          curr->size += sizeof(FreeBlock) + check->size;

          // Unlink 'check' from the free list
          if (prevCheck == NULL) {
            freeListHead = check->next;
          } else {
            prevCheck->next = check->next;
          }
          merged = 1;
          break;
        }
        prevCheck = check;
        check = check->next;
      }
      if (merged)
        break; // Restart scan from the head to catch cascade merges
      curr = curr->next;
    }
  }
}

void displayMemoryState() {
  printf("--- Free List Status ---\n");
  FreeBlock *curr = freeListHead;
  int count = 0;
  while (curr != NULL) {
    printf("  Block %d: Address = %p, Size = %zu bytes\n", count, (void *)curr,
           curr->size);
    curr = curr->next;
    count++;
  }
  if (count == 0) {
    printf("  No free blocks available.\n");
  }
  printf("------------------------\n\n");
}

// int main() {
//   printf("Initializing Memory Pool...\n");
//   initializeMemoryPool();
//   displayMemoryState();
//
//   printf("Allocating 100 bytes...\n");
//   void *ptr1 = BestFit(100);
//   displayMemoryState();
//
//   printf("Allocating 200 bytes...\n");
//   void *ptr2 = BestFit(200);
//   displayMemoryState();
//
//   printf("Allocating 300 bytes...\n");
//   void *ptr3 = BestFit(300);
//   displayMemoryState();
//
//   printf("Freeing 200 bytes (Address: %p)...\n", ptr2);
//   freeMemory(ptr2);
//   displayMemoryState();
//
//   printf("Allocating 150 bytes...\n");
//   void *ptr4 = BestFit(150);
//   displayMemoryState();
//
//   return 0;
// }

// int main() {
//   printf("Initializing Memory Pool...\n");
//   initializeMemoryPool();
//   displayMemoryState();
//
//   // a. Allocate three blocks: 100, 200, and 300 bytes.
//   // b. Display the free list after each.
//   printf("Allocating 100 bytes...\n");
//   void *ptr1 = BestFit(100);
//   displayMemoryState();
//
//   printf("Allocating 200 bytes...\n");
//   void *ptr2 = BestFit(200);
//   displayMemoryState();
//
//   printf("Allocating 150 bytes...\n");
//   void *ptr3 = BestFit(150);
//   displayMemoryState();
//
//   printf("Allocating 300 bytes...\n");
//   void *ptr4 = BestFit(300);
//   displayMemoryState();
//
//   // c. Free the second block (200 bytes).
//   printf("Freeing the second block (200 bytes)...\n");
//   freeMemory(ptr2);
//   displayMemoryState();
//
//   printf("Freeing the third block (150 bytes)...\n");
//   freeMemory(ptr3);
//   displayMemoryState();
//
//   // d. Allocate a new 150-byte block.
//   printf("Allocating 150 bytes...\n");
//   void *ptr5 = BestFit(140);
//   displayMemoryState();
//
//    coalesceFreeBlocks();
//    displayMemoryState();
//
//   return 0;
// }
//
int main() {
  printf("Initializing Memory Pool...\n");
  initializeMemoryPool();
  displayMemoryState();

  // a. Allocate three blocks: 100, 200, and 300 bytes.
  // b. Display the free list after each.
  printf("Allocating 100 bytes...\n");
  void *ptr1 = BestFit(100);
  displayMemoryState();

  printf("Allocating 200 bytes...\n");
  void *ptr2 = BestFit(200);
  displayMemoryState();

  printf("Allocating 150 bytes...\n");
  void *ptr3 = BestFit(150);
  displayMemoryState();

  // c. Free the second block (200 bytes).
  printf("Freeing the first block (100 bytes)...\n");
  freeMemory(ptr1);
  displayMemoryState();

  printf("Freeing the third block (150 bytes)...\n");
  freeMemory(ptr3);
  displayMemoryState();

  // d. Allocate a new 150-byte block.
  printf("Allocating 80 bytes...\n");
  void *ptr5 = BestFit(80);
  displayMemoryState();

  coalesceFreeBlocks();
  displayMemoryState();

  return 0;
}
