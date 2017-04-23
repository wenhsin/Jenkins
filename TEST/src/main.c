#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include "serial_warp.h"
#include "F740_command.h"
#include "transient_buffer.h"
#include "file_warp.h"
#include "network_warp.h"
#include "dcenter.h"
#include "tag_list.h"

#define READER_IDLE_TIME    60
#define MAX_CHANNEL_NO  	4
#define MAX_PACKAGE_SIZE 	64
#define true  1
#define false 0

unsigned char superloop = true;
unsigned char buf[512];
T_Conf_Parms parms;

pthread_mutex_t idle_time_mutex;
unsigned int idle_time = 0;
int error_cnt = 0;
char log_file[64];

int command_send(T_AntennaSetting *AntennaSetting, T_CommandCallback CommandCallback)
{
    int cmd_retry = 2, i, ret;
    
    do
    {
        lprintf("COMMAND SEND --->\n");
        int n = CommandCallback(buf, sizeof(buf), (void *) AntennaSetting);
        ret = serial_send(buf, n);
        if(ret != n)
        {
            lprintf("ERROR: serial_send size wrong!!!\n");
            return -1;
        }
        for(i=0; i < n ;i++)
        {
            lprintf("0x%02X ", buf[i]);
        }
        lprintf("\nWAIT FOR PROCESS\n");
        usleep(500000);

        memset(buf, 0, sizeof(buf));
        n = serial_recv(buf, sizeof(buf));
        if(n>0)
        {
            lprintf("COMMAND RESPONSE <---\n");
            for(i=0; i < n ;i++)
            {
                lprintf("0x%02X ", buf[i]);
            }
            lprintf("\n");
            if(check_checksum(buf, n) < 0)
            {
                lprintf("CHECK SUM FAIL!!!\n");
            }
            else
            {
                lprintf("CHECK SUM PASS...\n");
                break;
            }
        }
        lprintf("RETRY\n");
    }while(cmd_retry-->=0);
    
    if(cmd_retry<0)
    {
        lprintf("*************************\n");
        lprintf("ERROR: command send fail.\n");
        lprintf("*************************\n");
    }
    return cmd_retry;
}

int read_package(unsigned char *buf, int size)
{
	char tag[32];
	char temp[4];
    int ret = 0, i, index;
    if(size > MAX_PACKAGE_SIZE)
    {
        lprintf("Error: wrong package size...\n");
        return -1;
    }
#if 0
    for(i=0; i<size ;i++)
    {
        lprintf("pkt[%d]=%02X\n", i, buf[i]);
    }
#endif
    /*header check*/
    if(buf[0]!= PREAMBLE_HIGH || buf[1] != PREAMBLE_LOW || \
        buf[size-1] != END_MARK_LOW || buf[size-2] != END_MARK_HIGH)
    {
        lprintf("Error: wrong header...\n");
        return -2;
    }
    /*function code check*/
	if(buf[2]==0x01 && buf[3]==0x01)
	{
		lprintf("Reader alive...\n");
		return 0;
	}
    else if(buf[2]!=0x02 || buf[3]!=0x6C)
    {
        lprintf("Error: wrong function code...\n");
        return -3;
    }
	
    /*checksum check*/
    if(check_checksum(buf, size) < 0)
    {
        lprintf("Error: checksum fail...\n");
        return -4;
    }
    /*CRC Valid check*/
    if(buf[24] == 0x01)
    {
        lprintf("Error: CRC error...\n");
        return -5;
    }

    int Antenna = 0;
    unsigned char NB_RSSI=0, WB_RSSI=0;
	memset(tag, 0, sizeof(tag));
	memset(temp, 0, sizeof(temp));
    
    /*get payload length*/
    for(i=6, index=1;index<=buf[5];i++, index++)
    {
        switch(index)
        {
            case 6:
            Antenna = (int) buf[i];
            break;

            case 15:
            NB_RSSI = buf[i];
            break;
            case 18:
            WB_RSSI = buf[i];
            break;
            default:
                if(index >=24 && index<=35)//(22~37)
                {
                    sprintf(temp,"%02X", buf[i]);
					strncat(tag, temp, 2);
                }
            break;
        }
    }
	//for test scenrio
#if 0   
	if(strcmp(tag, "201305070000000000004910") == 0)
	{
		strncpy(tag, "DEED01010000000000001051", 24);
	}
	else
	{
		strncpy(tag, "DEED01010000000000001052", 24);
	}
#endif	
	printf("EPC=%s\n", tag);
    printf("\nAntenna=%d, NB_RSSI=%d, WB_RSSI=%d\n", Antenna, NB_RSSI, WB_RSSI);
    
	if(NB_RSSI> parms.NB_RSSI_Threshold && WB_RSSI > parms.WB_RSSI_Threshold)
		tag_add_list(tag);
    //lprintf("================================================\n");

    return ret;
}

