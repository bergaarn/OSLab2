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

    // Allocate 100 000 slots for the entire memory trace
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

    // Ensure there is 100 000 memory references read from file
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

    // If read isnt accurate, send error, free dynamic memory and close program
    if (!(verifyMemoryTrace == totalMemReferences))
    {
        printf("Verification unsuccesful when reading from file.\n");
        fclose(memFile);
        free(entireMemoryTrace);
        free(pageArray);
        return(1);
    }

    // Initialize pageFault and pageNumber variables
    unsigned int pageFaults = 0;
    unsigned int pageNumber = 0;

    // Loop through dynamically allocated memory trace from file
    for (int addrIndex = 0; addrIndex < totalMemReferences; addrIndex++)
    {   
        // Calculate page number from memory trace index
        pageNumber = entireMemoryTrace[addrIndex] / pageSize;

        // page fault flag
        bool pageFaultOccurred = true;
    
        // Check if page is already in memory, if so set pageFault to false
        for (int i = 0; i < physicalPages; i++)
        {
            if (pageArray[i] == pageNumber)
            {
                pageFaultOccurred = false;
                break;
            }
        }

        // If no page fault occured, do nothing otherwise proceed inside if statement
        if (pageFaultOccurred) 
        {
            // If page was not found in memory 
            pageFaults++;
            
            // Initialize to -1 in case no future reference is found in memory trace
            int pageToEvict = -1;
            // Initialize to -1 for future comparisons
            int furthestAwayPage = -1;

            // Check when/if the physical pages are referenced in the future in the memory trace
            for (int i = 0; i < physicalPages; i++)
            {
                // If it stays -1, it will never be referenced again and a great candidate for eviction
                int pageUsedAt = -1; 

                // Start looking at the current address index + 1 for future references
                for (int j = addrIndex + 1; j < totalMemReferences; j++)
                {
                    // If the physical page is referenced in the future, note index as pageUsedAt and break
                    if (pageArray[i] == entireMemoryTrace[j] / pageSize)
                    {
                        pageUsedAt = j;
                        break;
                    }          
                }        

                // If no future references was found, set to evict page at index i
                if (pageUsedAt == -1)
                {
                    pageToEvict = i; // Page never used again, break and evict;
                    break;
                }
                
                // At this point, all pages are referenced in the future so keep track of the furthest reference
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