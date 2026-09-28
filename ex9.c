[24bcs049@mepcolinux ex9]$cat ex9.c
#include <stdio.h>
#include <stdlib.h>

#define MAX_PAGES 100
#define MAX_FRAMES 20

/* Function to display the current frames */
void displayFrames(int frames[], int frameSize)
{
    int i;

    for (i = 0; i < frameSize; i++)
    {
        if (frames[i] == -1)
            printf("- ");
        else
            printf("%d ", frames[i]);
    }
}

/* Check whether a page is already present in frames */
int isPagePresent(int frames[], int frameSize, int page)
{
    int i;

    for (i = 0; i < frameSize; i++)
    {
        if (frames[i] == page)
            return i;
    }

    return -1;
}


/* ================= FIFO ================= */

void FIFO(int pages[], int n, int frameSize)
{
    int frames[MAX_FRAMES];
    int pointer = 0;
    int hits = 0, faults = 0;
    int i, position;

    for (i = 0; i < frameSize; i++)
        frames[i] = -1;

    printf("\n========== FIFO PAGE REPLACEMENT ==========\n\n");

    printf("Page\tFrames\t\t\tResult\n");
    printf("---------------------------------------------\n");

    for (i = 0; i < n; i++)
    {
        position = isPagePresent(frames, frameSize, pages[i]);

        printf("%d\t", pages[i]);

        if (position != -1)
        {
            hits++;
            displayFrames(frames, frameSize);
            printf("\tHIT\n");
        }
        else
        {
            faults++;

            frames[pointer] = pages[i];
            pointer = (pointer + 1) % frameSize;

            displayFrames(frames, frameSize);
            printf("\tPAGE FAULT\n");
        }
    }

    printf("\n========== FINAL RESULT ==========\n");
    printf("Total Page Hits : %d\n", hits);
    printf("Total Page Faults : %d\n", faults);
    printf("Total Pages : %d\n", n);
}


/* ================= LRU ================= */

void LRU(int pages[], int n, int frameSize)
{
    int frames[MAX_FRAMES];
    int lastUsed[MAX_FRAMES];

    int hits = 0, faults = 0;
    int i, j;
    int position;
    int replaceIndex;
    int minimum;

    for (i = 0; i < frameSize; i++)
    {
        frames[i] = -1;
        lastUsed[i] = -1;
    }

    printf("\n========== LRU PAGE REPLACEMENT ==========\n\n");

    printf("Page\tFrames\t\t\tResult\n");
    printf("---------------------------------------------\n");

    for (i = 0; i < n; i++)
    {
        position = isPagePresent(frames, frameSize, pages[i]);

        printf("%d\t", pages[i]);

        /* Page Hit */
        if (position != -1)
        {
            hits++;

            lastUsed[position] = i;

            displayFrames(frames, frameSize);
            printf("\tHIT\n");
        }

        /* Page Fault */
        else
        {
            faults++;

            /* Check for an empty frame */
            replaceIndex = -1;

            for (j = 0; j < frameSize; j++)
            {
                if (frames[j] == -1)
                {
                    replaceIndex = j;
                    break;
                }
            }

            /* If no empty frame, find Least Recently Used */
            if (replaceIndex == -1)
            {
                minimum = lastUsed[0];
                replaceIndex = 0;

                for (j = 1; j < frameSize; j++)
                {
                    if (lastUsed[j] < minimum)
                    {
                        minimum = lastUsed[j];
                        replaceIndex = j;
                    }
                }
            }

            frames[replaceIndex] = pages[i];
            lastUsed[replaceIndex] = i;

            displayFrames(frames, frameSize);
            printf("\tPAGE FAULT\n");
        }
    }

    printf("\n========== FINAL RESULT ==========\n");
    printf("Total Page Hits : %d\n", hits);
    printf("Total Page Faults : %d\n", faults);
    printf("Total Pages : %d\n", n);
}


/* ================= OPTIMAL ================= */

