#include<string.h>
#include<fcntl.h>
#include<stdio.h>
#include<fcntl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <unistd.h>
#include"pm_config.h"


#define  BOOT_DELAY "10"
#define  BKL_TIMEOUT "2"
#define  SUSPEND_TIMEOUT "3"
#define  NETWORK_TIMEOUT "10"
#define  DATA_TRANSFER   "20"


int parse_xml_file(char *docname);

#define  PM_CONFIG_FILE "/etc/pm_config.xml"



void get_pm_config_settings()
{

	memset(&pm_t,0x0,sizeof(pm_t));

	if ( read_pm_config_xml() != 0)
	{
		default_pm_xml();
		read_pm_config_xml();
	}


	if ( pm_t.bootdelay <= 0 )
                pm_t.bootdelay=atoi(BOOT_DELAY);

	if ( pm_t.bkl_timeout <= 0 )
		pm_t.bkl_timeout=atoi(BKL_TIMEOUT);

	if ( pm_t.suspend_timeout <= 0 )
		pm_t.suspend_timeout=atoi(SUSPEND_TIMEOUT);

	if ( pm_t.network_timeout <= 0 )
		pm_t.network_timeout=atoi(NETWORK_TIMEOUT);

	if ( pm_t.data_transfer <= 0 )
		pm_t.data_transfer=atoi(DATA_TRANSFER);


}

int default_pm_xml()
{
	
	remove(PM_CONFIG_FILE);

	char buff[100];

	memset(buff,0x0,sizeof(buff));

	xmlDocPtr doc = NULL;       /* document pointer */
	xmlNodePtr node =NULL;/* node pointers */

	LIBXML_TEST_VERSION;

	doc = xmlNewDoc(BAD_CAST "1.0");
	node = xmlNewNode(NULL,(xmlChar*)"PM_CONFIG");
	xmlDocSetRootElement(doc, node);

	/* PM_CONFIGURATION */

	xmlNewChild(node, NULL, BAD_CAST "BOOT_DELAY",BAD_CAST BOOT_DELAY );

	xmlNewChild(node, NULL, BAD_CAST "BKL_TIMEOUT",BAD_CAST BKL_TIMEOUT);

	xmlNewChild(node, NULL, BAD_CAST "SUSPEND_TIMEOUT",BAD_CAST SUSPEND_TIMEOUT);

	xmlNewChild(node, NULL, BAD_CAST "NETWORK_TIMEOUT",BAD_CAST NETWORK_TIMEOUT);

	xmlNewChild(node, NULL, BAD_CAST "DATA_TRANSFER", BAD_CAST DATA_TRANSFER);

	xmlSaveFormatFileEnc(PM_CONFIG_FILE, doc, "UTF-8", 1);

	xmlFreeDoc(doc);

	xmlCleanupParser();

	return 0;

}



int parse_xml_file(char *docname)
{


	xmlDocPtr doc;
	xmlNodePtr cur;
	doc = xmlParseFile(docname);

	if (doc == NULL)
	{
		printf("Document Parsing failed. \n");
		return -1;
	}
	cur = xmlDocGetRootElement(doc);

	if (cur == NULL)
	{
		xmlFreeDoc(doc);
		printf("Document is Empty!!!\n");
		return -1;
	}

	return 0;

}

int read_pm_config_xml()
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *key;
	int ret_flag=0;


	if(access(PM_CONFIG_FILE,F_OK)!=0)
	{
		fprintf(stderr,"File %s Not Present\n",PM_CONFIG_FILE);
		return -1;
	}

	doc = xmlParseFile(PM_CONFIG_FILE);

	if (doc == NULL)
	{
		printf("Document Parsing failed. \n");
		return -1;
	}
	cur = xmlDocGetRootElement(doc);

	if (cur == NULL)
	{
		xmlFreeDoc(doc);
		printf("Document is Empty!!!\n");
		return -1;
	}


	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"BOOT_DELAY")))
		{

			key = xmlNodeListGetString(doc, cur->xmlChildrenNode , 1);
			if (key != NULL)
			{
				pm_t.bootdelay=atoi((char*)key);	
			}

		}

		if ((!xmlStrcmp(cur->name, (const xmlChar *)"BKL_TIMEOUT")))
		{

			key = xmlNodeListGetString(doc, cur->xmlChildrenNode , 1);
			if (key != NULL)
			{
				pm_t.bkl_timeout=atoi((char*)key);
			}

		}




		if ((!xmlStrcmp(cur->name, (const xmlChar *)"SUSPEND_TIMEOUT")))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode , 1);

			if (key != NULL)
			{
				pm_t.suspend_timeout=atoi((char*)key);
			}

		}

		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"NETWORK_TIMEOUT")))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode , 1);

			if (key != NULL)
			{
				pm_t.network_timeout=atoi((char*)key);
			}


		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"DATA_TRANSFER")))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode , 1);

			if (key != NULL)
			{
				pm_t.data_transfer=atoi((char*)key);
			}

		}

		cur = cur->next;
	}
	xmlFreeDoc(doc);
	return ret_flag;

}
