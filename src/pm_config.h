struct pm_config
{
	int wakealarm;
	int batt_vol;
	int adpter;
	int otg;
	int  netconn;
	char  conn_mode[20];
	int rd_service;
	int rd_service_iri;
	int rhms_status;
	int data_transfer;
	
};

struct pm_config pm;


struct pm_time_config
{
	int bootdelay;

	int bkl_timeout;

	int suspend_timeout;

	int network_timeout;

	int data_transfer;

};

struct pm_time_config pm_t;



void get_battery_info();

void get_rd_info();

void get_network_info();

