//
//  portable777.h
//  crypto777
//
//  Created by James on 7/7/15.
//  Copyright (c) 2015 jl777. All rights reserved.
//

#ifndef crypto777_portable777_h
#define crypto777_portable777_h

#include <stdint.h>
#include "mutex.h"
#include "nn.h"
#include "pubsub.h"
#include "pipeline.h"
#include "survey.h"
#include "reqrep.h"
#include "bus.h"
#include "pair.h"
#include "sha256.h"
#include "cJSON.h"
#include "uthash.h"

#define OP_RETURN_OPCODE 0x6a
#define calc_predisplinex(startweekind,clumpsize,weekind) (((weekind) - (startweekind))/(clumpsize))
#define _extrapolate_Spline(Splines,gap) ((double)(Splines)[0] + ((gap) * ((double)(Splines)[1] + ((gap) * ((double)(Splines)[2] + ((gap) * (double)(Splines)[3]))))))
#define _extrapolate_Slope(Splines,gap) ((double)(Splines)[1] + ((gap) * ((double)(Splines)[2] + ((gap) * (double)(Splines)[3]))))

#define PRICE_BLEND(oldval,newval,decay,oppodecay) ((oldval == 0.) ? newval : ((oldval * decay) + (oppodecay * newval)))
#define PRICE_BLEND64(oldval,newval,decay,oppodecay) ((oldval == 0) ? newval : ((oldval * decay) + (oppodecay * newval) + 0.499))


int32_t portable_truncate(char *fname,long filesize);
void *map_file(char *fname,uint64_t *filesizep,int32_t enablewrite);
int32_t os_supports_mappedfiles();
char *os_compatible_path(char *str);
char *OS_rmstr();
int32_t OS_launch_process(char *args[]);
int32_t OS_getppid();
int32_t OS_getpid();
int32_t OS_waitpid(int32_t childpid,int32_t *statusp,int32_t flags);
int32_t OS_conv_unixtime(int32_t *secondsp,time_t timestamp);
uint32_t OS_conv_datenum(int32_t datenum,int32_t hour,int32_t minute,int32_t second);

char *nn_typestr(int32_t type);

// only OS portable functions in this file
#define portable_mutex_t struct nn_mutex
#define portable_mutex_init nn_mutex_init
#define portable_mutex_lock nn_mutex_lock
#define portable_mutex_unlock nn_mutex_unlock

struct queueitem { struct queueitem *next,*prev; };
typedef struct queue
{
	struct queueitem *list;
	portable_mutex_t mutex;
    char name[31],initflag;
} queue_t;

struct pingpong_queue
{
    char *name;
    queue_t pingpong[2],*destqueue,*errorqueue;
    int32_t (*action)();
    int offset;
};

#define ACCTS777_MAXRAMKVS 8
#define BTCDADDRSIZE 36
union _bits128 { uint8_t bytes[16]; uint16_t ushorts[8]; uint32_t uints[4]; uint64_t ulongs[2]; uint64_t txid; };
typedef union _bits128 bits128;
union _bits256 { uint8_t bytes[32]; uint16_t ushorts[16]; uint32_t uints[8]; uint64_t ulongs[4]; uint64_t txid; };
typedef union _bits256 bits256;
union _bits384 { bits256 sig; uint8_t bytes[48]; uint16_t ushorts[24]; uint32_t uints[12]; uint64_t ulongs[6]; uint64_t txid; };
typedef union _bits384 bits384;

struct ramkv777_item { UT_hash_handle hh; uint16_t valuesize,tbd; uint32_t rawind; uint8_t keyvalue[]; };
struct ramkv777
{
    char name[63],threadsafe;
    portable_mutex_t mutex;
    struct ramkv777_item *table;
    struct sha256_state state; bits256 sha256;
    int32_t numkeys,keysize,dispflag; uint8_t kvind;
};

#define ramkv777_itemsize(kv,valuesize) (sizeof(struct ramkv777_item) + (kv)->keysize + valuesize)
#define ramkv777_itemkey(item) (item)->keyvalue
#define ramkv777_itemvalue(kv,item) (&(item)->keyvalue[(kv)->keysize])

void ramkv777_lock(struct ramkv777 *kv);
void ramkv777_unlock(struct ramkv777 *kv);
int32_t ramkv777_delete(struct ramkv777 *kv,void *key);
void *ramkv777_write(struct ramkv777 *kv,void *key,void *value,int32_t valuesize);
void *ramkv777_read(int32_t *valuesizep,struct ramkv777 *kv,void *key);
void *ramkv777_iterate(struct ramkv777 *kv,void *args,void *(*iterator)(struct ramkv777 *kv,void *args,void *key,void *value,int32_t valuesize));
struct ramkv777 *ramkv777_init(int32_t kvind,char *name,int32_t keysize,int32_t threadsafe);
void ramkv777_free(struct ramkv777 *kv);
int32_t ramkv777_clone(struct ramkv777 *clone,struct ramkv777 *kv);
struct ramkv777_item *ramkv777_itemptr(struct ramkv777 *kv,void *value);

