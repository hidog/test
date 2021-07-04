#include "CPU.h"

#include <Windows.h>

#include <stdio.h>





#if defined(UNIX) | defined(MACOS)
#include <cpuid.h>
#endif

#include <intrin.h>


using namespace std;



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












// 取得製造商
std::string	cpu_get_vender()
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
std::string	cpu_get_serial()  
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






#if 0 // old code
void	cpuID( unsigned i, unsigned  regs[4] ) 
{
#ifdef _WIN32
    //cout << "get cpu id...\n";
    __cpuid((int *)regs, (int)i);

#else
    __get_cpuid( 0, &regs[0], &regs[1], &regs[2], &regs[3] );

    // old code...
    //asm volatile
    //("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
    //: "a" (i), "c" (0));
    // ECX is set to zero for CPUID function 4
#endif
}
#endif

#if 0  // old code
// 取得製造商
string	cpu_get_vender()
{ 
    unsigned	regs[4];

    // Get vendor
    char vendor[17] = "0";

    cpuID( 0, regs );

    ((unsigned *)vendor)[0] = regs[1]; // EBX
    ((unsigned *)vendor)[1] = regs[3]; // EDX
    ((unsigned *)vendor)[2] = regs[2]; // ECX
    ((unsigned *)vendor)[3] = regs[0];

    string cpuVendor = string(vendor,12);
    //string cpuBBB = string( (char*)regs[1], 20 );

    return	cpuVendor;
}
#endif


#if 0   // old code
string	cpu_get_serial()  
{  
#ifdef _WIN32
    // 應該沒有位元的問題吧....?  平台轉換時要注意...
    unsigned long	ulS1,	ulS2;		// note: u L S 1   是 long 的 l
                                        //string strCPUID,strCPUID1,strCPUID2;  
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
        mov eax,02h  
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
#else
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
#endif
}  
#endif





