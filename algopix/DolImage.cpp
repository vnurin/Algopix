//
//  DolImage.cpp
//  algopix
//
//  Created by Vahagn Nurijanyan on 2018-08-30.
//  Copyright © 2018 BABELONi INC. All rights reserved.
//

#include "DolImage.hpp"

DolImage::DolImage(const char *file_name, int8_t dimension)
{
    IFile=new std::ifstream(file_name, std::ios_base::binary);
    if(!IFile)
    {
        std::cout << file_name << " source file can't be opened!\n";
        FileType=T_UNKNOWN;
        return;
    }
    IFile->seekg(0,std::ios_base::end);
    IFileSize=IFile->tellg();
    if(IFileSize==0)
    {
        std::cout << file_name << " source file is empty!\n";
        FileType=T_UNKNOWN;
        return;
    }
    else if(IFileSize==-1)
    {
        std::cout << file_name << " source file doesn't exist!\n";
        FileType=T_UNKNOWN;
        return;
    }
    const char *ext[2]={".bmp",".apx"};
    if(strstr(file_name,ext[0])) {
        FileType=T_BMP;
        Mode=Dimension(dimension);
        DestName = new char[strlen(file_name)+1];
        strcpy(DestName, file_name);
        DestName[strlen(file_name)-4] = 0;
        strcat(DestName, ext[1]);
    }
    else if(strstr(file_name,ext[1])) {
        FileType=T_APX;
        DestName = new char[strlen(file_name)+1];
        strcpy(DestName, file_name);
        DestName[strlen(file_name)-strlen(ext[0])] = 0;
        strcat(DestName, ext[0]);
    }
    else {
        std::cout << file_name << " source file is unknown!\n";
        FileType=T_UNKNOWN;
        return;
    }
    IFile->seekg(0,std::ios_base::beg);
    if(!ReadHeader()) {
        FileType=T_UNKNOWN;
        return;
    }
}

DolImage::~DolImage()
{
    if(IFile) {
        IFile->close();
        delete IFile;
        if(DestName) {
            delete[] DestName;
        }
        if(OFile) {
            OFile->close();
            delete OFile;
        }
    }
}

bool DolImage::openDestination() {
    OFile=new std::ofstream(DestName, std::ios_base::binary);
    if(!OFile)
    {
        std::cout << DestName << " destination file can't be opened!\n";
        return false;
    }
    return true;
}

bool DolImage::ReadHeader()
{
    if(IFileSize<20)
    {
        std::cout << "Source file is too short!\n";
        return false;
    }
    else
    {
        char type1,type2;
        IFile->get(type1);
        IFile->get(type2);
        if(FileType==T_BMP) {
            if(type1=='B' && type2=='M')
            {
                return ReadBMPHeader();
            }
            else
            {
                std::cout  << "Source BMP file is invalid!\n";
                return false;
            }
        }
        else
        {
            if(type1=='A' && type2=='P')
            {
                return ReadDOLHeader();
            }
            else
            {
                std::cout  << "Source APX file is invalid!\n";
                return false;
            }
        }
        return true;
    }
}

bool DolImage::ReadBMPHeader()
{
    if(IFileSize<=26) {
        std::cout  << "Source BMP file is too small!\n";
        return false;
    }
    uint32_t reserved;
    IFile->read((char*)&reserved,4);//bfSize
    if(reserved!=0 && reserved!=IFileSize) {
        std::cout  << "Source BMP file's size is wrong!\n";
        return false;
    }
    IFile->read((char*)&reserved,4);//bfReserved1,bfReserved2
    if(reserved!=0) {
        std::cout  << "Source BMP file's Reserved is wrong!\n";
        return false;
    }
    IFile->read((char*)&OffBits,4);//bfOffBits
    if(OffBits<26 || OffBits>=IFileSize) {
        std::cout  << "Source BMP file's OffBits is wrong!\n";
        return false;
    }
    IFile->read((char*)&reserved,4);//bcSize or biSize or bV4Size or bV5Size
/*lena.    if(reserved!=40)
    {
        std::cout  << "Source BMP file's header size is wrong or BMP file is old!\n";
        return false;
    }*/
    IFile->read((char*)&Width,4);//biWidth or bV4Width or bV5Width
    IFile->read((char*)&Height,4);//biHeight or bV4Height or bV5Height
    if(Width<=1)
    {
        std::cout  << "Source BMP file's Width is 0 or 1!\n";
        return false;
    }
    if(Height>0)
    {
        BottomUp=true;
    }
    else
    {
        Height=-Height;
        BottomUp=false;
    }
    uint16_t planes;
    IFile->read((char*)&planes,2);//bcPlanes or biPlanes or bV4Planes or bV5Planes
    if(planes!=1)
    {
        std::cout  << "Source BMP file's Planes is wrong!\n";
        return false;
    }
    IFile->read((char*)&BitCount,2);//bcBitCount or biBitCount or bV4BitCount or bV5BitCount
    if(BitCount!=24)
    {
        std::cout  << "Source BMP file has unknown number of bits-per-pixel: "<<BitCount<<".\n";
        return false;
    }
    if(BitCount==0)
    {
        std::cout << "BMP files with JPEG and PNG compressions are not supported!\n";
        return false;
    }
    IFile->read((char*)&reserved,4);//biCompression or bV4Compression or bV5Compression
    if(reserved!=0)
    {
        std::cout << "BMP files with compression are not supported!\n";
        return false;
    }
    IFile->seekg(OffBits, std::ios_base::beg);
    return true;
}

