//@author hankjin(jindongh@gmail.com)
//@date 2013/10/19 09:30:31
//
#include "obj_cli.h"

/** map for option convert */
std::map<std::string, size_t> g_option_map;

/**
 * @brief
 */
bool is_completion(int argc, char *argv[])
{
    const int optValue = 1;
    const char *short_options=":";
    option long_options[]={
        {
            "autocompletion",
            0,
            0,
            optValue
        },{
            NULL,
            0,
            0,
            0
        }
    };
    int option_index = 0;
    while(1){
        int c = getopt_long(argc, argv, short_options, long_options, &option_index);
        if(-1 == c){
            break;
        }
        else if(c == optValue){
            return true;
        }
    }
    return false;
};


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
