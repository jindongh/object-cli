//@author hankjin(jindongh@gmail.com)
//@date 2013/10/19 09:30:31

#include "obj_cli.h"

struct CliContext
{
    void doop(const char *msg)
    {
        fprintf(stdout, "context.doop=%s\n", msg);
    }
};

struct Request
{
    CLI_DECLARE(dyndata_meta)
    CLI_DECLARE(dyndata)
    CLI_DECLARE(meta)
    CLI_DECLARE(verbose)
};
int cli_set_op(Request &request, CliContext *context)
{
    fprintf(stdout, "node set is called, meta=%s verbose=%d\n", request.meta, NULL!=request.verbose);
    context->doop("set");
    return 0;
};
int cli_get_op(Request &request, CliContext *)
{
    fprintf(stdout, "%s\n", request.meta);
    return 0;
};
CliObject<Request,CliContext> g_meta[] = {
    {
        "node",//null object
        {
            {
                "set",
                cli_set_op,
                {
                    { CLI_PARAM(Request,meta), "meta(in_json)", CLI_REQUIRED, CLI_HAS_ARG, NULL},
                    { CLI_PARAM(Request,verbose), "verbose", !CLI_REQUIRED, !CLI_HAS_ARG, NULL}
                }
            },
            {
                "get",
                cli_get_op,
                {
                    { CLI_PARAM(Request,meta), "meta", !CLI_REQUIRED, CLI_HAS_ARG, NULL}
                }
            }
        },
    },
    {
        "lock",//lock operation
        {
        }
    }
};
int main(int argc, char *argv[])
{
    CliContext context;
    int meta_num = sizeof(g_meta)/sizeof(CliObject<Request,CliContext>);
    int ret = CliInvoke<Request,CliContext>(argc, argv, g_meta, meta_num, &context);
    return ret;
};

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