void DolImage::WriteBMPHeader()
{
    OFile->put('B');
    OFile->put('M');
    OFile->seekp(4,std::ios_base::cur);//bfSize
    uint32_t reserved=0;
    OFile->write((char*)&reserved,4);//bfReserved1,bfReserved2
    OffBits=54;
    OFile->write((char*)&OffBits,4);//bfOffBits
    reserved=40;
    OFile->write((char*)&reserved,4);//biSize
    OFile->write((char*)&Width,4);//biWidth
     uint32_t height=-Height;
    OFile->write((char*)&height,4);//biHeight
//    OFile->write((char*)&Height,4);//biHeight
    uint16_t planes=1;
    OFile->write((char*)&planes,2);//biPlanes
    BitCount=24;
    OFile->write((char*)&BitCount,2);//biBitCount
    reserved=0;
    OFile->write((char*)&reserved,4);//biCompression
    OFile->seekp(4,std::ios_base::cur);//biSizeImage
    OFile->write((char*)&reserved,4);//biXPelsPerMeter
    OFile->write((char*)&reserved,4);//biYPelsPerMeter
    OFile->write((char*)&ClrUsed,4);//biClrUsed
    OFile->write((char*)&reserved,4);//biClrImportant
}

void DolImage::WriteDOLHeader()
{
    OFile->write("AP",2);
    DolVersion=4;
    DolSubVersion=0;
    uint8_t version=(DolVersion<<4)+DolSubVersion;
    if(Mode==line)
    {
        Flags=0x00;
    }
    else
    {
        Flags=0x01;
    }
    OFile->put(version);//APX version & subversion
    OFile->put(Flags);
    OFile->seekp(4,std::ios_base::cur);//file size
    OFile->write((char*)&Width,2);
    OFile->write((char*)&Height,2);
    OFile->put(BitCount);
    OFile->put(3);//delta method
    OFile->put(0);//text size
    OFile->write((char*)RBuffer,3);
}

bool DolImage::ReadDOLHeader()
{
    char c;
    IFile->get(c);
    uint32_t uc=c;
    DolVersion=uc>>4;
    DolSubVersion=uc&15;
    if(DolVersion!=4 || DolSubVersion!=0)
    {
        std::cout  << "Source APX file's version is unknown!\n";
        return false;
    }
    IFile->get(c);
    Flags=c;
    if(Flags!=0 && Flags!=1)
    {
        std::cout  << "Source APX file's Flags is invalid!\n";
        return false;
    }
    Mode=Dimension(Flags&0x01);
    uint32_t size;
    IFile->read((char*)&size,4);
    if(size!=0 && size!=IFileSize)
    {
        std::cout  << "Source APX file is invalid!\n";
        return false;
    }
    Width=0;
    IFile->read((char*)&Width,2);
    Height=0;
    IFile->read((char*)&Height,2);
    if(Width==0 || Height==0)
    {
        std::cout  << "Source APX file's Width and Height is zero!\n";
        return false;
    }
    IFile->get(c);
    BitCount=c;
    if(BitCount!=24){
        std::cout  << "Source APX file's bit count is invalid!\n";
        return false;
    }
    IFile->get(c);
    if(c!=3) {
    if(c>=0 && c<3){
        std::cout  << "Source APX file's Delta Method is invalid!\n";
        return false;
    }
    else {
        std::cout  << "Source APX file's Delta Method is not supported!\n";
        return false;

    }
    }
    IFile->seekg(1, std::ios_base::cur);
    return true;
}

void DolImage::EncodeLine0()
{
    DeltaOffset=0;
    uint8_t code1[3];
//    uint8_t code2[3]={0, 0, 0};
    uint8_t code2[3]={Buffer0[0],Buffer0[1],Buffer0[2]};
    Buffer0+=3;
    SignCodes0=SignCodes;
    int w=1;
    do {
        for(int s=0; s<3; s++)
        {
            code1[s]=*Buffer0++;
            Delta(code1[s], code2[s], SignCodes0++);
            code2[s]=code1[s];
        }
    } while(++w<Width);
}

