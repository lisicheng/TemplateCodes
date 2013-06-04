/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file ScriptEngine.cpp
 * @author chenjiansen(com@baidu.com)
 * @author suyong(com@baidu.com)
 * @date 2013/03/22 13:39:32
 * @brief 
 *  
 **/


#include "ScriptEngine.h"
//#include "ub.h"

static int64_t equal_func(int64_t left,int64_t right)    { return (int64_t)(left == right); }
static int64_t unequal_func(int64_t left,int64_t right)  { return (int64_t)(left != right); }
static int64_t or_func(int64_t left,int64_t right)       { return (int64_t)(left || right); }
static int64_t and_func(int64_t left,int64_t right)      { return (int64_t)(left && right); }
static int64_t add_func(int64_t left,int64_t right)      { return (int64_t)(left + right); }
static int64_t minus_func(int64_t left,int64_t right)    { return (int64_t)(left - right); }
static int64_t multiple_func(int64_t left,int64_t right) { return (int64_t)(left * right); }
static int64_t division_func(int64_t left,int64_t right) {
    if (0 == right) {
//        UB_LOG_FATAL("ScriptEngine: division_func with left={%d} right==0", left);
        return 0;
    }
    return (int64_t)(left / right);
}

static int64_t unequal_func(int64_t left,int right)
{
    return (int64_t)(left != right);
}

enum
{
    SYMBOL_EQUAL = 0,
    SYMBOL_UNEQUAL ,
    SYMBOL_OR ,
    SYMBOL_AND ,
    SYMBOL_LEFT_BRACKETS ,
    SYMBOL_RIGHT_BRACKETS,
    SYMBOL_ADD,
    SYMBOL_MINUS,
    SYMBOL_MULTIPLE,
    SYMBOL_DIVISION,
};


static const symbol_item_t g_symbol_tabel[] =
{
    {SYMBOL_EQUAL, "==", 2,equal_func},
    {SYMBOL_UNEQUAL, "!=", 2,unequal_func},
    {SYMBOL_OR, "||", 1,or_func},
    {SYMBOL_AND, "&&", 1,and_func},
    {SYMBOL_LEFT_BRACKETS, "(", 0,NULL},
    {SYMBOL_RIGHT_BRACKETS, ")", 0,NULL},
    {SYMBOL_ADD,"+",2,add_func},
    {SYMBOL_MINUS,"-",2,minus_func},
    {SYMBOL_MULTIPLE,"*",3,multiple_func},
    {SYMBOL_DIVISION,"/",4,division_func},
};


ScriptEngine::ScriptEngine()
{
    _exp_stack = new item_value_t[MAX_STACK_SIZE];
    _symbol_stack = new item_value_t[MAX_STACK_SIZE];
    _post_exp_stack = new item_value_t[MAX_STACK_SIZE];
    _post_exp_stack_compute = new item_value_t[MAX_STACK_SIZE];

    _exp_size = 0;

    _post_exp_size = 0;

    _symbol_size = 0;

}

ScriptEngine::~ScriptEngine()
{
    if(NULL != _exp_stack)
    {
        delete []_exp_stack;
        _exp_stack = NULL;
    }

    if(NULL != _symbol_stack)
    {
        delete []_symbol_stack;
        _symbol_stack = NULL;
    }

    if(NULL != _post_exp_stack)
    {
        delete []_post_exp_stack;
        _post_exp_stack = NULL;
    }

    if(NULL != _post_exp_stack_compute)
    {
        delete []_post_exp_stack_compute;
        _post_exp_stack_compute = NULL;
    }
}

int ScriptEngine::validate_expression(const char *expression) {
    int ret = 0;
    ret = this->_format_expression(expression);
    if (0 != ret) {
        printf("validate_expression failed ret=%d", ret);
        return ret;
    }
    ret = this->_change_to_post();
    if (0 != ret) {
        printf("change_to_post failed ret=%d", ret);
        return ret;
    }
    return ret;
}

