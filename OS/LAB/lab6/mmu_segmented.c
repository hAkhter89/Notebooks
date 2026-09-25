#include <stdint.h>
#include <stdio.h>

#define MAX_PROCESSES 2
#define MEMORY_SIZE 8192

// Segment types
#define SEG_CODE 0
#define SEG_DATA 1
#define SEG_HEAP 2
#define SEG_STACK 3
#define SEGMENT_COUNT 4

// Error codes
#define SUCCESS 0
#define ERROR_BOUNDS -1
#define ERROR_INVALID_PID -2
#define ERROR_INVALID_SEGMENT -3

// Global segment names for readability
char *seg_names[] = {"code", "data", "heap", "stack"};

struct segment {
  int base;
  int bounds; // also called size
};

struct process {
  int pid;
  struct segment segments[SEGMENT_COUNT];
};

uint8_t memory[MEMORY_SIZE];
struct process proc_list[MAX_PROCESSES];

void init_segmented_processes() {
  // Process 1 (PID = 1) Code: (base = 1000, size 400)
  // Data (6500, 200); Heap (7000, 500); Stack (4000, 500)
  proc_list[0].pid = 1;
  proc_list[0].segments[0].base = 1000;
  proc_list[0].segments[0].bounds = 400;
  proc_list[0].segments[1].base = 6500;
  proc_list[0].segments[1].bounds = 200;
  proc_list[0].segments[2].base = 7000;
  proc_list[0].segments[2].bounds = 500;
  proc_list[0].segments[3].base = 4000;
  proc_list[0].segments[3].bounds = 500;

  // Process 2 (PID = 2)
  // Code (6000, 300); Data (3500, 100); Heap (2000, 600); Stack (3000, 400)
  proc_list[1].pid = 2;
  proc_list[1].segments[0].base = 6000;
  proc_list[1].segments[0].bounds = 300;
  proc_list[1].segments[1].base = 3500;
  proc_list[1].segments[1].bounds = 100;
  proc_list[1].segments[2].base = 2000;
  proc_list[1].segments[2].bounds = 600;
  proc_list[1].segments[3].base = 3000;
  proc_list[1].segments[3].bounds = 400;

  // Print each process, prints base and bounds for all 4 segments.
  // Print Layout
  printf("Memory Layout:\n");
  for (int i = 0; i < MAX_PROCESSES; i++)
    printf("Process %d: Code=%d, %d, Data=%d, %d, Heap=%d, %d, Stack=%d,%d\n",
           proc_list[i].pid, proc_list[i].segments[0].base,
           proc_list[i].segments[0].bounds, proc_list[i].segments[1].base,
           proc_list[i].segments[1].bounds, proc_list[i].segments[2].base,
           proc_list[i].segments[2].bounds, proc_list[i].segments[3].base,
           proc_list[i].segments[3].bounds);
}

int segmented_translate(int pid, int segment, int offset,
                        int *physical_address) {
  // Return (0) on success, & place the physical address in physical_address
  // Return ERROR_INVALID_PID (-2) if PID not found
  // Return ERROR_INVALID_SEGMENT (-3) if segment ID is invalid
  // Return ERROR_BOUNDS (-1) if offset is out of bounds

  /* NOTE FROM INSTRUCTIONS:
     Code, Data, and Heap grow positively.
     Stack grows negatively (subtract offset from base). */
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (proc_list[i].pid == pid) {

      if (segment < 0 || segment > 3) {
        return -3;
      }

      if (offset >= 0 && offset < proc_list[i].segments[segment].bounds) {
        if (segment == 3) {
          *physical_address = proc_list[i].segments[3].base - offset;
        } else {
          *physical_address = proc_list[i].segments[segment].base + offset;
        }
        return 0;
      }
      return -1;
    }
  }
  return -2;
}

void test_segment_access(int pid, int segment, int offset, uint8_t value) {
  int physical_address, result;
  result = segmented_translate(pid, segment, offset, &physical_address);

  switch (result) {
  case SUCCESS:
    printf("[OK] PID %d: SEG %s offset=%d PA=%d\n", pid, seg_names[segment],
           offset, physical_address);
    break;
  case ERROR_INVALID_PID:
    printf("[ERROR] PID %d not found\n", pid);
    break;
  case ERROR_INVALID_SEGMENT:
    printf("[ERROR] Invalid segment ID: %d\n", segment);
    break;
  case ERROR_BOUNDS:
    printf("[FAULT] PID %d: Out of bounds\n", pid);
    break;
  }
}

int main() {
  init_segmented_processes();

  // Required test cases
  test_segment_access(1, SEG_CODE, 206, -1);  // Should succeed PA=1206
  test_segment_access(1, SEG_HEAP, 350, 66);  // Should succeed PA=7350
  test_segment_access(1, SEG_STACK, 500, 67); // Should fail (bounds)
  test_segment_access(2, SEG_CODE, 299, -1);  // Should succeed PA=6299
  test_segment_access(3, SEG_CODE, 0, 99);    // Should fail (invalid PID)
  test_segment_access(1, 5, 0, 88);           // Should fail (invalid segment)

  return 0;
}
