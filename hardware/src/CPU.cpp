#include "CPU.h"

#include <stdio.h>
#include <iostream>


#if defined(MACOS)
#include <cpuid.h>
#include <sys/sysctl.h>
#elif defined(UNIX)
#include <cpuid.h>
//#include <linux/sysctl.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <Windows.h>
#include <intrin.h>
#endif

#include <thread>


using namespace std;



namespace cpu
{


// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation
int get_real_cpu_core()
{
#if MACOS    
    // 另一個做法.
    // 可以參考  sysctl hw 執行結果
    int physical_cores;
    const char* cmd = "hw.physicalcpu";
    size_t size = sizeof(physical_cores);
    // ubuntu已被移除,不能用這個function
    sysctlbyname( cmd, &physical_cores, &size, nullptr, 0 );
    return physical_cores;
    
    //return std::thread::hardware_concurrency(); // 搜尋資料後發現直接用std的比較快....
#elif defined(UNIX)

    int num_cpus = sysconf( _SC_NPROCESSORS_ONLN );
    return num_cpus;

#elif defined(WIN32)
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
#endif
}





int get_logical_cpu_core()
{
#ifdef WIN32
	SYSTEM_INFO si;
	GetSystemInfo(&si);

    printf( "logical cpu core = %d\n", si.dwNumberOfProcessors );

    return si.dwNumberOfProcessors;
#elif defined(UNIX)

    // 跟mac版本不相容,目前沒找到好的方式. 可以試著研究syscall之類的函數
    printf("need maintain...\n");
    return 0;
    
#elif defined(MACOS)

    // 檢查是否能套用在linux環境
    int logical_cores;
    const char* cmd = "hw.logicalcpu";
    size_t size = sizeof(logical_cores);
    sysctlbyname( cmd, &logical_cores, &size, nullptr, 0 );
    return logical_cores;
    
#endif
}









// 取得製造商
string get_cpu_vender()
{
    unsigned int	regs[4];
    char	buf[17]		=	{0};
#ifdef WIN32
    __cpuid( (int*)regs, 0 );		// 0 = 製造商資訊
#elif defined(UNIX) | defined(MACOS)
    __get_cpuid( 0, &regs[0], &regs[1], &regs[2], &regs[3] );
#endif
    ((unsigned int*)buf)[0]		=	regs[1];
    ((unsigned int*)buf)[1]		=	regs[3];
    ((unsigned int*)buf)[2]		=	regs[2];
    ((unsigned int*)buf)[3]		=	regs[0];

    // remove 特殊符號
    bool	flag;
    for( int i = 0; i < 16; i++ )
    {
        flag	=	(buf[i] >= 'a' && buf[i] <= 'z') ||
            (buf[i] >= 'A' && buf[i] <= 'Z') ||
            (buf[i] >= '0' && buf[i] <= '9') ;

        if( !flag )
            buf[i]	=	0;
    }

    string	result(buf);
    return	result;
}





// 取得序號   (processor + 一串字串)
string get_cpu_serial()  
{
    unsigned int	regs[4];
    char	buf[50]		=	{0};
#ifdef WIN32
    __cpuid( (int*)regs, 1 );		// 1 = 製程資訊
#elif defined(UNIX) | defined(MACOS)
    __get_cpuid( 1, &regs[0], &regs[1], &regs[2], &regs[3] );
#endif

    regs[1]	&=	0xf0ffffff;	// need 因為有一碼會改變....頭痛   該位元估計是core index.
    sprintf( buf, "%08X-%08X-%08X-%08X", regs[3], regs[0], regs[1], regs[2] );

    string	result(buf);
    return	result;
}





string	cpu_get_serial_asm()  
{  
#ifdef _WIN64
    /*
        https://www.itread01.com/content/1549152032.html
        可以參考這篇
        win64的環境下不支援 __asm
    */
    printf("Visual studio 2019 not support __asm. need config.\n");
    return string("win64 could run this code.\n");
#else
#   ifdef _WIN32
    /*
        底下的code這次執行的時候後半段的序號錯了
        當成保留__asm語法的測試
        推測可能跟AMD的cpu有關
    */
    unsigned long	ulS1,	ulS2;		// note: u L S 1   是 long 的 l
                                        // string strCPUID,strCPUID1,strCPUID2;  
    char	strCPUID1[50],	strCPUID2[50];  
    string	strCPUID;

    __asm  
    {  
        mov eax,01h  
        xor edx,edx  
        cpuid  
        mov ulS1,edx  
        mov ulS2,eax  
    }  
    //strCPUID1.Format("%08X%08X",ulS1,ulS2);  
    // try過 00h 01h 02h 03h   最後選擇 01h跟00h   01h = wimc的結果   00h可以轉換出 CPU 型號.   類似 Intel i5 .... 的訊息
    sprintf( strCPUID1, "%08X%08X", ulS1, ulS2 );
    __asm  
    {  
        mov eax,03h  
        xor ecx,ecx  
        xor edx,edx  
        cpuid  
        mov ulS1,edx  
        mov ulS2,eax  
    }  
    //strCPUID2.Format("%08X%08X",ulS1,ulS2);  
    sprintf( strCPUID2, "%08X%08X", ulS1, ulS2 );

    //char buf[17];
    //((unsigned long*)buf)[0] = ulS1;
    //((unsigned long*)buf)[0] = ulS2;

    strCPUID = string(strCPUID1) + string(strCPUID2);  

    return strCPUID;  
#   else
    unsigned long   varEAX, varEBX, varECX, varEDX;

    char str[50];
    //%eax=1 gives most significant 32 bits in eax
    __asm__ __volatile__ ("cpuid": "=a" (varEAX), "=b" (varEBX), "=c" (varECX), "=d" (varEDX) : "a" (1));

    // sprintf(str, "%08X", varEDX); //i.e. XXXX-XXXX-xxxx-xxxx-xxxx-xxxx
    //sprintf(PSN, "%C%C%C%C-%C%C%C%C", str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);

    //sprintf(str, "%08X", varEAX); //i.e. XXXX-XXXX-xxxx-xxxx-xxxx-xxxx
    //sprintf(PSN, "%s-%C%C%C%C-%C%C%C%C",PSN,  str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);

    sprintf( str, "%08X%08X", varEDX, varEAX );

    string 	cpuid = string(str);

    return cpuid;

    //%eax=3 gives least significant 64 bits in edx and ecx [if PN is enabled]
    // __asm__ __volatile__ ("cpuid": "=a" (varEAX), "=b" (varEBX), "=c" (varECX), "=d" (varEDX) : "a" (1));

    //sprintf(str, "%08X", varEDX); //i.e. xxxx-xxxx-XXXX-XXXX-xxxx-xxxx
    //sprintf(PSN, "%s-%C%C%C%C-%C%C%C%C", PSN, str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);

    //sprintf(str, "%08X", varEBX); //i.e. xxxx-xxxx-xxxx-xxxx-XXXX-XXXX
    //sprintf(PSN, "%s-%C%C%C%C-%C%C%C%C", PSN, str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);
#   endif
#endif
}  




} // end namespace cpu






void test_cpu_info_func()
{
    cout << "get core = " << cpu::get_real_cpu_core() << "\n\n";
    cout << "get core = " << cpu::get_logical_cpu_core() << "\n\n";

    cout << "vendor = " << cpu::get_cpu_vender() << "\n\n";
    cout << "cpu id = " << cpu::get_cpu_serial() << "\n\n";

    // 不明原因win release 發生crash. 因為估計不太會用這個版本,暫時不管他.
#ifdef DEBUG
    cout << "cpu id = " << cpu::cpu_get_serial_asm() << "\n\n";
#endif
}
