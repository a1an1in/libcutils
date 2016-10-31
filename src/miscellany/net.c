#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>     
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <libdbg/debug.h>
#include <miscellany/net.h>

void print_mac(unsigned char *mac)
{
    printf("%x:%x:%x:%x:%x:%x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

int get_mac(char *ifname,unsigned char *mac, int len) 
{
    struct ifreq ifreq;
    int sock;

    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror ("socket");
        return -1;
    }
    strcpy (ifreq.ifr_name, ifname); 

    if (ioctl (sock, SIOCGIFHWADDR, &ifreq) < 0)
    {
        perror ("ioctl");
        return -1;
    }

    memcpy(mac,ifreq.ifr_hwaddr.sa_data,len);

    close(sock);

    return 1;
    
}

void print_ipaddr(char *ipaddr)
{
    printf("ipaddr:%s\n",ipaddr);
}

int get_local_ip(char *ifname,char *ipaddr)  
{  
    int sk;  
    struct   sockaddr_in *sin;  
    struct   ifreq ifreq;     

    if ((sk=socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {  
        printf("socket create failse...GetLocalIp!\n");  
        return -1;  
    }  

    memset(&ifreq, 0, sizeof(ifreq));     
    strncpy(ifreq.ifr_name, ifname, sizeof(ifreq.ifr_name) - 1);     

    if(ioctl( sk, SIOCGIFADDR, &ifreq) < 0) {     
        return -1;     
    }       
    sin = (struct sockaddr_in *)&ifreq.ifr_addr;     
    strcpy(ipaddr,inet_ntoa(sin->sin_addr));         

    close(sk);  
      
    return 1;  
}  
int set_local_ip(char *ifname,const char *ipaddr)  
{  
    int sk;  
    struct sockaddr_in sin;  
    struct ifreq ifr_set_ip;  
  
    bzero( &ifr_set_ip,sizeof(ifr_set_ip));  
   
    if( ipaddr == NULL )  
        return -1;  
  
    if((sk = socket( AF_INET, SOCK_STREAM, 0 )) == -1){  
        perror("socket create failse...SetLocalIp!\n");  
        return -1;  
    }  
   
    memset( &sin, 0, sizeof(sin));  
    strncpy(ifr_set_ip.ifr_name, ifname, sizeof(ifr_set_ip.ifr_name)-1);     
      
    sin.sin_family = AF_INET;  
    sin.sin_addr.s_addr = inet_addr(ipaddr);  
    memcpy( &ifr_set_ip.ifr_addr, &sin, sizeof(sin));  
  
    if( ioctl( sk, SIOCSIFADDR, &ifr_set_ip) < 0 )  
    {  
        perror( "Not setup interface\n");  
        return -1;  
    }  
  
    //设置激活标志  
    ifr_set_ip.ifr_flags |= IFF_UP |IFF_RUNNING;  
  
    //get the status of the device  
    if( ioctl( sk, SIOCSIFFLAGS, &ifr_set_ip ) < 0 )  
    {  
         perror("SIOCSIFFLAGS\n");  
         return -1;  
    }  
  
    close( sk );  
    return 0;  
} 
int get_local_netmask(char *ifname,char *netmask_addr)  
{  
    int sk;  
    struct ifreq ifreq;  
    struct sockaddr_in *net_mask;  
          
    sk = socket( AF_INET, SOCK_STREAM, 0 );  
    if( sk == -1)  {  
        perror("create socket failture...GetLocalNetMask\n");  
        return -1;  
    }  
      
    memset(&ifreq, 0, sizeof(ifreq));     
    strncpy(ifreq.ifr_name, ifname, sizeof(ifreq.ifr_name )-1);     
  
    if( (ioctl( sk, SIOCGIFNETMASK, &ifreq ) ) < 0 ) {  
        printf("mac ioctl error\n");  
        return -1;  
    }  
      
    net_mask = ( struct sockaddr_in * )&( ifreq.ifr_netmask );  
    strcpy( netmask_addr, inet_ntoa( net_mask -> sin_addr ) );  
      
    close( sk );  
    return 1;  
}

int set_local_netmask(char *ifname,char *netmask_addr)  
{  
    int sk;  
    struct ifreq ifreq;  
    struct sockaddr_in *sin;  
          
    sk = socket( AF_INET, SOCK_STREAM, 0 );  
    if( sk == -1) {  
        perror("Not create network socket connect\n");  
        return -1;  
    }  
      
    memset(&ifreq, 0, sizeof(ifreq));     
    strncpy(ifreq.ifr_name, ifname, sizeof(ifreq.ifr_name )-1);     
    sin = (struct sockaddr_in *)&ifreq.ifr_addr;  
    sin->sin_family = AF_INET;  
    inet_pton(AF_INET, netmask_addr, &sin->sin_addr);  
  
    if(ioctl(sk, SIOCSIFNETMASK, &ifreq ) < 0) {  
        printf("sk ioctl error\n");  
        return -1;  
    }  

    return 1;
}  

int test_miscellany_net()
{
    unsigned char  mac_addr[6];
    char ip_addr[100];

    get_mac((char *)"eth0",mac_addr, sizeof(mac_addr));
    print_mac(mac_addr);

    get_local_ip((char *)"eth0",ip_addr);
    print_ipaddr(ip_addr);

    set_local_ip((char *)"eth0","192.168.20.182");
    get_local_ip((char *)"eth0",ip_addr);
    print_ipaddr(ip_addr);

    dbg_str(DBG_DETAIL,"get local netmask");
    get_local_netmask((char *)"eth0",ip_addr); 
    print_ipaddr(ip_addr);
    dbg_str(DBG_DETAIL,"set local netmask");
    set_local_netmask((char *)"eth0",(char *)"255.255.255.0");  
    get_local_netmask((char *)"eth0",ip_addr); 
    print_ipaddr(ip_addr);

    dbg_str(DBG_DETAIL,"get_gateway");

    return 0;
}