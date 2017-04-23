#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_warp.h"
#include "network_warp.h"

//extern unsigned char buf[512];
extern T_Conf_Parms parms;
char buff[512];
char Seq[10];

void dcenter_init(void)
{
	int ret;
	
	memset(buff, 0,sizeof(buff));
	
	ret=network_recv(buff, sizeof(buff));
	lprintf("initial recv: %s\n", buff);

	innerText(Seq,  buff, "Seq=\"", "\"");
	//lprintf("Seq=%s\n", Seq);

	char msg0_0[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?><ClarIDy Class=\"Common\" Version=\"1\"><Head Command=\"GetInfo\" Direction=\"DeviceToServer\" Seq=\"";
	char msg0_1[] = "\" Type=\"Response\"/><Parameter><DeviceInfo Class=\"CS\" Encoding=\"UTF-8\" ModelID=\"";
	char msg0_2[] = "\" ProductID=\"";
	char msg0_3[] = "\" SerialNo=\"";
	char msg0_4[] = "\" VendorID=\"ClarIDy\" /></Parameter><Status>OK</Status></ClarIDy>";
	
	memset(buff, 0,sizeof(buff));
	strncpy(buff, "\x02", sizeof(buff));
	strncat(buff, msg0_0, strlen(msg0_0));
	strncat(buff, Seq, strlen(Seq));
	strncat(buff, msg0_1, strlen(msg0_1));
    strncat(buff, parms.ReaderName, strlen(parms.ReaderName));
    strncat(buff, msg0_2, strlen(msg0_2));
    strncat(buff, parms.ReaderType, strlen(parms.ReaderType));
	strncat(buff, msg0_3, strlen(msg0_3));	
    strncat(buff, parms.ReaderName, strlen(parms.ReaderName));
    strncat(buff, msg0_4, strlen(msg0_4));	
	strncat(buff, "\x03", 1);
	lprintf("initial send0: %s\n", buff);
	ret=network_send(buff, strlen(buff));


	char msg1_0[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?><ClarIDy Class=\"CS\" Version=\"1\"><Head Command=\"SendMessage\" Direction=\"DeviceToServer\" Seq=\"";
	char msg1_1[] = "\" Type=\"Event\" /><Parameter><Message Alert=\"1\" Msg=\"STATUS:READER\" /></Parameter><Status>OK</Status></ClarIDy>";

	memset(buff, 0,sizeof(buff));
	strncpy(buff, "\x02", sizeof(buff));
	strncat(buff, msg1_0, strlen(msg1_0));
	strncat(buff, Seq, strlen(Seq));
	strncat(buff, msg1_1, strlen(msg1_1));
	strncat(buff, "\x03", 1);
	lprintf("initial send1: %s\n", buff);
	ret=network_send(buff, strlen(buff));
	

	ret=network_recv(buff, sizeof(buff));
	lprintf("initial ack: %s\n", buff);
	
	
	char msg2_0[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?><ClarIDy Class=\"DT\" Version=\"1\"><Head Command=\"SendMessage\" Direction=\"DeviceToServer\" Seq=\"";
	char msg2_1[] = "\" Type=\"Ack\"/><Parameter/><Status>OK</Status></ClarIDy>";
	memset(buff, 0,sizeof(buff));
	strncpy(buff, "\x02", sizeof(buff));
	strncat(buff, msg2_0, strlen(msg2_0));
	strncat(buff, Seq, strlen(Seq));
	strncat(buff, msg2_1, strlen(msg2_1));
	strncat(buff, "\x03", 1);
	lprintf("initial send2: %s\n", buff);
	ret=network_send(buff, strlen(buff));
}

void dcenter_heart_beat(void)
{
	int ret;
	strncpy(buff, "\x02", sizeof(buff));
	strncat(buff, "\x03", 1);
	ret=network_send(buff, strlen(buff));
	//lprintf("heart beat ret = %d\n", ret);
}

void dcenter_send_tag(char *tag, int size)
{
	int ret;
	char tag_0[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?><ClarIDy Class=\"CS\" Version=\"1\"><Head Command=\"SendMessage\" Direction=\"DeviceToServer\" Seq=\"";
	char tag_1[] = "\" Type=\"Event\" /><Parameter><Message Alert=\"30\" Msg=\"TAG:";
	char tag_2[] = "\" /></Parameter><Status>OK</Status></ClarIDy>";
   
	strncpy(buff, "\x02", sizeof(buff));
	strncat(buff, tag_0, strlen(tag_0));
	strncat(buff, Seq, strlen(Seq));
	strncat(buff, tag_1, strlen(tag_1));
	strncat(buff, tag, size);
	strncat(buff, tag_2, strlen(tag_2));
	strncat(buff, "\x03", 1);
	ret = network_send(buff, strlen(buff));
	lprintf("DSEND[%d]=> %s\n", ret, tag);	
}
