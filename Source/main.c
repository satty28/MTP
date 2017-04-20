/* 
 *	Main.c
 *	
 *  
 *  Created on: Sep 21, 2015
 *  Author: Pranav Sai(pk6420@rit.edu)
 *
 *  Updated At : Sep 10 2015
 *  Updated: Rajesh Shetty(rss2158@g.rit.edu)
 */

#include <stdio.h>

#include <sys/time.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/if_ether.h>
#include <time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <ifaddrs.h>
#include <net/ethernet.h>
#include <signal.h>
#include <ctype.h>
#include <signal.h>
// NS added this last line to catch and gracefully end on Ctrl C

#include "feature_payload.h"
#include "mtp_send.h"

#define ETH_MTP_CTRL    0x8850
#define MAX_VID_LIST    20
#define CTRL_IP		"172"
#define BUFSIZE		1000
/* Function Prototypes */
void mtp_start();
int getActiveInterfaces(char **);
void learn_active_interfaces();
bool checkInterfaceIsActive(char *);
// *** added by NS
void sig_handler(int signo); 
// *** End addition by NS


/* Globals */
bool isRoot = false;
bool isSecondaryRoot = false;
bool startLoggingCreation = false;
char sysname[1024];

int rootPriority = 0; //Indicates the root priority of the application ( 1 => Primary, 2 => Secondary)
struct interface_tracker_t *interfaceTracker = NULL;
struct timeval currentTime;
struct timeval tcStart;
struct timeval rootCStart;

