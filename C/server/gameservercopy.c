#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int error_handler()
{
	perror(NULL);
	fflush(stderr);
	return 1;
}

#define INBUFSIZE 128
#define SESS_ARR_SIZE 16

int global_count = 0;

char msg_intro[] =	"* Welcome to the game server\n* Type 'help' for command list\n";
char msg_warn[] = 	"* Unknown command! Type 'help' for commands\n";

enum fsm_states {
	fsm_start,
	fsm_wait_connection = fsm_start,
	fsm_command,
	fsm_end_turn,
	fsm_finish,
	fsm_error
};

struct session { /*equals player in bank*/
	int fd;
	unsigned long from_ip;
	unsigned short from_port;
	char buf[INBUFSIZE];
	int buf_used;
	enum fsm_states state;
	/*after end turn, we look in the bank: all players have done turn?*/
	/*for game: information about resources*/
	/* ? int status; */
	int number; /*number of player*/ /*depends on count of players*/
	int money; /* 10000 */ /*start count*/
	int material; /* 4 */
	int product; /* 2 */
	int factory; /* 2 */
	/*refreshable inf: max count of prod*/
};

struct bank_stat {
	struct session **player; /*equals sess_array in serv, it's same*/
	int turn;
	int player_max_count; /*we wait for N players before the start*/
	int player_count;
};

struct bank_stat *bank;


/*void bank_setup(struct server_stat *serv, int max_player);*/
void bank_audit();
void bank_send_news_turn();
void bank_check_count_player();
void bank_check_end_turn();
void bank_check_finish();


/*procedure*/
/*for game: send to all players news (what have just changed)*/

/*==========SESSION ACTIONS==========*/
void session_send_string(struct session *sess, const char *str)
{
	write(sess->fd, str, strlen(str));
}

void player_send_string(struct session *sess, const char *str)
{
	session_send_string(sess, str);
}

void session_send_int(struct session *sess, const int n)
{
	char send[256];
	sprintf(send, "%i", n);
	session_send_string(sess, send);
	session_send_string(sess, "\n");
}

void player_send_int(struct session *sess, const int n)
{
	session_send_int(sess, n);
}

void player_setup(int sd)
{
	bank->player_count += 1;
	bank->player[sd]->number = bank->player_count;
	bank->player[sd]->money = 10000;
	bank->player[sd]->material = 4;
	bank->player[sd]->product = 2;
	bank->player[sd]->factory = 2;
}

void player_send_info_about(struct session *sess, int tag)
{
	struct session *player, *target = NULL;
	int i = 0;
	for (i=0;i<SESS_ARR_SIZE;i++) {
		if (bank->player[i] && bank->player[i]->number == tag) {
			target = bank->player[i];
			break;
		}
	}
	player = sess;
	if (target) {
		player_send_string(player,"* Number:     \t");
		player_send_int(player, target->number);
		player_send_string(player,"* Money:      \t");
		player_send_int(player, target->money);
		player_send_string(player,"* Material:   \t");
		player_send_int(player, target->material);
		player_send_string(player,"* Product:    \t");
		player_send_int(player, target->product);
		player_send_string(player,"* Factory:    \t");
		player_send_int(player, target->factory);
	} else {	
		player_send_string(player,"* No player ");
		player_send_int(player, tag);
	}
}

void player_send_help(struct session *player)
{
	player_send_string(player,"* myinfo\n");
	player_send_string(player,"\t- just your info\n");
	player_send_string(player,"* player <number>\n");
	player_send_string(player,"\t- receive inf about player\n");
	player_send_string(player,"* turn\n");
	player_send_string(player,"\t- end your turn\n");
	player_send_string(player,"* market\n");
	player_send_string(player,"\t- receive inf about market state\n");
	player_send_string(player,"* build <count>\n");
	player_send_string(player,"\t- request factories\n");
	player_send_string(player,"* prod <count>\n");
	player_send_string(player,"\t- make production\n");
	player_send_string(player,"* buy <count> <price>\n");
	player_send_string(player,"\t- buy material\n");
	player_send_string(player,"* sell <count> <price>\n");
	player_send_string(player,"\t- sell production\n");
}

struct session *session_make_new(int fd, struct sockaddr_in *from)
{
	struct session *sess = malloc(sizeof(*sess));
	sess->fd = fd;
	sess->from_ip = ntohl(from->sin_addr.s_addr);
	sess->from_port = ntohs(from->sin_port);
	sess->buf_used = 0;
	sess->state = fsm_start;

	session_send_string(sess, msg_intro); 
	/*for game: setup information about resources*/
		/*SOLVED in accept_client*/
	return sess;
}

/*=====support for handle=====*/
int word_count(char *line)
{
	int size = 0;
	char temp[INBUFSIZE], *word, *sep = " ";
	strcpy(temp, line);
	for(word = strtok(temp, sep);
		word;
		word = strtok(NULL, sep))
		size++;
	return size;
}

