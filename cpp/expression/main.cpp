/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file main.cpp
 * @author work(com@baidu.com)
 * @date 2013/03/22 21:28:24
 * @brief 
 *  
 **/

#include "ScriptEngine.h"

int64_t assign_callback(const char *name, void *data, int *mark) {
    if (strcmp(name, "product") == 0) {
        return 2;
    }
    else if (strcmp(name, "topic") == 0) {
        return 2;
    }
    else if (strcmp(name, "a") == 0) {
        return 'x';
    }
    else if (strcmp(name, "b") == 0) {
        return 3;
    }

    else {
//        *mark = 1;
        return 0;
    }
}


int main(int argc,char** argv)
{
    ScriptEngine engine;
    const char* exp = "(a==4)&&(b/0==0)";
    const char *exp2 = "product==2 && (topic==2)";
//    const char *exp = "(a==3 , || b!=3) && (c*d-e ==2 || f+g*h==6) && (i/j==k*l)";

    if(argc >=2 )
    {
        exp = argv[1];
    }

    printf("analyse exp :%s \n",exp);

    int ret= 0;
    ret = engine.validate_expression(exp);
    printf("validate ret=%d", ret);

    ret = 0;
    int result = engine.analyse(assign_callback, NULL, &ret);

    printf("result=%d,ret=%d\n", result, ret);


    return 0;
}



















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
