#ifndef GETBMP_H
#define GETBMP_H

using namespace std;

struct BitMapFile
{
    int sizeX;
    int sizeY;
    unsigned char* data;
};

BitMapFile* getbmp(std::string filename);

#endif