long heartbeat_cnt = 0;
void work_thread(void *arg)
{
	while(superloop) 
	{
        //dcenter heartbeat
        if(heartbeat_cnt % 5  == 0)
            dcenter_heart_beat();
		tag_remove_list();
		lprintf("len=%d\n", get_list_length());
		//lprintf("error packet count=%d\n", error_cnt);
		/*reader idle check*/	
		pthread_mutex_lock(&idle_time_mutex);
		idle_time++;
		pthread_mutex_unlock(&idle_time_mutex);		
		if(idle_time > READER_IDLE_TIME)
		{
			int n = GetSystemVersion(buf, sizeof(buf), NULL);
			int ret;
			ret = serial_send(buf, n);
			if(ret != n)
			{
				lprintf("ERROR: serial_send size wrong!!!\n");
				superloop = false;
			}
		}
		
		sleep(1);
	}
}

void pack_package(void *arg)
{
    int pkt_index;
    unsigned char data, last_data = 0x00;
    unsigned char pkt[MAX_PACKAGE_SIZE];
    //int tid = *((int *) arg);
    while(superloop)
    {
        pkt_index = 0;
        memset(pkt, 0, sizeof(pkt));

        data=read_transient_buffer();
        pkt[pkt_index++] = data;
        while(!(data == END_MARK_LOW && last_data ==END_MARK_HIGH))
        {
            last_data = data;
            data=read_transient_buffer();
            pkt[pkt_index++] = data;
        }
		pthread_mutex_lock(&idle_time_mutex);
		idle_time = 0;
		pthread_mutex_unlock(&idle_time_mutex);
		
        if(read_package(pkt, pkt_index) <0) 
        {
            error_cnt++;
            lprintf("ERROR: read_package error\n");
			lprintf("package dump:\n");
			int i;
			for(i=0; i<pkt_index ;i++)
			{
				lprintf("pkt[%d]=%02X\n", i, pkt[i]);
			}
			lprintf("------------\n");
        }
    }
}

void uart_recv(void)
{
	int n, i;
	while(superloop) 
	{
		n = serial_recv(buf, sizeof(buf));
		if(n>0) 
		{
		  //lprintf("receive %d bytes\n", n);
		  for(i=0; i < n ;i++)
		  {
			//lprintf("%d -> %02X\n", i, buf[i]);
            write_transient_buffer(buf[i]);
		  }
		}
		memset(buf, 0, sizeof(buf));
		usleep(100);
	}
//    usleep(1000*1000*3);
	pthread_exit("uart_recv finished.");
}

void sig_handler(int signo)
{
	switch(signo)
	{
		case (SIGINT):
		case (SIGKILL):
			lprintf("SIGINT\n");
            superloop = false;
			pthread_mutex_destroy(&idle_time_mutex);
			tag_list_destroy();
            transient_buffer_destroy();         
            serial_destroy();
            network_destroy();
			lprintf("*****RFID daemon END*****\n");
            exit(0);
		break;
		case (SIGUSR1):
		lprintf("SIGUSR1\n");
		int n = GetSystemVersion(buf, sizeof(buf), NULL);
		int ret, i;
        ret = serial_send(buf, n);
        if(ret != n)
        {
            lprintf("ERROR: serial_send size wrong!!!\n");
        }
        for(i=0; i < n ;i++)
        {
            lprintf("0x%02X ", buf[i]);
        }
		break;
		default:
			lprintf("DEFAULT\n");
		break;
	}
}

