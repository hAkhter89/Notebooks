#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

/* ------------------------------------------------------------------ */
/* Task 7: the block header                                            */
/* ------------------------------------------------------------------ */

typedef struct MemBlock {
    size_t            size;      /* usable bytes AFTER this header      */
    int               is_free;   /* 0 = allocated, 1 = free             */
    struct MemBlock  *next;      /* next block in the global list       */
} MemBlock;

/* The payload starts right after the header, so the header size decides
 * the alignment of every pointer we hand out. sizeof(MemBlock) is 24 on
 * x86-64, which is not a multiple of 16, so round it up. malloc must
 * return memory aligned for any type; 16 covers long double and SSE.   */
#define ALIGNMENT      16UL
#define ALIGN_UP(x, a) (((size_t)(x) + (a) - 1) & ~((size_t)(a) - 1))
#define HEADER_SIZE    (ALIGN_UP(sizeof(MemBlock), ALIGNMENT))
#define MIN_PAYLOAD    ALIGNMENT
#define CHUNK_SIZE     (64UL * 1024UL)   /* grab 64 KiB at a time from the OS */

static MemBlock *head = NULL;            /* start of the block list */

/* ------------------------------------------------------------------ */
/* helpers                                                             */
/* ------------------------------------------------------------------ */

static size_t page_size(void)
{
    static size_t ps = 0;
    if (ps == 0) ps = (size_t) sysconf(_SC_PAGESIZE);
    return ps;
}

/* Are these two blocks physically next to each other? Two blocks can be
 * neighbours in the linked list but live in different mmap chunks, and
 * merging those would produce a block that spans unmapped memory.      */
static int adjacent(const MemBlock *a, const MemBlock *b)
{
    return (const char *) a + HEADER_SIZE + a->size == (const char *) b;
}

/* Task 8 (c.1): ask the kernel for more memory and append it as one big
 * free block at the tail of the list.                                  */
static MemBlock *request_from_os(size_t need)
{
    size_t total = ALIGN_UP(HEADER_SIZE + need, page_size());
    if (total < CHUNK_SIZE) total = CHUNK_SIZE;

    void *mem = mmap(NULL, total, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) return NULL;          /* OS call failed */

    MemBlock *block = (MemBlock *) mem;
    block->size    = total - HEADER_SIZE;
    block->is_free = 1;
    block->next    = NULL;

    if (head == NULL) {
        head = block;
    } else {
        MemBlock *cur = head;
        while (cur->next != NULL) cur = cur->next;
        cur->next = block;
    }

    printf("      [os] mmap %6zu bytes at %p\n", total, mem);
    return block;
}

/* Task 8 (b.1): if the block is much larger than requested, cut it in two
 * and leave the tail free. Only split when the leftover can hold a header
 * plus a useful payload, otherwise we create unusable slivers.         */
static void split_block(MemBlock *block, size_t size)
{
    if (block->size < size + HEADER_SIZE + MIN_PAYLOAD)
        return;

    MemBlock *rest = (MemBlock *) ((char *) block + HEADER_SIZE + size);
    rest->size    = block->size - size - HEADER_SIZE;
    rest->is_free = 1;
    rest->next    = block->next;

    block->size = size;
    block->next = rest;
}

/* ------------------------------------------------------------------ */
/* Task 8: my_malloc                                                    */
/* ------------------------------------------------------------------ */

void *my_malloc(size_t size)
{
    if (size == 0) return NULL;                 /* edge case */

    size = ALIGN_UP(size, ALIGNMENT);

    /* (a) first fit: walk the list looking for a free block big enough */
    MemBlock *cur = head;
    while (cur != NULL) {
        if (cur->is_free && cur->size >= size) break;
        cur = cur->next;
    }

    /* (c.1) nothing suitable: get a fresh chunk from the kernel */
    if (cur == NULL) {
        cur = request_from_os(size);
        if (cur == NULL) return NULL;           /* OS call failed */
    }

    split_block(cur, size);                     /* (b.1) */
    cur->is_free = 0;                           /* (b.2) */
    return (char *) cur + HEADER_SIZE;          /* (b.3) usable memory */
}

/* ------------------------------------------------------------------ */
/* Task 9: my_free                                                      */
/* ------------------------------------------------------------------ */

