#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include "globals.h"

#define E_CORRUPTEDFILE 1050

extern int loadfile(unsigned char** pbBuffer, unsigned long& dwArraySize, const char* strFilename);
extern int writefile(unsigned char* pbBuffer, unsigned long dwArraySize, const char* strFilename);

extern unsigned short getWordInverted(unsigned int Offset, unsigned char* pbData);
extern unsigned long  getDWordInverted(unsigned int Offset, unsigned char* pbData);

extern void writeWord(unsigned int* Offset, unsigned short Value, unsigned char* pbData);
extern void writeDWord(unsigned int* Offset, unsigned int Value, unsigned char* pbData);
//returns true when the point is inside the square
extern bool checkBoundaries(UVector2& point, UVector2& leftTop, UVector2& squareSize); 

#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_DELETE(p)       { if(p) { delete (p)  ;   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#endif