//
//  io.hpp
//  algopix
//
//  Created by Vahagn Nurijanyan on 2018-08-30.
//  Copyright © 2018 BABELONi INC. All rights reserved.
//

#ifndef IO_H
#define IO_H
#define DEBG
#ifdef DEBG
    #define STAT
extern int h;
#endif
#include <iostream>
extern char* w_buffer;
extern int w_buf_index;
extern uint8_t w_pos;
//uint8_t w_byte=0;
extern uint8_t w_byte;
extern uint8_t w_base;
extern char* r_buffer;
extern int r_buf_index;
extern uint8_t r_pos;
extern uint8_t r_base;
extern uint8_t r_byte;

void write(bool bl);
void write(uint8_t byte);
void write(uint16_t word);
void write(uint32_t dword);
void read(bool& bl);
void read(uint8_t& byte);
void read(uint16_t& word);
void read(uint32_t& dword);

#endif /* io_hpp */