/* Entry point to the program */
int main (int argc, char** argv) {	
	char **interfaceNames;
/*temp */	
	int i = 0;
        interfaceNames = (char**) calloc (MAX_INTERFACES*MAX_INTERFACES, sizeof(char));
        memset(interfaceNames, '\0', sizeof(char) * MAX_INTERFACES * MAX_INTERFACES);
        int numberOfInterfaces = getActiveInterfaces(interfaceNames);
/*temp */
	printf ("printing the no of arguments: %d\n", argc);
	// Check number of Arguments.
	if (argc < 4) {
        printf("Usage: sudo bin/mtpd <0/1> <1/2> <X> VID Value\n");
		printf("Error: Node spec or ROOT MTS ID missing. Format ./main <non MTS/root MTS> <ROOT MTS ID>\n");
		printf("Error: 0 for non MTS, 1 for root MTS\n");
        printf("Error: 1 for Primary Root MTS, 2 for secondary root MTS\n");
		exit(1);
	}
	// Check if Node is Root MTS or Non MTS
	if (atoi(argv[1]) >= 1) {
		isRoot = true;
		printf("This node is root MTS\n");
        if (atoi(argv[2]) == 1) {
            printf("Pirmary Root for the application\n");
            rootPriority = 1;
        }else{
            printf("Secondary Root for the application\n");
			isSecondaryRoot = true;
            rootPriority = 1;
        }
	}
	else printf("This node is a non-root MTS\n");
    /* Opening the file for writing the code */
    //openLogsFile();
/* block to get port numbers from a file 
        int a = 0;
        int index;        
        //int vlanport[24];
        int *vlanport;
	//char vlanport[24];
        vlanport = malloc(sizeof(int));
	//vlanport = malloc(sizeof(char));
        FILE *fp = fopen(argv[4], "r");
        char buff[BUFSIZE];
        while(fgets(buff, BUFSIZE - 1, fp) != NULL)
        {
                printf ("%s\n", buff);
                vlanport[a] = atoi(buff);
		//vlanport[a] = buff;
                a++;
                
        }
        fclose(fp);
        for(index = 0; index < (sizeof(vlanport) / sizeof(vlanport[0])); index++) {
        	printf ("printing the first port from array: %d\n", vlanport[index]);
		//puts("\nprinting the first port from array:");
		//puts(vlanport[index]);
        }

 end of block to get port numbers from a file */
	if (argc == 5) {
/* Reading char from file and storing it array */
    	int a=0;
    	int numProgs=0;
//	int i=0;
    	char* vlanport[50];
    	char line[50];
	int j;
    	int numberofVports;
	FILE *file;
    	file = fopen(argv[4], "r");

    	while(fgets(line, sizeof line, file)!=NULL) {
        //check to be sure reading correctly
       	printf("%s", line);
        //add each filename into array of programs
        vlanport[a]=strdup(line); 
        a++;
        //count number of programs in file
        numProgs++;
    	}

    	fclose(file);
    	//check to be sure going into array correctly 
    	for (j=0 ; j<numProgs; j++) {
        printf("\nPrinting Port from File: %s", vlanport[j]);
	numberofVports = j+1;
    	}
	
	printf ("No. of VLANports: %d\n", numberofVports); 
	

/* End of Reading char from file and storing it array */
	printf("\nstep1");
	// Populate local host broadcast table, intially we mark all ports as host ports, if we get a MTP CTRL frame from any port we remove it.
	
        for (; i < numberofVports; i++) {
                // Allocate memory and intialize(calloc).
                struct local_bcast_tuple *new_node = (struct local_bcast_tuple*) calloc (1, sizeof(struct local_bcast_tuple));

                // Fill
                strncpy(new_node->eth_name, vlanport[i], strlen(vlanport[i]));
        puts("Printing eth_name of interfaces");
        puts(new_node->eth_name);
                new_node->next = NULL;
                add_entry_lbcast_LL(new_node); //returns bool but is not catched here

        }
	}
	else if (argc == 4){
        printf("\nstep1");
        // Populate local host broadcast table, intially we mark all ports as host ports, if we get a MTP CTRL frame from any port we remove it.
/*temp block */        
	interfaceNames = (char**) calloc (MAX_INTERFACES*MAX_INTERFACES, sizeof(char));
        memset(interfaceNames, '\0', sizeof(char) * MAX_INTERFACES * MAX_INTERFACES);
        int numberOfInterfaces = getActiveInterfaces(interfaceNames);
/*temp block*/
        printf("\nNumber of Interfaces: %d\n", numberOfInterfaces);
        gethostname(sysname, 1024);


	//int i = 0;
	for (; i < numberOfInterfaces; i++) {
		// Allocate memory and intialize(calloc).
		struct local_bcast_tuple *new_node = (struct local_bcast_tuple*) calloc (1, sizeof(struct local_bcast_tuple));

		// Fill
		strncpy(new_node->eth_name, interfaceNames[i], strlen(interfaceNames[i]));
        puts("Printing eth_name of interfaces");
        puts(new_node->eth_name);
		new_node->next = NULL; 
		add_entry_lbcast_LL(new_node); //returns bool but is not catched here

	}
	}

	// If Node is Root MTS
	if (isRoot) {
		// Check if Root VID is provided through CLI.
		if (argv[2] != NULL) {
			printf ("ROOT MTVID: %s\n", argv[2]);

			// Allocate memory and intialize(calloc).
			struct vid_addr_tuple *new_node = (struct vid_addr_tuple*) calloc (1, sizeof(struct vid_addr_tuple));

			// Fill data.
			strncpy(new_node->vid_addr, argv[2], strlen(argv[2]));
			strcpy(new_node->eth_name, "self");   	// own interface, so mark it as self, will be helpful while tracking own VIDs.
			new_node->last_updated = -1; 		        // -1 here because root ID should not be removed.
			new_node->port_status = PVID_PORT; 
			new_node->next = NULL;
			new_node->isNew = true;
			new_node->path_cost = PATH_COST;

			// Add into VID Table.s
			add_entry_LL(new_node); //add the values in the node based upon the Root VID

			i = 0;
			uint8_t *payload = NULL;
			uint8_t payloadLen;

			for (; i < numberOfInterfaces; i++) {
				payload = (uint8_t*) calloc (1, MAX_BUFFER_SIZE);
				payloadLen = build_VID_ADVT_PAYLOAD(payload, interfaceNames[i]);
                //puts("Length of the Payload");
                printf(" %d ", payloadLen);
				if (payloadLen) {
					ctrlSend(interfaceNames[i], payload, payloadLen);
				}
				free(payload);
			}	
		} else {
			printf ("Error: Provide ROOT Switch ID ./main <non MTS/root MTS> <ROOT MTS ID>\n");
			exit(1);
		}
	}
	free(interfaceNames);

	// learn all interfaces avaliable.
	learn_active_interfaces();

	// Start
	mtp_start();

	return 0;
}

