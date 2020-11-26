#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include<sys/time.h>
#include<stdio.h>
#include<string.h>


void get_date(int *day,int *month,int *year,int *hrs,int *mins,int *secs)
{
	struct tm *intim;
	struct tm my_tm;
	struct timeval tv;
	intim = &my_tm;

	memset(intim,0,sizeof(struct tm));

	gettimeofday (&tv,NULL);

	intim = localtime (&tv.tv_sec) ;

	*day=intim->tm_mday;
	*month=intim->tm_mon+1;
	*year=intim->tm_year+1900;
	*hrs=intim->tm_hour;
	*mins=intim->tm_min;
	*secs=intim->tm_sec;

	return;
}
void pm_log(char *msg)
{
	FILE *fp;

	int day=0,month=0,year=0,hrs=0,mins=0,secs=0;
	
	char log_file[128]="";

	get_date(&day,&month,&year,&hrs,&mins,&secs);

	sprintf(log_file,"/var/log/pm.log");

	fp = fopen(log_file,"a");
	if( fp == NULL)
	{
		fprintf(stderr," %s open failed\n",log_file);
		return;
	}

	fprintf(fp,"[ %02d/%02d/%04d-%02d:%02d:%02d ]\t%s\n",day,month,year,hrs,mins,secs,msg);

	fprintf(stdout,"[ %02d/%02d/%04d-%02d:%02d:%02d ]\t%s\n",day,month,year,hrs,mins,secs,msg); 

	fclose(fp);

	return ;
}

