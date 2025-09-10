#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main (int argc, char** argv)
{
    // Error Handling of program call (argc / argv)
    if (argc != 4)
    {
        fprintf(stderr, "Error #1: Incorrect amount of arguments.\n");
    }
    
    // Read in arguments and convert to integers
    int physicalPages = atoi(argv[1]);
    int pageSize = atoi(argv[2]);
    char* fileName = argv[3];

    FILE* memFile = fopen(fileName, "r");
    if (!memFile)
    {
        perror("Error #2: File could not be opened");
    }

    printf("Physical Pages: %d\nPage Size: %d\n", physicalPages, pageSize);

    // Allocate dynamic array based on amount of physical pages from program call
    int* pageArray = malloc(physicalPages * sizeof(int));
    // Initialize default value to all physical pages to be changed later
    for (int i = 0; i < physicalPages; i++)
    {
        pageArray[i] = -1;
    }

    // Initialize FIFO methodology variables
    unsigned int memAddress = 0;
    unsigned int totalMemReferences = 0;
    unsigned int pageFaults = 0;
    unsigned int oldestPage = 0;
    unsigned int pageNumber = 0;

    // Load Adresses
    printf("Loading Addresses from: %s\n", fileName);
    
    while (fscanf(memFile, "%u", &memAddress) == 1)
    {
        bool pageFaultOccurred = true;
        totalMemReferences++;
        pageNumber = memAddress / pageSize;
        

        // Check if page hit
        for (int i = 0; i < physicalPages; i++)
        {   
            // Check if page fault
            if (pageArray[i] == pageNumber)
            {
                pageFaultOccurred = false;
                break;
            }
        }
        
        if (pageFaultOccurred)
        {
            //Increment amount of page faults
            pageFaults++;

            // Evict oldest page
            pageArray[oldestPage] = pageNumber;

            // Track oldest page
            oldestPage = (oldestPage + 1) % physicalPages;
        }
    }
    
    fclose(memFile);
    free(pageArray);

    printf("Scanned a total of %u memory addresses from file %s\n", totalMemReferences, fileName);
    printf("Encountered %u page faults\n\n", pageFaults);
    
    return 0;
}