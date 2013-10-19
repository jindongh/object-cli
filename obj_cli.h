//@author hankjin(jindongh@gmail.com)
//@date 2013/10/19 09:30:31
#ifndef  __OBJ_CLI_H_
#define  __OBJ_CLI_H_

#include <string>
#include <vector>
#include <map>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/** MAX OPTION */
#define CLI_MAX_OPTION  255
/** MAX TYPE */
#define CLI_MAX_OBJECRequest  255
/** MAX ACTION */
#define CLI_MAX_ACTION  255
/** declare request variable */
#define CLI_DECLARE(filed)  const char *filed;
/** define param: name, offset, specified */
#define CLI_PARAM(Request, field) #field, g_option_map[#field]=offsetof(Request, field), false
/** param attr: required */
#define CLI_REQUIRED    true
/** param attr: has argument */
#define CLI_HAS_ARG     true

/** map for convert */
extern std::map<std::string, size_t> g_option_map;

/**
 * @brief option
 */
struct CliOption
{
    /** name */
    const char *name;
    /** offset of request */
    size_t offset;
    /** specified from command line */
    bool specified;

    /** description, structure fields */
    const char *description;
    /** required or not */
    bool required;
    /** has arg */
    bool has_arg;
    /** default value */
    const char *default_value;
};

/**
 * @brief action
 */
template <class Request, class Context>
struct CliAction
{
    /** function */
    typedef int (*CliAction_imp)(Request& request, Context *context);

    /** action name */
    const char* action;
    /** action implementation */
    CliAction_imp handler;
    /** action options */
    CliOption options[CLI_MAX_OPTION];

};

/**
 * @brief object
 */
template <class Request, class Context>
struct CliObject
{
    /** object */
    const char *object;
    /** action */
    CliAction<Request,Context> actions[CLI_MAX_ACTION];
};

struct IOContext
{
    /** stdout stream */
    FILE *out;
    /** stderr stream */
    FILE *err;
    /**
     * @brief constructor, replace stdout/stderr
     */
    IOContext(FILE *pout, FILE *perr)
    {
        out = stdout;
        err = stderr;
        stdout = pout;
        stderr = perr;
    }
    /**
     * @brief back
     */
    ~IOContext()
    {
        fflush(stdout);
        fflush(stderr);
        stdout = out;
        stderr = err;
    }
};
template <class Request, class Context>
struct InvokeContext
{
    //context
    const char *object;
    const char *action;
    int object_index;
    int action_index;
    //parameter
    const char *prog;
    CliObject<Request, Context> *meta;
    int meta_num;
    CliOption *global_opt;
    int global_opt_num;
    IOContext io;
    InvokeContext(const char *prog, CliObject<Request,Context> *meta, int meta_num,
            CliOption *global_opt=NULL, int global_opt_num=0,
            FILE *out=stdout, FILE *err=stderr)
        : prog(prog), meta(meta), meta_num(meta_num),
        global_opt(global_opt), global_opt_num(global_opt_num),
        io(out, err)
    {
        object = "";
        action = "";
        object_index = -1;
        action_index = -1;
    }
};
/**
 * @brief print usage msg
 */
template<class Request, class Context>
void print_usage(InvokeContext<Request,Context>  &env, bool completion)
{
    if(-1 == env.object_index){
        if(!completion){
            fprintf(stderr, "Usage: %s <object> <action> <params>\n", env.prog);
            fprintf(stderr, "Error: \"%s\" is invalid object, valid objects:\n", env.object);
        }
        for(int i=0; i<env.meta_num; i++){
            fprintf(stderr, "%s\n", env.meta[i].object);
        }
        return;
    }
    if(-1 == env.action_index){
        if(!completion){
            fprintf(stderr, "Usage: %s <object> <action> <params>\n", env.prog);
            fprintf(stderr, "Error: \"%s\" \"%s\" is invalid action, valid actions:\n", 
                env.object, env.action);
        }
        for(int i=0; env.meta[env.object_index].actions[i].action; i++){
            fprintf(stderr, "\t%s\n", env.meta[env.object_index].actions[i].action);
        }
        return ;
    }
    if(!completion){
        fprintf(stderr, "Usage: %s %s %s\n", env.prog, env.object, env.action);
    }
    CliOption *options = env.meta[env.object_index].actions[env.action_index].options;
    CliOption *option = NULL;
    const char *fmt;
    int i=0;
    int j=0; 
    while(options[i].name || j<env.global_opt_num){
        if(options[i].name){
            option = options+i;
        }
        else{
            option = env.global_opt+j;
        }
        if(option->has_arg){
            if(option->required || completion){
                fmt = "\t--%s='%s'\n";
            }
            else{
                fmt = "\t[--%s='%s']\n";
            }
        }
        else{
            if(option->required || completion){
                fmt = "\t--%s(%s)\n";
            }
            else{
                fmt = "\t[--%s(%s)]\n";
            }
        }
        if(!completion || !option->specified){
            fprintf(stderr, fmt, option->name, option->description);
        }
        if(options[i].name){
            i++;
        }
        else{
            j++;
        }
    }
};

/** strncmp */
#define BEGIN_WITH(sub,buf) (sub==buf[0])

/** 
 * @brief invoke with request
 */
template <class Request, class Context>
int CliInvoke(Request &request, Context *context, InvokeContext<Request,Context> &env)
{
    return (env.meta[env.object_index].actions[env.action_index].handler) (request, context);
}

/**
 * @brief is completion
 */