void lock_queue(queue_t *queue);
void queue_enqueue(char *name,queue_t *queue,struct queueitem *item);
void *queue_dequeue(queue_t *queue,int32_t offsetflag);
int32_t queue_size(queue_t *queue);
struct queueitem *queueitem(char *str);
struct queueitem *queuedata(void *data,int32_t datalen);
void free_queueitem(void *itemptr);
void *queue_delete(queue_t *queue,struct queueitem *copy,int32_t copysize);
void *queue_free(queue_t *queue);
void *queue_clone(queue_t *clone,queue_t *queue,int32_t size);

void calc_sha256(char hashstr[(256 >> 3) * 2 + 1],uint8_t hash[256 >> 3],uint8_t *src,int32_t len);
void calc_sha256cat(uint8_t hash[256 >> 3],uint8_t *src,int32_t len,uint8_t *src2,int32_t len2);
void update_sha256(uint8_t hash[256 >> 3],struct sha256_state *state,uint8_t *src,int32_t len);

int32_t init_hexbytes_noT(char *hexbytes,uint8_t *message,long len);
int32_t decode_hex(unsigned char *bytes,int32_t n,char *hex);
int init_base32(char *tokenstr,uint8_t *token,int32_t len);
int decode_base32(uint8_t *token,uint8_t *tokenstr,int32_t len);
uint64_t stringbits(char *str);

long _stripwhite(char *buf,int accept);
char *clonestr(char *str);
int32_t is_decimalstr(char *str);
int32_t safecopy(char *dest,char *src,long len);

int32_t hcalc_varint(uint8_t *buf,uint64_t x);
long hdecode_varint(uint64_t *valp,uint8_t *ptr,long offset,long mappedsize);

double milliseconds();
uint64_t peggy_basebits(char *name);
uint64_t peggy_relbits(char *name);
uint32_t set_assetname(uint64_t *multp,char *name,uint64_t assetbits);
int32_t _set_assetname(uint64_t *multp,char *buf,char *jsonstr,uint64_t assetid);
struct prices777 *prices777_poll(char *exchangestr,char *name,char *base,uint64_t refbaseid,char *rel,uint64_t refrelid);
int32_t is_native_crypto(char *name,uint64_t bits);
uint64_t InstantDEX_name(char *key,int32_t *keysizep,char *exchange,char *name,char *base,uint64_t *baseidp,char *rel,uint64_t *relidp);
uint64_t is_MGWcoin(char *name);
char *is_MGWasset(uint64_t);
int32_t unstringbits(char *buf,uint64_t bits);
int32_t get_assetname(char *name,uint64_t assetid);

int32_t parse_ipaddr(char *ipaddr,char *ip_port);
int32_t gen_randomacct(uint32_t randchars,char *NXTaddr,char *NXTsecret,char *randfilename);
char *dumpprivkey(char *coinstr,char *serverport,char *userpass,char *coinaddr);
uint64_t conv_NXTpassword(unsigned char *mysecret,unsigned char *mypublic,uint8_t *pass,int32_t passlen);
uint64_t conv_rsacctstr(char *rsacctstr,uint64_t nxt64bits);
uint64_t conv_NXTpassword(unsigned char *mysecret,unsigned char *mypublic,uint8_t *pass,int32_t passlen);
void set_best_amounts(uint64_t *baseamountp,uint64_t *relamountp,double price,double volume);

#define MAX_DEPTH 25
#define MINUTES_FIFO (1024)
#define HOURS_FIFO (64)
#define DAYS_FIFO (512)
#define INSTANTDEX_MINVOL 75
#define INSTANTDEX_ACCT "4383817337783094122"
#define MAX_TXPTRS 1024

struct InstantDEX_quote
{
    UT_hash_handle hh;
    uint64_t quoteid,baseid,baseamount,relid,relamount,nxt64bits;
    struct InstantDEX_quote *baseiQ,*reliQ;
    uint32_t timestamp,duration;
    uint8_t closed:1,sent:1,matched:1,isask:1,pad2:4,minperc:7;
    char exchangeid,gui[9];
};

/*struct orderbook
{
    uint64_t baseid,relid,jumpasset;
    char base[64],rel[64],jumper[64],exchangestr[64],name[64];
    //struct InstantDEX_quote bids[MAX_DEPTH],asks[MAX_DEPTH];
    struct prices777 *bidsources[MAX_DEPTH],*asksources[MAX_DEPTH];
    double lastbid,lastask,bidasks[MAX_DEPTH][4];
    int32_t numbids,numasks;
};*/

