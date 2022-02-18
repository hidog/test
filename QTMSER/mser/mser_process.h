#ifndef MSER_PROCESS_H
#define MSER_PROCESS_H

#include <vector>
#include "mser.h"


using namespace std;


bool loadJpeg(const char * filename, int & width, int & height, int & depth, vector<uint8_t> & bits);
QImage saveJpeg( int width, int height, int depth, vector<uint8_t> & bits);
void drawEllipse(const MSER::Region & region, int width, int height, int depth, vector<uint8_t> & bits, const uint8_t * color);

QImage mser_process( const char* filepath );

#endif
