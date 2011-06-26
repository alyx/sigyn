#import "irc.h"
#import <stddef.h>
#import <stdio.h>
#import <sys/types.h>
#import <libmowgli/mowgli.h>
#import "parser.h"
#import "test.h"

void test_parse(void)
{
    irc_event_t *out;

    out = parse(":NOTsvchost!~none@c-68-58-12-123.hsd1.il.comcast.net PRIVMSG #malkier :even though i dont buy tyhe notion that bachelor degree means almost nothing anymore, i do believe that a masters sets you apart");
    printf("User: %s!%s@%s\nCommand: %s\nParams: %s\n", out->origin->nick,
            out->origin->user, out->origin->host, out->command, out->message);
}
