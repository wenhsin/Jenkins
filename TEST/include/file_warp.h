#ifndef FILE_WARP_H
#define FILE_WARP_H

#define CONFIG_LABEL_ANTENNA_DWELL_TIME         "ANTENNA_DWELL_TIME"
#define CONFIG_LABEL_ANTENNA_INVENTORY_CYCLE    "ANTENNA_INVENTORY_CYCLE"
#define CONFIG_LABEL_MAXQ                       "MAXQ"
#define CONFIG_LABEL_MINQ                       "MINQ"
#define CONFIG_LABEL_STARTQ                     "STARTQ"
#define CONFIG_LABEL_RETRY_COUNT                "RETRY_COUNT"
#define CONFIG_LABEL_TOGGLE_TARGET              "TOGGLE_TARGET"
#define CONFIG_LABEL_THRESHOLD_MULTIPLIER       "THRESHOLD_MULTIPLIER"
#define CONFIG_LABEL_TAG_NB_RSSI_THRESHOLD      "TAG_NB_RSSI_THRESHOLD"
#define CONFIG_LABEL_TAG_WB_RSSI_THRESHOLD      "TAG_WB_RSSI_THRESHOLD"
#define CONFIG_LABEL_READER_NAME                "READER_NAME"
#define CONFIG_LABEL_READER_TYPE                "READER_TYPE"

#define CONFIG_LABEL_PORT1_EN                   "PORT1_EN"
#define CONFIG_LABEL_PORT2_EN                   "PORT2_EN"
#define CONFIG_LABEL_PORT3_EN                   "PORT3_EN"
#define CONFIG_LABEL_PORT4_EN                   "PORT4_EN"

#define CONFIG_LABEL_POWER1                     "POWER1"
#define CONFIG_LABEL_POWER2                     "POWER2"
#define CONFIG_LABEL_POWER3                     "POWER3"
#define CONFIG_LABEL_POWER4                     "POWER4"

typedef struct {
    unsigned char state;
    unsigned int Power;
}T_Port_Setting;

typedef struct {
    T_Port_Setting PortSetting[4];
    unsigned int DwellTime;
    unsigned int InventoryCycle;
    unsigned char MinQ;
    unsigned char MaxQ;
    unsigned char StartQ;
    unsigned char RetryCount;
    unsigned char ToggleTarget;
    unsigned char ThresholdMultiplier;
    unsigned char NB_RSSI_Threshold;
    unsigned char WB_RSSI_Threshold;
    char ReaderName[50];
    char ReaderType[50];
}T_Conf_Parms;
void lprintf(char *str,...);
void innerText(char *inner, char *pText, char *beginMark, char *endMark);
int read_config_file(T_Conf_Parms *parms);

#endif