void DolImage::EncodeLine()
{
//*ATON.
    uint8_t code1[3]={Buffer[0],Buffer[1],Buffer[2]},code2[3]={Buffer0[0],Buffer0[1],Buffer0[2]};
    Buffer+=3;
    Buffer0+=3;
//*/
    for(int s=0;s<3;s++)
    {
//ATON.       code1[s]=*Buffer++;
//ATON.        code2[s]=*Buffer0++;
        Delta(code1[s], code2[s], SignCodes0++);
    }
    int w=1;
    do {
        for(int s=0;s<3;s++)
        {
            code1[s]=*Buffer;
            code2[s]=Mod(*(Buffer-3)+*Buffer0-*(Buffer0-3));
            Delta(code1[s], code2[s], SignCodes0++);
            Buffer0++;
            Buffer++;
        }
    } while(++w<Width);
}
void DolImage::DecodeLine0() {
    //    uint8_t code[3]={0, 0, 0};
    uint8_t code[3]={Buffer0[0],Buffer0[1],Buffer0[2]};
    uint8_t *signCodes=SignCodes;
    uint8_t *deltaCodes=DeltaCodes;
    int i=3;
    do {
//        signCode=RIPLE[SignCodes[w]];
        for(int s=0; s<3; s++)
        {
            if(signCodes[s]==2) {
                Buffer0[i+s]=code[s];
            }
            else if(signCodes[s]==1) {
                Buffer0[i+s]=Mod(code[s]-*deltaCodes);
                deltaCodes++;
                code[s]=Buffer0[i+s];
            }
            else// if(signCodes[s]==0)
            {
                Buffer0[i+s]=Mod(code[s]+*deltaCodes);
                deltaCodes++;
                code[s]=Buffer0[i+s];
            }
        }
        signCodes+=3;
        i+=3;
    } while (i<Width3);
}

void DolImage::DecodeLine() {
    uint8_t code[3]={Buffer0[0],Buffer0[1],Buffer0[2]};
//    const uint8_t *signCode=RIPLE[SignCodes[0]];
    uint8_t *signCodes=SignCodes;
    uint8_t *deltaCodes=DeltaCodes;
    for(int s=0; s<3; s++)
    {
        code[s]=Buffer0[s];
        if(signCodes[s]==2) {
            Buffer[s]=code[s];
        }
        else if(signCodes[s]==1) {
            Buffer[s]=Mod(code[s]-*deltaCodes);
            deltaCodes++;
        }
        else// if(signCodes[s]==0)
        {
            Buffer[s]=Mod(code[s]+*deltaCodes);
            deltaCodes++;
        }
    }
    int i=3;
    do {
        signCodes+=3;
//        signCode=RIPLE[SignCodes[w]];
        for(int s=0; s<3; s++)
        {
            code[s]=Mod(Buffer[i-3+s]+Buffer0[i+s]-Buffer0[i-3+s]);
            if(signCodes[s]==2) {
                Buffer[i+s]=code[s];
            }
            else if(signCodes[s]==1) {
                Buffer[i+s]=Mod(code[s]-*deltaCodes);
                deltaCodes++;
            }
            else// if(signCode[s]==0)
            {
                Buffer[i+s]=Mod(code[s]+*deltaCodes);
                deltaCodes++;
            }
        }
        i+=3;
    } while(i<Width3);
}

