#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "F740_command.h"

#define PREAMBLE_HIGH	0xFA
#define PREAMBLE_LOW	0x5A
#define END_MARK_HIGH	0x7E
#define END_MARK_LOW	0xA5

typedef int (*CommandCallback)(unsigned char *buf, int size, void *parm);

int check_checksum(unsigned char *buf, int size)
{
    unsigned char sum = 0x00;
    int i;
    for(i=2;i<size-3;i++)
    {
        sum ^= buf[i];
    }

    return (sum == buf[size-3])? 0: -1;
}

void set_buffer_value(unsigned char value, unsigned char *buf, int *index)
{
	buf[*index] = value;
	(*index)++;
}

unsigned char check_sum(T_packet *pkt, unsigned char payload_sum) 
{
	unsigned char sum = 0;
	sum ^= pkt->MT;
	sum ^= pkt->MC;
	sum ^= pkt->PL[1];
	sum ^= pkt->PL[0];
	if(pkt->Payload != NULL)
	{
        sum ^= payload_sum;
	}		
	return sum;
}

int prepare_buffer(T_packet *pkt, unsigned char *buf)
{
    int index = 0, i = 0;
    unsigned char payload_sum = 0;
    memset(buf, 0, sizeof(buf));
    set_buffer_value(PREAMBLE_HIGH, buf, &index);
    set_buffer_value(PREAMBLE_LOW, buf, &index);
    set_buffer_value(pkt->MT, buf, &index);
    set_buffer_value(pkt->MC, buf, &index);
    set_buffer_value(pkt->PL[1], buf, &index);
    set_buffer_value(pkt->PL[0], buf, &index);
    if(pkt->Payload != NULL)
    {
        for(i=0;i<pkt->payload_len;i++)
        {
            set_buffer_value(pkt->Payload[i], buf, &index);
            payload_sum ^= pkt->Payload[i];
        }
    }
    set_buffer_value(check_sum(pkt, payload_sum), buf, &index);
    set_buffer_value(END_MARK_HIGH, buf, &index);
    set_buffer_value(END_MARK_LOW, buf, &index);
    return index;
}

int GetSystemVersion(unsigned char *buf, int size, void *parm)
{
	T_packet pkt = {.MT=0x00, .MC=0x01};
	
//	printf("DB: %02X,  %02X\n", pkt.PL[0], pkt.PL[1]);
    return prepare_buffer(&pkt, buf);
}

int GetRFAntennaPortState(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL) 
    {
        printf("GetRFAntennaPortState Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    //printf("port:%02X, state:%02X\n", aps->port, aps->state);
    T_packet pkt = {.MT=0x00, .MC=0x45};
    unsigned char payload_buf[1] = {aps->port};
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);
    
    return prepare_buffer(&pkt, buf);
}

int SetRFAntennaPortState(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("SetRFAntennaPortState Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x44};
    unsigned char payload_buf[2] = {aps->port, aps->state};//0: Antenna Port, 1: Port State
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf); 
    return prepare_buffer(&pkt, buf);
}

int GetRFAntennaPower(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("GetRFAntennaPower Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x47};
    unsigned char payload_buf[1] = {aps->port};
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);

    return prepare_buffer(&pkt, buf); 
}

int SetRFAntennaPower(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("SetRFAntennaPower Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x46};
    //0: Antenna Port, 
    //1: Power(High)
    //2: Power
    //3: Power
    //4: Power(Low)
    unsigned char payload_buf[5] = {aps->port, aps->Power[3], aps->Power[2], aps->Power[1], aps->Power[0]};
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);
    return prepare_buffer(&pkt, buf);
}

int GetRFAntennaDwellTime(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("GetRFAntennaDwellTime Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x49};
    unsigned char payload_buf[1] = {aps->port};
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);

    return prepare_buffer(&pkt, buf);
}

int SetRFAntennaDwellTime(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("SetRFAntennaDwellTime Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x48};
    //0: Antenna Port,
    //1: Dwell Time(High)
    //2: Dwell Time
    //3: Dwell Time
    //4: Dwell Time(Low)
    unsigned char payload_buf[5] = {aps->port, aps->DwellTime[3], aps->DwellTime[2], aps->DwellTime[1], aps->DwellTime[0]};
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);
    return prepare_buffer(&pkt, buf);
}

