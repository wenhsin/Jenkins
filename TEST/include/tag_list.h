#ifndef TAG_LIST_H
#define TAG_LIST_H

#define TAG_VALID_TIMES     50
#define COLD_TIME   5
#define true  1
#define false 0

typedef struct TAG
{
    unsigned char enable;
    char id[32];
    int detect_cnt;
    int cold_time;
    struct TAG* next;
}T_TAG;

T_TAG* create(char *tag_id);
void insert(char *tag_id);
void del(T_TAG* a);

void tag_list_init(void);
void tag_list_destroy(void);
int get_list_length(void);
void tag_add_list(char *tag_id);
void tag_remove_list(void);

#endif

