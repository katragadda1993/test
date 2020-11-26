#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include "pm_config.h"


int main(void)
{
	int ret_suspend =0 ;

	char msg[256]="";

	int ret_net = 0;
	int ret_otg = 0;

	memset(msg,0x0,sizeof(msg));

	sprintf(msg,"PM App Verion 1.0 Last Compiled %s %s",__DATE__,__TIME__);

	pm_log(msg);

	check_running_instance();

	/* Get PM configuration from /etc/pm_config.xml */

	get_pm_config_settings();


	memset(&pm,0x0,sizeof(pm));

	/* Wait until  mins from device uptime */

	memset(msg,0x0,sizeof(msg));

	sprintf(msg,"Waiting Initial Boot delay %d min", pm_t.bootdelay);

	pm_log(msg);

	wait_for_bootdelay(pm_t.bootdelay);

	memset(msg,0x0,sizeof(msg));

	sprintf(msg,"Initial Boot delay completed %d min", pm_t.bootdelay);

	pm_log(msg);

	get_network_rd_init();

	while(1)
	{
		ret_suspend = check_suspend_mode(pm_t.bkl_timeout); // if backlight continously off in mins
		
		printf("ret_suspend == %d\n",ret_suspend);

		if ( ret_suspend  == 1 )
		{
			ret_net = check_network_traffic();

			ret_otg = check_otg_ID_pin();

			get_battery_info();

			ret_suspend = check_suspend_mode(1);

			if ( ret_suspend == 1 && ret_net == 0 && ret_otg == 0 )
			{
				
				print_all_data();
				
				printf("Entering Sleep mode...\n");
				
				enable_sleep_mode();

				wakeup_power_off_devices();

				ret_suspend = 0;

			}

			sleep (10);
		}
		else
		{
			printf("ret_suspend == %d\n",ret_suspend);
check_suspend_mode
		}

		sleep(10);	
	}

	return 0;
}


