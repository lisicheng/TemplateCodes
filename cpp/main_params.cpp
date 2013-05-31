/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file main_params.cpp
 * @author lisicheng(com@baidu.com)
 * @date 2013/04/08 13:50:47
 * @brief 
 *  
 **/

#include<iostream>

int fun(void *val) {
    long m = -9;
    long *lv = (long*)val;
    *lv = m;
    return 0;
}


int main(int argc, char **argv) {
    std::cout<<"argc="<<argc<<std::endl;
    for (int i = 0; i < argc; i++) {
        std::cout<<"argv["<<i<<"]="<<argv[i]<<std::endl;
    }

    int val = 0;
    fun((void*)&val);
    std::cout<<"val="<<val<<std::endl;

    return 0;
}





















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
