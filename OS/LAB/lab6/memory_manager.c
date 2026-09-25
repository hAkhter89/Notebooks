#include <stdio.h>
#include <string.h>

#define MAX_PROCESSES 5
#define MEMORY_SIZE 2048

// a
struct segment {
  int base, limit;
};

struct process {
  int pid, is_active;
  struct segment code, data, heap, stack; // 'data' is unused in shell commands
};

// b
struct {
  struct process processes[MAX_PROCESSES];
  char memory[MEMORY_SIZE];
  int next_address; // Tracks the next available address
} manager;

// c
void init_manager() {
  for (int i = 0; i < MAX_PROCESSES; i++) {
    manager.processes[i].is_active = 0;
  }
  manager.next_address = 0;
}

// d
int create_process(int pid, int code_size, int heap_size, int stack_size) {
  int total_size = code_size + heap_size + stack_size;

  // Check if there is enough memory
  if (manager.next_address + total_size > MEMORY_SIZE) {
    return -1;
  }

  // Find an inactive slot in the process list
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (!manager.processes[i].is_active) {
      manager.processes[i].pid = pid;
      manager.processes[i].is_active = 1;

      // Allocate Code
      manager.processes[i].code.base = manager.next_address;
      manager.processes[i].code.limit = code_size;
      manager.next_address += code_size;

      // Allocate Heap
      manager.processes[i].heap.base = manager.next_address;
      manager.processes[i].heap.limit = heap_size;
      manager.next_address += heap_size;

      // Allocate Stack
      manager.processes[i].stack.base = manager.next_address;
      manager.processes[i].stack.limit = stack_size;
      manager.next_address += stack_size;

      return 0; // Success
    }
  }
  return -1; // No empty process slots available
}

// e
int terminate_process(int pid) {
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (manager.processes[i].is_active && manager.processes[i].pid == pid) {
      manager.processes[i].is_active =
          0; // Mark inactive (memory not reclaimed)
      return 0;
    }
  }
  return -1;
}

// f
void show_memory_map() {
  printf("=== Memory Map ===\n");
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (manager.processes[i].is_active) {
      struct process p = manager.processes[i];
      printf("Process %d:\n", p.pid);
      printf("  code:  [%d-%d] size = %d\n", p.code.base,
             p.code.base + p.code.limit - 1, p.code.limit);
      printf("  heap:  [%d-%d] size = %d\n", p.heap.base,
             p.heap.base + p.heap.limit - 1, p.heap.limit);
      printf("  stack: [%d-%d] size = %d\n", p.stack.base,
             p.stack.base + p.stack.limit - 1, p.stack.limit);
    }
  }
}

// g
void list_processes() {
  printf("=== Active Processes ===\n");
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (manager.processes[i].is_active) {
      printf("PID %d\n", manager.processes[i].pid);
    }
  }
}

//
int main() {
  init_manager();
  char command[32];

  while (1) {
    printf("> ");
    if (scanf("%s", command) != 1)
      break;

    if (strcmp(command, "create") == 0) {
      int pid, code, heap, stack;
      scanf("%d %d %d %d", &pid, &code, &heap, &stack);
      if (create_process(pid, code, heap, stack) == 0) {
        printf("Process %d created\n", pid);
      } else {
        printf("Error: Insufficient memory or process limit reached.\n");
      }
    } else if (strcmp(command, "terminate") == 0) {
      int pid;
      scanf("%d", &pid);
      if (terminate_process(pid) == 0) {
        printf("Process %d terminated\n", pid);
      } else {
        printf("Error: Process %d not found.\n", pid);
      }
    } else if (strcmp(command, "map") == 0 || strcmp(command, "mem") == 0) {
      show_memory_map();
    } else if (strcmp(command, "list") == 0) {
      list_processes();
    } else if (strcmp(command, "exit") == 0) {
      break;
    } else {
      printf("Unknown command.\n");
    }
  }
  return 0;
}