int main(int argc, char *argv[])
{
	int ret, producer_id, i, consumer_id = 0, worker_id;
	void *thread_result;
	pthread_t producer, consumer, worker;
	if(signal(SIGINT, sig_handler) == SIG_ERR) 
	{
		perror("Catch SIGINT error.");
	}
	if(signal(SIGUSR1, sig_handler) == SIG_ERR) 
	{
		perror("Catch SIGUSR1 error.");
	}
	 
    
    time_t now;
    struct tm tm_now;
    
    now = time(NULL);
    localtime_r(&now, &tm_now);
    strftime(log_file, sizeof(log_file), "/var/log/RFIDD_%Y%m%d_%H_%M_%S.log", &tm_now);

#if 0
	tag_list_init();
	serial_init();
	transient_buffer_init();
    if(ret=(network_init()) <0)
    {
        lprintf("ERROR CODE:%d\n");
    }
    else
    {
        //dcenter_init();
		//char tag[] = "DEED01010000000000001051";
        //dcenter_send_tag(tag, strlen(tag));  
		
		tag_add_list("ABC");
		tag_add_list("ABC");
		tag_add_list("ABC");
		
		lprintf("len=%d\n", get_list_length());
		
		tag_add_list("DEF");
		tag_add_list("ABC");
		lprintf("len=%d\n", get_list_length());
		

		for(i=0; i <=20;i++)
		{
			tag_remove_list();
			lprintf("[%d] len=%d\n", i, get_list_length());
		}
			

        network_destroy();
    }
	tag_list_destroy();
    return 0;
#endif
    //lprintf("HOLD 60s FOR READER BOOTING...\n");
    //usleep(1000*1000*60);
	lprintf("*****RFID daemon START*****\n");
    if(read_config_file(&parms) < 0)
    {
        goto Error;
    }
    else
    {
        lprintf("PORT1_EN: %d\n", parms.PortSetting[0].state);
        lprintf("POWER: %d\n", parms.PortSetting[0].Power);
        lprintf("PORT2_EN: %d\n", parms.PortSetting[1].state);
        lprintf("POWER: %d\n", parms.PortSetting[1].Power);
        lprintf("PORT3_EN: %d\n", parms.PortSetting[2].state);
        lprintf("POWER: %d\n", parms.PortSetting[2].Power);
        lprintf("PORT4_EN: %d\n", parms.PortSetting[3].state);
        lprintf("POWER: %d\n", parms.PortSetting[3].Power);

        lprintf("DwellTime: %d\n", parms.DwellTime);
        lprintf("InventoryCycle:%d\n", parms.InventoryCycle);
        lprintf("MaxQ:%d\n", parms.MaxQ);
        lprintf("MinQ:%d\n", parms.MinQ);
        lprintf("StartQ:%d\n", parms.StartQ);
        lprintf("RetryCount:%d\n", parms.RetryCount);
        lprintf("ToggleTarget:%d\n", parms.ToggleTarget);
        lprintf("ThresholdMultiplier:%d\n", parms.ThresholdMultiplier); 
        lprintf("NB RSSI Threshold:%d\n", parms.NB_RSSI_Threshold);
        lprintf("WB RSSI Threshold:%d\n", parms.WB_RSSI_Threshold);

        lprintf("Reader Name: %s\n", parms.ReaderName);
        lprintf("Reader Type: %s\n", parms.ReaderType);
        
    }
    serial_init();
    lprintf("======READER REBOOT======\n");
    int m = RestoreFactoryDefault(buf, sizeof(buf), NULL);
    ret = serial_send(buf, m);
    usleep(1000000*60);
    m = serial_recv(buf, sizeof(buf));
    serial_destroy();    
    lprintf("======READER ONLINE======\n");


    if((ret=network_init()) < 0)
    {
        lprintf("*************************\n");
        lprintf("ERROR: network_init error code=%d\n", ret);
        goto Error;    
    }
	serial_init();
	pthread_mutex_init(&idle_time_mutex, NULL);
    transient_buffer_init();	
	tag_list_init();

    lprintf("======READER INITIAL START======\n");

    T_AntennaSetting asAll = {.Algorithm=0x01,
                           .StartQ= parms.StartQ,
                           .MinQ=parms.MinQ,
                           .MaxQ=parms.MaxQ,
                           .RetryCount=parms.RetryCount,
                           .ToggleTarget=parms.ToggleTarget, 
                           .ThresholdMultiplier=parms.ThresholdMultiplier
                          };
    
    for(i=1; i <= MAX_CHANNEL_NO;i++)
    {
        T_AntennaSetting as = {.port=i, 
                               .state=parms.PortSetting[i-1].state,
                               .p=parms.PortSetting[i-1].Power, 
                               .dwell=parms.DwellTime, 
                               .inventory=parms.InventoryCycle
                              };
        lprintf("SetRFAntennaPortState\n");
        if(command_send(&as, SetRFAntennaPortState) < 0)
        {
            goto Error;
        }
        lprintf("SetRFAntennaPower\n");
        if(command_send(&as, SetRFAntennaPower) <0)
        {
            goto Error;
        }
        lprintf("SetRFAntennaDwellTime\n");
        if(command_send(&as, SetRFAntennaDwellTime) <0)
        {
            goto Error;
        }
        lprintf("SetRFAntennaInventoryCycle\n");
        if(command_send(&as, SetRFAntennaInventoryCycle) <0)
        {
            goto Error;
        }

        lprintf("GetRFAntennaPortState\n");
        if(command_send(&as, GetRFAntennaPortState) < 0)
        {
            goto Error;
        }
        lprintf("GetRFAntennaPower\n");
        if(command_send(&as, GetRFAntennaPower) < 0)
        {
            goto Error;
        }
        lprintf("GetRFAntennaDwellTime\n");
        if(command_send(&as, GetRFAntennaDwellTime) < 0)
        {
            goto Error;
        }
        lprintf("GetRFAntennaInventoryCycle\n");
        if(command_send(&as, GetRFAntennaInventoryCycle) < 0)
        {
            goto Error;
        }
        lprintf("-----------------------------\n");
    }//end of for

    lprintf("SetRFCurrentAlgorithm\n");
    if(command_send(&asAll, SetRFCurrentAlgorithm) < 0)
    {
        goto Error;
    }

    lprintf("GetRFCurrentAlgorithm\n");
    if(command_send(&asAll, GetRFCurrentAlgorithm) < 0)
    {
        goto Error;
    }
    lprintf("-----------------------------\n");
    
#if 0    
    lprintf("SetRFDynamicQAlgorithmConfiguration\n");
    if(command_send(&asAll, SetRFDynamicQAlgorithmConfiguration) < 0)
    {
        goto Error;
    }
#endif
    lprintf("GetRFDynamicQAlgorithmConfiguration\n");
    if(command_send(&asAll, GetRFDynamicQAlgorithmConfiguration) < 0)
    {
        goto Error;
    }
    lprintf("-----------------------------\n");
    lprintf("GetTheToggleTargetValueOfTheDynamicQAlgorithm\n");
    if(command_send(&asAll, GetTheToggleTargetValueOfTheDynamicQAlgorithm) < 0)
    {
        goto Error;
    }
    lprintf("-----------------------------\n");
#if 0    
    lprintf("GetInventoryDataFormat\n");
    if(command_send(&asAll, GetInventoryDataFormat) < 0)
    {
        lprintf("*************************\n");
        lprintf("ERROR: command send fail.\n");
        goto Error;
    }
    lprintf("-----------------------------\n"); 
#endif

    lprintf("RunTagInventoryCommand\n");
    int n = RunTagInventoryCommand(buf, sizeof(buf), NULL);
    ret = serial_send(buf, n);

    lprintf("======READER INITIAL DONE======\n");
	
    usleep(500000);	
    if((ret = pthread_create(&consumer, NULL, (void *) &pack_package, &consumer_id)))
    {
		lprintf("Create consumer fail.");
		goto Error;
    }
	
    if((ret = pthread_create(&worker, NULL, (void *) &work_thread, &worker_id)))
    {
		lprintf("Create worker fail.");
		goto Error;
    }

	/*create thread*/
	ret = pthread_create(&producer, NULL, (void *) &uart_recv, &producer_id);
	if(ret != 0) 
	{
		lprintf("Create producer fail.");
		goto Error;
	}
    
	ret = pthread_join(producer, &thread_result);
	if(ret != 0)
	{
		lprintf("Join producer fail.");
		goto Error;
	}

	lprintf("thread result: %s\n", (char *)thread_result);

Error:
	pthread_mutex_destroy(&idle_time_mutex);
    network_destroy();
    serial_destroy();
    transient_buffer_destroy();
	tag_list_destroy();
    lprintf("ERROR!!!\n");
    lprintf("*****RFID daemon ABORT*****\n");
    return 0;
}
