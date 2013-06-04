/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file ScriptEngine.h
 * @author chenjiansen(com@baidu.com)
 * @author suyong(com@baidu.com)
 * @date 2013/03/22 13:11:23
 * @brief 
 *  
 **/




#ifndef  __SCRIPTENGINE_H_
#define  __SCRIPTENGINE_H_


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


#define MAX_NAME_SIZE 64
#define MAX_STACK_SIZE 128


enum 
{
    ITEM_TYPE_SYMBOL = 0x01,
    ITEM_TYPE_VALUE = 0x02,
    ITEM_TYPE_VARIABLE = 0x04,

    ITEM_TYPE_FINISH = 0x10,

};

typedef int64_t (*PFUN_VARIABLE_CALLBACK)(const char* name, void* data, int *mark);
typedef int64_t (*PFUN_OPEARTOR)(int64_t,int64_t);

typedef struct _symbol_item_t
{
    int value;
    const char* symbol;
    int priority;
    PFUN_OPEARTOR pfun;
}symbol_item_t;

typedef struct _item_value_t
{
    int type;
    char name[MAX_NAME_SIZE];
    int64_t value;
}item_value_t;



class ScriptEngine
{
    enum
    {
        EXPRESSION_TRUE = 0,
        EXPRESSION_FALSE ,
        EXPRESSION_ILLEGAL,
    };

    public:
        ScriptEngine();
        ~ScriptEngine();

    public:

        /**
         * validate expression, whether the expression given is leggal or illegal.
         * ret : 0 leggal
         * ret : -1 illegal length, length should be < 1024
         * ret : -2 illegal, unsupported character such as {, [, #, ...
         * ret : -3 illegal type
         * ret : -4 illegal, unmatched brackets, ((a==2)
         */
        int validate_expression(const char *expression);

        /**
         * analyse, call validate_expression first.
         * ret : the result
         * mark : 0 normal
         * mark : 1 some variable value not found in data.
         */
        int analyse(PFUN_VARIABLE_CALLBACK callback, void *data, int *mark);

        int analyse(const char *expression, PFUN_VARIABLE_CALLBACK callback);
        int analyse(const char *expression, PFUN_VARIABLE_CALLBACK callback, void *data);
        int analyse(const char *expression, PFUN_VARIABLE_CALLBACK callback, void *data, int *mark);

    protected:

        /**
         * ret : 0 normal
         * ret : -1 length > 1024
         * ret : -2 illegal character
         */
        int _format_expression(const char* exp);

        /**
         * ret : 0 normal
         * ret : -3 illegal type
         * ret : -4 unmatch
         */
        int _change_to_post();

        /**
         * result : the result value;
         * ret : 0 correct, the result value is valid.
         * ret : <0 error, result is invalid.
         */
        int _calculate(int *result);

        void _add_item(const char* begin,const char* end);

        int _str_to_item(const char* str,item_value_t& item);

    private:
        item_value_t* _exp_stack;
        item_value_t* _post_exp_stack;
        item_value_t* _post_exp_stack_compute;
        item_value_t* _symbol_stack;


        int _exp_size;

        int _post_exp_size;

        int _symbol_size;

        int _post_exp_begin;
        int _post_exp_end;

        int _symbol_beign;
        int _symbol_end;
};

#endif  //__SCRIPTENGINE_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
