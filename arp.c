/*
	name : arp.c
	author: renhuan.ding
	created date:2016.8.31
	finished date:?
*/



#include "arp.h"

extern int oldtime;
const char IP[] = "IP";
const char MAC[] = "MAC";
const char TYPE[] = "TYPE";
//see if the mac address valid
int isMacValid(const char *mac)
{
	if(strlen(mac) != 17)
	{
		return -1;
	}
	const char * checkmac = mac;
	int i = 0;
	while(*checkmac != '\0')
	{
		i++;
		if((i % 3) == 0 && *checkmac == ':')
			++checkmac;
		else if(('0' <= *checkmac && '9' >= *checkmac) | ('a'<= *checkmac && 'f'>= *checkmac))
			++checkmac;
		else
			return -1;	
	}
	return 0;
}

//find arp entry
struct arp_entry* find_arpd(char *ip, struct arp_entry* arp_root)
{
	//is ip address valid?
	struct in_addr in_addr_temp;
	if((in_addr_temp.s_addr = inet_addr(ip)) == INADDR_NONE)
	{
		printf("invalid IP address:%s\n", ip);
		return NULL;
	}
	
	//find the arp entry
	struct arp_entry* arp_temp;
	arp_temp = arp_root;
	while(arp_temp != NULL && arp_temp->ain_addr.s_addr != in_addr_temp.s_addr)
	{
		arp_temp = arp_temp->next;
	}
	
	//display the arp entry
	if(arp_temp != NULL)
	{
		char * ipTemp;
		ipTemp = inet_ntoa(arp_temp->ain_addr);	
		printf("%-30s%-30s%-30s\n", IP, MAC, TYPE);
		printf("%-30s%-30s%-30s\n", ipTemp, arp_temp->mac,ISSTATIC(arp_temp->isStatic));
	}
	else
	{
		printf("not find!\n");
	}

	return arp_temp;
}

// delete an arp entry from the list
struct arp_entry* delete_arpd(char * ip, struct arp_entry * arp_root, int* success)
{
	//is ip valid?
	struct in_addr in_addr_temp;
	if((in_addr_temp.s_addr = inet_addr(ip)) == INADDR_NONE)
	{
		printf("invalid IP address:%s\n", ip);
		return arp_root;
	}
	
	//find the arp entry and delete it	
	struct arp_entry* toDel;
	if(arp_root->ain_addr.s_addr == in_addr_temp.s_addr)
	{
		toDel = arp_root;
		arp_root = arp_root->next;
	}
	else
	{	
		struct arp_entry* arpTemp; 
		arpTemp = arp_root;
		while(arpTemp->next != NULL && arpTemp->next->ain_addr.s_addr != in_addr_temp.s_addr)
			arpTemp = arpTemp->next;
		if(arpTemp->next == NULL)
		{
			printf("not find!\n");	
			*success = 0;
			return arp_root;
		}
		else
		{
			toDel = arpTemp->next;
			arpTemp->next = arpTemp->next->next;
		}
	}
	if(!toDel->isStatic)
		timer_delete(toDel->timerid);
	free(toDel);
	*success = 1;
	return arp_root;
}

//add a arp entry to the list
struct arp_entry* add_arpd(const char* ip, const char* mac, int isStatic, struct arp_entry* arp_root, int* success)
{
	//is ip valid?
	
	struct in_addr in_addr_temp;
	if((in_addr_temp.s_addr = inet_addr(ip)) == INADDR_NONE)
	{
		printf("invalid IP address:%s\n", ip);
		*success = -1;
		return arp_root;
	}
	
	if(isMacValid(mac) < 0)
	{	
		printf("invalid MAC address:%s\n", mac);
		*success = -1;
		return arp_root;
	}
	
	//find if there already exist the arp entry
	struct arp_entry* arp_temp;
	arp_temp = arp_root;
	while(arp_temp != NULL && arp_temp->ain_addr.s_addr != in_addr_temp.s_addr)
	{
		arp_temp = arp_temp->next;
	}
	if(arp_temp != NULL)
	{
		printf("the entry alread exist!\n");
		arp_root = delete_arpd(inet_ntoa(arp_temp->ain_addr), arp_root, success);
		arp_root = add_arpd(ip, mac, isStatic, arp_root, success);
		*success = 0;
		return arp_root;
	}

	//assign new arpEntry
	struct arp_entry * arpEntry;
	arpEntry = malloc(sizeof(struct arp_entry));
	arpEntry->ain_addr = in_addr_temp;
	strcpy(arpEntry->mac, mac);
	arpEntry->isStatic = isStatic;
	if(isStatic == 0)
		arpEntry->timerid = set_alarm(oldtime);
	arpEntry->next = NULL;
		
	//add it to list
	if(arp_root == NULL)
		arp_root = arpEntry;
	else
	{
		struct arp_entry *arpTemp;
		arpTemp = arp_root;
		while(arpTemp->next != NULL)
			arpTemp = arpTemp->next;
		arpTemp->next = arpEntry;
	}
	
	*success = 0;
	return arp_root;
}

//dispaly all the arp table entry
void display_arpd(struct arp_entry* arp_root)
{
	char* ipTemp;
	printf("%-30s%-30s%-30s\n", IP, MAC, TYPE);
	while(arp_root != NULL)
	{
		ipTemp = inet_ntoa(arp_root->ain_addr);	
		printf("%-30s%-30s%-30s\n", ipTemp, arp_root->mac,ISSTATIC(arp_root->isStatic));
		arp_root = arp_root->next;
	}
}

timer_t set_alarm(unsigned int seconds)
{
	struct sigevent evp;
	struct itimerspec ts;
	timer_t timer;
	evp.sigev_value.sival_ptr = &timer;
	evp.sigev_notify = SIGEV_SIGNAL;
	evp.sigev_signo = SIGALRM;
	timer_create(CLOCK_REALTIME,&evp, &timer);
	
	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 0;
	ts.it_value.tv_sec = seconds;
	ts.it_value.tv_nsec = 0;

	timer_settime(timer, 0 , &ts, NULL);
	return timer;
	
}



void alrm_handler(int sig, siginfo_t * siginfo, void * unuse)
{
	struct arp_entry* arp_temp;
	arp_temp = arp_root;
	struct itimerspec gts;
	int success;
	while(arp_temp != NULL)
	{
		if(arp_temp->isStatic == 0)
		{
			timer_gettime(arp_temp->timerid, &gts);
			if(gts.it_value.tv_sec == 0 && gts.it_value.tv_nsec == 0)
				break;
		}
		arp_temp = arp_temp->next;
	}
	
	if(arp_temp != NULL)
	{
		arp_root = delete_arpd(inet_ntoa(arp_temp->ain_addr), arp_root, &success);	
	}
	else
	{
		printf("not find, exist some error!\n");
	}
}

