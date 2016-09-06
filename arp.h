#ifndef __ARP_H__
#define __ARP_H__

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define MAC_LENGTH 18
#define IP_LENGTH 16
#define ISSTATIC(sd) sd?"static":"dynamic"

struct arp_entry{
	struct in_addr ain_addr;
	char mac[MAC_LENGTH];
	int isStatic;
	timer_t timerid;
	struct arp_entry *next;
};
struct arp_entry* arp_root;
struct arp_entry* find_arpd(char * ip, struct arp_entry* arp_root);
struct arp_entry* delete_arpd(char * ip, struct arp_entry* arp_root, int* success);
struct arp_entry* add_arpd(const char* ip, const char* mac, int isStatic, struct arp_entry* arp_root, int *success);
int isMacValid(const char  *mac);
void display_arpd(struct arp_entry* arp_root);
void sig_alrm(int signo);
void alrm_handler(int sig, siginfo_t * siginfo, void * unuse);
timer_t set_alarm(unsigned int seconds);
#endif
