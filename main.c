#include "arp.h"

int oldtime = 30;

void signalset()
{	
	struct sigaction act;
	act.sa_sigaction = alrm_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	
	if(sigaction(SIGALRM, &act, NULL) < 0)
		printf("sigaction error!");
}

void commandHelp()
{
	printf("IP address is typed in dotted decimal!\n");
	printf("MAC address is typed like xx:xx:xx:xx:xx:xx\n");
	printf("static type 1, dynamic type 0\n\n");
	
	printf("add an arp entry: add IP MAC static/dynamic\n");
	printf("find an arp entry: find IP\n");
	printf("delete an arp entry: delete IP\n");
	printf("set old time: oldtime integer\n");
	printf("dispaly the arp table: display\n");
	printf("start a auto test: autotest\n");
	printf("quit the program: q\n\n");

	
}

void processCom(char * comm, FILE* fd)
{

	char inip[IP_LENGTH] = {0};
	char inmac[MAC_LENGTH] = {0};
	int innum = 0;
	int success = 0;
	char com[8] = {0};
	//fputs(comm, stdin);
	sscanf(comm,"%s",com);
	if(!strcmp(com,"add"))
	{
		sscanf(comm, "%*s%s%s%d", inip, inmac, &innum);
		printf("ipaddress is:%s\n", inip);
		//sscanf(comm, "%s", inmac);
		//sscanf(comm, "%d", &innum);
		
		arp_root = add_arpd(inip, inmac, innum,  arp_root, &success);
		if(success < 0)
			printf("add failed!\n");
		else
			printf("added an arp entry!\n");
	}
	else if(!strcmp(com,"find"))
	{
		sscanf(comm, "%*s%s", inip);
		find_arpd(inip, arp_root);
	}
	else if(!strcmp(com,"delete"))
	{
		sscanf(comm, "%*s%s", inip);
		arp_root = delete_arpd(inip, arp_root, &success);
		if(success == 1)
			printf("deleted the arp entry!\n");
		else
			printf("delete failed!\n");
	}
	else if(!strcmp(com,"oldtime"))
	{
		sscanf(comm, "%*s%d", &innum);
		oldtime = innum;
		printf("oldtime set success!\n");
	}
	else if(!strcmp(com, "display"))
	{
		display_arpd(arp_root);
	}
	else if(!strcmp(com, "q"))
	{
		exit(0);
	}
	else
	{
		printf("command input error:%s\n", comm);
	}		
}

void autotest()
{
	char comm[40] = {0};
	FILE* fd = fopen("test", "r");
	while(fgets(comm,40,fd) != NULL)
	{
		printf("%s",comm);
		processCom(comm, fd);
		printf("\n");
	}	
}


void main()
{
	signalset();
	commandHelp();
	while(1)
	{
		char comm[40] = {0};
		char com[8] = {0};
		printf(">> ");
		while(fgets(comm, 40, stdin) == NULL);
		sscanf(comm,"%s",com);
		if(!strcmp(com,"autotest"))
		{
			autotest();
		}
		else
		{
			processCom(comm, stdin);
		}
	}
}


