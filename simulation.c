//  CITS2002 Project 2 2024
//  Student1:   24295462   Yashwardhan Laharia
//  Student2:   24264717   Zeel Vavliya

// Included libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define RAM_SIZE 16
#define PAGE_SIZE 2
#define NUM_PROCESSES 5
#define PAGES_PER_PROCESS 4
#define VIRTUAL_MEMORY_SIZE (NUM_PROCESSES * PAGES_PER_PROCESS * PAGE_SIZE)

// Structures
typedef struct
{
    int process_id;
    int page_num;
    int last_accessed;
} memory;

// Function prototypes
void initialize_virtual_memory(memory *virtual_memory[]);
void initialize_page_tables(int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS]);
void simulate(memory *virtual_memory[], memory *RAM[], int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS], char *input_filename);
int find_free_frame(memory *RAM[]);
void update_ram(memory *RAM[], memory *virtual_memory[], int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS], int process_id, int page_num, int time_step);
void evict_page(memory *RAM[], int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS], int process_id, int time_step, memory *virtual_memory[], int page_num);
void print_page_tables(int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS], FILE *output_file);
void print_ram(memory *RAM[], FILE *output_file);

// Main function
int main(int argc, char *argv[])
{
    if (argc != 3) // Check if correct number of arguments
    {
        printf("Usage: simulation in.txt out.txt\n"); // Print usage message
        return 1;
    }
    // Allocate memory for virtual memory, RAM, and page tables
    memory *virtual_memory[VIRTUAL_MEMORY_SIZE];
    memory *RAM[RAM_SIZE];
    int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS];

    // Initialize virtual memory, RAM, and page tables
    initialize_virtual_memory(virtual_memory);
    initialize_page_tables(page_tables);
    memset(RAM, 0, sizeof(RAM));

    // Run the simulation
    simulate(virtual_memory, RAM, page_tables, argv[1]);

    // Write output to file
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file)
    {
        printf("Error opening output file.\n");
        return 1;
    }
    // Print output
    print_page_tables(page_tables, output_file);
    print_ram(RAM, output_file);
    fclose(output_file);

    // Free allocated memory
    for (int i = 0; i < VIRTUAL_MEMORY_SIZE; i++)
    {
        free(virtual_memory[i]); // Free allocated memory in virtual_memory
    }

    // Free allocated memory in RAM
    for (int i = 0; i < RAM_SIZE; i++)
    {
        RAM[i] = NULL; // Free allocated memory in RAM
    }

    return 0;
}

// Initialize virtual memory
void initialize_virtual_memory(memory *virtual_memory[])
{
    for (int process_id = 0; process_id < NUM_PROCESSES; process_id++) // 4 processes
    {
        for (int page_num = 0; page_num < PAGES_PER_PROCESS; page_num++) // 4 pages per process
        {
            for (int offset = 0; offset < PAGE_SIZE; offset++) // 2 bytes per page
            {
                // Allocate memory for each page in virtual memory
                int index = (process_id * PAGES_PER_PROCESS * PAGE_SIZE) + (page_num * PAGE_SIZE) + offset;
                virtual_memory[index] = (memory *)malloc(sizeof(memory));
                if (virtual_memory[index] == NULL)
                {
                    printf("Memory allocation failed at virtual_memory[%d]\n", index);
                    exit(1);
                }
                // Initialize process_id, page_num, and last_accessed
                virtual_memory[index]->process_id = process_id;
                virtual_memory[index]->page_num = page_num;
                // Initialize last_accessed to -1 to indicate the page has never been accessed
                virtual_memory[index]->last_accessed = -1;
            }
        }
    }
}

// Initialize page tables
void initialize_page_tables(int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS]) // 4 processes * 4 pages per process
{
    for (int i = 0; i < NUM_PROCESSES; i++) // 4 processes
    {
        for (int j = 0; j < PAGES_PER_PROCESS; j++) // 4 pages per process
        {
            page_tables[i][j] = 1; // All pages are initially in virtual memory
        }
    }
}

// Run the simulation
void simulate(memory *virtual_memory[], memory *RAM[], int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS], char *input_filename)
{
    // Open input file
    FILE *input_file = fopen(input_filename, "r");
    if (!input_file)
    {
        printf("Error opening input file.\n");
        exit(1);
    }
    // Run simulation
    int process_id;
    int time_step = 0;
    int next_page[NUM_PROCESSES] = {0};
    // Read input file and update RAM
    while (fscanf(input_file, "%d", &process_id) == 1)
    {
        int current_page = next_page[process_id];
        if (page_tables[process_id][current_page] == 0)
        {
            // Page is already in RAM, update last_accessed
            for (int i = 0; i < RAM_SIZE; i++)
            {
                // Check if page is in RAM
                if (RAM[i] && RAM[i]->process_id == process_id && RAM[i]->page_num == current_page)
                {
                    RAM[i]->last_accessed = time_step;
                }
            }
        }
        else
        {
            // Page not in RAM, bring it in
            update_ram(RAM, virtual_memory, page_tables, process_id, current_page, time_step);
        }
        // Update next_page
        next_page[process_id] = (next_page[process_id] + 1) % PAGES_PER_PROCESS;
        time_step++;
    }

    fclose(input_file);
}

