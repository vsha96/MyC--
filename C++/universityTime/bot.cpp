#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

class Player {
		int number;
		int money;
		int material;
		int product;
		int factory; 	
	public:
		void SetNum(int n) { number = n; }
		void SetMon(int m) { money = m; }
		void SetMat(int m) { material = m; }
		void SetProd(int p) { product = p; }
		void SetFac(int f) { factory = f; }
	
};

class Game {
		struct list_player {
			class Player pl;
			class Player *next;
		};
		struct list_player *list;
	public:
		Game() { list = NULL; };
		~Game() {}; //TODO
};

//remake
char cmdt[] = "test\n";
char cmd1[] = "me\n";
char cmd2[] = "player\n";
char cmd3[] = "market\n";
char cmd4[] = "build";
char cmd5[] = "prod";
char cmd6[] = "buy";
char cmd7[] = "sell";
char cmde[] = "turn\n"; 

class Bot: public Player {
		int sd;
	public:
		Bot() {
			// link with Game object
			// dont forget about destructor
			SetNum(-1);
			SetMon(10000);
			SetMat(4);
			SetProd(2);
			SetFac(2);
		}
		bool BotConnect(char *address, char *str_port);
		void ShowSD() { printf("my sd = [%i]\n", sd); }
		void Say(char *string);
		char *Listen();
};

//put this shit in module!
bool Bot::BotConnect(char *address, char *str_port)
{
	struct sockaddr_in addr;
	int port;
	char *endptr;
	port = strtol(str_port, &endptr, 10);
    if (*endptr) {
        printf("error: wrong port\n");
        return false;
    }
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (!inet_aton(address, &(addr.sin_addr))) {
		printf("error: wrong address\n");
        return false;
	}
	printf("addr[%s] port[%i]\n", address, port);
	//connection
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("error: socket can't be open\n");
        return false;
	}
	printf("sd[%i]\n", sd);
	if (-1 == connect(sd, (sockaddr*) &addr, sizeof(addr))) {
		printf("error: unable to connect\n");
		return false;
	}
	/*
	int flags = fcntl(sd, F_GETFL);
	fcntl(sd, F_SETFL, flags | O_NONBLOCK);
	*/
	printf("###connection is established!\n");

	// send recv?
	//this is bulshit
	/*
	int c = '1', rc;
	while (c != EOF) {
		while(read(sd, &rc, 1)) {
			printf("%c", rc);
		}
		while((c = getchar()) != '\n') {
			if (c == EOF)
				break;
			write(sd, &c, sizeof(c));
		}
	}
	*/

	return true;
}

void Bot::Say(char *s)
{
    	int i;
    	for (i=0; s[i]; i++);
    	write(sd, s, i);
		printf("### I said: %s", s);
}

char *Bot::Listen()
{
	// same thing in serv with memmov and memcopy
	// use buf
}

int main(int argc, char **argv)
{
	Bot robbie;
	char *address, *str_port;

	if (argc != 3) {
		printf("usage: ./bot <address> <port>\n");
		return 1;
	}
	
	address = argv[1];
	str_port = argv[2];

	if (!robbie.BotConnect(address, str_port)) {
		printf("err: unable to connect\n");
	}

	/*AND NOW WE'RE TALKING*/
	robbie.ShowSD();
	robbie.Say(cmd1);
	robbie.Say("fuck off");
	robbie.Say("fuck off\n");
	robbie.Say(cmde);
	
	for(;;) { sleep(1); }
}
