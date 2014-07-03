#include "../irc.h"
#include "../module.h"
#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static char *mod_invokers[1] = {NULL};
static void moe(char **args, enum irc_type type);

static Module mod = {
        "kawaii",
        "$0 to make your message Kawaii",
        mod_invokers,
        do_kawaii,
        1,
        T_JOIN,
        NULL
};

static void
give_op(char **args, enum irc_type type)
{
        char buf[IRC_MSG_LEN];

        (void) type;

        sprintf("I don't know what the fuck I'm doing, please help\n");
}

void
mod_kawaii(void)
{
        mod_add(&mod);
}