int ScriptEngine::analyse(PFUN_VARIABLE_CALLBACK callback, void *data, int *mark) {
    int ret = 0;
    for(int i = 0; i < _post_exp_size; ++i)
    {
        _post_exp_stack_compute[i] = _post_exp_stack[i];
        item_value_t& temp_item = _post_exp_stack_compute[i];
        if(ITEM_TYPE_VARIABLE == temp_item.type)
        {
            if (callback != NULL) {
                temp_item.value = callback(temp_item.name, data, mark);
                temp_item.type = ITEM_TYPE_VALUE;
            }
            else {
                *mark = 1;
                printf("analyse: callback passed is NULL, can't convert VARIABLE to VALUE\n");
            }
        }
    }

    //计算后缀表达式
    
    int result = 0;
    if (*mark == 0) {
        ret = this->_calculate(&result);
    }

    if (ret < 0) {
        printf("calculate error ret=%d", ret);
        *mark = 1;
    }

    return result;
}


int ScriptEngine::analyse(const char *expression, PFUN_VARIABLE_CALLBACK callback)
{
    return this->analyse(expression, callback, NULL);
}

int ScriptEngine::analyse(const char *expression, PFUN_VARIABLE_CALLBACK callback, void *data) {
    int mark = 0;
    return this->analyse(expression, callback, &mark);
}
int ScriptEngine::analyse(const char *expression, PFUN_VARIABLE_CALLBACK callback, void *data, int *mark)
{
    int ret = 0;
    if (callback == NULL) {
        printf("analyse: callback passed is NULL.\n");
    }

    ret = this->_format_expression(expression);
    if (0 != ret) {
        printf("format_expression failed ret=%d", ret);
        *mark = 1;
        return 0;
    }

    //LOG the _exp_stack.
//    for(int i = 0; i < _exp_size ; ++i) {
//        const item_value_t& temp_item = _exp_stack[i];
//    }

    this->_change_to_post();

    //printf("\n\n");
//    for(int i = 0; i < _post_exp_size; ++i)
//    {
//        const item_value_t& temp_item = _post_exp_stack[i];
//        printf("index:%d name:%s type:%d value:%d \n",i,temp_item.name,temp_item.type,temp_item.value);
//    }

    //计算变量的值

    for(int i = 0; i < _post_exp_size; ++i)
    {
        _post_exp_stack_compute[i] = _post_exp_stack[i];
        item_value_t& temp_item = _post_exp_stack_compute[i];
        if(ITEM_TYPE_VARIABLE == temp_item.type)
        {
            if (callback != NULL) {
                temp_item.value = callback(temp_item.name, data, mark);
                temp_item.type = ITEM_TYPE_VALUE;
            }
            else {
                *mark = 1;
                printf("analyse: callback passed is NULL, can't convert VARIABLE to VALUE\n");
            }
        }
    }

    //计算后缀表达式
    
    int result = 0;
    if (*mark == 0) {
        ret = this->_calculate(&result);
        if (ret < 0) {
            *mark = 1;
        }
    }

    return result;
}

