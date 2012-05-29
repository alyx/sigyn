#include "sigyn.h"

#ifndef ENABLE_STACK
void sigyn_log_backtrace(void) { return; }
#else
#include <libunwind.h>
void sigyn_log_backtrace(void)
{
    char func[64];
    unw_word_t    offset, pc;
    unw_cursor_t  cursor;
    unw_context_t context;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    while (unw_step(&cursor) > 0)
    {
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        func[0] = '\0';
        unw_get_proc_name(&cursor, func, sizeof(func), &offset);
        fprintf(stderr, "%p : (%s+0x%x) [%x]\n", (void *)pc, func, offset, pc);
    }
}
#endif
