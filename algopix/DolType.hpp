#ifndef DOL_TYPE_H
#define DOL_TYPE_H
//**
#include <vector>
//**
enum IMAGE_TYPE {T_UNKNOWN,T_BMP,T_APX};
//**
typedef struct tagSORT_TABLE
{
 unsigned int code;
 unsigned int index;
} SORT_TABLE;
bool operator<(const SORT_TABLE &s1,const SORT_TABLE &s2);
//**
typedef struct tagSORT_HUFFMAN
{
 unsigned int count;
 unsigned int index;
} SORT_HUFFMAN;
bool operator<(const SORT_HUFFMAN &s1,const SORT_HUFFMAN &s2);
//**
typedef struct tagSORT_COLORS
{
 unsigned int code;
 unsigned int width;
//V22HARD. unsigned int height;
 unsigned int height;
} SORT_COLORS;
bool operator<(const SORT_COLORS &s1,const SORT_COLORS &s2);
//**
typedef struct tagSUBCOLORS
{
 unsigned char r;
 unsigned char g;
 unsigned char b;
} SUBCOLORS;
bool operator<(const SUBCOLORS& rgb1,const SUBCOLORS& rgb2);
//**
class Node
{
public:
 bool Side;
/*
 false - left
 true - right
*/
 bool Leaf;
/*
 false - node
 true - leaf
*/
 unsigned int Gen;
 Node* N0;
 Node* N1;
 unsigned int Value;
//
 Node(bool side,unsigned int gen=1);
 void Divide(Node** node0,Node** node1);
};
//**
typedef struct tagCOLOR
{
 SUBCOLORS rgb;
 unsigned int count;
 unsigned int index;
} COLOR;
//**
typedef struct tagGRAY_COLOR
{
 unsigned char color;
 unsigned int count;
 unsigned int index;
} GRAY_COLOR;
//**
class Cube
{
public:
 unsigned int QColors;
 char Generation;
 int Parent;
 char Number;
 Cube();
 virtual void Divide(Cube** childs)=0;
 virtual int Distance(Cube* cube)=0;
 virtual void AddColors(Cube* cube)=0;
 virtual void BecomeParent()=0;
 virtual unsigned int AverageColor()=0;
};
//**
class Cube3 : public Cube
{
 SUBCOLORS Left;
 SUBCOLORS Right;
 int BitDepth;
public:
    std::vector<COLOR> Colors;
 Cube3(int bit_depth);
 Cube3(Cube3* cube,bool r_half,bool g_half,bool b_half,unsigned char n);
 virtual void Divide(Cube** childs);
 virtual int Distance(Cube* cube);
 virtual void AddColors(Cube* cube);
 virtual void BecomeParent();
 virtual unsigned int AverageColor();
};
//**
class Cube1 : public Cube
{
 unsigned char Left;
 unsigned char Right;
public:
    std::vector<GRAY_COLOR> Colors;
 Cube1();
 Cube1(Cube1* cube,bool half);
 virtual void Divide(Cube** childs);
 virtual int Distance(Cube* cube);
 virtual void AddColors(Cube* cube);
 virtual void BecomeParent();
 virtual unsigned int AverageColor();
};
//**
#endif