void DolImage::Compress(int32_t length) {
    uint32_t triple_count[SHQ];
    uint32_t count[129];
    uint32_t count1[128];
    uint32_t count3[HQ3];
    int row_size;
    uint8_t gn;
    uint32_t rem;
    std::vector<uint32_t> lengths[3]={std::vector<uint32_t>(),std::vector<uint32_t>(),std::vector<uint32_t>()};
    std::vector<uint32_t> *length0,
    *length1,*length2;
    const int8_t TRIPLE_15[27]={-2,-1,-1,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11};
    length0=&lengths[0];
    length1=&lengths[1];
    length2=&lengths[2];
    int8_t triple;
    int quantity=0;
    for(int i=0;i<SHQ;i++)
        triple_count[i]=0;
    for(int i=1;i<129;i++)
        count[i]=0;
    for(int i=1;i<128;i++)
        count1[i]=0;
    for(int i=0;i<7;i++)
        count3[i]=0;
    std::vector<int8_t> intervalTypes;
    std::vector<bool> intervalTypes1;
    triple=TRIPLE[SignCodes[0]][SignCodes[1]][SignCodes[2]];
    if(triple==0) {
        intervalType=0;
    }
    else if(triple<=2) {
        intervalType=1;
    }
    else {
        intervalType=2;
    }
    intervalTypes.push_back(intervalType);
    intervalTypes1.push_back(intervalType%2 ? true : false);
    uint32_t i=0,j=0;
    DeltaOffset=0;
    do {
        switch (intervalType) {
            case 0:
                do {
/*                    Signs[j]=triple;
                    triple=TRIPLE_15[triple];*/
                    j++;
                    if(j<length) {
                        i+=3;
                        triple=TRIPLE[SignCodes[i]][SignCodes[i+1]][SignCodes[i+2]];
                        if(triple==0) {
                            quantity++;
                        }
                        else {
                            if(triple==1 || triple==2) {
                                intervalType=1;
                                intervalTypes1.push_back(false);
                            }
                            else {
                                intervalType=2;
                                intervalTypes1.push_back(true);
                            }
                            intervalTypes.push_back(intervalType);
                            length0->push_back(quantity);
                            quantity=0;
                            break;
                        }
                    }
                    else {
                        break;
                    }
                } while(true);
                break;
            case 1:
                do {
                    Signs[j]=triple;
//                    triple=TRIPLE_15[triple];
                    Deltas[j]=DeltaCodes[DeltaOffset];
                    Ones[j]=0;
                    if(DeltaCodes[DeltaOffset]==DeltaCodes[DeltaOffset+1] && DeltaCodes[DeltaOffset+1]==DeltaCodes[DeltaOffset+2])
                    {
                        count3[0]++;
                    }
                    else
                    {
                        for(int k=1;k<3;k++)
                        {
                            if(DeltaCodes[DeltaOffset+k]<Deltas[j])
                            {
                                Deltas[j]=DeltaCodes[DeltaOffset+k];
                            }
                        }
                        for(int k=0;k<3;k++)
                        {
                            DeltaCodes[DeltaOffset+k]-=Deltas[j];
                            if(DeltaCodes[DeltaOffset+k]!=0)
                            {
                                Ones[j]|=1<<k;
                                count1[DeltaCodes[DeltaOffset+k]]++;
                            }
                        }
                        count3[Ones[j]]++;
                    }
                    DeltaOffset+=3;
                    count[Deltas[j]]++;
                    j++;
                    if(j<length) {
                        i+=3;
                        triple=TRIPLE[SignCodes[i]][SignCodes[i+1]][SignCodes[i+2]];
                        if(triple==1 || triple==2) {
                            quantity++;
                        }
                        else {
                            if(triple==0) {
                                intervalType=0;
//FSTR.                                intervalTypes1.push_back(true);
                                intervalTypes1.push_back(false);//FSTR.
                            }
                            else {
                                intervalType=2;
//FSTR.                                intervalTypes1.push_back(false);
                                intervalTypes1.push_back(true);//FSTR.
                            }
                            intervalTypes.push_back(intervalType);
                            length1->push_back(quantity);
                            quantity=0;
                            break;
                        }
                    }
                    else {
                        break;
                    }
                } while(true);
                break;
            default:
                do {
                    Signs[j]=triple;
//                    triple=TRIPLE_15[triple];
                    triple_count[TRIPLE_15[triple]]++;
                    if(triple<=8)
                    {
                        for(int k=0;k<3;k++)
                        {
                            count[DeltaCodes[DeltaOffset++]]++;
                        }
                    }
                    else if(triple<=20)
                    {
                        for(int k=0;k<2;k++)
                        {
                            count[DeltaCodes[DeltaOffset++]]++;
                        }
                    }
                    else {
                        count[DeltaCodes[DeltaOffset++]]++;
                    }
                    j++;
                    if(j<length) {
                        i+=3;
                        triple=TRIPLE[SignCodes[i]][SignCodes[i+1]][SignCodes[i+2]];
                        if(triple>2) {
                            quantity++;
                        }
                        else {
                            if(triple==0) {
                                intervalType=0;
                                intervalTypes1.push_back(false);
                            }
                            else {
                                intervalType=1;
                                intervalTypes1.push_back(true);
                            }
                            intervalTypes.push_back(intervalType);
                            length2->push_back(quantity);
                            quantity=0;
                            break;
                        }
                    }
                    else {
                        break;
                    }
                } while(true);
                break;
        }
    } while(j<length);
    lengths[intervalType].push_back(quantity);
    row_size=TripleSize(triple_count,lengths);
    if(QLHL[1]>0 || QLHL[2]>0) {
        row_size+=DeltaSize(count,count1,count3);
    }
    WBufferSize=row_size/8;
    if(row_size%8!=0)
        WBufferSize++;
    WBuffer=new uint8_t[WBufferSize];
    w_buffer=(char*)WBuffer;
    w_buf_index=0;
    w_byte=0;
    w_pos=8;
    for(int z=0;z<3;z++) {
        w_base=4;
        write(QLHL[z]);
        if(QLHL[z]>1)
            WriteHuffmanTable(LHufLCodeLength[z],QLHL[z],HufLTree[z]);
    }
//    if(QLHL[2]>0) {
    if(Presented!=1) {
        w_base=4;
        write(QSH);
        if(QSH>1)
            WriteHuffmanTable(SHufCodeLength,QSH,HufSTree);
    }
    if(QLHL[1]>0 || QLHL[2]>0) {
        w_base=4;
        write(QH);
        if(QH>1)
        {
            WriteHuffmanTable(HufCodeLength,QH,HufTree);
            if(QLHL[1]>0) {
                w_base=3;
                write(QH3);
                if(QH3>1) {
                    WriteHuffmanTable(HufCodeLength3,QH3,HufTree3);
                    w_base=4;
                    write(QH1);
                    if(QH1>1)
                    {
                        WriteHuffmanTable(HufCodeLength1,QH1,HufTree1);
                    }
                }
            }
        }
    }
    uint32_t len;
    int q;
    uint32_t index=0;
    uint32_t right=0;
    int number[3]={0,0,0};
    u_long interval_number=0;
    if(Presented==0) {
//        w_base=1;
        write(intervalTypes[0]>>1!=0);
    }
    else {
        write(intervalTypes[0]!=0);
    }
    DeltaOffset=0;
    do {
        if(Presented==0) {
            write(intervalTypes1[interval_number]);
        }
        intervalType=intervalTypes[interval_number];
        len=lengths[intervalType][number[intervalType]++];
        right+=len+1;
        q=0;
        if(QLHL[intervalType]>2)
        {
            while(len>=4097)
            {
                len-=4097;
                q++;
                w_base=LHufLCodeLength[intervalType][LHQ-1];
                write(HufLTree[intervalType][LHQ-1]);
                w_base=ExtraBits2[LHQ-1];
                write((uint16_t)2047);
            }
            GroupNumber1(len,gn,rem);
            w_base=LHufLCodeLength[intervalType][gn];
            write(HufLTree[intervalType][gn]);
            if(ExtraBits2[gn]!=0)
            {
                w_base=ExtraBits2[gn];
                write(rem);
            }
        }
        else if(QLHL[intervalType]==2)
            write(len!=0);
        switch (intervalType) {
            case 0:
                index=right;
                break;
            case 1:
                if(QH>2)
                {
                    do {
                        write(Signs[index]==2);
                        WriteCompDelta1(Deltas[index],HufCodeLength,HufCodeLength1,HufCodeLength3,Ones[index]);
                    } while(++index<right);
                }
                else if(QH==2)
                {
                    do {
                        write(Signs[index]==2);
                        WriteCompDelta1(Deltas[index],HufCodeLength3,Ones[index]);
                    } while(++index<right);
                }
                else// if(QH==1)
                {
                    do {
                        write(Signs[index]==2);
                    } while(++index<right);
                }
                break;
            default:
                if(QH>2)
                {
                    do {
                        uint8_t triple=Signs[index];
                        WriteCompSign(triple,TRIPLE_15[triple],SHufCodeLength);
                        WriteCompDelta2(triple,HufCodeLength);
                    } while(++index<right);
                }
                else if(QH==2)
                {
                    do {
                        uint8_t triple=Signs[index];
                        WriteCompSign(triple,TRIPLE_15[triple],SHufCodeLength);
                        WriteCompDelta2(triple);
                    } while(++index<right);
                }
                else// if(QH==1)
                {
                    do {
                        uint8_t triple=Signs[index];
                        WriteCompSign(triple,TRIPLE_15[triple],SHufCodeLength);
                    } while(++index<right);
                }
                break;
        }
    }while(++interval_number<intervalTypes.size());
    if(w_pos!=8)
        WBuffer[w_buf_index]=w_byte;
#ifdef DEBG
    uint32_t size=w_buf_index;
    if(w_pos!=8)
        size++;
    std::cout<<std::to_string(h)<<'\t';
    if(size!=WBufferSize) {
        std::cout<<std::to_string(WBufferSize)<<' '<<std::to_string(size)<<'\n';
        exit(7);
    }
    h++;
#endif
}

