#ifndef DOL_IMAGE_H
#define DOL_IMAGE_H
#include <iostream>
#include <fstream>
#include <vector>
#include "HufImage.hpp"
#include "io.hpp"

enum Dimension { none = -1, line, area };

class DolImage: HufImage
{
    std::ifstream* IFile=NULL;
    std::ofstream* OFile=NULL;
    char *DestName=NULL;
    long long IFileSize;
    int32_t Width;
    int32_t Width3;
    int32_t Height;
    uint32_t OffBits;
    uint16_t BitCount;
    const uint ClrUsed=256;
//    const uint16_t DolHeaderSize=20;
    uint8_t DolVersion;
    uint8_t DolSubVersion;
    uint8_t Flags;
    Dimension Mode;
    bool BottomUp;
    uint8_t *RBuffer,*WBuffer;
    uint8_t *Buffer0,*Buffer;
    int32_t RBufferSize,RBufferSize3;
    int32_t WBufferSize;
    uint8_t *SignCodes;
    uint8_t *SignCodes0;
    const uint8_t TRIPLE[3][3][3]=
    {
        {
            {1,3,13},
            {5,8,19},
            {11,17,25}
        },
        {
            {7,6,20},
            {4,2,14},
            {18,12,26}
        },
        {
            {9,15,23},
            {16,10,24},
            {21,22,0}
        }
    };
    const uint8_t RIPLE[27][3]={{2, 2, 2}, {0, 0, 0}, {1, 1, 1}, {0, 0, 1}, {1, 1, 0}, {0, 1, 0}, {1, 0, 1}, {1, 0, 0}, {0, 1, 1}, {2, 0, 0}, {2, 1, 1}, {0, 2, 0}, {1, 2, 1}, {0, 0, 2}, {1, 1, 2}, {2, 0, 1}, {2, 1, 0}, {0, 2, 1}, {1, 2, 0}, {0, 1, 2}, {1, 0, 2}, {2, 2, 0}, {2, 2, 1}, {2, 0, 2}, {2, 1, 2}, {0, 2, 2}, {1, 2, 2}};
    bool ReadHeader();
    bool ReadBMPHeader();
    void WriteBMPHeader();
    void WriteDOLHeader();
    bool ReadDOLHeader();
//    void Delta(P code1, P code2, P& delta, uint8_t& sign);
    void Delta(uint8_t code1, uint8_t code2, uint8_t* sign);
    uint8_t Mod(uint8_t delta);
#ifdef STAT
    int S2=0;
    int S0=0;
    int S1=0;
#endif
    void EncodeLine0();
    void EncodeLine();
    void DecodeLine0();
    void DecodeLine();
    void Compress(int32_t length);
    void DecompressHeader();
    void Decompress(int32_t length);
public:
    IMAGE_TYPE FileType;
    int32_t lengthRem;
    uint8_t intervalType;
    bool openDestination();
    void StoreAPX();
    void StoreBMP();
    void TransmitAPX();//100% identical to StoreAPS(). Just writing of the buffer is replaced with transmssion
    void TransmitBMP();//100% identical to StoreBMP(). Just reading of the buffer is replaced with reception
    DolImage(const char *file_name, int8_t dimension);
    ~DolImage();
};

#endif