/* Start MTP Protocol. */
void mtp_start() {
	int sockCtrl = 0, sockData = 0, recv_len = 0;
	uint8_t recvBuffer[MAX_BUFFER_SIZE];
	struct ether_header *eheader = NULL;
	struct sockaddr_ll src_addr;
	char **interfaceNames, **deletedVIDs;

	// time_t, timers for checking hello time.
	time_t time_advt_beg;
	time_t time_advt_fin;
    //time snapshots for recording convergence;
    //time convergence start
    time_t time_conv_start;
    time_t time_conv_end;

	// clear the memory
	interfaceNames = (char**) calloc (MAX_INTERFACES* MAX_INTERFACES, sizeof(char));
	deletedVIDs = (char**) calloc (MAX_VID_LIST * MAX_VID_LIST, sizeof(char));

	// Create Socket, ETH_MTP_CTRL is used because we are listening packets of all kinds.
	if ((sockCtrl = socket(AF_PACKET, SOCK_RAW, htons (ETH_MTP_CTRL))) < 0) {
		perror("Error: MTP socket()");
		exit(1);
	}

	// Create Socket, ETH_ is used because we are listening packets of all kinds.
	if ((sockData = socket(AF_PACKET, SOCK_RAW, htons (ETH_P_ARP))) < 0) {
		perror("Error: MTP socket()");
		exit(1);
	}
//** insert by NS	
	if (signal (SIGINT, sig_handler)== SIG_ERR)
		printf("\nCant Catch SIGINT");
	
//** End insert by NS

	time(&time_advt_beg);
	while (true) {
		time(&time_advt_fin);
		// Send Hello Periodic, only if have atleast One VID in Main VID Table.
		if ((double)(difftime(time_advt_fin, time_advt_beg) >= PERIODIC_HELLO_TIME)) {
			memset(interfaceNames, '\0', sizeof(char) * MAX_INTERFACES * MAX_INTERFACES);
			int numberOfInterfaces = getActiveInterfaces(interfaceNames);

			uint8_t *payload = NULL;
			int payloadLen = 0;

			payload = (uint8_t*) calloc (1, MAX_BUFFER_SIZE);

			// send JOIN MSG if there are no VID entries in the Main VID Table.
			if (isMain_VID_Table_Empty() && isSec_VID_Table_Empty()) {
				payloadLen = build_JOIN_MSG_PAYLOAD(payload);
			} else {
				// send if entries already present in Main VID Table.
				payloadLen = build_PERIODIC_MSG_PAYLOAD(payload);
			}

			if (payloadLen) {
				int i = 0;
				for (; i < numberOfInterfaces; ++i) {			
					ctrlSend(interfaceNames[i], payload, payloadLen);
				}
			}
			free(payload);

			memset(deletedVIDs, '\0', sizeof(char) * MAX_VID_LIST * MAX_VID_LIST);

			// check for failures and delete if any VID exceeds periodic hello by (PERIODIC_HELLO_TIME * 3)
			int numberOfDeletions = checkForFailures(deletedVIDs);

			bool hasCPVIDDeletions = checkForFailuresCPVID();

			if ( numberOfDeletions != 0) {

				int i = 0;
				for (; i < numberOfInterfaces; i++) {
					payload = (uint8_t*) calloc (1, MAX_BUFFER_SIZE);

					payloadLen = build_VID_CHANGE_PAYLOAD(payload, interfaceNames[i], deletedVIDs, numberOfDeletions); 
					if (payloadLen) {
						ctrlSend(interfaceNames[i], payload, payloadLen);
					}  
					free(payload);
				} 

				// Also check CPVID Table.
				i = 0;
				for (; i < numberOfDeletions; i++) {
					printf("\n\nCalling delete entry cpvid %d ",numberOfDeletions);
					delete_entry_cpvid_LL(deletedVIDs[i]);  
				}


				struct vid_addr_tuple* c1 =  getInstance_vid_tbl_LL();
				if (c1 != NULL) {
					payload = (uint8_t*) calloc (1, MAX_BUFFER_SIZE);
					payloadLen = build_VID_ADVT_PAYLOAD(payload, c1->eth_name);
					if (payloadLen) {
						ctrlSend(c1->eth_name, payload, payloadLen);
					}
					free(payload);
				}
			}

			// print all tables.
			if ((hasCPVIDDeletions == true) || (numberOfDeletions > 0)) {
				print_entries_LL();                     // MAIN VID TABLE
                print_entries_sec_LL();                 // SECONDARY VID TABLE
				print_entries_bkp_LL();                 // ROOT1 BKP VID TABLE
                print_entries_sec_bkp_LL();             // ROOT2  VID TABLE
				print_entries_cpvid_LL();               // CHILD PVID TABLE
				print_entries_lbcast_LL();              // LOCAL HOST PORTS
                gettimeofday(&rootCStart,0);
                if(isMain_VID_Table_Empty()){
                    //Check if the current MTS is the Secondary root.
                    if(isSecondaryRoot){
                        //Log the time indicating start of Root convergence.
                        float diff;
                        struct timeval curr;
                        gettimeofday(&curr,0);
                        diff = (curr.tv_sec - tcStart.tv_sec)*1000.0f + (curr.tv_usec - tcStart.tv_usec) / 1000.0f;
                        time_t currTime;
                        char timeString[26];
                        struct tm* currTimeDetailed;
                        time(&currTime);
                        currTimeDetailed = localtime(&currTime);
                        strftime(timeString, 26, "%Y-%m-%d %H:%M:%S", currTimeDetailed);
                        printf("Root Convergence Started");
                        printf("%s , %s, %f ",sysname,timeString,diff);
                        printf("\n");
                    }
                    float diff;
                    struct timeval currRoot;
                    gettimeofday(&currRoot,0);
                    diff = (currRoot.tv_sec - rootCStart.tv_sec)*1000.0f + (currRoot.tv_usec - rootCStart.tv_usec) / 1000.0f;
                    time_t currTime;
                    char timeString[26];
                    struct tm* currTimeDetailed;
                    time(&currTime);
                    currTimeDetailed = localtime(&currTime);
                    strftime(timeString, 26, "%Y-%m-%d %H:%M:%S", currTimeDetailed);
                    printf("Performing Root-Convergence");
                    printf("%s , %s, %f ",sysname,timeString,diff);
                    printf("\n");
                    //Mark the start of Root Convergence
                    //performRootSwitch();
                    //performRootSwitch();
                    time(&time_conv_end);
                }

			}
			// reset time.
			time(&time_advt_beg);
		} 

		socklen_t addr_len = sizeof(src_addr);

		recv_len = recvfrom(sockCtrl, recvBuffer, MAX_BUFFER_SIZE, MSG_DONTWAIT, (struct sockaddr*) &src_addr, &addr_len);
		if (recv_len > 0) {

			char recvOnEtherPort[5];
			// read ethernet header
			eheader = (struct ether_header*)recvBuffer;
			//printf("Source MAC: %s\n", ether_ntoa((struct ether_addr *) &eheader->ether_shost));
			//printf("Destination MAC: %s\n", ether_ntoa((struct ether_addr *)&eheader->ether_dhost));

			if_indextoname(src_addr.sll_ifindex, recvOnEtherPort);

			char ctrlInterface[] = "eth0";

			// ignore frames that are from the control interface.
			if ((strcmp(recvOnEtherPort, ctrlInterface)) == 0) {
				continue;
			} else {
				// This is a MTP frame so, incase this port is in Local host broadcast table remove it.
				delete_entry_lbcast_LL(recvOnEtherPort); 
			}

            //Start convergence time.


			switch ( recvBuffer[14] ) {
				case MTP_TYPE_JOIN_MSG:
					{

                        uint8_t *payload = NULL;
						int payloadLen = 0;

						payload = (uint8_t*) calloc (1, MAX_BUFFER_SIZE);

						// recvOnEtherPort - Payload destination will same from where Join message has orginated.
						payloadLen = build_VID_ADVT_PAYLOAD(payload, recvOnEtherPort);

						if (payloadLen) {
							ctrlSend(recvOnEtherPort, payload, payloadLen);
						}

						free(payload);
						// Send VID Advt
					}
					break;
				case MTP_TYPE_PERODIC_MSG:
					{


                        // printf ("MTP_TYPE_PERODIC_MSG\n");
						// Record MAC ADDRESS, if not already present.

						struct ether_addr src_mac;
						bool retMainVID, retCPVID; 

						memcpy(&src_mac, (struct ether_addr *)&eheader->ether_shost, sizeof(struct ether_addr));
						retMainVID = update_hello_time_LL(&src_mac);
						retCPVID = update_hello_time_cpvid_LL(&src_mac);

						if ( (retMainVID == true) || (retCPVID == true) ) {

						} else {
							if (!isRoot) {
								uint8_t *payload = NULL;
								int payloadLen = 0;
								payload = (uint8_t*) calloc (1, MAX_BUFFER_SIZE);
								payloadLen = build_JOIN_MSG_PAYLOAD(payload);
								if (payloadLen) {
									ctrlSend(recvOnEtherPort, payload, payloadLen);
								}
								free(payload);
							}
						}
					}
					break;
				case MTP_TYPE_VID_ADVT:
					{


						//printf ("MTP_TYPE_VID_ADVT\n");
						// Got VID Advt, check relationship, if child add to Child PVID Table.
						// Number of VIDs
						// Message ordering <MSG_TYPE> <OPERATION> <NUMBER_VIDS>  <PATH COST> <VID_ADDR_LEN> <MAIN_TABLE_VID + EGRESS PORT>
						uint8_t operation = (uint8_t) recvBuffer[15];


                        if(!startLoggingCreation){
                            gettimeofday(&tcStart ,0);
                            float diff;
                            struct timeval curr;
                            gettimeofday(&curr,0);
                            diff = (curr.tv_sec - tcStart.tv_sec)*1000.0f + (curr.tv_usec - tcStart.tv_usec) / 1000.0f;
                            time_t currTime;
                            char timeString[26];
                            struct tm* currTimeDetailed;
                            time(&currTime);
                            currTimeDetailed = localtime(&currTime);
                            strftime(timeString, 26, "%Y-%m-%d %H:%M:%S", currTimeDetailed);
                            printf("Initializing Start Time %s,%s,%f",sysname,timeString,diff);
                            startLoggingCreation = true;

                        }

						if (operation == VID_ADD) { 
							uint8_t numberVIDS = (uint8_t) recvBuffer[16];
							//printf ("numberVIDS %u\n", numberVIDS);
							int tracker = 17;		
							bool hasAdditions = false;
							while (numberVIDS != 0) {
								uint8_t path_cost = (uint8_t)recvBuffer[tracker];
								//printf("Path Cost: %u\n", recvBuffer[tracker]);

								// next byte
								tracker = tracker + 1;

								// <VID_ADDR_LEN>
								uint8_t vid_len = recvBuffer[tracker];

								// next byte 
								tracker = tracker + 1;

								char vid_addr[vid_len];

								memset(vid_addr, '\0', vid_len);
								strncpy(vid_addr, &recvBuffer[tracker], vid_len);
								vid_addr[vid_len] = '\0';
								tracker += vid_len;

								int ret = isChild(vid_addr);
                                printf("Return value %d for vid: %s", ret,vid_addr);
								// if VID child ignore, incase part of PVID add to Child PVID table. 
								if ( ret == 1) {
									// if this is the first VID in the table and is a child, we have to add into child PVID Table

									printf("numberVIDS %d recvBuffer %d ", numberVIDS, recvBuffer[16] );
									if (numberVIDS == (uint8_t) recvBuffer[16] ) { // if same first ID
										struct child_pvid_tuple *new_cpvid = (struct child_pvid_tuple *) calloc(1,
																												sizeof(struct child_pvid_tuple));
										// Fill data.
										strncpy(new_cpvid->vid_addr, vid_addr, strlen(vid_addr));
										strncpy(new_cpvid->child_port, recvOnEtherPort, strlen(recvOnEtherPort));
										memcpy(&new_cpvid->mac, (struct ether_addr *) &eheader->ether_shost,
											   sizeof(struct ether_addr));
										new_cpvid->next = NULL;
										new_cpvid->last_updated = time(0);        // last updated time

										// Add into child PVID table, if already there update it if any changes.
										printf("\nAdding node node %s\n", new_cpvid->vid_addr);
										if (add_entry_cpvid_LL(new_cpvid)) {
											printf("\nAddded node %s\n", new_cpvid->vid_addr);
										} else { // if already there deallocate node memory
											free(new_cpvid);
										}
									}

									//Update the childs to Secondary Root CPVID.
									else{
										//if(isSecondaryRoot){
											printf("\n Starting to Add :  %s\n\n", vid_addr);
											struct child_pvid_tuple *new_cpvid = (struct child_pvid_tuple*) calloc (1, sizeof(struct child_pvid_tuple));
											// Fill data.
											strncpy(new_cpvid->vid_addr, vid_addr, strlen(vid_addr));
											strncpy(new_cpvid->child_port, recvOnEtherPort, strlen(recvOnEtherPort));
											memcpy(&new_cpvid->mac, (struct ether_addr *)&eheader->ether_shost, sizeof(struct ether_addr));
											new_cpvid->next = NULL;
											new_cpvid->last_updated = time(0);        // last updated time
											char *vid_addr_temp = new_cpvid->vid_addr;
											printf("\n vid_add_temp :  %s\n\n", vid_addr_temp);
											printf("\n len :  %zu\n\n", strlen(vid_addr_temp));

											//Get the PVID of the Secondary Table
											int pVID_len = getSecPVIDLen();
											printf("\n len :  %d\n\n", pVID_len);
											if(vid_addr_temp[0] == '2' && strlen(vid_addr_temp)==pVID_len){
												printf("\n adding from secondary root:  %s\n\n", vid_addr_temp);
												if (add_entry_sec_cpvid_LL(new_cpvid)) {
													printf("\nAddded node %s\n",new_cpvid->vid_addr);
												} else { // if already there deallocate node memory
													free(new_cpvid);
												}
 											}
										//}
									}
								} else if ( ret == -1) { 
									// Add to Main VID Table, if not a child, make it PVID if there is no better path already in the table.

									// Allocate memory and intialize(calloc).
									struct vid_addr_tuple *new_node = (struct vid_addr_tuple*) calloc (1, sizeof(struct vid_addr_tuple));

									// Fill data.
									strncpy(new_node->vid_addr, vid_addr, strlen(vid_addr));
									strncpy(new_node->eth_name, recvOnEtherPort, strlen(recvOnEtherPort));
									new_node->last_updated = time(0); // current timestamp 
									new_node->port_status = PVID_PORT;
									new_node->next = NULL;
									new_node->isNew = true;
									new_node->membership = 0;	 // Intialize with '0', will find outpreference based on cost during add method.
									new_node->path_cost = (uint8_t) path_cost;
									memcpy(&new_node->mac, (struct ether_addr *)&eheader->ether_shost, sizeof(struct ether_addr));

									int mainVIDTracker = add_entry_LL(new_node);
									// Add into VID Table, if addition success, update all other connected peers about the change.
									if (mainVIDTracker > 0) {
										if (mainVIDTracker <= 3) {
											hasAdditions = true;
										}
										// If peer has VID derived from me earlier and has a change now.
										if (numberVIDS == (uint8_t) recvBuffer[16]) { // if same first ID
											// Check PVID used by peer is a derived PVID from me.
											delete_MACentry_cpvid_LL(&new_node->mac); 
										} 
									}
								} else {
									// Dont do anything, may be a parent vid or duplicate
								}
								numberVIDS--;
							}
							
							if (hasAdditions) {
								memset(interfaceNames, '\0', sizeof(char) * MAX_INTERFACES * MAX_INTERFACES);
								int numberOfInterfaces = getActiveInterfaces(interfaceNames);

								uint8_t *payload = NULL;
								int payloadLen = 0;

								payload = (uint8_t*) calloc (1, MAX_BUFFER_SIZE);

								int i = 0;
								for (; i < numberOfInterfaces; ++i) {
									// recvOnEtherPort - Payload destination will be same from where Join message has orginated.
									payloadLen = build_VID_ADVT_PAYLOAD(payload, interfaceNames[i]);
									if (payloadLen) {
										ctrlSend(interfaceNames[i], payload, payloadLen);
									}
								}
								free(payload);
							}
						} else if (operation == VID_DEL){
							//printf ("GOT VID_DEL\n");
							// Message ordering <MSG_TYPE> <OPERATION> <NUMBER_VIDS> <VID_ADDR_LEN> <MAIN_TABLE_VID + EGRESS PORT>
                            //Initialize the convergence start to

                            time(&time_conv_start);
							uint8_t numberVIDS = (uint8_t) recvBuffer[16];

							// delete all local entries, get a list and send to others who derive from this VID. 
							memset(deletedVIDs, '\0', sizeof(char) * MAX_VID_LIST * MAX_VID_LIST);

							uint8_t numberOfDeletions = numberVIDS;
							bool hasDeletions = false;

							int i = 0;
							int tracker = 17;
							while (i < numberOfDeletions) {
								//<VID_ADDR_LEN>
								uint8_t vid_len = recvBuffer[tracker];

								// next byte, make tracker point to VID_ADDR
								tracker = tracker + 1;

								deletedVIDs[i] = (char*)calloc(vid_len, sizeof(char));
								strncpy(deletedVIDs[i], &recvBuffer[tracker], vid_len);
								recvBuffer[vid_len] = '\0';
								hasDeletions = delete_entry_LL(deletedVIDs[i]);
								delete_entry_cpvid_LL(deletedVIDs[i]);
								tracker += vid_len; 
								i++;
							} 

							uint8_t *payload;
							int payloadLen; 
							// Only if we have deletions we will be advertising it to our connected peers.
							if (hasDeletions) {
								memset(interfaceNames, '\0', sizeof(char) * MAX_INTERFACES * MAX_INTERFACES);
								int numberOfInterfaces = getActiveInterfaces(interfaceNames);

								i = 0;
								for (; i < numberOfInterfaces; i++) {
									payload = (uint8_t*) calloc (1, MAX_BUFFER_SIZE);

									payloadLen = build_VID_CHANGE_PAYLOAD(payload, interfaceNames[i], deletedVIDs, numberOfDeletions);
									if (payloadLen) {
										ctrlSend(interfaceNames[i], payload, payloadLen);
									}
									free(payload);              
								}

								payload = (uint8_t*) calloc (1, MAX_BUFFER_SIZE);

								struct vid_addr_tuple* c1 =  getInstance_vid_tbl_LL();
								if (c1 != NULL) {
									payloadLen = build_VID_ADVT_PAYLOAD(payload, c1->eth_name);
									if (payloadLen) {
										ctrlSend(c1->eth_name, payload, payloadLen);
										//printf("Sending %s\n", c1->vid_addr);
									}
									free(payload);
								}
								///If main VID Table is EMPTY then perform ROOT SWITCH
                                gettimeofday(&rootCStart,0);
								if(isMain_VID_Table_Empty()){
                                    //Check if the current MTS is the Secondary root.
                                    if(isSecondaryRoot){
                                        //Log the time indicating start of Root convergence.
                                        float diff;
                                        struct timeval curr;
                                        gettimeofday(&curr,0);
                                        diff = (curr.tv_sec - tcStart.tv_sec)*1000.0f + (curr.tv_usec - tcStart.tv_usec) / 1000.0f;
                                        time_t currTime;
                                        char timeString[26];
                                        struct tm* currTimeDetailed;
                                        time(&currTime);
                                        currTimeDetailed = localtime(&currTime);
                                        strftime(timeString, 26, "%Y-%m-%d %H:%M:%S", currTimeDetailed);
                                        printf("Root Convergence Started");
                                        printf("%s , %s, %f ",sysname,timeString,diff);
                                        printf("\n");
                                    }
                                    float diff;
                                    struct timeval currRoot;
                                    gettimeofday(&currRoot,0);
                                    diff = (currRoot.tv_sec - rootCStart.tv_sec)*1000.0f + (currRoot.tv_usec - rootCStart.tv_usec) / 1000.0f;
                                    time_t currTime;
                                    char timeString[26];
                                    struct tm* currTimeDetailed;
                                    time(&currTime);
                                    currTimeDetailed = localtime(&currTime);
                                    strftime(timeString, 26, "%Y-%m-%d %H:%M:%S", currTimeDetailed);
                                    printf("Performing Root-Convergence");
                                    printf("%s , %s, %f ",sysname,timeString,diff);
                                    printf("\n");
                                    //Mark the start of Root Convergence
                                    //performRootSwitch();
                                    performRootSwitch();
                                    time(&time_conv_end);
								}
							}

						} else {
							printf("Unknown VID Advertisment\n");
						}

                        float diff;
                        struct timeval curr;
                        gettimeofday(&curr,0);
                        diff = (curr.tv_sec - tcStart.tv_sec)*1000.0f + (curr.tv_usec - tcStart.tv_usec) / 1000.0f;
                        //gethostname(sysname, 1024);

						print_entries_LL();
                        print_entries_sec_LL();                 //SECONDARY VID TABLE
						print_entries_bkp_LL();
                        print_entries_sec_bkp_LL();
                        print_entries_cpvid_LL();
						print_entries_sec_cpvid_LL();
						print_entries_lbcast_LL();

                        time_t currTime;
                        char timeString[26];
                        struct tm* currTimeDetailed;
                        time(&currTime);
                        currTimeDetailed = localtime(&currTime);
                        strftime(timeString, 26, "%Y-%m-%d %H:%M:%S", currTimeDetailed);
                        printf("Logging the results");
                        printf("%s , %s, %f ",sysname,timeString,diff);
                        printf("\n");

					} 
					break;
				default:
					printf("Unknown Packet\n");
					break;	
			}
		}

		/* Receive data traffic */
		recv_len = recvfrom(sockData, recvBuffer, MAX_BUFFER_SIZE, MSG_DONTWAIT, (struct sockaddr*) &src_addr, &addr_len);
		if (recv_len > 0) {
			char recvOnEtherPort[5];

			if_indextoname(src_addr.sll_ifindex, recvOnEtherPort);
			char ctrlInterface[] = "eth0";

			// ignore frames that are from the control interface.
			if ((strcmp(recvOnEtherPort, ctrlInterface)) == 0) {
				continue;
			}

			// read ethernet header
			eheader = (struct ether_header*)recvBuffer;

			// read ethernet header
			/*printf("Source MAC: %s\n", ether_ntoa((struct ether_addr *) &eheader->ether_shost));
			  printf("Destination MAC: %s\n", ether_ntoa((struct ether_addr *)&eheader->ether_dhost));
			  printf("Message Type: %x\n", ntohs(eheader->ether_type));*/

			// Check if the data frame is a broadcast.
			if (strncmp(ether_ntoa((struct ether_addr *)&eheader->ether_dhost), "ff:ff:ff:ff:ff:ff", 17) == 0) {
				// if the frame is a broadcast frame.
				printf("Received broadcast frame\n");

				// Send it to all host ports, first.
				struct local_bcast_tuple* current =  getInstance_lbcast_LL();

				for (; current != NULL; current = current->next) {
					// port should not be the same from where it received frame.
					if (strcmp(current->eth_name, recvOnEtherPort) != 0) {
						dataSend(current->eth_name, recvBuffer, recv_len);
						printf("Sent to host %s\n", current->eth_name);
					}
				}

				//If Root has switched From Root1 to Root2 Send to all
				//ports on the Secondary Root Child PVID table
				struct child_pvid_tuple* cpt;
				if(checkRootSwitch()){
					cpt = getInstance_sec_cpvid_LL();
				}else{
					cpt = getInstance_cpvid_LL();
				}

				// Next, Send to all ports on Child PVID Table.
				//struct child_pvid_tuple* cpt = getInstance_cpvid_LL();

				for (; cpt != NULL; cpt = cpt->next) {
					// port should not be the same from where it received frame.
					if (strcmp(cpt->child_port, recvOnEtherPort) != 0) {
						dataSend(cpt->child_port, recvBuffer, recv_len);
						printf("Sent to child %s\n", cpt->child_port);
					}
				}         

				// Next Send it port from where current PVID is acquired, if it is not same as the received port.
				if (!isRoot) {
					struct vid_addr_tuple* vid_t = getInstance_vid_tbl_LL(); 
					if (strcmp(vid_t->eth_name, recvOnEtherPort) != 0) {
						dataSend(vid_t->eth_name, recvBuffer, recv_len);
						printf("Sent to PVID%s\n", vid_t->eth_name);
					} 
				}
				//print_entries_cpvid_LL();
			} 

		}
		// check if there are any pending VID Adverts
	} // end of while
}