int GetRFAntennaInventoryCycle(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("GetRFAntennaInventoryCycle Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x4B};
    unsigned char payload_buf[1] = {aps->port};
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);

    return prepare_buffer(&pkt, buf);
}

int SetRFAntennaInventoryCycle(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("SetRFAntennaInventoryCycle Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x4A};
    //0: Antenna Port,
    //1: Inventory Cycle(High)
    //2: Inventory Cycle
    //3: Inventory Cycle
    //4: Inventory Cycle(Low)
    unsigned char payload_buf[5] = {aps->port, aps->InventoryCycle[3], aps->InventoryCycle[2], aps->InventoryCycle[1], aps->InventoryCycle[0]};
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);
    return prepare_buffer(&pkt, buf);
}

int GetRFCurrentAlgorithm(unsigned char *buf, int size, void *parm)
{
   if(parm == NULL)
    {
        printf("GetRFCurrentAlgorithm Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_packet pkt = {.MT=0x00, .MC=0x4F};
    pkt.Payload = NULL;

    return prepare_buffer(&pkt, buf); 
}

int SetRFCurrentAlgorithm(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("SetRFCurrentAlgorithm Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x4E};
    unsigned char payload_buf[1] = {aps->Algorithm};
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);

    return prepare_buffer(&pkt, buf);
}

int GetRFDynamicQAlgorithmConfiguration(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("GetRFDynamicQAlgorithmConfiguration Error: got null parm.\n")    ;
        exit(EXIT_FAILURE);
    }
    T_packet pkt = {.MT=0x00, .MC=0x53};
    pkt.Payload = NULL;
    
    return prepare_buffer(&pkt, buf); 
}

int SetRFDynamicQAlgorithmConfiguration(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("SetRFDynamicQAlgorithmConfiguration Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x52};
    unsigned char payload_buf[6] = {
                                    aps->StartQ, 
                                    aps->MinQ, 
                                    aps->MaxQ, 
                                    aps->RetryCount, 
                                    aps->ToggleTarget, 
                                    aps->ThresholdMultiplier
                                   };
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);
    return prepare_buffer(&pkt, buf);
}

int GetTheToggleTargetValueOfTheDynamicQAlgorithm(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("GetTheToggleTargetValueOfTheDynamicQAlgorithm Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_packet pkt = {.MT=0x00, .MC=0x59};
    pkt.Payload = NULL;

    return prepare_buffer(&pkt, buf);
}

int SetTheToggleTargetValueOfTheDynamicQAlgorithm(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("SetTheToggleTargetValueOfTheDynamicQAlgorithm Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x58};
    unsigned char payload_buf[1] = {aps->ToggleTarget};
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);

    return prepare_buffer(&pkt, buf); 
}

int GetInventoryDataFormat(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("GetInventoryDataFormat Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_packet pkt = {.MT=0x00, .MC=0x6B};
    pkt.Payload = NULL;

    return prepare_buffer(&pkt, buf);
}

//XXX - not yet testing
int SetInventoryDataFormat(unsigned char *buf, int size, void *parm)
{
    if(parm == NULL)
    {
        printf("SetInventoryDataFormat Error: got null parm.\n");
        exit(EXIT_FAILURE);
    }
    T_AntennaSetting *aps = (T_AntennaSetting *) parm;
    T_packet pkt = {.MT=0x00, .MC=0x6A};
    unsigned char payload_buf[2] = {aps->InvDataFormat[1], aps->InvDataFormat[0]};
    pkt.Payload = payload_buf;
    pkt.payload_len = sizeof(payload_buf);

    return prepare_buffer(&pkt, buf);
}

int RunTagInventoryCommand(unsigned char *buf, int size, void *parm)
{
//    if(parm == NULL)
//    {
//        printf("RunTagInventoryCommand Error: got null parm.\n");
//        exit(EXIT_FAILURE);
//    }
    T_packet pkt = {.MT=0x00, .MC=0x6C};
    pkt.Payload = NULL;

    return prepare_buffer(&pkt, buf);  
}

int RebootSystem(unsigned char *buf, int size, void *parm)
{
    
    T_packet pkt = {.MT=0x00, .MC=0x02};
    pkt.Payload = NULL;
    
    return prepare_buffer(&pkt, buf);
}

int RestoreFactoryDefault(unsigned char *buf, int size, void *parm)
{

    T_packet pkt = {.MT=0x00, .MC=0x09};
    pkt.Payload = NULL;

    return prepare_buffer(&pkt, buf);
}