//struct prices777_nxtquote { uint64_t baseid,relid,nxt64bits,quoteid,qty,priceNQT,baseamount,relamount; uint32_t timestamp; };
//struct prices777_nxtbooks { struct prices777_nxtquote orderbook[MAX_DEPTH][2],prevorderbook[MAX_DEPTH][2],prev2orderbook[MAX_DEPTH][2]; };
struct prices777_basket { struct prices777 *prices; double wt; int32_t groupid,groupsize,aski,bidi; char base[64],rel[64]; };
struct prices777_order { struct prices777 *source; double price,vol,wt; uint64_t id; uint32_t timestamp; uint16_t slot_ba; };
struct prices777_orderentry { struct prices777_order bid,ask; };
#define MAX_GROUPS 8

struct prices777_basketinfo
{
    int32_t numbids,numasks; uint32_t timestamp;
    struct prices777_orderentry book[MAX_GROUPS+1][MAX_DEPTH];
};

struct prices777
{
    char url[512],exchange[64],base[64],rel[64],lbase[64],lrel[64],key[512],oppokey[512],contract[64],origbase[64],origrel[64];
    uint64_t contractnum,ap_mult,baseid,relid; int32_t keysize,oppokeysize; double lastupdate,decay,oppodecay,lastprice,lastbid,lastask;
    uint32_t pollnxtblock,exchangeid,numquotes,updated,lasttimestamp,RTflag;
    portable_mutex_t mutex;
    char *orderbook_jsonstrs[2][2];
    struct prices777_basketinfo O,O2; double groupwts[MAX_GROUPS + 1];
    uint8_t changed,type; uint8_t **dependents; int32_t numdependents,numgroups,basketsize;
    struct prices777_basket basket[];
};

struct exchange_info
{
    double (*updatefunc)(struct prices777 *prices,int32_t maxdepth);
    int32_t (*supports)(int32_t exchangeid,uint64_t *assetids,int32_t n,uint64_t baseid,uint64_t relid);
    uint64_t (*trade)(char **retstrp,struct exchange_info *exchange,char *base,char *rel,int32_t dir,double price,double volume);
    char name[16],apikey[MAX_JSON_FIELD],apisecret[MAX_JSON_FIELD],userid[MAX_JSON_FIELD];
    uint32_t num,exchangeid,pollgap,refcount,polling; uint64_t nxt64bits; double lastupdate;
    portable_mutex_t mutex;
};

struct exchange_info *get_exchange(int32_t exchangeid);
char *exchange_str(int32_t exchangeid);
struct exchange_info *find_exchange(int32_t *exchangeidp,char *exchangestr);
struct exchange_info *exchange_find(char *exchangestr);
void prices777_exchangeloop(void *ptr);
char *fill_nxtae(uint64_t nxt64bits,int32_t dir,double price,double volume,uint64_t baseid,uint64_t relid);

double prices777_price_volume(double *volumep,uint64_t baseamount,uint64_t relamount);
struct prices777 *prices777_makebasket(char *basketstr,cJSON *basketjson);
char *InstantDEX(char *jsonstr,char *remoteaddr,int32_t localaccess);
//cJSON *prices777_InstantDEX_json(char *_base,char *_rel,int32_t depth,int32_t invert,int32_t localaccess,uint64_t *baseamountp,uint64_t *relamountp,struct InstantDEX_quote *iQ,uint64_t refbaseid,uint64_t refrelid,uint64_t jumpasset);
uint64_t calc_baseamount(uint64_t *relamountp,uint64_t assetid,uint64_t qty,uint64_t priceNQT);
uint64_t calc_asset_qty(uint64_t *availp,uint64_t *priceNQTp,char *NXTaddr,int32_t checkflag,uint64_t assetid,double price,double vol);

int32_t iQ_exchangestr(char *exchange,struct InstantDEX_quote *iQ);
int32_t create_InstantDEX_quote(struct InstantDEX_quote *iQ,uint32_t timestamp,int32_t isask,uint64_t quoteid,double price,double volume,uint64_t baseid,uint64_t baseamount,uint64_t relid,uint64_t relamount,char *exchange,uint64_t nxt64bits,char *gui,struct InstantDEX_quote *baseiQ,struct InstantDEX_quote *reliQ,int32_t duration);
//void add_to_orderbook(struct orderbook *op,int32_t iter,int32_t *numbidsp,int32_t *numasksp,struct InstantDEX_quote *iQ,int32_t polarity,int32_t oldest,char *gui);
//void free_orderbook(struct orderbook *op);
//void sort_orderbook(struct orderbook *op);
uint64_t calc_quoteid(struct InstantDEX_quote *iQ);
double check_ratios(uint64_t baseamount,uint64_t relamount,uint64_t baseamount2,uint64_t relamount2);
double make_jumpquote(uint64_t baseid,uint64_t relid,uint64_t *baseamountp,uint64_t *relamountp,uint64_t *frombasep,uint64_t *fromrelp,uint64_t *tobasep,uint64_t *torelp);

char *peggyrates(uint32_t timestamp);

#endif