int ScriptEngine::_calculate(int *result)
{
    int symbol_index,left_index,right_index;
    int64_t cal_value,left_value,right_value;
    const item_value_t* ptr_item;
    while(true)
    {
        symbol_index = -1;

        for(int i = 0; i < _post_exp_size; ++i)
        {
            if(ITEM_TYPE_SYMBOL == _post_exp_stack_compute[i].type)
            {
                symbol_index = i;
                break;
            }
        }

        if(-1 == symbol_index)
            break;

        right_index = -1;
        for(int i = symbol_index -1; i >=0 ; --i)
        {
            if(ITEM_TYPE_VALUE == _post_exp_stack_compute[i].type)
            {
                right_index = i;
                break;
            }
        }

        left_index = -1;
        for(int i = right_index - 1; i >= 0; --i)
        {
            if(ITEM_TYPE_VALUE == _post_exp_stack_compute[i].type)
            {
                left_index = i;
                break;
            }
        }

        if(-1 == right_index || -1 == left_index)
        {
            printf("illegal expression![left:%d][right:%d]\n",left_index,right_index);
            return -2;
        }

        PFUN_OPEARTOR func = g_symbol_tabel[_post_exp_stack_compute[symbol_index].value].pfun;
        left_value = _post_exp_stack_compute[left_index].value;
        right_value = _post_exp_stack_compute[right_index].value;
        if (NULL == func) {
            return -4;
        }
        else {
            cal_value = func(left_value,right_value);
        }

        //有点trick，主要是不像移动内存

        _post_exp_stack_compute[symbol_index].value = cal_value;
        _post_exp_stack_compute[symbol_index].type = ITEM_TYPE_VALUE;

        _post_exp_stack_compute[left_index].type = ITEM_TYPE_FINISH;
        _post_exp_stack_compute[right_index].type = ITEM_TYPE_FINISH;
    }

    int result_index = -1;
    int result_num = 0;
    for(int i = 0 ; i < _post_exp_size; ++i)
    {
        if(ITEM_TYPE_VALUE == _post_exp_stack_compute[i].type)
        {
            result_index = i;
            ++ result_num;
        }
    }

    if(-1 == result_index)
    {
        printf("illegal exp because no result index!\n");
        return -2;
    }

    if(1 != result_num)
    {
        printf("illegal exp because result num is: %d \n",result_num);
        return -3;
    }

    printf("result: %lld \n",_post_exp_stack_compute[result_index].value);
    *result = _post_exp_stack_compute[result_index].value;
    return 0;
}

int ScriptEngine::_str_to_item(const char* str, item_value_t& item)
{
    //判别是否是符号
    static int symbol_table_size = sizeof(g_symbol_tabel) / sizeof(g_symbol_tabel[0]);
    for(int i = 0; i < symbol_table_size; ++i)
    {
        if(0 == strcmp(g_symbol_tabel[i].symbol,str))
        {
            snprintf(item.name,sizeof(item.name),"%s",str);
            item.type = ITEM_TYPE_SYMBOL;
            item.value = g_symbol_tabel[i].value ;

            return 0;
        }
    }

    //数字

    bool is_digit = true;
    int str_size = strlen(str);
    for(int i = 0; i < str_size ; ++i)
    {
        if(! isdigit(str[i]))
        {
            is_digit = false;
            break;
        }
    }

    if(true == is_digit)
    {
        snprintf(item.name,sizeof(item.name),"%s",str);
        item.type = ITEM_TYPE_VALUE;
        item.value = atol(str);
        return 0;
    }
    else
    {
        snprintf(item.name,sizeof(item.name),"%s",str);
        item.type = ITEM_TYPE_VARIABLE;
        item.value = 0;
        return 0;
    }
}

void ScriptEngine::_add_item(const char* start,const char* end)
{
    if(_exp_size >= MAX_STACK_SIZE)
        return;

    char temp_name[128];
    item_value_t temp_item;
    int size = (int)(end - start);
    if(size <= 0 || size >= sizeof(temp_name))
        return;

    memcpy(temp_name,start,size);
    temp_name[size] = '\0';

    this->_str_to_item(temp_name,temp_item);
    _exp_stack[_exp_size ++] = temp_item;
}

