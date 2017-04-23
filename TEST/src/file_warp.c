#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include "file_warp.h"

#define true  1
#define false 0

#define GetCurrentDir getcwd
#define CONFIG_FILE             "Reader.conf"
#define CONFIG_MAX_PATH_SIZE    (1024)

unsigned char LogCreated = false;
extern char log_file[32];

void lprintf(char *str,...)
{
    FILE *file;
    if (!LogCreated) {
        file = fopen(log_file, "w");
        LogCreated = true;
    }
    else        
        file = fopen(log_file, "a");
        
    if (file == NULL) {
        if (LogCreated)
            LogCreated = false;
        return;
    }
    else
    {
        time_t timep; 
        time(&timep);
        fprintf(file,"%s-",ctime(&timep));
        va_list arglist;
        va_start(arglist,str);
        vfprintf(file,str,arglist);
        va_end(arglist);
        fprintf(file,"\n");
    }
 
    if (file)
        fclose(file);
}

void innerText(char *inner, char *pText, char *beginMark, char *endMark) {
     char *beginStart = strstr(pText, beginMark);
     if (beginStart == NULL)
        return;
     char *beginEnd = beginStart + strlen(beginMark);
     char *endStart = strstr(beginEnd, endMark);
     if (endStart < 0) 
         return;
     int len = endStart-beginEnd;
     strncpy(inner, beginEnd, len);
     inner[len] = '\0';
}

int count_nonspace(const char* str)
{
    int count = 0;
    while(*str)
    {
        if(!isspace(*str++))
        count++;
    }
    return count;
}

int read_config_file(T_Conf_Parms *parms)
{
    int ret = 0, beginIndex, endIndex, i;
    FILE *fp;
    char cCurrentPath[CONFIG_MAX_PATH_SIZE] = { '\0' };
    char buf[128], temp[128];

    GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));
    strncat(cCurrentPath, "/",sizeof(cCurrentPath));
    strncat(cCurrentPath, CONFIG_FILE,sizeof(cCurrentPath));
    printf("Config File Path: %s\n", cCurrentPath);

    fp = fopen(cCurrentPath, "r");
    if(fp == NULL) 
    {
        ret = -1;
        goto ERROR;
    }

    while(fgets(buf, sizeof(buf), fp) != NULL) {    
        for(i = 0; buf[i] != '\0'; i++, endIndex = i)  
            if(buf[i]=='\n')    buf[i]='\0';
  
 
        i = strncmp(buf, CONFIG_LABEL_ANTENNA_DWELL_TIME, count_nonspace(CONFIG_LABEL_ANTENNA_DWELL_TIME));
        if(i == 0) 
        {
            beginIndex = count_nonspace(CONFIG_LABEL_ANTENNA_DWELL_TIME);
            beginIndex++;
            parms->DwellTime =atoi(buf + beginIndex);
        }
        
        i = strncmp(buf, CONFIG_LABEL_ANTENNA_INVENTORY_CYCLE, count_nonspace(CONFIG_LABEL_ANTENNA_INVENTORY_CYCLE));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_ANTENNA_INVENTORY_CYCLE);
            beginIndex++;
            parms->InventoryCycle =atoi(buf + beginIndex);
        }

        i = strncmp(buf, CONFIG_LABEL_MAXQ, count_nonspace(CONFIG_LABEL_MAXQ));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_MAXQ);
            beginIndex++;
            parms->MaxQ =atoi(buf + beginIndex);
        }

        i = strncmp(buf, CONFIG_LABEL_MINQ, count_nonspace(CONFIG_LABEL_MINQ));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_MINQ);
            beginIndex++;
            parms->MinQ =atoi(buf + beginIndex);
        }

        i = strncmp(buf, CONFIG_LABEL_STARTQ, count_nonspace(CONFIG_LABEL_STARTQ));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_STARTQ);
            beginIndex++;
            parms->StartQ =atoi(buf + beginIndex);
        }