bool is_completion(int argc, char *argv[]);
/**
 * @brief parse options
 */
template <class Request, class Context>
int CliParse(int argc, char *argv[], Request &request, InvokeContext<Request,Context> &env)
{
    int missing_index = -1;
    int option_index = 0;
    const char *short_options = ":";
    option long_options[CLI_MAX_OPTION];
    //<object> <action>
    if(argc > 2 && !BEGIN_WITH('-',argv[1]) && !BEGIN_WITH('-', argv[2])){
        env.object = argv[1];
        env.action = argv[2];
    }
    //<action> || <object>+missing action
    else if(argc > 1 && !BEGIN_WITH('-', argv[1])){
        env.action = argv[1];
    }
    for(int i=0; i<env.meta_num; i++){
        if(0 == strcmp(env.meta[i].object, env.object)){
            env.object_index = i;
            break;
        }
        if(0 == strcmp(env.meta[i].object, env.action)){
            missing_index = i;
        }
    }
    //<object> + missing action
    if(env.object_index < 0 && 0==strcmp("", env.object)){
        env.object = env.action;
        env.action = "";
        env.object_index = missing_index;
    }
    //autocomplete
    bool autocomplete = is_completion(argc, argv);
    optind=0;

    if(env.object_index < 0){
        print_usage(env, autocomplete);
        return EXIT_FAILURE;
    }
    for(int i=0; env.meta[env.object_index].actions[i].action; i++){
        if(0 == strcmp(env.meta[env.object_index].actions[i].action, env.action)){
            env.action_index = i;
            break;
        }
    }
    if(env.action_index < 0){
        print_usage(env, autocomplete);
        return EXIT_FAILURE;
    }
    CliOption *options = env.meta[env.object_index].actions[env.action_index].options;
    for(; options[option_index].name; option_index++){
        long_options[option_index].name = options[option_index].name;
        long_options[option_index].has_arg = options[option_index].has_arg;
        long_options[option_index].flag = 0;
        long_options[option_index].val = option_index;
        options[option_index].specified = 0;
    }
    for(int i=0; i<env.global_opt_num; i++,option_index++){
        long_options[option_index].name = env.global_opt[i].name;
        long_options[option_index].has_arg = env.global_opt[i].has_arg;
        long_options[option_index].flag = 0;
        long_options[option_index].val = option_index;
        options[option_index].specified = 0;
    }

    long_options[option_index].name = NULL;
    int option_num = option_index;
    //3.2 parse long options
    //opterr = 0;
    while(1){
        int c = getopt_long(argc, argv, short_options, long_options, &option_index);
        if(-1 == c){
            break;
        }
        if(c == '?'){
            if(autocomplete){// && 0 == strcmp("--autocompletion",argv[local_opt_data.optind-1])){
                continue;
            }
            fprintf(stderr, "%s: unrecognized option `%s'\n", argv[0], argv[option_index]);
            print_usage(env, autocomplete);
            return EXIT_FAILURE;
        }
        if(c+env.global_opt_num < option_num){
            options[c].specified = 1;
        }
        else{
            env.global_opt[c-(option_num-env.global_opt_num)].specified = 1;
        }
        std::map<std::string, size_t>::iterator it=g_option_map.find(long_options[c].name);
        const char **ptr = (const char**)( (char*)&request + it->second );
        if(long_options[c].has_arg){
            (*ptr) = argv[optind];
        }
        else{
            (*ptr) = "";
        }
    }
    //3.3 validate
    int meta_i = 0;
    int opt_i = 0;
    options = env.meta[env.object_index].actions[env.action_index].options;
    CliOption *option = NULL;
    while(options[meta_i].name || opt_i < env.global_opt_num){
        //current
        if(options[meta_i].name){
            option = options+meta_i;
        }
        else{
            option = env.global_opt+opt_i;
        }
        const char **ptr = (const char**)( (char*)&request + option->offset);
        //required but not specified
        if(!option->specified){
            if(autocomplete){
                print_usage(env, autocomplete);
                return EXIT_FAILURE;
            }
            else if(option->required){
                fprintf(stderr, "missing option \"--%s\"\n", option->name);
                print_usage(env, autocomplete);
                return EXIT_FAILURE;
            }
            else if(NULL != option->default_value){
                *ptr = option->default_value;
            }
            else{
                (*ptr) = NULL;
            }
        }
        //next.
        if(options[meta_i].name){
            meta_i ++;
        }
        else{
            opt_i ++;
        }
    }
    return autocomplete;
}

/**
 * @brief invoke with 
 */
template <class Request, class Context>
int CliInvoke(int argc, char *argv[], Context *context, InvokeContext<Request,Context> &env)
{
    Request request;
    int ret = CliParse(argc, argv, request, env);
    if(0 != ret){
        return ret;
    }
    return CliInvoke<Request, Context>(request, context, env);
}
/**
 * @brief invoke with 
 */
template <class Request, class Context>
int CliInvoke(int argc, char *argv[], 
        CliObject<Request,Context> *meta, int meta_num, Context *context,
        CliOption *global_opt=NULL, int global_opt_num=0,
        FILE *out=stdout, FILE *err=stderr)
{
    InvokeContext<Request,Context> env(argv[0], meta, meta_num,
            global_opt, global_opt_num, out, err);
    Request request;
    int ret = CliParse(argc, argv, request, env);
    if(0 != ret){
        return ret;
    }
    return CliInvoke<Request, Context>(request, context, env);
}



#endif  //__OBJ_CLI_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