/*
備份資料


void cpuID(unsigned i, unsigned regs[4]) {
#ifdef _WIN32
cout << "get cpu id...\n";
__cpuid((int *)regs, (int)i);

#else
asm volatile
("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
: "a" (i), "c" (0));
// ECX is set to zero for CPUID function 4
#endif
}



inf test()
{
unsigned regs[4];

// Get vendor
char vendor[17] = "0";
cpuID(0, regs);
((unsigned *)vendor)[0] = regs[1]; // EBX
((unsigned *)vendor)[1] = regs[3]; // EDX
((unsigned *)vendor)[2] = regs[2]; // ECX
((unsigned *)vendor)[3] = regs[0];
string cpuVendor = string(vendor,12);
//string cpuBBB = string( (char*)regs[1], 20 );

// Get CPU features
cpuID(1, regs);
unsigned cpuFeatures = regs[3]; // EDX

// Logical core count per CPU
cpuID(1, regs);
unsigned logical = (regs[1] >> 16) & 0xff; // EBX[23:16]
cout << " logical cpus: " << logical << endl;
unsigned cores = logical;

if (cpuVendor == "GenuineIntel") {
// Get DCP cache info
cpuID(4, regs);
cores = ((regs[0] >> 26) & 0x3f) + 1; // EAX[31:26] + 1

} else if (cpuVendor == "AuthenticAMD") {
// Get NC: Number of CPU cores - 1
cpuID(0x80000008, regs);
cores = ((unsigned)(regs[2] & 0xff)) + 1; // ECX[7:0] + 1
}

cout << "    cpu cores: " << cores << endl;

// Detect hyper-threads  
bool hyperThreads = cpuFeatures & (1 << 28) && cores < logical;

cout << "hyper-threads: " << (hyperThreads ? "true" : "false") << endl;
}























// MSDN的東西  而且有問題  去他的- - "  還要自己手動改寫...

// InstructionSet.cpp 
// Compile by using: cl /EHsc /W4 InstructionSet.cpp
// processor: x86, x64
// Uses the __cpuid intrinsic to get information about 
// CPU extended instruction set support.

#include <iostream>
#include <vector>
#include <bitset>
#include <array>
#include <string>
#include <intrin.h>

class InstructionSet
{
// forward declarations
class InstructionSet_Internal;

public:
// getters
static std::string Vendor(void) { return CPU_Rep.vendor_; }
static std::string Brand(void) { return CPU_Rep.brand_; }

static bool SSE3(void) { return CPU_Rep.f_1_ECX_[0]; }
static bool PCLMULQDQ(void) { return CPU_Rep.f_1_ECX_[1]; }
static bool MONITOR(void) { return CPU_Rep.f_1_ECX_[3]; }
static bool SSSE3(void) { return CPU_Rep.f_1_ECX_[9]; }
static bool FMA(void) { return CPU_Rep.f_1_ECX_[12]; }
static bool CMPXCHG16B(void) { return CPU_Rep.f_1_ECX_[13]; }
static bool SSE41(void) { return CPU_Rep.f_1_ECX_[19]; }
static bool SSE42(void) { return CPU_Rep.f_1_ECX_[20]; }
static bool MOVBE(void) { return CPU_Rep.f_1_ECX_[22]; }
static bool POPCNT(void) { return CPU_Rep.f_1_ECX_[23]; }
static bool AES(void) { return CPU_Rep.f_1_ECX_[25]; }
static bool XSAVE(void) { return CPU_Rep.f_1_ECX_[26]; }
static bool OSXSAVE(void) { return CPU_Rep.f_1_ECX_[27]; }
static bool AVX(void) { return CPU_Rep.f_1_ECX_[28]; }
static bool F16C(void) { return CPU_Rep.f_1_ECX_[29]; }
static bool RDRAND(void) { return CPU_Rep.f_1_ECX_[30]; }

static bool MSR(void) { return CPU_Rep.f_1_EDX_[5]; }
static bool CX8(void) { return CPU_Rep.f_1_EDX_[8]; }
static bool SEP(void) { return CPU_Rep.f_1_EDX_[11]; }
static bool CMOV(void) { return CPU_Rep.f_1_EDX_[15]; }
static bool CLFSH(void) { return CPU_Rep.f_1_EDX_[19]; }
static bool MMX(void) { return CPU_Rep.f_1_EDX_[23]; }
static bool FXSR(void) { return CPU_Rep.f_1_EDX_[24]; }
static bool SSE(void) { return CPU_Rep.f_1_EDX_[25]; }
static bool SSE2(void) { return CPU_Rep.f_1_EDX_[26]; }

static bool FSGSBASE(void) { return CPU_Rep.f_7_EBX_[0]; }
static bool BMI1(void) { return CPU_Rep.f_7_EBX_[3]; }
static bool HLE(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_7_EBX_[4]; }
static bool AVX2(void) { return CPU_Rep.f_7_EBX_[5]; }
static bool BMI2(void) { return CPU_Rep.f_7_EBX_[8]; }
static bool ERMS(void) { return CPU_Rep.f_7_EBX_[9]; }
static bool INVPCID(void) { return CPU_Rep.f_7_EBX_[10]; }
static bool RTM(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_7_EBX_[11]; }
static bool AVX512F(void) { return CPU_Rep.f_7_EBX_[16]; }
static bool RDSEED(void) { return CPU_Rep.f_7_EBX_[18]; }
static bool ADX(void) { return CPU_Rep.f_7_EBX_[19]; }
static bool AVX512PF(void) { return CPU_Rep.f_7_EBX_[26]; }
static bool AVX512ER(void) { return CPU_Rep.f_7_EBX_[27]; }
static bool AVX512CD(void) { return CPU_Rep.f_7_EBX_[28]; }
static bool SHA(void) { return CPU_Rep.f_7_EBX_[29]; }

static bool PREFETCHWT1(void) { return CPU_Rep.f_7_ECX_[0]; }

static bool LAHF(void) { return CPU_Rep.f_81_ECX_[0]; }
static bool LZCNT(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_ECX_[5]; }
static bool ABM(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[5]; }
static bool SSE4a(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[6]; }
static bool XOP(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[11]; }
static bool TBM(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[21]; }

static bool SYSCALL(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_EDX_[11]; }
static bool MMXEXT(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[22]; }
static bool RDTSCP(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_EDX_[27]; }
static bool _3DNOWEXT(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[30]; }
static bool _3DNOW(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[31]; }

private:
static const InstructionSet_Internal CPU_Rep;

class InstructionSet_Internal
{
public:
InstructionSet_Internal()

{
//int cpuInfo[4] = {-1};
std::array<int, 4> cpui;

// Calling __cpuid with 0x0 as the function_id argument
// gets the number of the highest valid function ID.
__cpuid(cpui.data(), 0);
nIds_ = cpui[0];

for (int i = 0; i <= nIds_; ++i)
{
__cpuidex(cpui.data(), i, 0);
data_.push_back(cpui);
}

// Capture vendor string
char vendor[0x20];
memset(vendor, 0, sizeof(vendor));
*reinterpret_cast<int*>(vendor) = data_[0][1];
*reinterpret_cast<int*>(vendor + 4) = data_[0][3];
*reinterpret_cast<int*>(vendor + 8) = data_[0][2];
vendor_ = vendor;
if (vendor_ == "GenuineIntel")
{
isIntel_ = true;
}
else if (vendor_ == "AuthenticAMD")
{
isAMD_ = true;
}

// load bitset with flags for function 0x00000001
if (nIds_ >= 1)
{
f_1_ECX_ = data_[1][2];
f_1_EDX_ = data_[1][3];
}

// load bitset with flags for function 0x00000007
if (nIds_ >= 7)
{
f_7_EBX_ = data_[7][1];
f_7_ECX_ = data_[7][2];
}

// Calling __cpuid with 0x80000000 as the function_id argument
// gets the number of the highest valid extended ID.
__cpuid(cpui.data(), 0x80000000);
nExIds_ = cpui[0];

char brand[0x40];
memset(brand, 0, sizeof(brand));

for (int i = 0x80000000; i <= nExIds_; ++i)
{
__cpuidex(cpui.data(), i, 0);
extdata_.push_back(cpui);
}

// load bitset with flags for function 0x80000001
if (nExIds_ >= 0x80000001)
{
f_81_ECX_ = extdata_[1][2];
f_81_EDX_ = extdata_[1][3];
}

// Interpret CPU brand string if reported
if (nExIds_ >= 0x80000004)
{
memcpy(brand, extdata_[2].data(), sizeof(cpui));
memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
brand_ = brand;
}
};

int nIds_;
int nExIds_;
std::string vendor_;
std::string brand_;
bool isIntel_;
bool isAMD_;
std::bitset<32> f_1_ECX_;
std::bitset<32> f_1_EDX_;
std::bitset<32> f_7_EBX_;
std::bitset<32> f_7_ECX_;
std::bitset<32> f_81_ECX_;
std::bitset<32> f_81_EDX_;
std::vector<std::array<int, 4>> data_;
std::vector<std::array<int, 4>> extdata_;
};
};

// Initialize static member data
const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;


// Print out supported instruction set extensions
int f()
{
auto& outstream = std::cout;

auto support_message = [&outstream](std::string isa_feature, bool is_supported) {
outstream << isa_feature << (is_supported ? " supported" : " not supported") << std::endl;
};

std::cout << InstructionSet::Vendor() << std::endl;
std::cout << InstructionSet::Brand() << std::endl;

support_message("3DNOW",       InstructionSet::_3DNOW());
support_message("3DNOWEXT",    InstructionSet::_3DNOWEXT());
support_message("ABM",         InstructionSet::ABM());
support_message("ADX",         InstructionSet::ADX());
support_message("AES",         InstructionSet::AES());
support_message("AVX",         InstructionSet::AVX());
support_message("AVX2",        InstructionSet::AVX2());
support_message("AVX512CD",    InstructionSet::AVX512CD());
support_message("AVX512ER",    InstructionSet::AVX512ER());
support_message("AVX512F",     InstructionSet::AVX512F());
support_message("AVX512PF",    InstructionSet::AVX512PF());
support_message("BMI1",        InstructionSet::BMI1());
support_message("BMI2",        InstructionSet::BMI2());
support_message("CLFSH",       InstructionSet::CLFSH());
support_message("CMPXCHG16B",  InstructionSet::CMPXCHG16B());
support_message("CX8",         InstructionSet::CX8());
support_message("ERMS",        InstructionSet::ERMS());
support_message("F16C",        InstructionSet::F16C());
support_message("FMA",         InstructionSet::FMA());
support_message("FSGSBASE",    InstructionSet::FSGSBASE());
support_message("FXSR",        InstructionSet::FXSR());
support_message("HLE",         InstructionSet::HLE());
support_message("INVPCID",     InstructionSet::INVPCID());
support_message("LAHF",        InstructionSet::LAHF());
support_message("LZCNT",       InstructionSet::LZCNT());
support_message("MMX",         InstructionSet::MMX());
support_message("MMXEXT",      InstructionSet::MMXEXT());
support_message("MONITOR",     InstructionSet::MONITOR());
support_message("MOVBE",       InstructionSet::MOVBE());
support_message("MSR",         InstructionSet::MSR());
support_message("OSXSAVE",     InstructionSet::OSXSAVE());
support_message("PCLMULQDQ",   InstructionSet::PCLMULQDQ());
support_message("POPCNT",      InstructionSet::POPCNT());
support_message("PREFETCHWT1", InstructionSet::PREFETCHWT1());
support_message("RDRAND",      InstructionSet::RDRAND());
support_message("RDSEED",      InstructionSet::RDSEED());
support_message("RDTSCP",      InstructionSet::RDTSCP());
support_message("RTM",         InstructionSet::RTM());
support_message("SEP",         InstructionSet::SEP());
support_message("SHA",         InstructionSet::SHA());
support_message("SSE",         InstructionSet::SSE());
support_message("SSE2",        InstructionSet::SSE2());
support_message("SSE3",        InstructionSet::SSE3());
support_message("SSE4.1",      InstructionSet::SSE41());
support_message("SSE4.2",      InstructionSet::SSE42());
support_message("SSE4a",       InstructionSet::SSE4a());
support_message("SSSE3",       InstructionSet::SSSE3());
support_message("SYSCALL",     InstructionSet::SYSCALL());
support_message("TBM",         InstructionSet::TBM());
support_message("XOP",         InstructionSet::XOP());
support_message("XSAVE",       InstructionSet::XSAVE());

return 0;
}









*/