void DolImage::DecompressHeader() {
    r_buffer=(char*)RBuffer;
//    if(Mode==line) {
        r_buf_index=0;
        r_pos=0;
//    }
    for(int z=0;z<3;z++) {
        r_base=4;
        read(QLHL[z]);
        if(QLHL[z]>1)
        {
            ReadHuffmanTable(LLength[z],QLHL[z]);
            if(QLHL[z]>2)
                HuffmanTree(LLength[z],QLHL[z],LTree[z]);
        }
    }
    
    if(QLHL[1]>0 && QLHL[2]>0) {
        Presented=0;
    }
    else if(QLHL[2]==0) {
        Presented=1;
    }
    else// if(QLHL[1]==0)
    {
        Presented=2;
    }
    if(QLHL[2]>0) {
        r_base=4;
        read(QSH);
        if(QSH>1)
        {
            ReadHuffmanTable(SLength,QSH);
            if(QSH>2)
                HuffmanTree(SLength,QSH,STree);
        }
    }
    if(QLHL[1]>0 || QLHL[2]>0) {
        r_base=4;
        read(QH);
        if(QH>1)
        {
            ReadHuffmanTable(Length,QH);
            if(QH>2) {
                ReaCo=&HufImage::ReadCompPixel1;
                ReaDe=&HufImage::ReadDelta;
                HuffmanTree(Length,QH,Tree);
            }
            else//if(QH==2)
            {
                ReaCo=&HufImage::ReadCompPixel1_2;
                ReaDe=&HufImage::ReadDelta_2;
            }
            if(QLHL[1]>0) {
                r_base=3;
                read(QH3);
                if(QH3>1)
                {
                    ReadHuffmanTable(Length3,QH3);
                    if(QH3>2)
                        HuffmanTree(Length3,QH3,Tree3);
                    r_base=4;
                    read(QH1);
                    if(QH1>1)//if(QH>2)
                    {
                        ReadHuffmanTable(Length1,QH1);
                        if(QH1>2) {
                            ReaDe1=&HufImage::ReadDelta;
                            HuffmanTree(Length1,QH1,Tree1);
                        }
                        else if(QH1==2) {
                            ReaDe1=&HufImage::ReadDelta_2;
                        }
                    }
                    else if(QH>2)//if(QH1==1)
                    {
                        ReaDe1=&HufImage::ReadDelta_1;
                    }
                }
            }
        }
        else//if(QH==1)
        {
            ReaCo=&HufImage::ReadCompPixel1_1;
            ReaDe=&HufImage::ReadDelta_1;
        }
    }
    lengthRem=0;
    intervalType=0xFF;
}