// Find free frame
int find_free_frame(memory *RAM[])
{
    for (int i = 0; i < RAM_SIZE; i += PAGE_SIZE)
    {
        if (RAM[i] == NULL && RAM[i + 1] == NULL)
        {
            return i;
        }
    }
    return -1; // No free frame
}

// Update RAM
void update_ram(memory *RAM[], memory *virtual_memory[], int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS], int process_id, int page_num, int time_step)
{
    int free_frame = find_free_frame(RAM);
    if (free_frame != -1)
    {
        // Free frame found, bring page into RAM
        for (int i = 0; i < PAGE_SIZE; i++)
        {
            int vm_index = (process_id * PAGES_PER_PROCESS * PAGE_SIZE) + (page_num * PAGE_SIZE) + i;
            RAM[free_frame + i] = virtual_memory[vm_index];
            RAM[free_frame + i]->last_accessed = time_step;
        }
        // Update page table
        page_tables[process_id][page_num] = 0;
    }
    else
    {
        // No free frame, need to evict a page
        evict_page(RAM, page_tables, process_id, time_step, virtual_memory, page_num);
    }
}

// Evict a page
void evict_page(memory *RAM[], int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS], int process_id, int time_step, memory *virtual_memory[], int page_num)
{

    int lru_time = __INT_MAX__;
    int lru_index = -1;

    // Find least recently used page of the same process
    for (int i = 0; i < RAM_SIZE; i += PAGE_SIZE)
    {
        if (RAM[i] && RAM[i]->process_id == process_id)
        {
            if (RAM[i]->last_accessed < lru_time)
            {
                lru_time = RAM[i]->last_accessed;
                lru_index = i;
            }
        }
    }

    if (lru_index == -1)
    {
        // No pages of the same process in RAM
        // Find any empty frame
        int free_frame = find_free_frame(RAM);
        if (free_frame != -1)
        {
            // Free frame found, bring page into RAM
            for (int i = 0; i < PAGE_SIZE; i++)
            {
                int vm_index = (process_id * PAGES_PER_PROCESS * PAGE_SIZE) + (page_num * PAGE_SIZE) + i;
                RAM[free_frame + i] = virtual_memory[vm_index];
                RAM[free_frame + i]->last_accessed = time_step;
            }
            page_tables[process_id][page_num] = 0;
            return;
        }
        else
        {
            // No free frame, evict global LRU
            lru_time = __INT_MAX__;
            for (int i = 0; i < RAM_SIZE; i += PAGE_SIZE)
            {
                if (RAM[i] && RAM[i]->last_accessed < lru_time)
                {
                    lru_time = RAM[i]->last_accessed;
                    lru_index = i;
                }
            }
        }
    }

    // Evict page at lru_index
    int evicted_process_id = RAM[lru_index]->process_id;
    int evicted_page_num = RAM[lru_index]->page_num;
    page_tables[evicted_process_id][evicted_page_num] = 1;

    // Bring new page into RAM
    for (int i = 0; i < PAGE_SIZE; i++)
    {
        int vm_index = (process_id * PAGES_PER_PROCESS * PAGE_SIZE) + (page_num * PAGE_SIZE) + i;
        RAM[lru_index + i] = virtual_memory[vm_index];
        RAM[lru_index + i]->last_accessed = time_step;
    }
    page_tables[process_id][page_num] = 0;
}

// Print page tables
void print_page_tables(int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS], FILE *output_file)
{
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        fprintf(output_file, "Process %d: ", i);
        for (int j = 0; j < PAGES_PER_PROCESS; j++)
        {
            fprintf(output_file, "%d", page_tables[i][j]);
            if (j < PAGES_PER_PROCESS - 1)
            {
                fprintf(output_file, ", ");
            }
        }
        fprintf(output_file, "\n");
    }
}

// Print RAM
void print_ram(memory *RAM[], FILE *output_file)
{
    fprintf(output_file, "RAM Content:\n");
    for (int i = 0; i < RAM_SIZE; i += PAGE_SIZE)
    {
        if (RAM[i])
        {
            fprintf(output_file, "%d,%d,%d; %d,%d,%d",
                    RAM[i]->process_id, RAM[i]->page_num, RAM[i]->last_accessed,
                    RAM[i + 1]->process_id, RAM[i + 1]->page_num, RAM[i + 1]->last_accessed);
        }
        else
        {
            fprintf(output_file, "Empty; Empty");
        }
        if (i < RAM_SIZE - PAGE_SIZE)
        {
            fprintf(output_file, "; ");
        }
    }
    fprintf(output_file, "\n");
}