void packline_print(char **packline)
{
	char **temp = packline;
	printf("packline_print:");
	while(temp[0]) {
		printf("[%s]", temp[0]);
		temp++;
	}
	printf("\n");
}

int packline_size(char **packline)
{
	char **temp = packline;
	int size = 0;
	while(temp[0]) {
		size++;
		temp++;
	}
	return size;
}

char **session_handle_packline(const char *line)
{
	char **packline;
    char temp[INBUFSIZE], *word, *sep = " ";
    int i = 0, size;
    size = word_count((char*)line);
    packline = malloc((size+1)*sizeof(char*));
    packline[size] = NULL;

    strcpy(temp, line);
    for (word = strtok(temp, sep);
        word;
        word = strtok(NULL, sep))
	{
		packline[i] = word;
		i++;
	}
    /* DEBUG */ //packline_print(packline);
    return packline;
}
/*=====end support for handle=====*/

/*=====COMMAND HANDLERS=====*/
int handler_command_1(struct session *player, char **cmd)
{
	
	if (!strcmp("finish", cmd[0])) {
		player->state = fsm_finish;
		return 1;
	}

	if (!strcmp("myinfo", cmd[0])) {
		player_send_info_about(player, player->number);
	} else if (!strcmp("market", cmd[0])) {
		player_send_string(player, "* cmd: market\n");
		/*TODO*/
	} else if (!strcmp("turn", cmd[0])) {
		player_send_string(player, "* cmd: turn\n");
		/* DEBUG */ printf("PLAYER[%i]: end_turn\n", player->number);
		player->state = fsm_end_turn;
		player_send_string(player, "* you end turn\n");
	} else if (!strcmp("help", cmd[0])) {
		player_send_help(player);
	} else {
		player_send_string(player, msg_warn);
		return 0;
	}
	return 1;
}

int handler_command_2(struct session *player, char **cmd)
{
	char *endptr;
	int number = strtol(cmd[1], &endptr, 10);
	if (*endptr) {
		session_send_string(player, msg_warn);
		return 0;
	}

	if (!strcmp("player", cmd[0])) {
		player_send_string(player, "* cmd: player <number>\n");
		player_send_info_about(player, number);
	} else if (!strcmp("prod", cmd[0])) {
		player_send_string(player, "* cmd: prod <count>\n");
		/*player has max count of prod, refresh every month*/
		/*TODO*/
	} else if (!strcmp("build", cmd[0])) {
		player_send_string(player, "* cmd: build <count>\n");
		/*request for factory with turn of creation*/
		/*bank looks to the list of requests and builds if it's time*/
		/*TODO*/
	} else {
		session_send_string(player, msg_warn);
		return 0;
	}
	return 1;
}

int handler_command_3(struct session *player, char **cmd)
{
	char *endptr1, *endptr2;
	int count = strtol(cmd[1], &endptr1, 10);
	int price = strtol(cmd[2], &endptr2, 10);
	if (*endptr1 || *endptr2) {
		session_send_string(player, msg_warn);
		return 0;
	}

	if (!strcmp("buy", cmd[0])) {
		player_send_string(player, "* cmd: buy <count> <price>\n");
	} else if (!strcmp("sell", cmd[0])) {
		player_send_string(player, "* cmd: sell <count> <price>\n");
	} else {
		session_send_string(player, msg_warn);
		return 0;
	}
	return 1;
}
/*=====END OF COMMAND HANDLERS=====*/

void session_handle_command(struct session *sess, const char *line)
{
	char **cmd;
	int size;
	/* DEBUG */ //printf("PLAYER[%i]:", sess->number);
	/* DEBUG */ //printf(" handle_command: \n\t");
	cmd = session_handle_packline(line);
	size = packline_size(cmd);
	
	if (size == 1) {
		handler_command_1(sess, cmd);
	} else if (size == 2) {
		handler_command_2(sess, cmd);
	} else if (size == 3) {
		handler_command_3(sess, cmd);
	} else {
		if (*cmd)
			session_send_string(sess, msg_warn);
	}
	
	/* smt for change resources*/
}

void session_fsm_step(struct session *sess, char *line)
{
	switch(sess->state)
	{
		case fsm_wait_connection:
			break;
		case fsm_command:
			session_handle_command(sess, line);

			bank_audit();
			//bank_check_end_turn();
			//bank_check_finish();

			free(line);
			break;
		/* for game: */
		case fsm_end_turn:
			break;
		/* somewhere must be procedure for check 'are all users end?'
		void server_end_turn(clients fds) */
		case fsm_finish:
			/* smt will point end of the game... */
		case fsm_error:
			free(line);
	}
}

