#include "CPU.h"

#include <Windows.h>

#include <stdio.h>




// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation
int get_real_cpu_core()
{
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
	DWORD returnLength = 0;
	DWORD byteOffset = 0;
	DWORD processorCoreCount = 0;

    while( true )
    {
        DWORD rc = GetLogicalProcessorInformation( buffer, &returnLength );
        if (FALSE == rc) 
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
            {
                if (buffer) 
                    free(buffer);
                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
                if (NULL == buffer) 
                {
					printf( "Allocation failure" );
                    return 1;
                }
            } 
            else 
            {
				printf( "Error %d", GetLastError() );
                return 1;
            }
        } 
        else
			break;
    }
	ptr = buffer;
    while( byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength ) 
    {
        switch( ptr->Relationship )
        {
			case RelationProcessorCore:
				processorCoreCount++;
				break;
			case RelationNumaNode:
			case RelationCache:
			case RelationProcessorPackage:
			default:
				break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }    
    free(buffer);
	printf( "physical core = %d\n", processorCoreCount );

	return processorCoreCount;
}





int get_logical_cpu_core()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

    printf( "logical cpu core = %d\n", si.dwNumberOfProcessors );

    return si.dwNumberOfProcessors;
}