void DolImage::Decompress(int32_t length) {
    uint8_t triple;
    uint32_t len;
    uint32_t w=0;
    Signs=SignCodes;
    Deltas=DeltaCodes;
    do {
        if(lengthRem==0) {
            if(intervalType!=0xFF) {
                if(Presented==0)
                {
                    bool type;
                    read(type);
/*FSTR.
                    if(type) {
                        if(intervalType==0) {
                            intervalType=2;
                        }
                        else if(intervalType==1) {
                            intervalType=0;
                        }
                        else {
                            intervalType=1;
                        }
                    }
                    else {
                        if(intervalType==0) {
                            intervalType=1;
                        }
                        else if(intervalType==1) {
                            intervalType=2;
                        }
                        else {
                            intervalType=0;
                        }
                    }
*/
//*FSTR.
                    if(type) {
                        if(intervalType!=2) {
                            intervalType=2;
                        }
                        else {
                            intervalType=1;
                        }
                    }
                    else {
                        if(intervalType!=0) {
                            intervalType=0;
                        }
                        else {
                            intervalType=1;
                        }
                    }
//*/
                }
                else {
                    intervalType=intervalType == 0 ? Presented : 0;
                }
            }
            else {
                if(Presented==0) {
                    r_base=2;
                    read(intervalType);
                }
                else {
                    r_base=1;
                    read(intervalType);
                    if(Presented==2 && intervalType==1) {
                        intervalType++;
                    }
                }
            }
            uint32_t len1;
            if(QLHL[intervalType]>2)
            {
                uint32_t l;
                len1=1;
                do {
                    uint8_t g_n;
                    ReadCode(g_n,LTree[intervalType]);
                    if(ExtraBits2[g_n]==0)
                        l=g_n;
                    else
                    {
                        uint16_t rem;
                        r_base=ExtraBits2[g_n];
                        read(rem);
                        l=Borders1[g_n-1]+rem;
                    }
                    len1+=l;
                }while(l==4097);
            }
            else if(QLHL[intervalType]==2)
            {
                bool bl;
                read(bl);
                if(bl)
                    len1=2;
                else
                    len1=1;
            }
            else
                len1=1;
            len=w+len1;
            if(len>length)
            {
                lengthRem=len-length;
                len=length;
            }
        }
        else {
            if(lengthRem<=length) {
                len=lengthRem;
                lengthRem=0;
            }
            else {
                len=length;
                lengthRem-=length;
            }
        }
        switch (intervalType) {
            case 0:
                do {
                    for(int s=0;s<3;s++) {
                        Signs[s]=2;
                    }
                    Signs+=3;
                } while(++w<len);
                break;
            case 1:
                bool trip;//FTPL.
/*
                if(QH>2)
                {
                    do {
                        read(trip);//FTPL.
                        triple=trip?1:0;//FTPL.
//FTPL.                        r_base=1;
//FTPL.                        read(triple);
                        ReadCompPixel1(triple);
                        Signs+=3;
                        Deltas+=3;
                    } while(++w<len);
                }
                else if(QH==2)
                {
                    do {
                        read(trip);//FTPL.
                        triple=trip?1:0;//FTPL.
//FTPL.                        r_base=1;
//FTPL.                        read(triple);
                        ReadCompPixel1_2(triple);
                        Signs+=3;
                        Deltas+=3;
                    } while(++w<len);
                }
                else
                {
                    do {
                        read(trip);//FTPL.
                        triple=trip?1:0;//FTPL.
//FTPL.                        r_base=1;
//FTPL.                        read(triple);
                        ReadCompPixel1_1(triple);
                        Signs+=3;
                        Deltas+=3;
                    } while(++w<len);
                }
*/
                do {
                    read(trip);//FTPL.
                    triple=trip?1:0;//FTPL.
//FTPL.                        r_base=1;
//FTPL.                        read(triple);
                    (this->*ReaCo)(triple);
                    Signs+=3;
                    Deltas+=3;
                } while(++w<len);
                break;
            default:
                do {
                    ReadCode(QSH,triple,STree);
                    ReadCompPixel2(triple);
                    Signs+=3;
                } while(++w<len);
                break;
        }
    } while(len<length);
}