void session_check_lf(struct session *sess)
{
	int i, pos = -1;
	char *line;
	for (i=0; i < sess->buf_used; i++)
	{
		if (sess->buf[i] == '\n')
		{
			pos = i;
			break;
		}
	}
	if (pos == -1)
		return;

	line = malloc(pos+1);
	memcpy(line, sess->buf, pos);
	line[pos] = 0;
	memmove(sess->buf, sess->buf+pos, pos+1);
	sess->buf_used -= (pos+1);

	if (line[pos-1] == '\r')
		line[pos-1] = 0;

	/*DEBUG*/ /*printf("check_lf: [%s]\n", line);*/
	session_fsm_step(sess, line);
}

int session_read(struct session *sess)
{
	int rr, bufp = sess->buf_used;
	rr = read(sess->fd, sess->buf + bufp, INBUFSIZE - bufp);
	if (rr < 0)
	{
		sess->state = fsm_error;
		return 0;
	}
	if (rr == 0)
	{
		sess->state = fsm_finish;
	}
	sess->buf_used += rr;
	session_check_lf(sess);
	if (sess->buf_used >= INBUFSIZE)
	{
		session_send_string(sess, "Line too long! Goodbye...\n");
		sess->state = fsm_error;
		return 0;
	}
	if (sess->state == fsm_finish)
		return 0;
	return 1;
}

void session_cleanup(struct session *sess)
{
	/*may be useful*/
}

/*==========SERVER SETUP AND RUN==========*/
struct server_stat {
	int ls;
	struct session **sess_array; /* depends of SESS_ARR_SIZE */
	/* is game started? than new connection will be 'watcher'*/
	/* step of the game - ?enum round?*/
	int sess_array_size;
};

/*==========BANK ACTIONS==========*/
void bank_audit()
{
	int i;
	printf("***BANK AUDIT***\n");
	printf("*turn         \t%i\n",bank->turn);
	printf("*max players: \t%i\n",bank->player_max_count);
	printf("*player_count:\t%i\n",bank->player_count);
	for (i=4;i < 4 + bank->player_max_count;i++) {
		if (bank->player[i]) {
			printf("*\tplayer %i [%i] ",i,bank->player[i]->number);
			if (bank->player[i]->state == fsm_wait_connection) {
				printf("waiting\n");
			} else if (bank->player[i]->state == fsm_command) {
				printf("active\n");
			} else if (bank->player[i]->state == fsm_end_turn) {
				printf("end turn\n");
			} else if (bank->player[i]->state == fsm_finish) {
				printf("finish\n");
			}
		} else {
			printf("*\tplayer NULL\n");
		}
	}
}

void bank_send_news_turn()
{
	int i;
	for (i=0;i < SESS_ARR_SIZE;i++) {
		if (bank->player[i]) {
			player_send_string(bank->player[i], "* TURN ");
			player_send_int(bank->player[i], bank->turn);
		}
	}
}

void bank_send_news_result()
{
	/*TODO*/
}

void bank_send_news_finish()
{
	int i, winner;
	for (i=0;i < SESS_ARR_SIZE;i++) {
		if (bank->player[i]) {
			player_send_string(bank->player[i], "* GAME OVER \n");
			if (bank->player[i]->state != fsm_finish) {
				winner = i;
				bank->player[winner]->state = fsm_finish;
			}
		}
	}
	for (i=0;i < SESS_ARR_SIZE;i++) {
		if (bank->player[i]) {
			player_send_string(bank->player[i], "* WINNER - PLAYER ");
			player_send_int(bank->player[i],
									bank->player[winner]->number);
		}
	}
}

void bank_check_count_player()
{
	int i;
	if (bank->player_count == bank->player_max_count) {
		for (i=0;i < SESS_ARR_SIZE;i++) {
			if	(bank->player[i]) {
				bank->player[i]->state = fsm_command;
			}
		}
		/* DEBUG */ printf("=====GAME START=====\n");
		bank->turn += 1;
	}
}

void bank_check_end_turn()
{
	int i;
	for (i=0;i < SESS_ARR_SIZE;i++) {
		if	(bank->player[i] &&
			bank->player[i]->state != fsm_end_turn &&
			bank->player[i]->state != fsm_finish) {
			printf("BANK: there are active players\n");
			break;
		}
	}
	if (i == SESS_ARR_SIZE) {
		printf("BANK: there are NO active players\n");
		/*TODO*/
		/*handle requests for auction*/
		/*handle requests for buildings*/
		/*refresh players inf*/
		bank->turn += 1;
		printf("=====TURN %i=====\n", bank->turn);
		bank_send_news_turn();
		bank_audit();
	}
}

