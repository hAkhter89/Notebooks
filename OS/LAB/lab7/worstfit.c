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

void *WorstFit(size_t requestSize) {
  FreeBlock *prev = NULL;
  FreeBlock *curr = freeListHead;

  FreeBlock *worstprev = NULL;
  FreeBlock *worstcurr = NULL;
  size_t maxFound = 0;

  // Loop the entire free_list to find largest
  while (curr != NULL) {
    if (curr->size >= requestSize && curr->size > maxFound) {
      maxFound = curr->size;
      worstcurr = curr;
      worstprev = prev;
    }
    prev = curr;
    curr = curr->next;
  }
  if (worstcurr == NULL) {
    printf("Allocation Failed of %zu\n", requestSize);
    return NULL;
  }

  // Splitting
  if (worstcurr->size >= requestSize + sizeof(FreeBlock) + 1) {
    // casts the struct pointer to a 1-byte char*
    // to skip exactly past the header and requested data bytes,
    // pinpointing the start of the leftover memory before casting
    // that address back into a new FreeBlock* node.
    FreeBlock *newFreeBlock =
        (FreeBlock *)((char *)worstcurr + sizeof(FreeBlock) + requestSize);
    newFreeBlock->size = worstcurr->size - requestSize - sizeof(FreeBlock);
    newFreeBlock->next = worstcurr->next;

    if (worstprev == NULL) {
      freeListHead = newFreeBlock;
    } else {
      worstprev->next = newFreeBlock;
    }
    worstcurr->size = requestSize;
  } else {
    // Unlink the block entirely if it matches exactly (or close enough)
    if (worstprev == NULL) {
      freeListHead = worstcurr->next;
    } else {
      worstprev->next = worstcurr->next;
    }
  }
  //
  return (void *)((char *)worstcurr + sizeof(FreeBlock));
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
        // Check if 'curr' and 'check' are adjacent in memory
        if ((char *)curr + sizeof(FreeBlock) + curr->size == (char *)check) {
          curr->size += sizeof(FreeBlock) + check->size;

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
        break;
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
// TASK 3
// int main() {
//   printf("Initializing Memory Pool...\n");
//   initializeMemoryPool();
//   displayMemoryState();
//
//   // a. Allocate three blocks: 100, 200, and 300 bytes.
//   // b. Display the free list after each.
//   printf("Allocating 100 bytes...\n");
//   void *ptr1 = WorstFit(100);
//   displayMemoryState();
//
//   printf("Allocating 200 bytes...\n");
//   void *ptr2 = WorstFit(200);
//   displayMemoryState();
//
//   printf("Allocating 150 bytes...\n");
//   void *ptr3 = WorstFit(150);
//   displayMemoryState();
//
//   printf("Allocating 300 bytes...\n");
//   void *ptr4 = WorstFit(300);
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
//   void *ptr5 = WorstFit(150);
//   displayMemoryState();
//
//   coalesceFreeBlocks();
//   displayMemoryState();
//
//   return 0;
// }

int main() {
  printf("Memory Pool...\n");
  initializeMemoryPool();
  displayMemoryState();

  printf("Memory Exhaustion Test\n");
  // 400 bytes untill memory full
  int count = 1;
  void *ptr = NULL;

  while (1) {
    ptr = WorstFit(400);

    // Break the loop when the allocator returns NULL (memory is full)
    if (ptr == NULL) {
      printf("Memory pool exhausted.%d.\n", count);
      break;
    }
    count++;
  }

  printf("Memory State After Exhaustion \n");
  displayMemoryState();

  //  Attempt one more allocation.
  printf("Attempting one final allocation of 400 bytes...\n");
  void *finalPtr = WorstFit(400);

  if (finalPtr == NULL) {
    printf("Success, returned NULL.\n");
  }

  return 0;
}
