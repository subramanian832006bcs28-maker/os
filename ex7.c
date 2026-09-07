#include <stdio.h>
#include <stdbool.h>

struct MemoryBlock
{
    int capacity;
    int id;
    bool used;
};

struct Job
{
    int required;
    int assignedBlock;
};

// Common allocation function
void assignMemory(struct MemoryBlock blocks[], struct Job jobs[], int jobCount, int blockCount)
{
    int i, j;

    for (i = 0; i < jobCount; i++)
    {
        jobs[i].assignedBlock = -1;

        for (j = 0; j < blockCount; j++)
        {
            if (blocks[j].used == false &&
                blocks[j].capacity >= jobs[i].required)
            {
                jobs[i].assignedBlock = blocks[j].id;
                blocks[j].used = true;
                break;
            }
        }
    }
}

// Display result and calculate fragmentation
void showResult(struct MemoryBlock blocks[], struct Job jobs[],
                int jobCount, int blockCount)
{
    int internalFrag = 0;
    int externalFrag = 0;
    int i, j;

    printf("\n--------------------------------------------------\n");
    printf("Process\tRequired\tBlock\tWasted Space\n");
    printf("--------------------------------------------------\n");

    for (i = 0; i < jobCount; i++)
    {
        printf("P%d\t%d\t\t", i + 1, jobs[i].required);

        if (jobs[i].assignedBlock == -1)
        {
            printf("Not Allocated\t-\n");
        }
        else
        {
            int blockCapacity = 0;

            for (j = 0; j < blockCount; j++)
            {
                if (blocks[j].id == jobs[i].assignedBlock)
                {
                    blockCapacity = blocks[j].capacity;
                    break;
                }
            }

            int waste = blockCapacity - jobs[i].required;

            internalFrag += waste;

            printf("B%d\t\t%d\n",
                   jobs[i].assignedBlock, waste);
        }
    }

    // Calculate external fragmentation
    for (i = 0; i < blockCount; i++)
    {
        if (blocks[i].used == false)
        {
            externalFrag += blocks[i].capacity;
        }
    }

    printf("--------------------------------------------------\n");
    printf("Total Internal Fragmentation : %d\n", internalFrag);
    printf("Total External Fragmentation : %d\n", externalFrag);
    printf("--------------------------------------------------\n");
}

int main()
{
    int blockCount, jobCount, option;
    int i, j;
    char choice;

    struct MemoryBlock original[20];

    printf("Enter number of memory blocks: ");
    scanf("%d", &blockCount);

    printf("\nEnter memory block sizes:\n");

    for (i = 0; i < blockCount; i++)
    {
        printf("Block %d: ", i + 1);
        scanf("%d", &original[i].capacity);

        original[i].id = i + 1;
        original[i].used = false;
    }

    printf("\nEnter number of processes: ");
    scanf("%d", &jobCount);

    struct Job jobs[20];
    struct Job tempJobs[20]; // Temporary array to preserve original jobs

    printf("\nEnter process sizes:\n");

    for (i = 0; i < jobCount; i++)
    {
        printf("Process %d: ", i + 1);
        scanf("%d", &jobs[i].required);
        jobs[i].assignedBlock = -1;
    }

    // Continuous execution loop
    do
    {
        printf("\n========== MEMORY ALLOCATION ==========\n");
        printf("1. First Fit\n");
        printf("2. Best Fit\n");
        printf("3. Worst Fit\n");
        printf("4. Exit\n");
        printf("========================================\n");

        printf("Choose allocation method: ");
        scanf("%d", &option);

        if (option == 4) {
            printf("Exiting program.\n");
            break;
        }

        struct MemoryBlock blocks[20];

        // Reset and copy original blocks for a clean allocation state
        for (i = 0; i < blockCount; i++)
        {
            blocks[i] = original[i];
        }

        // Reset and copy original process requests
        for (i = 0; i < jobCount; i++)
        {
            tempJobs[i] = jobs[i];
        }

        switch (option)
        {
            // FIRST FIT
            case 1:
                assignMemory(blocks, tempJobs, jobCount, blockCount);
                printf("\n******** FIRST FIT ALLOCATION ********\n");
                showResult(blocks, tempJobs, jobCount, blockCount);
                break;

            // BEST FIT
            case 2:
                // Arrange blocks in ascending order
                for (i = 0; i < blockCount - 1; i++)
                {
                    for (j = i + 1; j < blockCount; j++)
                    {
                        if (blocks[i].capacity > blocks[j].capacity)
                        {
                            struct MemoryBlock temp;
                            temp = blocks[i];
                            blocks[i] = blocks[j];
                            blocks[j] = temp;
                        }
                    }
                }

                assignMemory(blocks, tempJobs, jobCount, blockCount);
                printf("\n******** BEST FIT ALLOCATION ********\n");
                showResult(blocks, tempJobs, jobCount, blockCount);
                break;

            // WORST FIT
            case 3:
                // Arrange blocks in descending order
                for (i = 0; i < blockCount - 1; i++)
                {
                    for (j = i + 1; j < blockCount; j++)
                    {
                        if (blocks[i].capacity < blocks[j].capacity)
                        {
                            struct MemoryBlock temp;
                            temp = blocks[i];
                            blocks[i] = blocks[j];
                            blocks[j] = temp;
                        }
                    }
                }

                assignMemory(blocks, tempJobs, jobCount, blockCount);
                printf("\n******** WORST FIT ALLOCATION ********\n");
                showResult(blocks, tempJobs, jobCount, blockCount);
                break;

            default:
                printf("\nInvalid allocation method selected!\n");
        }

        if (option >= 1 && option <= 3) {
            printf("\nDo you want to try another method? (y/n): ");
            scanf(" %c", &choice);
        } else {
            choice = 'y'; // Keep looping if an invalid option was input
        }

    } while (choice == 'y' || choice == 'Y');

    return 0;
}