void Optimal(int pages[], int n, int frameSize)
{
    int frames[MAX_FRAMES];

    int hits = 0, faults = 0;
    int i, j, k;

    int position;
    int replaceIndex;
    int farthest;
    int nextUse;

    for (i = 0; i < frameSize; i++)
        frames[i] = -1;

    printf("\n========== OPTIMAL PAGE REPLACEMENT ==========\n\n");

    printf("Page\tFrames\t\t\tResult\n");
    printf("---------------------------------------------\n");

    for (i = 0; i < n; i++)
    {
        position = isPagePresent(frames, frameSize, pages[i]);

        printf("%d\t", pages[i]);

        /* Page Hit */
        if (position != -1)
        {
            hits++;

            displayFrames(frames, frameSize);
            printf("\tHIT\n");
        }

        /* Page Fault */
        else
        {
            faults++;

            /* Check for empty frame */
            replaceIndex = -1;

            for (j = 0; j < frameSize; j++)
            {
                if (frames[j] == -1)
                {
                    replaceIndex = j;
                    break;
                }
            }

            /* If all frames are full */
            if (replaceIndex == -1)
            {
                farthest = -1;
                replaceIndex = -1;

                for (j = 0; j < frameSize; j++)
                {
                    nextUse = -1;

                    /* Find next occurrence of current frame page */
                    for (k = i + 1; k < n; k++)
                    {
                        if (frames[j] == pages[k])
                        {
                            nextUse = k;
                            break;
                        }
                    }

                    /* Page will never be used again */
                    if (nextUse == -1)
                    {
                        replaceIndex = j;
                        break;
                    }

                    /* Find page used farthest in future */
                    if (nextUse > farthest)
                    {
                        farthest = nextUse;
                        replaceIndex = j;
                    }
                }
            }

            frames[replaceIndex] = pages[i];

            displayFrames(frames, frameSize);
            printf("\tPAGE FAULT\n");
        }
    }

    printf("\n========== FINAL RESULT ==========\n");
    printf("Total Page Hits : %d\n", hits);
    printf("Total Page Faults : %d\n", faults);
    printf("Total Pages : %d\n", n);
}


/* ================= MAIN FUNCTION ================= */

int main()
{
    int pages[MAX_PAGES];
    int n;
    int frameSize = 3;
    int choice;
    int i;

    printf("=========================================\n");
    printf(" PAGING PAGE REPLACEMENT SIMULATOR\n");
    printf("=========================================\n");

    printf("\nEnter number of page references: ");
    scanf("%d", &n);

    if (n <= 0 || n > MAX_PAGES)
    {
        printf("Invalid number of pages!\n");
        return 0;
    }

    printf("Enter the page reference string:\n");

    for (i = 0; i < n; i++)
    {
        scanf("%d", &pages[i]);
    }

    do
    {
        printf("\n=========================================\n");
        printf(" MAIN MENU\n");
        printf("=========================================\n");

        printf("1. FIFO Page Replacement\n");
        printf("2. LRU Page Replacement\n");
        printf("3. Optimal Page Replacement\n");
        printf("4. Change Frame Size\n");
        printf("5. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                FIFO(pages, n, frameSize);
                break;

            case 2:
                LRU(pages, n, frameSize);
                break;

            case 3:
                Optimal(pages, n, frameSize);
                break;

            case 4:
                printf("\nEnter new frame size: ");
                scanf("%d", &frameSize);

                if (frameSize <= 0 || frameSize > MAX_FRAMES)
                {
                    printf("Invalid frame size! Setting frame size to 3.\n");
                    frameSize = 3;
                }
                else
                {
                    printf("Frame size successfully changed to %d.\n",
                           frameSize);
                }

                break;

         
                printf("\nExiting program...\n");
                printf("Thank You!\n");
                break;

            default:
                printf("\nInvalid choice! Please try again.\n");
        }

    } while (choice != 5);

    return 0;
}
[24bcs049@mepcolinux ex9]$^C
[24bcs049@mepcolinux ex9]$./ex9
=========================================
 PAGING PAGE REPLACEMENT SIMULATOR
=========================================

Enter number of page references: 7
Enter the page reference string:
1 3 0 3 5 6 3

=========================================
 MAIN MENU
=========================================
1. FIFO Page Replacement
2. LRU Page Replacement
3. Optimal Page Replacement
4. Change Frame Size
5. Exit