void DolImage::StoreAPX() {
    if(Mode==line) {
        RBufferSize=Width;
    }
    else {
        RBufferSize=Width*Height;
    }
    RBufferSize3=3*RBufferSize;
    Width3=3*Width;
    RBuffer=new uint8_t[2*Width3];
    uint pad=Width3%4;
    if(pad!=0)
        pad=4-pad;
    if(BottomUp) {
        IFile->seekg(OffBits+(Height-1)*(Width3+pad),std::ios_base::beg);
    }
    else {
        IFile->seekg(OffBits,std::ios_base::beg);
    }
    IFile->read((char*)RBuffer,Width3);
    WriteDOLHeader();
    SignCodes=new uint8_t[RBufferSize3];
    DeltaCodes=new uint8_t[RBufferSize3];
    Signs=new uint8_t[RBufferSize];
    Deltas=new uint8_t[RBufferSize];
    Ones=new uint8_t[RBufferSize];
    Buffer0=RBuffer;
    EncodeLine0();
    if(Mode==line) {
        Compress(RBufferSize-1);
        OFile->write((char*)&WBufferSize,2);
        OFile->write((char*)WBuffer,WBufferSize);
        delete[] WBuffer;
        DeltaOffset=0;
        SignCodes0=SignCodes;
    }
    bool orderIsReverse=false;
//    Buffer=RBuffer+Width3;
    if(BottomUp) {
        for(int h=Height-2; h>=0; h--) {
            if(orderIsReverse) {
                Buffer0=RBuffer+Width3;
                Buffer=RBuffer;
            }
            else {
                Buffer0=RBuffer;
                Buffer=RBuffer+Width3;
            }
            orderIsReverse=!orderIsReverse;
            IFile->seekg(OffBits+h*(Width3+pad),std::ios_base::beg);
            IFile->read((char*)Buffer,Width3);
            EncodeLine();
            if(Mode==line) {
                Compress(RBufferSize);
                OFile->write((char*)&WBufferSize,2);
                OFile->write((char*)WBuffer,WBufferSize);
                delete[] WBuffer;
                DeltaOffset=0;
                SignCodes0=SignCodes;
            }
        }
    }
    else {
        for(int h=1; h<Height; h++) {
            if(orderIsReverse) {
                Buffer0=RBuffer+Width3;
                Buffer=RBuffer;
            }
            else {
                Buffer0=RBuffer;
                Buffer=RBuffer+Width3;
            }
            orderIsReverse=!orderIsReverse;
//            IFile->seekg(OffBits+h*(Width3+pad),std::ios_base::beg);
            IFile->seekg(pad,std::ios_base::cur);
            IFile->read((char*)Buffer,Width3);
            EncodeLine();
            if(Mode==line) {
                Compress(RBufferSize);
                OFile->write((char*)&WBufferSize,2);
                OFile->write((char*)WBuffer,WBufferSize);
                delete[] WBuffer;
                DeltaOffset=0;
                SignCodes0=SignCodes;
            }
        }
    }
    if(Mode==area) {
        Compress(RBufferSize-1);
        OFile->write((char*)&WBufferSize,4);
        OFile->write((char*)WBuffer,WBufferSize);
        delete[] WBuffer;
    }
    delete[] Ones;
    delete[] Deltas;
    delete[] Signs;
    delete[] DeltaCodes;
    delete[] SignCodes;
    delete[] RBuffer;
}

