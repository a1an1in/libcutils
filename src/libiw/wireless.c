
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <err.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <math.h>

#include <libdbg/debug.h>
#include "iwlib.h"
#include "wireless.h"

#if 1
#define IEEE80211_PARAM_MACCMD      17                                
#define IEEE80211_PARAM_HIDESSID    19                                                           
#define IEEE80211_PARAM_MONITOR     496                                
#define IEEE80211_PARAM_SHORT_GI    71                                
#define IEEE80211_PARAM_BANDWIDTH   336                                
#define IEEE80211_PARAM_MAXSTA      242                                        
#define IEEE80211_PARAM_WMM         18  /* WMM mode (on, off) */                                
#define IEEE80211_PARAM_ME          94                                
#define IEEE80211_PARAM_MAXSNPENTRY 497                                          
#define IEEE80211_PARAM_MCCU        498                                
#define IEEE80211_PARAM_BCENHANCE   499 


#define SIOCIWFIRSTPRIV 0x8BE0
#define SIOCIWLASTPRIV  0x8BFF
#define IEEE80211_IOCTL_ADDMAC      (SIOCIWFIRSTPRIV+10)        /* Add ACL MAC Address */
#define IEEE80211_IOCTL_DELMAC      (SIOCIWFIRSTPRIV+12)        /* Del ACL MAC Address */
#define IEEE80211_IOCTL_KICKMAC     (SIOCIWFIRSTPRIV+15)
#define ATH_IOCTL_SETCOUNTRY    (SIOCIWFIRSTPRIV+2)
#define IEEE80211_IOCTL_SETMODE     (SIOCIWFIRSTPRIV+18)
#define IEEE80211_CLONE_BSSID       0x0001      /* allocate unique mac/bssid */
#define IEEE80211_ADDR_LEN 6

/*
 *#define AT_PLATFORM_AP152_AFI
 */