// get active interfaces on the node.
int getActiveInterfaces(char **ptr ) {
	// find all interfaces on the node.
	int indexLen = 0;
	struct ifaddrs *ifaddr, *ifa;

	if (getifaddrs(&ifaddr) ) {
		perror("Error: getifaddrs Failed\n");
		exit(0);
	}

	// loop through the list
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}
		int family;
		family = ifa->ifa_addr->sa_family;

		// populate interface names, if interface is UP and if ethernet interface doesn't belong to control interface and Loopback interface.
		if (family == AF_INET && (strncmp(ifa->ifa_name, "lo", 2) != 0) && (ifa->ifa_flags & IFF_UP) != 0) {
			char networkIP[NI_MAXHOST];

			struct sockaddr_in *ipaddr = ((struct sockaddr_in*) ifa->ifa_addr);

			inet_ntop(AF_INET, &(ipaddr->sin_addr), networkIP, INET_ADDRSTRLEN);

			if (strncmp(networkIP, CTRL_IP, 3) == 0) {
				// skip, as it is control interface.
				continue;
			}
			ptr[indexLen] = (char*)calloc(strlen(ifa->ifa_name), sizeof(char));
			strncpy(ptr[indexLen], ifa->ifa_name, strlen(ifa->ifa_name));
			indexLen++;
		}
	}
	freeifaddrs(ifaddr);

	return indexLen;
}