int ScriptEngine::_format_expression(const char* expression)
{
    _exp_size = 0;

    char temp_str[1024];
    int temp_str_index = 0;
    int exp_size = strlen(expression);
    if(exp_size >= 1024)
    {
        return -1;
    }

    for(int i = 0 ; i < exp_size ; ++i)
    {
        if(' ' != expression[i] && '\t' != expression[i])
        {
            temp_str[temp_str_index ++] = expression[i];
        }
    }
    temp_str[temp_str_index] = '\0';

    printf("temp_str:{%s} \n",temp_str);

    const char* curr;
    const char* start;

    curr = start =  temp_str;

    while( *curr )
    {
        if('(' == *curr || ')' == *curr || '+' == *curr || '-' == *curr || '*' == *curr || '/' == *curr)
        {
            this->_add_item(start,curr);

            start = curr;
            curr = curr + 1;
            this->_add_item(start,curr);

            start = curr;
        }
        else if('!' == *curr || '=' == *curr)
        {
            if('=' != *(curr+1))
            {

            }

            this->_add_item(start,curr);

            start = curr;
            curr = curr + 2;
            this->_add_item(start,curr);

            start = curr;

        }
        else if('&' == *curr)
        {
            if('&' != *(curr+1))
            {

            }
            this->_add_item(start,curr);

            start = curr;
            curr = curr + 2;
            this->_add_item(start,curr);
            start = curr;
        }
        else if('|' == *curr)
        {
            if('|' != *(curr + 1))
            {

            }
            this->_add_item(start,curr);

            start = curr;
            curr = curr + 2;
            this->_add_item(start,curr);
            start = curr;
        }
        else if ((*curr >= 0x30 && *curr <= 0x39) || (*curr >= 0x41 && *curr <= 0x5a) || (*curr >= 0x61 && *curr <= 0x7a) || *curr == 0x5f) {
            //0-9,A-Z,a-z,_
            ++ curr;
        }
        else
        {
            printf("Illegal Expression:{%s},illegal character{%c}", expression, *curr);
            return -2;
        }
    }
    
    this->_add_item(start,curr);

    return 0;
}


int ScriptEngine::_change_to_post()
{
    _post_exp_begin = _post_exp_end = 0;
    _symbol_beign = _symbol_end = 0;
    _post_exp_end = 1;
    _symbol_end = 1;

    int cur_priority;
    int stack_priority;
    bool is_right_brackets;

    for(int i = 0; i < _exp_size; ++i)
    {
        const item_value_t& temp_item = _exp_stack[i];
        switch(temp_item.type)
        {
            case ITEM_TYPE_VALUE:
            case ITEM_TYPE_VARIABLE:
                _post_exp_stack[_post_exp_size ++] = temp_item;
                break;

            case ITEM_TYPE_SYMBOL:
                is_right_brackets = false;

                if(SYMBOL_LEFT_BRACKETS == temp_item.value) {
                    _symbol_stack[_symbol_size ++] = temp_item;
                    break;
                }

                cur_priority = g_symbol_tabel[temp_item.value].priority;
                while(_symbol_size > 0) {
                    stack_priority = g_symbol_tabel[ _symbol_stack[_symbol_size-1].value].priority;

                    if(cur_priority <= stack_priority) {
                        if(SYMBOL_RIGHT_BRACKETS == temp_item.value && SYMBOL_LEFT_BRACKETS == _symbol_stack[_symbol_size -1].value) {
                            -- _symbol_size;
                            is_right_brackets = true;
                            break;
                        } else {
                            _post_exp_stack[_post_exp_size ++] = _symbol_stack[-- _symbol_size];
                        }
                    }
                    else {
                        break;
                    }
                }
                
                if (SYMBOL_RIGHT_BRACKETS == temp_item.value && _symbol_size <= 0 && false == is_right_brackets) {
                    printf("change_to_post unmatched brackets {%s}", temp_item.name);
                    return -4;
                }

                if(false == is_right_brackets) {
                    _symbol_stack[_symbol_size ++] = temp_item;
                }

                break;

            default:
                printf("illegl item type:%d \n",temp_item.type);
                return -3;
        }
    }

    while(_symbol_size > 0) {
        _post_exp_stack[_post_exp_size ++] = _symbol_stack[-- _symbol_size];
        if (SYMBOL_LEFT_BRACKETS == _symbol_stack[_symbol_size].value) {
            printf("change_to_post unmatched brackets {%s}", _symbol_stack[_symbol_size].name);
            return -4;
        }
    }

    return 0;

}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