Enter your choice: 1

========== FIFO PAGE REPLACEMENT ==========

Page	Frames			Result
---------------------------------------------
1	1 - - 	PAGE FAULT
3	1 3 - 	PAGE FAULT
0	1 3 0 	PAGE FAULT
3	1 3 0 	HIT
5	5 3 0 	PAGE FAULT
6	5 6 0 	PAGE FAULT
3	5 6 3 	PAGE FAULT

========== FINAL RESULT ==========
Total Page Hits : 1
Total Page Faults : 6
Total Pages : 7

=========================================
 MAIN MENU
=========================================
1. FIFO Page Replacement
2. LRU Page Replacement
3. Optimal Page Replacement
4. Change Frame Size
5. Exit

Enter your choice: 
2

========== LRU PAGE REPLACEMENT ==========

Page	Frames			Result
---------------------------------------------
1	1 - - 	PAGE FAULT
3	1 3 - 	PAGE FAULT
0	1 3 0 	PAGE FAULT
3	1 3 0 	HIT
5	5 3 0 	PAGE FAULT
6	5 3 6 	PAGE FAULT
3	5 3 6 	HIT

========== FINAL RESULT ==========
Total Page Hits : 2
Total Page Faults : 5
Total Pages : 7

=========================================
 MAIN MENU
=========================================
1. FIFO Page Replacement
2. LRU Page Replacement
3. Optimal Page Replacement
4. Change Frame Size
5. Exit

Enter your choice: 3

========== OPTIMAL PAGE REPLACEMENT ==========

Page	Frames			Result
---------------------------------------------
1	1 - - 	PAGE FAULT
3	1 3 - 	PAGE FAULT
0	1 3 0 	PAGE FAULT
3	1 3 0 	HIT
5	5 3 0 	PAGE FAULT
6	6 3 0 	PAGE FAULT
3	6 3 0 	HIT

========== FINAL RESULT ==========
Total Page Hits : 2
Total Page Faults : 5
Total Pages : 7

=========================================
 MAIN MENU
=========================================
1. FIFO Page Replacement
2. LRU Page Replacement
3. Optimal Page Replacement
4. Change Frame Size
5. Exit

Enter your choice: 4

Enter new frame size: 4
Frame size successfully changed to 4.

=========================================
 MAIN MENU
=========================================
1. FIFO Page Replacement
2. LRU Page Replacement
3. Optimal Page Replacement
4. Change Frame Size
5. Exit

Enter your choice: 1

========== FIFO PAGE REPLACEMENT ==========

Page	Frames			Result
---------------------------------------------
1	1 - - - 	PAGE FAULT
3	1 3 - - 	PAGE FAULT
0	1 3 0 - 	PAGE FAULT
3	1 3 0 - 	HIT
5	1 3 0 5 	PAGE FAULT
6	6 3 0 5 	PAGE FAULT
3	6 3 0 5 	HIT

========== FINAL RESULT ==========
Total Page Hits : 2
Total Page Faults : 5
Total Pages : 7

=========================================
 MAIN MENU
=========================================
1. FIFO Page Replacement
2. LRU Page Replacement
3. Optimal Page Replacement
4. Change Frame Size
5. Exit

Enter your choice: 4

Enter new frame size: 2
Frame size successfully changed to 2.

=========================================
 MAIN MENU
=========================================
1. FIFO Page Replacement
2. LRU Page Replacement
3. Optimal Page Replacement
4. Change Frame Size
5. Exit

Enter your choice: 1

========== FIFO PAGE REPLACEMENT ==========

Page	Frames			Result
---------------------------------------------
1	1 - 	PAGE FAULT
3	1 3 	PAGE FAULT
0	0 3 	PAGE FAULT
3	0 3 	HIT
5	0 5 	PAGE FAULT
6	6 5 	PAGE FAULT
3	6 3 	PAGE FAULT

========== FINAL RESULT ==========
Total Page Hits : 1
Total Page Faults : 6
Total Pages : 7

=========================================
 MAIN MENU
=========================================
1. FIFO Page Replacement
2. LRU Page Replacement
3. Optimal Page Replacement
4. Change Frame Size
5. Exit

