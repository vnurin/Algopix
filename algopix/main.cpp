//
//  main.cpp
//  algopix
//
//  Created by Vahagn Nurijanyan on 2018-08-29.
//  Copyright © 2018 BABELONi INC. All rights reserved.
//

#include <iostream>
#include "DolImage.hpp"

int main(int argc, const char * argv[]) {
    if(argc == 3) {
        if(strcmp(argv[1], "-1") != 0 && strcmp(argv[1], "-2") != 0) {
            std::cout << "Wrong parameters!\n";
            return 1;
        }
        int8_t dimension = strcmp(argv[1], "-1") == 0 ? 0 : 1;
        DolImage image = DolImage(argv[2], dimension);
        if(image.FileType == T_BMP && image.openDestination()) {
            image.StoreAPX();
        }
    }
    else if(argc == 2) {
        DolImage image = DolImage(argv[1], -1);
        if(image.FileType == T_APX && image.openDestination()) {
            image.StoreBMP();
        }
    }
    else {
        std::cout << "Wrong number of parameters!\n";
        return 2;
    }
    return 0;
}
