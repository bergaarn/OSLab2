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

    // Total number of references
    unsigned int totalMemReferences = 100000;

    FILE* memFile = fopen(fileName, "r");
    if (!memFile)
    {
        perror("Error #2: File could not be opened");
    }

    printf("Physical Pages: %d\nPage Size: %d\n", physicalPages, pageSize);

    // Allocate dynamic array based on amount of physical pages from program call
    int* pageArray = malloc(physicalPages * sizeof(int));
    unsigned int* entireMemoryTrace = malloc(totalMemReferences * sizeof(unsigned int));

    // Initialize physical page memory array
    for (int i = 0; i < physicalPages; i++)
    {
        pageArray[i] = -1;   // -1 (default value, empty page)
        
    }   
    // Initialize memory trace array
    for (int i = 0; i < totalMemReferences; i++)
    {
        entireMemoryTrace[i] = -1;    // -1 (default value, before memory trace has been read)
    }

    // Ensure there is 10000 memory references read from file
    unsigned int verifyMemoryTrace = 0;

    // Load Adresses
    printf("Loading Addresses from: %s\n", fileName);

    for (int i = 0; i < totalMemReferences; i++)
    {
        if (fscanf(memFile, "%u", &entireMemoryTrace[i]) == 1)
        {
            verifyMemoryTrace++;
        }
        else
        {
            perror("Error reading from file");
            break;
        }
    }

    if (!(verifyMemoryTrace == totalMemReferences))
    {
        printf("Verification unsuccesful when reading from file.\n");
        fclose(memFile);
        free(entireMemoryTrace);
        free(pageArray);
        return(1);
    }

    // Initialize FIFO methodology variables, uints to handle large positive integers while saving memory
    unsigned int memAddress = 0;
    unsigned int pageFaults = 0;
    unsigned int pageNumber = 0;

    for (int addrIndex = 0; addrIndex < totalMemReferences; addrIndex++)
    {   
        // Calculate page number from memory trace index
        pageNumber = entireMemoryTrace[addrIndex] / pageSize;

        bool pageFaultOccurred = true;
    
        // Check if page is already in memory, if so set pageFault to false and save index
        for (int i = 0; i < physicalPages; i++)
        {
            if (pageArray[i] == pageNumber)
            {
                pageFaultOccurred = false;
                break;
            }
        }

        if (pageFaultOccurred) // If page is found, do nothing 
        {
            // If page was not found in memory 
            pageFaults++;
            
            int pageToEvict = -1;
            int furthestAwayPage = -1;

            // Find the least recently used page
            for (int i = 0; i < physicalPages; i++)
            {
                int pageUsedAt = -1; // If it stays -1, it will never be referenced again and a great candidate for eviction
                for (int j = addrIndex + 1; j < totalMemReferences; j++)
                {
                    if (pageArray[i] == entireMemoryTrace[j] / pageSize)
                    {
                        pageUsedAt = j;
                        break;
                    }          
                }        

                if (pageUsedAt == -1)
                {
                    pageToEvict = i; // Page never used again, break and evict;
                    break;
                }

                if (pageUsedAt > furthestAwayPage)
                {
                    furthestAwayPage = pageUsedAt; 
                    pageToEvict = i; // Current Best Eviction candiate
                }

                // Repeat for all physical pages
            }

            // Replace the furthest away page referenced with the new page
            pageArray[pageToEvict] = pageNumber;
        }
    }
    
    fclose(memFile);
    free(entireMemoryTrace);
    free(pageArray);

    printf("Scanned a total of %u memory addresses from file %s\n", totalMemReferences, fileName);
    printf("Encountered %u page faults\n\n", pageFaults);
    
    return 0;
}