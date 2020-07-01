//
//  HufImage.hpp
//  algopix
//
//  Created by Vahagn Nurijanyan on 2018-09-14.
//  Copyright © 2018 BABELONi INC. All rights reserved.
//

#ifndef HufImage_hpp
#define HufImage_hpp

#include <iostream>
#include "DolType.hpp"

class HufImage {
    const int EB=6;
    const uint8_t ExtraBits[15]={0,0,0,0,0,0,1,1,2,2,3,3,4,4,6};
    const uint8_t Group[15]={1,1,1,1,1,1,2,2,4,4,8,8,16,16,62};
    const uint8_t Group1[15]={1,1,1,1,1,1,2,2,4,4,8,8,16,16,61};//COMC.
    const int Group2[15]={1,1,1,1,2,4,8,16,32,64,128,256,512,1024,2048};
    const uint8_t Borders[15]={2,3,4,5,6,7,9,11,15,19,27,35,51,67,129};
//COMC.    const uint8_t Borders1[15]={2,3,4,5,6,7,9,11,15,19,27,35,51,67,128};
    //    const uint8_t Borders[15]={1,2,3,4,5,6,8,10,14,18,26,34,50,66,129};
    uint8_t HUF[16][15]={
        {0},
        {0},
        {1,1},
        {2,1,2},
        {2,2,2,2},
        {3,2,2,2,3},
        {3,3,2,2,3,3},
        {3,3,3,2,3,3,3},
        {4,3,3,2,3,3,3,4},
        {5,4,3,2,2,3,3,6,6},
        {6,4,3,2,2,3,3,5,7,7},
        {7,5,3,2,2,3,3,4,6,8,8},
        {7,5,3,2,2,3,3,4,6,8,9,9},
        {7,6,3,2,2,3,3,4,5,8,9,10,10},
        {7,6,3,2,2,3,3,4,5,8,9,10,11,11},
        {8,7,4,3,2,2,3,4,4,5,6,9,10,11,11}};
    const uint16_t HUF_TABLE[16][15]={
        {0},
        {0},
        {0,1},//{0,1}
        {2,0,3},//{10,0,11}
        {0,1,2,3},//{00,01,10,11}
        {6,0,1,2,7},//{110,00,01,10,111}
        {4,5,0,1,6,7},//{100,101,00,01,110,111}
        {2,3,4,0,5,6,7},//{010,011,100,00,101,110,111}
        {14,2,3,0,4,5,6,15},//{1110,010,011,00,100,101,110,1111}
        {30,14,4,0,1,5,6,62,63},//{11110,1110,100,00,01,101,110,111110,111111}
        {62,14,4,0,1,5,6,30,126,127},//{111110,1110,100,00,01,101,110,11110,1111110,1111111}
        {126,30,4,0,1,5,6,14,62,254,255},//{1111110,11110,100,00,01,101,110,1110,111110,11111110,11111111}
        {126,30,4,0,1,5,6,14,62,254,510,511},//{1111110,11110,100,00,01,101,110,1110,111110,11111110,111111110,111111111}
        {126,62,4,0,1,5,6,14,30,254,510,1022,1023},//{1111110,111110,100,00,01,101,110,1110,11110,11111110,111111110,1111111110,1111111111}
        {126,62,4,0,1,5,6,14,30,254,510,1022,2046,2047},//{1111110,111110,100,00,01,101,110,1110,11110,11111110,111111110,1111111110,11111111110,11111111111}
        {254,126,12,4,0,1,5,13,14,30,62,510,1022,2046,2047}};//{11111110,11111110,1100,100,00,01,101,1101,1110,11110,111110,111111110,1111111110,11111111110,11111111111}
    void Huffman(uint32_t A[],int n);
    uint32_t HuffmanTableSize(uint32_t* length,int q_h);
    void GroupNumber(uint8_t code,uint8_t& num,uint8_t& rem);
    void BitLength(uint8_t* length,uint8_t q_huf);
    void ClearTree(std::vector<Node*>& tree);

protected:
    int Presented;
//Presented==0 is for color photo & graphical, Presented==1 is for grayscale nearly for sure [or graphical], Presented==2 is for graphical nearly for sure [or color photo] compressible unit
//We don't save this in compressed unit because decompression algorithm doesn't need it: decompression algorithm is completely common for all 3 (color photo, grayscale & graphical) types of compressed units
    uint8_t *Signs;
    uint8_t *Deltas;
    uint8_t *Ones;
    uint8_t *DeltaCodes;
    long DeltaOffset;
    const int LHQ=15;
    const int SHQ=12;
    const int HQ=15;
    const int HQ3=7;
    uint8_t QLHL[3];
    uint8_t QSH;
    uint8_t QH;
    uint8_t QH1;//without extra bits QH1 < QH at least by 1...
    uint8_t QH3;
    const uint8_t ExtraBits2[15]={0,0,0,0,1,2,3,4,5,6,7,8,9,10,11};
    const int  Borders1[15]={1,2,3,4,6,10,18,34,66,130,258,514,1026,2050,4098};
    uint32_t LHufLCodeLength[3][15];
    uint32_t SHufCodeLength[12];
    uint32_t HufCodeLength[15];
    uint32_t HufCodeLength1[15];
    uint32_t HufCodeLength3[7];
    uint32_t HufLTree[3][15];
    uint32_t HufSTree[12];
    uint32_t HufTree[15];
    uint32_t HufTree1[15];
    uint32_t HufTree3[7];
    uint8_t LLength[3][15];
    uint8_t SLength[12];
    uint8_t Length[15];
    uint8_t Length1[15];
    uint8_t Length3[7];
    std::vector<Node*> TableTree;
    std::vector<Node*> LTree[3];
    std::vector<Node*> STree;
    std::vector<Node*> Tree;
    std::vector<Node*> Tree1;
    std::vector<Node*> Tree3;
    typedef void (HufImage::*TripleInit)(bool extra_bit);
    TripleInit TriIn[12];
    typedef void (HufImage::*RCom)(uint8_t triple);
    RCom ReaCo;
    typedef uint8_t (HufImage::*RDel)(std::vector<Node*> tree);
    RDel ReaDe;
    RDel ReaDe1;
   uint32_t TripleSize(uint32_t* count,std::vector<uint32_t> lengths[]);
    uint32_t DeltaSize(uint32_t* count,uint32_t* count1,uint32_t* count3);
    void WriteHuffmanTable(uint32_t* length,uint8_t q_h,uint32_t* huf_tree);
    void ReadHuffmanTable(uint8_t* length,uint8_t q_h);
    void HuffmanTree(uint8_t* length,uint8_t q_h,std::vector<Node*>& tree);
    void ReadCode(uint8_t& value,std::vector<Node*>& tree);
    void ReadCode(uint8_t q_huf,uint8_t& value,std::vector<Node*>& tree);
    void ReadCompPixel2(uint8_t triple);
    void WriteCompSign(uint8_t triple,uint8_t s_gn,uint32_t *s_code_length);
    void WriteCompDelta1(uint8_t min,uint32_t *code_length,uint32_t *code_length1,uint32_t *code3_length,uint8_t one);
    void WriteCompDelta2(uint8_t triple,uint32_t *code_length);
    void WriteCompDelta1(uint8_t min,uint32_t *code3_length,uint8_t one);
    void WriteCompDelta2(uint8_t triple);
    void GroupNumber1(uint32_t len,uint8_t& num,uint32_t& rem);
    void ClearTrees();
public:
    uint8_t ReadDelta(std::vector<Node*> tree);
    uint8_t ReadDelta_2(std::vector<Node*> tree);
    uint8_t ReadDelta_1(std::vector<Node*> tree);
/*    uint8_t ReadDelta1();
    uint8_t ReadDelta1_2();
    uint8_t ReadDelta1_1();*/
    void TripleInit3(bool extra_bit);
    void TripleInit4(bool extra_bit);
    void TripleInit5(bool extra_bit);
    void TripleInit6(bool extra_bit);
    void TripleInit7(bool extra_bit);
    void TripleInit8(bool extra_bit);
    void TripleInit9(bool extra_bit);
    void TripleInit10(bool extra_bit);
    void TripleInit11(bool extra_bit);
    void TripleInit12(bool extra_bit);
    void TripleInit13(bool extra_bit);
    void TripleInit14(bool extra_bit);
    void ReadCompPixel1(uint8_t triple);
    void ReadCompPixel1_2(uint8_t triple);
    void ReadCompPixel1_1(uint8_t triple);
};
#endif /* HufImage_hpp */
