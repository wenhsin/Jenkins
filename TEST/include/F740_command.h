#ifndef F740_COMMAND_H
#define F740_COMMAND_H

#define PREAMBLE_HIGH   0xFA
#define PREAMBLE_LOW    0x5A
#define END_MARK_HIGH   0x7E
#define END_MARK_LOW    0xA5

enum E_InventoryDataFormat{
    IDF_ANTENNA_ENABLED = 0x0001,
    IDF_TIMESTAME_ENABLED = 0x0002,
    IDF_NB_RSSI_ENABLED = 0x0004,
    IDF_WB_RSSI_ENABLED = 0x0008,
    IDF_RSSI_ENABLED = 0x0010,
    IDF_CHANNEL_ENABLED = 0x0020,
    IDF_FAST_ID_ENABLED = 0x0040
};

typedef int (* T_CommandCallback) (unsigned char *buf, int size, void *parm);

typedef struct packet{
  unsigned char MT;
  unsigned char MC;
  union{
    short payload_len;
    unsigned char PL[2];
  };
  unsigned char *Payload;  
}T_packet;

typedef struct AntennaSetting{
  unsigned char port;
  unsigned char state;
  unsigned char Algorithm;
  unsigned char StartQ;
  unsigned char MinQ;
  unsigned char MaxQ;
  unsigned char RetryCount;
  unsigned char ToggleTarget;
  unsigned char ThresholdMultiplier;
  union{
    unsigned int p;
    unsigned char Power[4];
  };
  union{
    unsigned int dwell;
    unsigned char DwellTime[4];
  };
  union{
    unsigned int inventory;
    unsigned char InventoryCycle[4];
  };
  union{
    short inv_data_format;
    unsigned char InvDataFormat[2];
  };
}T_AntennaSetting;

int check_checksum(unsigned char *buf, int size);
int GetSystemVersion(unsigned char *buf, int size, void *parm);
int GetRFAntennaPortState(unsigned char *buf, int size, void *parm);
int SetRFAntennaPortState(unsigned char *buf, int size, void *parm);
int GetRFAntennaPower(unsigned char *buf, int size, void *parm);
int SetRFAntennaPower(unsigned char *buf, int size, void *parm);
int GetRFAntennaDwellTime(unsigned char *buf, int size, void *parm);
int SetRFAntennaDwellTime(unsigned char *buf, int size, void *parm);
int GetRFAntennaInventoryCycle(unsigned char *buf, int size, void *parm);
int SetRFAntennaInventoryCycle(unsigned char *buf, int size, void *parm);
int GetRFCurrentAlgorithm(unsigned char *buf, int size, void *parm);
int SetRFCurrentAlgorithm(unsigned char *buf, int size, void *parm);
int GetRFDynamicQAlgorithmConfiguration(unsigned char *buf, int size, void *parm);
int SetRFDynamicQAlgorithmConfiguration(unsigned char *buf, int size, void *parm);
int GetTheToggleTargetValueOfTheDynamicQAlgorithm(unsigned char *buf, int size, void *parm);
int SetTheToggleTargetValueOfTheDynamicQAlgorithm(unsigned char *buf, int size, void *parm);
int GetInventoryDataFormat(unsigned char *buf, int size, void *parm);
int SetInventoryDataFormat(unsigned char *buf, int size, void *parm);
int RunTagInventoryCommand(unsigned char *buf, int size, void *parm);
int RebootSystem(unsigned char *buf, int size, void *parm);
int RestoreFactoryDefault(unsigned char *buf, int size, void *parm);
#endif