void bank_check_finish()
{
	int i, count=0;
	for (i=0;i < SESS_ARR_SIZE;i++) {
		if (bank->player[i] && bank->player[i]->state == fsm_finish) {
			count++;
		}
	}
	if ((bank->player_max_count - count) == 1 || 
			bank->player_count == 1) {
		printf("=====GAME END=====\n");
		bank_send_news_finish();
	}
}

void bank_setup(struct server_stat *serv, int max_player)
{
	bank = malloc(sizeof(bank));
	bank->player = serv->sess_array;
	bank->player_max_count = max_player;
	bank->player_count = 0;
	bank->turn = 0;
	bank_audit();
}
/*==========END OF BANK ACTIONS==========*/

int server_setup(struct server_stat *serv, int port, int max_player)
{
	int i;
	struct sockaddr_in addr;

	serv->ls = socket(AF_INET, SOCK_STREAM, 0);
	if (serv->ls == -1)
		return error_handler();

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (-1 == bind(serv->ls, (struct sockaddr*)&addr, sizeof(addr)))
		return error_handler();
	if (-1 == listen(serv->ls, 16))
		return error_handler();
	
	/*there may be more parameters, if we change struct server_stat*/
	serv->sess_array = malloc(SESS_ARR_SIZE * sizeof(*serv->sess_array));
	serv->sess_array_size = SESS_ARR_SIZE;
	for(i=0; i < SESS_ARR_SIZE; i++)
		serv->sess_array[i] = NULL;
	/*make the global bank*/
	bank_setup(serv, max_player);
	return 0;
}

void server_accept_client(struct server_stat *serv)
{
	int sd;
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	sd = accept(serv->ls, (struct sockaddr*) &addr, &len);
	if (sd >= SESS_ARR_SIZE)
	{
		printf("warning: count of ready to connect > SESS_ARR_SIZE\n");
		return;
	}
	/* DEBUG */ printf("SERVER: accept_client: %i\n", sd);
	if (sd == -1)
	{
		perror("accept");
		return;
	}
	serv->sess_array[sd] = session_make_new(sd, &addr);

	player_setup(sd);
	bank_check_count_player();
	bank_audit();
}

void server_session_close(struct server_stat *serv, int sd)
{
	/* DEBUG */ printf("SERVER: session_close: %i\n", sd);
	close(sd);
	serv->sess_array[sd]->fd = -1;
	session_cleanup(serv->sess_array[sd]);
	free(serv->sess_array[sd]);
	serv->sess_array[sd] = NULL;

	bank->player_count -= 1;
}

int server_run(struct server_stat *serv)
{
	fd_set readfds /*writefds*/;
	int i, ssr, maxfd;
	for(;;)
	{
		maxfd = serv->ls;
		FD_ZERO(&readfds);
		/*FD_ZERO(&writefds);*/
		FD_SET(maxfd, &readfds);
		for (i=0; i < serv->sess_array_size; i++)
		{
			if (serv->sess_array[i])
			{
				FD_SET(i, &readfds);
				if (i > maxfd)
					maxfd = i;
			}
		}

		if (-1 == select(maxfd+1, &readfds, NULL, NULL, NULL))
		{
			perror("select");
			return 1;
		}

		if (FD_ISSET(serv->ls, &readfds))
			server_accept_client(serv);
			/* make a struct for gamer via global bank */

		for (i=0; i < serv->sess_array_size; i++)
		{
			int old_global_count = global_count, j; /* for news */
			if (serv->sess_array[i] && FD_ISSET(i, &readfds))
			{
				printf("SERVER: call: session_read: %i\n", i);
				ssr = session_read(serv->sess_array[i]);
				if (old_global_count != global_count) /* for news */
					j = i;
				if (!ssr)
					server_session_close(serv, i);
			}
			/*send news*/
			if (old_global_count != global_count)
			{
				for (i=0; i < serv->sess_array_size; i++)
				{	
					if (serv->sess_array[i] && i != j)
					{
						session_send_string
							(serv->sess_array[i], 
								"Value have changed\n");	
						session_send_int
							(serv->sess_array[i],
								global_count);
					}
				}
			}
			
			/*sent news*/
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct server_stat serv;
	//struct bank_stat bank;
	int port, max_player;
	char *endptr;

	if (argc != 3) {
        printf("Usage: ./server <port> <number of players>\n");
        return 1;
    }

	port = strtol(argv[1], &endptr, 10);
	if (*endptr) {
		printf("error: wrong port\n");
		return 1;
	}

	max_player = strtol(argv[2], &endptr, 10);
	if (*endptr || max_player > SESS_ARR_SIZE) {
		printf("error: wrong number of players (max = %i)\n",
				SESS_ARR_SIZE);
		return 1;
	}
	
	/*there we alse have setup for bank*/
	if (server_setup(&serv, port, max_player)) 
		return 1;
	

	server_run(&serv);
	return 0;
}