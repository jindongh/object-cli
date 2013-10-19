obj-cli
=======

Object Oriented Command Line Interface Library of C++

Introduce
--------
This is used to handle command line input.
Programers can write a struct to describe the input and accord function,
Despite the getopt process and error handler

### Example
        make
        ./objcli node set --meta=hank
        ./objcli node get --meta=hank

Usage
--------
First, define a Request struct, include all possible parameters

Second, define a Context class, include the 

Thirdly, define the class and actions of the cli

Finally, call CliInvoke to parse request and execute actions.

Here are some example:

### Simple
        #include <obj-cli.h>
        struct Request{
                CLI_DECLARE(param1);
                CLI_DECLARE(param2);
        }
        struct Context{
                const char *password;
        }
        int func_dummy1(Request &, Context *){
                return 0;
        }
        int func_dummy1(Request &, Context *){
                return 0;
        }
        CliObject<Request,Context> meta[] = {
                {
                        "class1", {
                                {
                                        "action1",
                                        func_dummy,
                                        {
                                        }
                                },
                                        "action2",
                                        func_dummy2,
                                        {
                                        }
                                }
                        }
                }, {
                        "class2",{
                        }
                }
        };
        // ./a.out class1 action1 --param1=a --param2=b
        // ./a.out class1 action2 --param2=a
        int main(int argc, const char *argv[]) {
                return CliInvoke<Request, Context>(argc, argv, meta, 3);
        }
### Advanced
        int main(int argc, const char* argv[]) {
                Request request;
                Context context;
                InvokeContext<Request, Context> env(argv[0], g_meta, meta_num, g_opt, opt_num);
                int ret = CliParse(argc, argv, request, env);
                if(0 != ret){
                        return ret;
                }
                if(0 != strcmp(request.password, "secret")){
                        return -1;
                }
                context.password = "holy pot";
                return CliInvoke(request, context, env);
        }

AutoComplete
-------
Besides, this program supplies auto-complete functions.

Users can use <Tab> to call the program give Tips.

### example
        ./objcli node <Tag>
        set get


About
--------
For more information, please refer to the code
For more and more information, please contact me via email which hides in codes.