void learn_active_interfaces() {
	int numberOfInterfaces;
	char **iNames;

	iNames = (char**) calloc (MAX_INTERFACES*MAX_INTERFACES, sizeof(char));
	memset(iNames, '\0', sizeof(char) * MAX_INTERFACES * MAX_INTERFACES);

	numberOfInterfaces = getActiveInterfaces(iNames);

	int i = 0;
	for (; i < numberOfInterfaces; i++) {
		struct interface_tracker_t *temp = (struct interface_tracker_t*) calloc (1, sizeof(struct interface_tracker_t));
		strncpy (temp->eth_name, iNames[i], strlen(iNames[i]));
		temp->isUP = true;
		temp->next = interfaceTracker;
		interfaceTracker = temp;
	}
}

bool checkInterfaceIsActive(char *str) {
	// find all interfaces on the node.
	struct ifaddrs *ifaddr, *ifa;

	if (getifaddrs(&ifaddr) ) {
		perror("Error: getifaddrs Failed\n");
		exit(0);
	}

	// loop through the list
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}
		int family;
		family = ifa->ifa_addr->sa_family;

		if (family == AF_INET && (strncmp(ifa->ifa_name, str, strlen(str)) == 0) && (ifa->ifa_flags & IFF_UP) != 0) {
			freeifaddrs(ifaddr);  
			return true;
		}
	}
	freeifaddrs(ifaddr);  
	return false;
}

// ***  addition by NS
void sig_handler(int signo)
{
	if (signo == SIGINT){
	
		printf("received SIGINT\n");
		exit (1);
		
	}
}
// *** End addition by NS



