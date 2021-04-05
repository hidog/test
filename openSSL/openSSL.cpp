// openSSL.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>

#include <time.h>


unsigned char pvt_rsa[] = "-----BEGIN RSA PRIVATE KEY-----\n"\
"MIICXQIBAAKBgQDQ8Gy3PHKypyJ8ySov0IkH1oJLPLYuJFyjHrCGR//2qdHcIeqg\n"\
"LpryE6P4pu/gFljQ7ajXsekycD9B++hmUpe5xRnyH7Wxsl61vZ/pgOB1SGg0H4X1\n"\
"ZfXeZ+CsMgVGgaGI4O2vQDV4u8ONbGn1TifeufeddyzgrW3+asvbaygBYwIDAQAB\n"\
"AoGBAMYDdSTbFuCVYSveAis6eQOCmgzvxsBIGbtF9InuZnjATHFRe2i/gnz1fCrF\n"\
"GwTGaPnoomF47MaudmT0djWzBLKFp3hEnVGPfGJ/baWqV6EwvY7IBr12Mfnvmf36\n"\
"U7awc8gJn0Jy4KkajtUXfk8vXsjKdwBIifnIZjj/9LHz80BBAkEA70zo9EaBeO52\n"\
"yAyt09231RK7djzdAYg8H/TmyjUpfq6CGkK9+FAVCT/wF7I+GfDHBYeqDrG8QT/y\n"\
"+if0Ie0vcQJBAN+FHLTOAPHm8Xre9gkI/ZED8hjarTDVu0S/X6nm4KTl4miQRDWT\n"\
"Fkc1pC4M1agTrZkvc3jLMWeJ7bSGtrzfPBMCQQDKPSqnv6qOZKKYYTbLVpeViPZL\n"\
"cBarw9Jkg5pFDHbcLGmh/gQFBtEwdwvXSvl2/OmGf+g18PoT/xK/XeLShVzRAkAe\n"\
"MMiT009lXSA4zGzHA6PcNoA4wzCpa/UDcCXnuPuWykYugW31HozfyWsHnUwui5nj\n"\
"Gvmp0yg1J/gnbhq4oB5pAkBHRpg+U+6TljArdsAakvDR9i8Q65KUj50kOzxkZ3C/\n"\
"001Q9EFjj0HGiKkXZlP1Dje1vQEZghPaZrLLYiVBVPH2\n"\
"-----END RSA PRIVATE KEY-----\n";


RSA* createRSA(unsigned char* key)
{
    RSA *rsa = NULL;
    BIO *keybio;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio == NULL) {
        return 0;
    }
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    return rsa;
}




int _tmain(int argc, _TCHAR* argv[])
{
	std::ifstream logInFile(argv[1],std::ifstream::binary);
    std::ostringstream logEntries;
    std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
    std::cout.precision(2);
	if (logInFile.is_open()) {
        clock_t start = clock();
        logInFile.seekg(0, std::ios::end);
        int totalSize = logInFile.tellg();
		logInFile.seekg(0, std::ios::beg);
        float printProgress = 0, currentSize = 0;;
		unsigned char buffer[128];
        size_t size = sizeof(buffer);
        std::cout << "Decrypting log file:: size:" << totalSize << std::endl;
        int modSize = totalSize%size;
        if (modSize!=0)
            std::cout << "Log file size error: " << modSize << std::endl;
		while (logInFile.read((char*)buffer,size)&&(currentSize+size<=totalSize)) {
            unsigned char outBuffer[256];
			RSA *rsa = createRSA(pvt_rsa);
            int dec_len = RSA_private_decrypt(size,buffer,outBuffer,rsa,RSA_PKCS1_PADDING);
			//std::cout << "dec_len: " << dec_len << std::endl;
            if (dec_len>0) {
                if (dec_len<sizeof(outBuffer)) outBuffer[dec_len] = '\0';
                logEntries << std::string((char*)outBuffer);
            }
            currentSize += size;
            printProgress = (currentSize/totalSize);
            std::cout << "\r" << printProgress*100 << "% (" << (int)currentSize << "/" << totalSize << ") " << std::flush;
//            std::cout << std::string((char*)outBuffer) << std::endl;
		}
        printf("\nDecrypting time spent: %.2f sec\n", (double)(clock()-start)/CLOCKS_PER_SEC);
	}
    if (argv[2]) {
        std::ofstream outFile(argv[2]);
        outFile << logEntries.str();
        outFile.close();
    } else {
        std::cout << logEntries.str() << std::endl;
    }

	//system("PAUSE");
	return 0;
}

