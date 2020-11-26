#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

int get_bat_status( float *filter_voltage)
{
	FILE *fp = NULL;
	int  adapter_adc = 0;
	int  battery_adc = 0;
	int adapter_status = 0;

	fp = fopen("/sys/devices/platform/imx6q-ecspi.1/spi_master/spi1/spi1.1/bat1","r");

        if (fp == NULL )
        {
                return -1;
        }

        fscanf(fp,"%d",&adapter_adc);
        fclose(fp);

	if ( adapter_adc >= 1000 )
		adapter_status = 1;


	fp = fopen("/sys/devices/platform/imx6q-ecspi.1/spi_master/spi1/spi1.1/bat2","r");

        if (fp == NULL )
        {
                return -1;
        }

        fscanf(fp,"%d",&battery_adc);
        fclose(fp);


	*filter_voltage = ( 8.4 * (battery_adc) ) / 1024;

	if( battery_adc > 400  && adapter_status ) 
	{
		return adapter_status;

	}
	
	return 0;
}

