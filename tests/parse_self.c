#include "irc.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <libmowgli/mowgli.h>
#include "logger.h"
#include "parser.h"
#include "test.h"

int main(void)
{
    irc_event_t *out;
    logger_init("sigyn.log");

    /*out = parse(":NOTsvchost!~none@c-68-58-12-123.hsd1.il.comcast.net PRIVMSG #malkier :even though i dont buy tyhe notion that bachelor degree means almost nothing anymore, i do believe that a masters sets you apart");*/
    char string[] = ":alyx!~alyx@omg.ponies PRIVMSG #atheme-project :Majic: <3";
    out = parse(string);
    /*printf("User: %s!%s@%s\nCommand: %s\nParams: %s\n", out->origin->nick,*/
            /*out->origin->user, out->origin->host, out->command, out->message);*/
   printf("Origin: %s\nCommand: %s\nData: %s\n", out->origin, out->command, out->data); 
    return 0;
}