void my_free(void *ptr)
{
    if (ptr == NULL) return;                    /* edge case: free(NULL) */

    /* (a) step back over the header to find the block record */
    MemBlock *block = (MemBlock *) ((char *) ptr - HEADER_SIZE);

    /* (b) mark it free */
    block->is_free = 1;

    /* (c) coalescing, forwards: swallow every free neighbour that sits
     *     immediately after this one.                                   */
    while (block->next != NULL && block->next->is_free &&
           adjacent(block, block->next)) {
        block->size += HEADER_SIZE + block->next->size;
        block->next  = block->next->next;
    }

    /* Coalescing backwards. The lab only asks for the forward merge, but
     * without this you still fragment: free(A) then free(B) where B sits
     * before A in memory would leave two separate free blocks.          */
    MemBlock *prev = NULL, *cur = head;
    while (cur != NULL && cur != block) { prev = cur; cur = cur->next; }
    if (prev != NULL && prev->is_free && adjacent(prev, block)) {
        prev->size += HEADER_SIZE + block->size;
        prev->next  = block->next;
    }
}

/* ------------------------------------------------------------------ */
/* Task 10: tests                                                       */
/* ------------------------------------------------------------------ */

static void dump_heap(const char *label)
{
    printf("\n--- %s ---\n", label);
    int i = 0;
    size_t free_bytes = 0, used_bytes = 0;
    for (MemBlock *cur = head; cur != NULL; cur = cur->next, i++) {
        printf("  block %-2d hdr=%p payload=%p size=%7zu  %s\n",
               i, (void *) cur, (void *) ((char *) cur + HEADER_SIZE),
               cur->size, cur->is_free ? "FREE" : "USED");
        if (cur->is_free) free_bytes += cur->size; else used_bytes += cur->size;
    }
    printf("  (%d block(s), %zu bytes used, %zu bytes free)\n",
           i, used_bytes, free_bytes);
}

int main(void)
{
    printf("sizeof(MemBlock) = %zu, HEADER_SIZE (aligned) = %zu\n",
           sizeof(MemBlock), HEADER_SIZE);

    /* ---------- (a) normal cases ---------- */

    puts("\n=== 1. allocate small / medium / large ===");
    char *small  = my_malloc(32);
    char *medium = my_malloc(500);
    char *large  = my_malloc(4000);
    printf("  small  = %p\n  medium = %p\n  large  = %p\n",
           (void *) small, (void *) medium, (void *) large);

    strcpy(small, "hello allocator");
    memset(medium, 'A', 500);
    memset(large,  'B', 4000);
    printf("  small holds: \"%s\"\n", small);
    dump_heap("after 3 allocations");

    puts("\n=== 2. free in REVERSE order of allocation ===");
    my_free(large);
    my_free(medium);
    my_free(small);
    dump_heap("after freeing large, medium, small");
    puts("  everything coalesced back into one big free block.");

    puts("\n=== 3. reuse check: allocate again, expect no new mmap ===");
    char *again = my_malloc(32);
    printf("  my_malloc(32) = %p   (first time it was %p)\n",
           (void *) again, (void *) small);
    printf("  reused the freed space: %s\n", again == small ? "YES" : "NO");
    my_free(again);

    puts("\n=== 4. free in MIXED order ===");
    void *a = my_malloc(64);
    void *b = my_malloc(128);
    void *c = my_malloc(256);
    dump_heap("allocated a, b, c");

    my_free(b);                       /* hole in the middle */
    dump_heap("freed b (middle) - note the hole: fragmentation");

    my_free(a);                       /* should merge with b's hole */
    dump_heap("freed a - a and b coalesce");

    my_free(c);                       /* should merge with everything */
    dump_heap("freed c - one free block again");

    /* ---------- (b) bad cases ---------- */

    puts("\n=== 5. edge cases ===");
    void *zero = my_malloc(0);
    printf("  my_malloc(0)  -> %p  (expected (nil))\n", zero);

    my_free(NULL);
    puts("  my_free(NULL) -> returned without crashing");

    void *huge = my_malloc((size_t) -1 / 2);   /* absurd request */
    printf("  my_malloc(huge) -> %p  (expected (nil), mmap refuses)\n", huge);

    dump_heap("final state");

    puts("\nNote: this allocator never munmaps. Real free() is allowed to\n"
         "return memory to the kernel, but it usually keeps it too.");
    return 0;
}
