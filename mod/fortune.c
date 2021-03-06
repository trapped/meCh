#include "../irc.h"
#include "../module.h"
#include "../config.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void do_fortune(Module *m, char **args, enum irc_type type);
static char *mod_invokers[2] = {"fortune", NULL};

static Module fortune = {
	"Fortune",
	"$0 to know your fortune",
	mod_invokers,
	do_fortune,
	3,
	T_CHAN|T_MSG,
	NULL
};

static void
do_fortune(Module *m, char **args, enum irc_type type)
{
	int fd[2];
	int status, i;
	char buf[IRC_MSG_LEN];

	pipe(fd);
	if(!fork()) {
		dup2(fd[1], 1);
		close(fd[0]);
		execlp("fortune", "fortune", NULL);
	}
	close(fd[1]);
	wait(&status);
	i = read(fd[0], buf, IRC_MSG_LEN);
	close(fd[0]);
	buf[i] = '\0';

	for(i=0; buf[i]; i++)
		if(buf[i]=='\t' || buf[i]=='\n')
			buf[i] = ' ';
	
	if(type==T_CHAN)
		irc_say(buf);
	else
		irc_msg(args[0], buf);
}

void
mod_fortune(void)
{
	int status;

	/* try to execute the "fortune" program */
	if(!fork()) {
		execlp("fortune", "fortune", NULL);
		exit(1); /* should not be reachable */
	}
	wait(&status);
	printf("\"fortune\" exit status: %d\n", status);
	if(status) {
		puts("The \"fortune\" program cannot be found in this system. The Fortune "
			"module will not be loaded");
		return;
	}
	mod_add(&fortune);
}
