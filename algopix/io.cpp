//
//  io.cpp
//  algopix
//
//  Created by Vahagn Nurijanyan on 2018-08-30.
//  Copyright © 2018 BABELONi INC. All rights reserved.
//

#include <iostream>
#include "io.hpp"
char* w_buffer;
int w_buf_index;
uint8_t w_pos=8;
//uint8_t w_byte=0;
uint8_t w_byte;
uint8_t w_base;
char* r_buffer;
int r_buf_index;
uint8_t r_pos;
uint8_t r_base;
uint8_t r_byte;

#ifdef DEBG
int h;
#endif
void write(bool bl)
{
    w_pos--;
    if(bl)
        w_byte+=1<<w_pos;
    if(!w_pos)
    {
        w_buffer[w_buf_index++]=w_byte;
        w_byte=0;
        w_pos=8;
    }
/*
#ifdef DEBG
    std::cout<<std::to_string(bl)<<"/B ";
#endif
*/
}
void write(uint8_t byte)
{
    if(w_pos>w_base)
    {
        w_pos-=w_base;
        w_byte+=byte<<w_pos;
    }
    else if(w_pos<w_base)
    {
        w_byte+=byte>>(w_base-w_pos);
        w_buffer[w_buf_index++]=w_byte;
        w_pos+=8-w_base;
        w_byte=byte<<w_pos;
    }
    else
    {
        w_byte+=byte;
        w_buffer[w_buf_index++]=w_byte;
        w_byte=0;
        w_pos=8;
    }
/*
#ifdef DEBG
    std::cout<<std::to_string(byte)<<'/'<<std::to_string(w_base)<<" ";
#endif
*/
}
void write(uint16_t word)
{
    uint8_t base=w_base;
    if(base>=w_pos && w_pos!=8)
    {
        base-=w_pos;
        w_byte+=word>>base;
        w_buffer[w_buf_index++]=w_byte;
        w_byte=0;
        w_pos=8;
    }
    if(base>=w_pos)
    {
        base-=w_pos;
        w_buffer[w_buf_index++]=word>>base;
    }
    if(base)
    {
        w_pos-=base;
        w_byte+=word<<w_pos;
    }
/*
#ifdef DEBG
    std::cout<<std::to_string(word)<<'/'<<std::to_string(w_base)<<" ";
#endif
*/
}
void write(uint32_t dword)
{
    uint8_t base=w_base;
    if(base>=w_pos && w_pos!=8)
    {
        base-=w_pos;
        w_byte+=dword>>base;
        w_buffer[w_buf_index++]=w_byte;
        w_byte=0;
        w_pos=8;
    }
    while(base>=w_pos)
    {
        base-=w_pos;
        w_buffer[w_buf_index++]=dword>>base;
    }
    if(base)
    {
        w_pos-=base;
        w_byte+=dword<<w_pos;
    }
/*
#ifdef DEBG
    std::cout<<std::to_string(dword)<<'/'<<std::to_string(w_base)<<" ";
#endif
*/
}
void read(bool& bl)
{
    if(r_pos)
    {
        r_pos--;
        bl=(r_byte>>r_pos)&1;
    }
    else
    {
        r_byte=r_buffer[r_buf_index++];
        r_pos=7;
        bl=r_byte>>7!=0;
    }
/*
#ifdef DEBG
    std::cout<<std::to_string(bl)<<"/B ";
#endif
*/
}
void read(uint8_t& byte)
{
    if(r_pos>=r_base)
    {
        r_pos-=r_base;
        byte=(r_byte>>r_pos)&(0xFF>>(8-r_base));
    }
    else if(!r_pos)
    {
        r_byte=r_buffer[r_buf_index++];
        r_pos=8-r_base;
        byte=r_byte>>r_pos;
    }
    else
    {
        byte=(r_byte<<(r_base-r_pos))&(0xFF>>(8-r_base));
        r_byte=r_buffer[r_buf_index++];
        r_pos+=8-r_base;
        byte+=r_byte>>r_pos;
    }
/*
#ifdef DEBG
    std::cout<<std::to_string(byte)<<'/'<<std::to_string(r_base)<<" ";
#endif
*/
}
void read(uint16_t& word)
{
    if(!r_pos)
    {
        r_byte=r_buffer[r_buf_index++];
        r_pos=8;
    }
    uint8_t base=r_base;
    word=0;
    if(base>=r_pos && r_pos!=8)
    {
        base-=r_pos;
        word+=(r_byte&(~(0xFFFF<<r_pos)))<<base;
        r_byte=r_buffer[r_buf_index++];
        r_pos=8;
    }
    if(base>=r_pos)
    {
        base-=r_pos;
        word+=r_byte<<base;
        r_byte=r_buffer[r_buf_index++];
    }
    if(base)
    {
        r_pos-=base;
        word+=(r_byte>>r_pos)&(~(0xFFFF<<base));
    }
/*
#ifdef DEBG
    std::cout<<std::to_string(word)<<'/'<<std::to_string(r_base)<<" ";
#endif
*/
}

void read(uint32_t& dword)
{
    if(!r_pos)
    {
        r_byte=r_buffer[r_buf_index++];
        r_pos=8;
    }
    uint8_t base=r_base;
    dword=0;
    if(base>=r_pos && r_pos!=8)
    {
        base-=r_pos;
        dword+=(r_byte&(~(0xFFFFFFFF<<r_pos)))<<base;
        r_byte=r_buffer[r_buf_index++];
        r_pos=8;
    }
    while(base>=r_pos)
    {
        base-=r_pos;
        dword+=r_byte<<base;
        r_byte=r_buffer[r_buf_index++];
    }
    if(base)
    {
        r_pos-=base;
        dword+=(r_byte>>r_pos)&(~(0xFFFFFFFF<<base));
    }
/*
#ifdef DEBG
    std::cout<<std::to_string(dword)<<'/'<<std::to_string(r_base)<<" ";
#endif
*/
}

