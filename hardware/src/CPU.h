#ifndef CPU_H
#define CPU_H

#include <string>

namespace cpu 
{

/*

ubuntu command
CPU ID
sudo dmidecode -t 4 | grep ID
MB
sudo dmidecode -t 2 | grep Serial
MAC
sudo lshw -c network | grep serial | head -n 1

參考資料
http://en.wikipedia.org/wiki/CPUID#EAX.3D80000002h.2C80000003h.2C80000004h:_Processor_Brand_String  wiki CPU ID
http://stackoverflow.com/questions/14266772/how-do-i-call-cpuid-in-linux   未使用
http://stackoverflow.com/questions/1666093/cpuid-implementations-in-c  windows取得製造商的做法 
http://msdn.microsoft.com/en-us/library/hskdteyh.aspx   MicroSoft的CPUID  但是code有編譯錯誤 
http://www.codeproject.com/Questions/437627/Processor-serial-number
http://www.codeproject.com/Questions/437379/Cplusplus-source-code-to-get-CPU-number-motherboar   use this to modify....

 
這邊討論串看到一些資料,但實際執行結果未必跟想要的相符
https://stackoverflow.com/questions/2901694/how-to-detect-the-number-of-physical-processors-cores-on-windows-mac-and-linu
討論串看到這個,別人寫的library. 
https://github.com/anrieff/libcpuid
*/


int get_real_cpu_core();
int get_logical_cpu_core();

std::string get_cpu_vender();		// 取得製造商
std::string get_cpu_serial();		// 取得序號

std::string cpu_get_serial_asm();



} // end namespace cpu



void test_cpu_info_func();



#endif