//
        i = strncmp(buf, CONFIG_LABEL_RETRY_COUNT, count_nonspace(CONFIG_LABEL_RETRY_COUNT));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_RETRY_COUNT);
            beginIndex++;
            parms->RetryCount =atoi(buf + beginIndex);
        }
        
        i = strncmp(buf, CONFIG_LABEL_TOGGLE_TARGET, count_nonspace(CONFIG_LABEL_TOGGLE_TARGET));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_TOGGLE_TARGET);
            beginIndex++;
            parms->ToggleTarget =atoi(buf + beginIndex);
        }

        i = strncmp(buf, CONFIG_LABEL_THRESHOLD_MULTIPLIER, count_nonspace(CONFIG_LABEL_THRESHOLD_MULTIPLIER));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_THRESHOLD_MULTIPLIER);
            beginIndex++;
            parms->ThresholdMultiplier =atoi(buf + beginIndex);
        }

        i = strncmp(buf, CONFIG_LABEL_PORT1_EN, count_nonspace(CONFIG_LABEL_PORT1_EN));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_PORT1_EN);
            beginIndex++;
            strncpy(temp, buf+beginIndex, endIndex - beginIndex);            
            temp[endIndex - beginIndex]='\0';
            parms->PortSetting[0].state = (strncmp(temp, "ON",2)==0)?1:0;
        }

        i = strncmp(buf, CONFIG_LABEL_PORT2_EN, count_nonspace(CONFIG_LABEL_PORT2_EN));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_PORT2_EN);
            beginIndex++;
            strncpy(temp, buf+beginIndex, endIndex - beginIndex);
            temp[endIndex - beginIndex]='\0';
            parms->PortSetting[1].state = (strncmp(temp, "ON",2)==0)?1:0;
        }


        i = strncmp(buf, CONFIG_LABEL_PORT3_EN, count_nonspace(CONFIG_LABEL_PORT3_EN));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_PORT3_EN);
            beginIndex++;
            strncpy(temp, buf+beginIndex, endIndex - beginIndex);
            temp[endIndex - beginIndex]='\0';
            parms->PortSetting[2].state = (strncmp(temp, "ON",2)==0)?1:0;
        }


        i = strncmp(buf, CONFIG_LABEL_PORT4_EN, count_nonspace(CONFIG_LABEL_PORT4_EN));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_PORT4_EN);
            beginIndex++;
            strncpy(temp, buf+beginIndex, endIndex - beginIndex);
            temp[endIndex - beginIndex]='\0';
            parms->PortSetting[3].state = (strncmp(temp, "ON",2)==0)?1:0;
        }

        i = strncmp(buf, CONFIG_LABEL_POWER1, count_nonspace(CONFIG_LABEL_POWER1));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_POWER1);
            beginIndex++;
            parms->PortSetting[0].Power = atoi(buf + beginIndex);
        }

        i = strncmp(buf, CONFIG_LABEL_POWER2, count_nonspace(CONFIG_LABEL_POWER2));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_POWER2);
            beginIndex++;
            parms->PortSetting[1].Power = atoi(buf + beginIndex);
        }


        i = strncmp(buf, CONFIG_LABEL_POWER3, count_nonspace(CONFIG_LABEL_POWER3));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_POWER3);
            beginIndex++;
            parms->PortSetting[2].Power = atoi(buf + beginIndex);
        }


        i = strncmp(buf, CONFIG_LABEL_POWER4, count_nonspace(CONFIG_LABEL_POWER4));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_POWER4);
            beginIndex++;    
            parms->PortSetting[3].Power = atoi(buf + beginIndex);
        } 
        
        i = strncmp(buf, CONFIG_LABEL_TAG_NB_RSSI_THRESHOLD, count_nonspace(CONFIG_LABEL_TAG_NB_RSSI_THRESHOLD));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_TAG_NB_RSSI_THRESHOLD);
            beginIndex++;
            parms->NB_RSSI_Threshold = atoi(buf + beginIndex);
        }

        i = strncmp(buf, CONFIG_LABEL_TAG_WB_RSSI_THRESHOLD, count_nonspace(CONFIG_LABEL_TAG_WB_RSSI_THRESHOLD));
        if(i == 0)
        {
            beginIndex = count_nonspace(CONFIG_LABEL_TAG_WB_RSSI_THRESHOLD);
            beginIndex++;
            parms->WB_RSSI_Threshold = atoi(buf + beginIndex);
        }

            i = strncmp(buf, CONFIG_LABEL_READER_NAME, count_nonspace(CONFIG_LABEL_READER_NAME));
            if (i == 0) {
                memset(parms->ReaderName, '\0',sizeof(parms->ReaderName));
                beginIndex = count_nonspace(CONFIG_LABEL_READER_NAME);
                beginIndex++;
                strncpy(parms->ReaderName, buf + beginIndex, endIndex - beginIndex);
            }
            i = strncmp(buf, CONFIG_LABEL_READER_TYPE, count_nonspace(CONFIG_LABEL_READER_TYPE));
            if (i == 0) {
                memset(parms->ReaderType, '\0',sizeof(parms->ReaderType));
                beginIndex = count_nonspace(CONFIG_LABEL_READER_TYPE);
                beginIndex++;
                strncpy(parms->ReaderType, buf + beginIndex, endIndex - beginIndex);
            }


    }//end of while
    
    printf("PORT1_EN: %d\n", parms->PortSetting[0].state);
    printf("POWER: %d\n", parms->PortSetting[0].Power);
    printf("PORT2_EN: %d\n", parms->PortSetting[1].state);
    printf("POWER: %d\n", parms->PortSetting[1].Power);
    printf("PORT3_EN: %d\n", parms->PortSetting[2].state);
    printf("POWER: %d\n", parms->PortSetting[2].Power);
    printf("PORT4_EN: %d\n", parms->PortSetting[3].state);
    printf("POWER: %d\n", parms->PortSetting[3].Power);
    printf("DwellTime: %d\n", parms->DwellTime);
    printf("InventoryCycle:%d\n", parms->InventoryCycle);
    printf("MaxQ:%d\n", parms->MaxQ);
    printf("MinQ:%d\n", parms->MinQ);
    printf("StartQ:%d\n", parms->StartQ);
    printf("RetryCount:%d\n", parms->RetryCount);
    printf("ToggleTarget:%d\n", parms->ToggleTarget);
    printf("ThresholdMultiplier:%d\n", parms->ThresholdMultiplier);
    printf("Reader Name: %s\n", parms->ReaderName);
    printf("Reader Type: %s\n", parms->ReaderType);
    if(fp) fclose(fp);
ERROR:    
    return ret;
}

