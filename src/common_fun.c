#include<stdio.h>
#include<stdio.h>
#include<string.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <net/if.h>
#include <signal.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <netinet/tcp.h>
#include <resolv.h>
#include <arpa/nameser.h>
#include <unistd.h>
#include <stdlib.h>
#include "pm_config.h"

void enable_sleep_mode()
{	
	char cmd_buff[200]="";

	int  wakealarm = pm_t.suspend_timeout*60;
	
	memset(cmd_buff,0x0,sizeof(cmd_buff));

	sprintf(cmd_buff,"echo +%d > /sys/class/rtc/rtc1/wakealarm",wakealarm);

      	system(cmd_buff);
	
	system("echo mem > /sys/power/state");

	system("echo +%0 > /sys/class/rtc/rtc1/wakealarm");

}



int wakeup_power_off_devices()
{

	;

}


int check_suspend_mode ( int pwr_delay )
{
        unsigned long int ret_key=0;

        int deinit_flag = 0;

        gl11_iodev_open();

        ret_key = gl11_lastkey_timeout();

        gl11_iodev_close();

        if ( ret_key == -1 )
        {
                return 0;
        }

        if ( ret_key > pwr_delay * 60 * 100 )
        {
                //printf("Deinit Flag is Set to One.....\n");
                deinit_flag=1;
        }
        else
        {
                //printf("Deinit Flag is Set to Zero.....\n");
                deinit_flag=0;
        }

        return deinit_flag;
}


int get_wait_bklight_status(void)
{
        gl11_iodev_open();
        gl11_wait_bklon();
        gl11_iodev_close();
        return 0;
}

void wait_for_bootdelay (int delay_mins)
{
	FILE *fp=NULL;
	float uptime=0;

	while (1)
	{
		fp =fopen("/proc/uptime","r");
		fscanf (fp,"%f",&uptime);
		fclose(fp);
		if (uptime >  delay_mins  * 60 )
			break;
		sleep(8);
	}
}


void get_network_rd_init()
{
	get_network_info();

	sleep(5);

	get_rd_info();

	sleep(5);

	get_battery_info();

}

void get_battery_info()
{
	int ret=0;
	float voltage=0;

	ret = get_bat_status(&voltage);

	if (ret == 1 )
		pm.adpter = 1;
	else 
		pm.adpter = 0;

	pm.batt_vol = voltage;
}


void get_rd_info()
{
	int ret=0;

	ret = system("pidof rds &> /dev/null");

	if ( ret == 0 )
	{
		pm.rd_service = 1;

	}

	ret = system("cat /var/log/mk2120ul-mgmtserv.log | tail -3 | grep \"Registered: YES.\" &> /dev/null");

	if ( ret == 0 )
	{
		pm.rd_service_iri = 1;
	}

}

void get_network_info()
{
	if( Get_Interface_Connected("ppp0") == 0)
	{
		sprintf(pm.conn_mode,"ppp0");
		pm.netconn=1;

	}

	else if ( Get_Interface_Connected ("wlan0") == 0 )
	{
		sprintf(pm.conn_mode,"wlan0");
		pm.netconn=1;

	}

	else if ( Get_Interface_Connected("eth0") == 0 )
	{
		sprintf(pm.conn_mode,"wlan0");
		pm.netconn=1;
	}
	else
	{
		sprintf(pm.conn_mode,"Not Connected");
		pm.netconn=0;

	}
}

int Get_Interface_Connected ( char *interface_name )
{
	int fd=0;
	int ret=0;

	struct ifreq ifr;
	char gprsip[64]="";

	fd = socket(AF_INET,SOCK_DGRAM,0);

	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name,interface_name,IFNAMSIZ-1);

	ret =  ioctl(fd,SIOCGIFADDR,&ifr);

	close(fd);

	if (ret!=0)
		return -1;
	return 0;
}


int check_otg_ID_pin()
{
	int ret = 0;
	
	ret = system("cat /proc/driver/isp1504_otg | grep OTGSC | cut -d ':' -f2 | cut -c 7| grep f");	

	if ( ret == 0 )
	{
		return 1;
	}

	return 0;

}

int check_network_traffic()
{
	int get_pre_byte = 0;
	int get_post_byte = 0;
	
	get_network_info();

	if ( pm.netconn == 0 )
	{
		return 0;
	}

	get_pre_byte = get_data_packets_network(pm.conn_mode);

	if ( get_pre_byte == -1 )
	{
		fprintf(stdout,"Disconnected %s\n",pm.conn_mode);
		return 0;
	}
	
	sleep(pm_t.network_timeout);

        get_post_byte = get_data_packets_network(pm.conn_mode);
	
	if ( get_post_byte == -1 )
        {       
                fprintf(stdout,"Disconnected %s\n",pm.conn_mode);
                return 0;
        }

	if ( get_post_byte < get_pre_byte )
	{
		return 0;
	}	
	
	if ( ( get_post_byte - get_pre_byte) > pm_t.data_transfer  )
	{
		pm.data_transfer = pm_t.data_transfer;
		return 1;

	}	

	return 0;
	
}



int get_data_packets_network(char * interface)
{
	unsigned long int rx_bytes=0;

	unsigned long int tx_bytes=0;

	int ret=0;

	FILE *fp;

	char interface_file[100]="";

	memset(interface_file,0x0,sizeof(interface_file));

	sprintf(interface_file,"/sys/class/net/%s/statistics/rx_bytes",interface);

	fp = fopen(interface_file,"r");

	if (fp == NULL)

		return -1;

	ret = fscanf(fp,"%ld",&rx_bytes);

	fclose(fp);

	if(ret!=1)

		return -1;
	memset(interface_file,0x0,sizeof(interface_file));
	
	sprintf(interface_file,"/sys/class/net/%s/statistics/tx_bytes",interface);

	fp = fopen(interface_file,"r");

	if (fp == NULL)

		return -1;

	ret = fscanf(fp,"%ld",&tx_bytes);

	fclose(fp);

	if(ret!=1)

		return -1;

	fprintf(stdout,"Data used on %s interface in KB %d\n",interface_file,(unsigned int)((tx_bytes+rx_bytes)/1024));

	return (unsigned int)((tx_bytes+rx_bytes)/1024);
}


void print_all_data()
{
	char msg[512]="";

	sprintf(msg,"Network: %s,Data_Transfer :%d,Battery Vol:%d,RD_Serv_FP: %d,RD_Serv_Iri : %d",pm.conn_mode,pm.data_transfer,pm.batt_vol,pm.rd_service,pm.rd_service_iri);

	pm_log(msg);
		

}


void check_running_instance()
{
        int fd,ret=0;

        fd = open("/tmp/.pm.lock",O_CREAT | O_CLOEXEC);

        ret = flock(fd,LOCK_EX|LOCK_NB);

        if (ret == -1 && fd > 0)
        {
                close(fd);
                fprintf(stdout,"Another Instance of PM App Running\n");
                exit(-1);
        }
}