void DolImage::StoreBMP() {
    Width3=3*Width;
    uint pad=Width3%4;
    if(pad!=0)
        pad=4-pad;
    WriteBMPHeader();
    WBuffer=new uint8_t[2*Width3];
    IFile->read((char*)WBuffer,3);
    if(Mode==line) {
        RBufferSize=0;
        IFile->read((char*)&RBufferSize, 2);
    }
    else {
        IFile->read((char*)&RBufferSize, 4);
    }
    RBuffer=new uint8_t[RBufferSize];
    IFile->read((char*)RBuffer,RBufferSize);
    TriIn[0]=&HufImage::TripleInit3;
    TriIn[1]=&HufImage::TripleInit4;
    TriIn[2]=&DolImage::TripleInit5;
    TriIn[3]=&DolImage::TripleInit6;
    TriIn[4]=&DolImage::TripleInit7;
    TriIn[5]=&DolImage::TripleInit8;
    TriIn[6]=&DolImage::TripleInit9;
    TriIn[7]=&DolImage::TripleInit10;
    TriIn[8]=&DolImage::TripleInit11;
    TriIn[9]=&DolImage::TripleInit12;
    TriIn[10]=&DolImage::TripleInit13;
    TriIn[11]=&DolImage::TripleInit14;
    SignCodes=new uint8_t[Width3];
    DeltaCodes=new uint8_t[Width3];
    DecompressHeader();
    Decompress(Width-1);
#ifdef DEBG
    if(Mode==line) {
        uint32_t size=r_buf_index;
        if(r_pos == 8)
            size--;
        std::cout<<std::to_string(h)<<'\t';
        if(size!=RBufferSize) {
            std::cout<<std::to_string(RBufferSize)<<' '<<std::to_string(size)<<'\n';
            exit(8);
        }
    }
#endif
    Buffer0=WBuffer;
    DecodeLine0();
    OFile->write((char*)Buffer0,Width3+pad);
    bool orderIsReverse=false;
    Buffer=WBuffer+Width3;
    for(int h=1;h<Height;h++) {
        if(Mode==line) {
            delete[] RBuffer;
            ClearTrees();
            IFile->read((char*)&RBufferSize, 2);
            RBuffer=new uint8_t[RBufferSize];
            IFile->read((char*)RBuffer, RBufferSize);
            DecompressHeader();
        }
        Decompress(Width);
#ifdef DEBG
        if(Mode==line) {
            uint32_t size=r_buf_index;
            if(r_pos == 8)
                size--;
            std::cout<<std::to_string(h)<<'\t';
            if(size!=RBufferSize) {
                std::cout<<std::to_string(RBufferSize)<<' '<<std::to_string(size)<<'\n';
                exit(8);
            }
        }
#endif
        DecodeLine();
        OFile->write((char*)Buffer,Width3+pad);
        orderIsReverse=!orderIsReverse;
        if(orderIsReverse) {
            Buffer0=WBuffer+Width3;
            Buffer=WBuffer;
        }
        else {
            Buffer0=WBuffer;
            Buffer=WBuffer+Width3;
        }
    }
#ifdef DEBG
    if(Mode==area) {
        uint32_t size=r_buf_index;
        if(r_pos == 8)
            size--;
        std::cout<<std::to_string(h)<<'\t';
        if(size!=RBufferSize) {
            std::cout<<std::to_string(RBufferSize)<<' '<<std::to_string(size)<<'\n';
            exit(8);
        }
    }
#endif
    delete[] DeltaCodes;
    delete[] SignCodes;
    delete[] RBuffer;
    delete[] WBuffer;
    ClearTrees();
}

/*
 void DolImage::Delta(uint8_t code1, uint8_t code2, uint8_t& delta, uint8_t& sign)
 {
 if(code1==code2)
 {
 delta=0;
 sign=2;
 }
 else
 {
 uint8_t delta1,delta2;
 if(code1>code2)
 {
 delta1=code1-code2;
 delta=ClrUsed-delta1;
 }
 else
 {
 delta=code2-code1;
 delta1=ClrUsed-delta;
 }
 //  if(delta1>delta)
 if(delta1>=delta)
 {
 sign=1;
 }
 else
 {
 delta=delta1;
 sign=0;
 }
 }
 }
 */
/*
 void DolImage::Delta(uint8_t code1, uint8_t code2, uint8_t* delta, uint8_t* sign, long& delta_size)
 {
 if(code1==code2)
 {
 *sign=2;
 }
 else
 {
 if(code1>code2)
 {
 *delta++=code1-code2;
 *sign=0;
 }
 else
 {
 *delta++=code2-code1;
 *sign=1;
 }
 //        delta++;
 delta_size++;
 }
 }
 */
/*
void DolImage::Delta(uint8_t code1, uint8_t code2, uint8_t* sign)
{
    if(code1==code2)
    {
        *sign=2;
#ifdef STAT
        S2++;
#endif
    }
    else
    {
        if(code1>code2)
        {
            DeltaCodes[DeltaOffset++]=code1-code2;
            *sign=0;
#ifdef STAT
            S0++;
#endif
        }
        else
        {
            DeltaCodes[DeltaOffset++]=code2-code1;
            *sign=1;
#ifdef STAT
            S1++;
#endif
        }
    }
}
*/
void DolImage::Delta(uint8_t code1, uint8_t code2, uint8_t* sign)
{
    if(code1==code2)
    {
        *sign=2;
#ifdef STAT
        S2++;
#endif
    }
    else
    {
        uint8_t delta,delta1;
        if(code1>code2)
        {
            delta1=code1-code2;
            delta=ClrUsed-delta1;
        }
        else
        {
            delta=code2-code1;
            delta1=ClrUsed-delta;
        }
        //  if(delta1>delta)
        if(delta1>=delta)
        {
            *sign=1;
#ifdef STAT
            S1++;
#endif
        }
        else
        {
            delta=delta1;
            *sign=0;
#ifdef STAT
            S0++;
#endif
        }
        DeltaCodes[DeltaOffset++]=delta;
    }
}

uint8_t DolImage::Mod(uint8_t delta)
{
    if(delta<ClrUsed)
        return delta;
    else if(delta>128)
        return delta+ClrUsed;
    else
        return delta-ClrUsed;
}

