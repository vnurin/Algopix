#include "DolType.hpp"
//**SORT_TABLE
bool operator<(const SORT_TABLE &s1,const SORT_TABLE &s2)
{
 return s1.code<s2.code;
}
//**SORT_HUFFMAN
bool operator<(const SORT_HUFFMAN &s1,const SORT_HUFFMAN &s2)
{
 return s1.count<s2.count;
}
//**SORT_COLORS
bool operator<(const SORT_COLORS &s1,const SORT_COLORS &s2)
{
 return s1.code<s2.code;
}
//**SUBCOLORS
bool operator<(const SUBCOLORS& rgb1,const SUBCOLORS& rgb2)
{
 if(rgb1.r<rgb2.r)
  return true;
 if(rgb1.r==rgb2.r)
 {
  if(rgb1.g<rgb2.g)
   return true;
  if(rgb1.g==rgb2.g && rgb1.b<rgb2.b)
   return true;
 }
 return false;
}
//**Node
Node::Node(bool side,unsigned int gen)
{
 Leaf=false;
 Side=side;
 Gen=gen;
 N0=N1=NULL;
}
//**
void Node::Divide(Node** node0,Node** node1)
{
 N0=new Node(false,Gen+1);
 N1=new Node(true,Gen+1);
 *node0=N0;
 *node1=N1;
}
//**Cube
Cube::Cube()
{
 Parent=0;
 Number=0;
}
//**Cube3
Cube3::Cube3(int bit_depth) : Cube()
{
 Left.r=Left.g=Left.b=0;
 if(bit_depth==8)
 {
  Generation=24;
  Right.r=Right.g=Right.b=255;
 }
 else if(bit_depth==5)
 {
  Generation=15;
  Right.r=Right.g=Right.b=31;
 }
 BitDepth=bit_depth;
}
//**
Cube3::Cube3(Cube3* cube,bool r_half,bool g_half,bool b_half,unsigned char n)
{
 if(r_half)//right
 {
  Left.r=(cube->Left.r+cube->Right.r+1)/2;
  Right.r=cube->Right.r;
 }
 else//left
 {
  Left.r=cube->Left.r;
  Right.r=(cube->Left.r+cube->Right.r-1)/2;
 }
 if(g_half)//right
 {
  Left.g=(cube->Left.g+cube->Right.g+1)/2;
  Right.g=cube->Right.g;
 }
 else//left
 {
  Left.g=cube->Left.g;
  Right.g=(cube->Left.g+cube->Right.g-1)/2;
 }
 if(b_half)//right
 {
  Left.b=(cube->Left.b+cube->Right.b+1)/2;
  Right.b=cube->Right.b;
 }
 else//left
 {
  Left.b=cube->Left.b;
  Right.b=(cube->Left.b+cube->Right.b-1)/2;
 }
 Generation=cube->Generation-3;
 Parent=cube->Parent+(cube->Number<<Generation);
 Number=n;
 BitDepth=cube->BitDepth;
}
//**
void Cube3::Divide(Cube** childs)
{
 childs[0]=new Cube3(this,false,false,false,0);
 childs[1]=new Cube3(this,false,false,true,1);
 childs[2]=new Cube3(this,false,true,false,2);
 childs[3]=new Cube3(this,false,true,true,3);
 childs[4]=new Cube3(this,true,false,false,4);
 childs[5]=new Cube3(this,true,false,true,5);
 childs[6]=new Cube3(this,true,true,false,6);
 childs[7]=new Cube3(this,true,true,true,7);
 Cube3* childs3[8];
 for(int i=0;i<8;i++)
  childs3[i]=(Cube3*)childs[i];
 SUBCOLORS rgb=childs3[0]->Right;
 unsigned char r_middle=rgb.r;
 unsigned char g_middle=rgb.g;
 unsigned char b_middle=rgb.b;
 int n;
 for(unsigned int i=0;i<QColors;i++)
 {
  n=0;
  rgb=Colors[i].rgb;
  if(rgb.r>r_middle)
   n+=4;
  if(rgb.g>g_middle)
   n+=2;
  if(rgb.b>b_middle)
   n++;
  childs3[n]->Colors.push_back(Colors[i]);
 }
 for(n=0;n<8;n++)
  childs3[n]->QColors=childs3[n]->Colors.size();
}
//**
int Cube3::Distance(Cube* cube)
{
 Cube3* cube3=(Cube3*)cube;
 char r_middle1=(Left.r+Right.r+1)/2;
 char g_middle1=(Left.g+Right.g+1)/2;
 char b_middle1=(Left.b+Right.b+1)/2;
 char r_middle2=(cube3->Left.r+cube3->Right.r+1)/2;
 char g_middle2=(cube3->Left.g+cube3->Right.g+1)/2;
 char b_middle2=(cube3->Left.b+cube3->Right.b+1)/2;
 int r_delta=r_middle2-r_middle1;
 int g_delta=g_middle2-g_middle1;
 int b_delta=b_middle2-b_middle1;
 return r_delta*r_delta+g_delta*g_delta+b_delta*b_delta;
}
//**
void Cube3::AddColors(Cube* cube)
{
 Cube3* cube3=(Cube3*)cube;
 unsigned int q_colors=cube3->QColors;
 for(unsigned int j=0;j<q_colors;j++)
  Colors.push_back(cube3->Colors[j]);
 QColors+=q_colors;
}
//**
void Cube3::BecomeParent()
{
 Number=(Parent>>Generation)&7;
 Generation+=3;
 Parent=Parent&(0x7FFFFFFF<<Generation);
}
//**
unsigned int Cube3::AverageColor()
{
 double r_sum=0.0;
 double g_sum=0.0;
 double b_sum=0.0;
 double count_sum=0.0;
 double d_count;
 double average;
 COLOR color;
 SUBCOLORS a_color;
 for(unsigned int i=0;i<QColors;i++)
 {
  color=Colors[i];
  d_count=color.count;
  r_sum+=color.rgb.r*d_count;
  g_sum+=color.rgb.g*d_count;
  b_sum+=color.rgb.b*d_count;
  count_sum+=d_count;
 }
 average=r_sum/count_sum;
 a_color.r=average;
 if(average-(double)a_color.r>=0.5)
  a_color.r++;
 average=g_sum/count_sum;
 a_color.g=average;
 if(average-(double)a_color.g>=0.5)
  a_color.g++;
 average=b_sum/count_sum;
 a_color.b=average;
 if(average-(double)a_color.b>=0.5)
  a_color.b++;
 if(BitDepth==8)
  return (a_color.b<<16)+(a_color.g<<8)+a_color.r;
 else if(BitDepth==5)
  return (a_color.r<<10)+(a_color.g<<5)+a_color.b;
 return 0;
}
//**Cube1
Cube1::Cube1() : Cube()
{
 Left=0;
 Right=255;
 Generation=8;
}
//**
Cube1::Cube1(Cube1* cube,bool half)
{
 if(half)
 {
  Left=(cube->Left+cube->Right+1)/2;
  Right=cube->Right;
  Number=1;
 }
 else
 {
  Left=cube->Left;
  Right=(cube->Left+cube->Right-1)/2;
  Number=0;
 }
 Generation=cube->Generation-1;
 Parent=cube->Parent+(cube->Number<<Generation);
}
//**
void Cube1::Divide(Cube** childs)
{
 childs[0]=new Cube1(this,false);
 childs[1]=new Cube1(this,true);
 Cube1* childs1[2];
 childs1[0]=(Cube1*)childs[0];
 childs1[1]=(Cube1*)childs[1];
 unsigned char middle=childs1[0]->Right;
 for(unsigned int i=0;i<QColors;i++)
 {
  if(Colors[i].color>middle)
   childs1[1]->Colors.push_back(Colors[i]);
  else
   childs1[0]->Colors.push_back(Colors[i]);
 }
 childs1[0]->QColors=childs1[0]->Colors.size();
 childs1[1]->QColors=childs1[1]->Colors.size();
}
//**
int Cube1::Distance(Cube* cube)
{
 Cube1* cube1=(Cube1*)cube;
 char middle1=(Left+Right+1)/2;
 char middle2=(cube1->Left+cube1->Right+1)/2;
 int delta=middle2-middle1;
 if(delta<0)
  return -delta;
 else
  return delta;
}
//**
void Cube1::AddColors(Cube* cube)
{
 Cube1* cube1=(Cube1*)cube;
 unsigned int q_colors=cube1->QColors;
 for(unsigned int j=0;j<q_colors;j++)
  Colors.push_back(cube1->Colors[j]);
 QColors+=q_colors;
}
//**
void Cube1::BecomeParent()
{
 Number=(Parent>>Generation)&1;
 Generation++;
 Parent=Parent&(0x7FFFFFFF<<Generation);
}
//**
unsigned int Cube1::AverageColor()
{
 double sum=0.0;
 double count_sum=0.0;
 double d_count;
 double average;
 unsigned char a_color;
 GRAY_COLOR color;
 for(unsigned int i=0;i<QColors;i++)
 {
  color=Colors[i];
  d_count=color.count;
  sum+=color.color*d_count;
  count_sum+=d_count;
 }
 average=sum/count_sum;
 a_color=average;
 if(average-(double)a_color>=0.5)
  a_color++;
 return a_color;
}
//**

