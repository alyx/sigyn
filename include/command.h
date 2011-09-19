#include <libmowgli/mowgli.h>

typedef struct command_ command_t;
typedef void (*command_function_t)(void *event, int user_argstack_size, char **user_arguments);

struct command_
{
    char *name;
    command_function_t function;
};

void command_init(void);
command_t *command_find(const char *name);
void command_add(const char *name, void *function);
void command_del(const char *name, void *function);
