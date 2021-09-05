#include <cstdio>
#include <string>
#include <functional>
#include <list>

#include <liback/utils/lexer.h>
#include <liback/utils/result.h>

struct ksh_info
{
    FILE* in;
    FILE* out;
};

typedef std::list<std::string> ksh_arg_list_t;

struct ksh_function
{
    std::string name;
    std::string description;
    std::function<int(ksh_info&, ksh_arg_list_t)> callback;
};

static int ksh_execute(ksh_info& info, std::string cmd)
{
    std::list<ksh_function> functions;

    ksh_function function =
    {
        .name = "test",
        .description = "Prints hello world",
        .callback = [](ksh_info& kinfo, ksh_arg_list_t arg_list)
        {
            fputs("Hello world\n", kinfo.out);

            return utils::result::SUCCESS;
        }
    };

    functions.push_back(function);

    ksh_arg_list_t argument_list;
    std::string func_name;

    utils::lexer lexer(cmd);

    while(lexer)
    {
        if(lexer.current() == '(')
        {
        }
        else if(lexer.current() == ')')
        {

        }
        else
        {
            func_name += std::string(1, lexer.current());
        }

        lexer.advance();
    }

    int result = utils::result::SUCCESS;

    if(func_name == std::string("help"))
    {
        fputs("ackOS kernel shell\n", info.out);
        fputs("Example: print(\"Hello World!\")\n\nFunctions:\n", info.out);

        for(auto& func : functions)
        {
            fprintf(info.out, "%s - %s\n", func.name.c_str(), func.description.c_str());
        }
    }
    else if(!func_name.empty())
    {
        std::function<int(ksh_info&, ksh_arg_list_t)> function = nullptr;

        for(auto& func : functions)
        {
            if(func.name == func_name)
            {
                if(func.callback)
                {
                    function = func.callback;
                }
            }
        }

        if(function)
        {
            result = function(info, argument_list);
        }
        else
        {
            fprintf(info.out, "error: The function \"%s\" does not exist\n", func_name.c_str());

            result = utils::result::ERR_INVALID_COMMAND;
        }
    }

    return result;
}

static void ksh_prompt(ksh_info& info)
{
    fputs("$ ", info.out);

    char c = 0;

    std::string cmd;

    do
    {
        c = getc(info.in);

        if(c == '\r')
        {
            break;
        }
        else
        {
            putc(c, info.out);

            cmd += std::string(1, c);
        }
    }
    while(c != EOF);

    putc('\n', info.out);

    utils::result res = ksh_execute(info, cmd);

    if(!res && res != utils::result::ERR_INVALID_COMMAND)
    {
        printf("error: %s\n", res.get_description());
    }

    ksh_prompt(info);
}

void ksh_start(FILE* out_stream, FILE* in_stream)
{
    ksh_info info { out_stream, in_stream };

    puts("Greetings, \n");

    fputs(
R"(              ,--.    ,-----.  ,---.   
 ,--,--. ,---.|  |,-.'  .-.  ''   .-'  
' ,-.  || .--'|     /|  | |  |`.  `-.  
\ '-'  |\ `--.|  \  \'  '-'  '.-'    | 
 `--`--' `---'`--'`--'`-----' `-----' 

)", out_stream);

    fputs("NOTE: This shell is a work in progress\n\n", out_stream);

    ksh_prompt(info);
}
