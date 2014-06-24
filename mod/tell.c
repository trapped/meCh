#include "../irc.h"
#include "../module.h"
#include "../config.h"
#include "../utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* we want to be called even if no invoker is used */
static char *mod_invokers[1] = {NULL};
static void do_tell(Module *m, char **args, enum irc_type type);

static Module tell = {
	"Tell",
	".tell <nick> <message> to leave message <message> for <nick>. If you tell that "
	"to the bot privately, it will send the message in private.",
	mod_invokers,
	do_tell,
	4,
	T_JOIN|T_CHAN|T_MSG,
	NULL
};

/* We'll use a linked list for storing messages. O(n) is okay */
typedef struct TellMsg TellMsg;
struct TellMsg {
	char name[IRC_NICK_LEN];
	char sender[IRC_NICK_LEN];
	char msg[IRC_MSG_LEN];
	char private;
	TellMsg *next;
};

static TellMsg base = {"", "", "", 0, NULL};

static void
add_msg(char *sender, char *name, char *msg, char private)
{
	TellMsg *new;

	new = (TellMsg *) malloc(sizeof(TellMsg));
	strcpy(new->sender, sender);
	strcpy(new->name, name);
	strcpy(new->msg, msg);
	new->private = private;
	new->next = base.next;
	base.next = new;
}

static TellMsg *
get_msg(char *name)
{
	TellMsg *prevm, *m;
	prevm = &base;
	m = base.next;
	while(m) {
		if(!strcmp(m->name, name)) {
			prevm->next = m->next;
			return m;
		}
		prevm = m;
		m = m->next;
	}
	return NULL;
}

static void
do_tell(Module *m, char **args, enum irc_type type)
{
	TellMsg *tmsg;
	char buf[IRC_MSG_LEN];
	int i;

	printf("tell: '%s' '%s' '%s' '%s'\n", args[0], args[1], args[2], args[3]);

	/* check if we have any message for that nick */
	while((tmsg=get_msg(args[0]))) {
		buf[0] = '(';
		strcpy(buf+1, tmsg->sender);
		strcat(buf, ") ");
		strcat_msg(buf, tmsg->msg);
		if(tmsg->private)
			irc_msg(args[0], buf);
		else {
			strcpy(tmsg->msg, args[0]);
			strcat(tmsg->msg, ": ");
			strcat_msg(tmsg->msg, buf);
			irc_say(tmsg->msg);
		}
		free(tmsg);
	}

	for(i=0; i<4; i++)
		if(!*args[i])
			return;

	if(type==T_CHAN) {
		if(strcmp(args[1], ".tell"))
			return;
	} else if(type==T_MSG) {
		if(strcmp(args[1], "tell"))
			return;
	} else return;

	if(!strcmp(args[0], args[2])) {
		switch(type) {
			case T_CHAN:
				sprintf(buf, "%s: You can tell yourself that.", args[0]);
				irc_say(buf);
				return;
			case T_MSG:
				irc_msg(args[0], "You can tell yourself that.");
				return;
			default: return;
		}
	}

	add_msg(args[0], args[2], args[3], (type==T_MSG));

	switch(type) {
		case T_CHAN:
			sprintf(buf, "%s: I'll pass your message to %s.", args[0], args[2]);
			irc_say(buf);
			return;
		case T_MSG:
			sprintf(buf, "I'll pass your message to %s privately.", args[2]);
			irc_msg(args[0], buf);
			return;
		default: return;
	}
}

static void
erase_tell(void)
{
	TellMsg *t, *tn;

	t = base.next;
	while(t) {
		tn = t->next;
		free(t);
		t = tn;
	}
}

void
mod_tell(void)
{
	atexit(erase_tell);
	mod_add(&tell);
}
