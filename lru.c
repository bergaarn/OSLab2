#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

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
    int* lruArray = malloc(physicalPages * sizeof(int));

    // Initialize arrays
    for (int i = 0; i < physicalPages; i++)
    {
        pageArray[i] = -1;   // -1 (default value, empty page)
        lruArray[i] = -1;    // -1 (default value, before a recently used value has been set)
    }

    // Initialize LRU methodology variables, uints to handle large positive integers while saving memory
    unsigned int memAddress = 0;
    unsigned int totalMemReferences = 0;
    unsigned int pageFaults = 0;
    unsigned int pageNumber = 0;

    // Load Adresses
    printf("Loading Addresses from: %s\n", fileName);
    
     while (fscanf(memFile, "%u", &memAddress) == 1)
    {
        totalMemReferences++;
        pageNumber = memAddress / pageSize;

        bool pageFaultOccurred = true;
        int pageFoundIndex = -1;

        // Check if page is already in memory, if so set pageFault to false and save index
        for (int i = 0; i < physicalPages; i++)
        {
            if (pageArray[i] == pageNumber)
            {
                pageFaultOccurred = false;
                pageFoundIndex = i;
                break;
            }
        }

        if (!pageFaultOccurred)
        {
            // If page is found, update the index with the latest mem reference (last used value)
            lruArray[pageFoundIndex] = totalMemReferences;
        }
        else
        {
            // If page fault occurs 
            pageFaults++;
            int leastUsedIndex = 0;
            int leastUsedReference = INT_MAX;

            // Find the least recently used page
            for (int i = 0; i < physicalPages; i++)
            {
                if (lruArray[i] < leastUsedReference)
                {
                    // Track which slot is the least used by memory reference value
                    leastUsedReference = lruArray[i];
                    // Update index to be replaced in arrays
                    leastUsedIndex = i;
                }
            }

            // Replace the least used page and the least used memory reference
            pageArray[leastUsedIndex] = pageNumber;
            lruArray[leastUsedIndex] = totalMemReferences; 
        }
    }
    
    fclose(memFile);
    free(lruArray);
    free(pageArray);

    printf("Scanned a total of %u memory addresses from file %s\n", totalMemReferences, fileName);
    printf("Encountered %u page faults\n\n", pageFaults);
    
    return 0;
}