int set_essid(int skfd, char *if_name, char *ssid)
{
    struct iwreq		wrq;
    char essid[100];

    memset(essid, '\0', sizeof(essid));
    memset(&wrq, 0, sizeof(wrq));

    strcpy(essid, ssid);	

    wrq.u.essid.pointer = (caddr_t) essid;
    wrq.u.essid.length = strlen(essid);
    wrq.u.essid.flags = 1;

    if(iw_set_ext(skfd, if_name, SIOCSIWESSID, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface essid %s fail",if_name,essid);
        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface essid %s success",if_name,essid);
    return 0;

}

int set_maxsta(int skfd, char *if_name, int count)
{

    struct iwreq		wrq;
    memset(&wrq, 0, sizeof(wrq));

    strncpy(wrq.ifr_name, if_name, IFNAMSIZ);

#ifdef AT_PLATFORM_AP152_AFI
    wrq.u.data.length = 0;
    wrq.u.data.flags = count;
#else
    wrq.u.data.length = count;
    wrq.u.data.flags = 0;
#endif

    wrq.u.mode = IEEE80211_PARAM_MAXSTA;

    if(iw_set_ext(skfd, if_name, SIOCIWFIRSTPRIV, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface maxsta %d fail",if_name,count);
        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface maxsta %d success",if_name,count);
    return 0;
}


int set_channel(int skfd, char *if_name, double channel)
{
    struct iwreq  wrq;
    memset(&wrq, 0, sizeof(wrq));

    iw_float2freq(channel, &(wrq.u.freq));

    wrq.u.freq.flags = IW_FREQ_FIXED;

    if(iw_set_ext(skfd, if_name, SIOCSIWFREQ, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface Channel %.0f fail",if_name,channel);
        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface Channel %.0f success",if_name,channel);
    return 0;
}


int set_txpower(int skfd, char *if_name, int power)
{
    struct iwreq  wrq;
    memset(&wrq, 0, sizeof(wrq));

    /* Prepare the request */
    wrq.u.txpower.value = power;
    wrq.u.txpower.fixed = 1;
    wrq.u.txpower.disabled = 0;
    wrq.u.txpower.flags = IW_TXPOW_DBM;

    if(iw_set_ext(skfd, if_name, SIOCSIWTXPOW, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface Txpower %d fail",if_name,power);
        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface Txpower %d success",if_name,power);
    return 0;
}

int set_monitor(int skfd, char *if_name, int flag)
{
    struct iwreq		wrq;
    memset(&wrq, 0, sizeof(wrq));

    strncpy(wrq.ifr_name, if_name, IFNAMSIZ);

#ifdef AT_PLATFORM_AP152_AFI
    wrq.u.data.length = 0;
    wrq.u.data.flags = flag;
#else
    wrq.u.data.length = flag;
    wrq.u.data.flags = 0;
#endif

    wrq.u.mode = IEEE80211_PARAM_MONITOR;

    if(iw_set_ext(skfd, if_name, SIOCIWFIRSTPRIV, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface monitor %d fail",if_name,flag);

        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface monitor %d success",if_name,flag);
    return 0;
}


int set_shortGI(int skfd, char *if_name, int shortgi)
{

    struct iwreq        wrq;
    memset(&wrq, 0, sizeof(wrq));

    strncpy(wrq.ifr_name, if_name, IFNAMSIZ);

#ifdef AT_PLATFORM_AP152_AFI
    wrq.u.data.length = 0;
    wrq.u.data.flags = shortgi;
#else
    wrq.u.data.length = shortgi;
    wrq.u.data.flags = 0;
#endif

    wrq.u.mode = IEEE80211_PARAM_SHORT_GI;

    if(iw_set_ext(skfd, if_name, SIOCIWFIRSTPRIV, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface shortGI %d fail",if_name,shortgi);
        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface shortGI %d success",if_name,shortgi);
    return 0;

}

int set_mode(int skfd, char *if_name, char *mode)
{
    struct iwreq        wrq;
    char buffer[64];

    memset(&wrq, 0, sizeof(wrq));
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer,mode,strlen(mode));

    //strncpy(wrq.ifr_name, if_name, IFNAMSIZ);

    wrq.u.essid.pointer = (caddr_t) buffer;
    wrq.u.essid.length = strlen(buffer)+1;
    wrq.u.essid.flags = 0;

    if(iw_set_ext(skfd, if_name, IEEE80211_IOCTL_SETMODE, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface mode %s fail",if_name,buffer);
        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface mode %s success",if_name,buffer);

    return 0;
}

int set_country(int skfd, char *if_name, char *country)
{
    struct iwreq        wrq;
    char buffer[64];

    memset(&wrq, 0, sizeof(wrq));
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer,country,strlen(country));

    //strncpy(wrq.ifr_name, if_name, IFNAMSIZ);

    wrq.u.essid.pointer = (caddr_t) buffer;
    wrq.u.essid.length = strlen(buffer)+1;
    wrq.u.essid.flags = 0;

    if(iw_set_ext(skfd, if_name, ATH_IOCTL_SETCOUNTRY, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface country %s fail",if_name,buffer);
        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface country %s success",if_name,buffer);

    return 0;
}

int set_bandwidth(int skfd, char *if_name, int bandwidth)
{
    struct iwreq        wrq;
    memset(&wrq, 0, sizeof(wrq));

    strncpy(wrq.ifr_name, if_name, IFNAMSIZ);

#ifdef AT_PLATFORM_AP152_AFI
    wrq.u.data.length = 0;
    wrq.u.data.flags = bandwidth;
#else
    wrq.u.data.length = bandwidth;
    wrq.u.data.flags = 0;
#endif

    wrq.u.mode = IEEE80211_PARAM_BANDWIDTH;

    if(iw_set_ext(skfd, if_name, SIOCIWFIRSTPRIV, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface channelwidth %d fail",if_name,bandwidth);

        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface channelwidth %d success",if_name,bandwidth);

    return 0;

}

int set_maccmd(int skfd, char *if_name, int cmd)
{
    struct iwreq		wrq;
    memset(&wrq, 0, sizeof(wrq));

    strncpy(wrq.ifr_name, if_name, IFNAMSIZ);

#if 1
#ifdef AT_PLATFORM_AP152_AFI
    wrq.u.data.length = 0;
    wrq.u.data.flags = cmd;
#else
    wrq.u.data.length = cmd;
    wrq.u.data.flags = 0;
#endif
#endif

    wrq.u.mode = IEEE80211_PARAM_MACCMD;

    if(iw_set_ext(skfd, if_name, SIOCIWFIRSTPRIV, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface maccmd %d fail",if_name,cmd);
        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface maccmd %d success",if_name,cmd);
    return 0;

}


int set_addmac(int skfd, char *if_name,unsigned char *macaddr)
{
    struct iwreq		wrq;
    struct sockaddr addr;

    memset(&addr, 0, sizeof(addr));
    memset(&wrq, 0, sizeof(wrq));

    memcpy(&addr.sa_data, macaddr, IEEE80211_ADDR_LEN);
    strncpy(wrq.ifr_name, if_name, IFNAMSIZ);

    memcpy(wrq.u.name, &addr, sizeof(addr));

    if(iw_set_ext(skfd, if_name, IEEE80211_IOCTL_ADDMAC, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface addmac %02X:%02X:%02X:%02X:%02X:%02X fail",if_name,\
                macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
        return (-1);
    }
    dbg_str(DBG_DETAIL,"set %s interface addmac %02X:%02X:%02X:%02X:%02X:%02X success",if_name,\
            macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
    return 0;

}

int set_delmac(int skfd, char *if_name,unsigned char *macaddr)
{
    struct iwreq		wrq;
    struct sockaddr addr;

    memset(&addr, 0, sizeof(addr));
    memset(&wrq, 0, sizeof(wrq));

    memcpy(&addr.sa_data, macaddr, IEEE80211_ADDR_LEN);
    strncpy(wrq.ifr_name, if_name, IFNAMSIZ);

    memcpy(wrq.u.name, &addr, sizeof(addr));

    if(iw_set_ext(skfd, if_name, IEEE80211_IOCTL_DELMAC, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"set %s interface delmac %02X:%02X:%02X:%02X:%02X:%02X fail",if_name,\
                macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
        return (-1);
    }

    dbg_str(DBG_DETAIL,"set %s interface delmac %02X:%02X:%02X:%02X:%02X:%02X success",if_name,\
            macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);

    return 0;

}


int get_txpower(int skfd, char *if_name,int *txpower)
{
    struct iwreq  wrq;
    memset(&wrq, 0, sizeof(wrq));

    if(iw_set_ext(skfd, if_name, SIOCGIWTXPOW, &wrq) < 0)
    {
        dbg_str(DBG_DETAIL,"get %s interface Txpower fail",if_name);
        return (-1);
    }

    *txpower = wrq.u.txpower.value;

    dbg_str(DBG_DETAIL,"get %s interface Txpower %d success",if_name,*txpower);

    return 0;
}

/*
 *int test_inet_wireless(char *ifname, void *arg)
 */
int test_inet_wireless(int argc, char **argv)
{
    int skfd = 0;
    char *ifname = argv[0];
    void *arg = argv[1];

    dbg_str(DBG_DETAIL,"ifname:%s, arg:%s",ifname, arg);

    skfd = iw_sockets_open();
    /*
     *set_maccmd(skfd, ifname,2);
     */
    /*
     *set_bandwidth(skfd, ifname, 7);
     */
    set_essid(skfd, ifname, arg);
    iw_sockets_close(skfd);

}
#endif
