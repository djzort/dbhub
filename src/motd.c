#include "motd.h"
#include "utils.h"
#include "logger.h"
#include "fileio.h"
#include <stdlib.h>

void LoadAllMOTDs(void){
	loadfile2buf(&motd_mem, MOTD_FILE);
	loadfile2buf(&motdn_mem, MOTD_N);
	loadfile2buf(&motdp_mem, MOTD_P);
	loadfile2buf(&motdr_mem, MOTD_R);
	loadfile2buf(&motdv_mem, MOTD_V);
	loadfile2buf(&motdk_mem, MOTD_K);
	loadfile2buf(&motdo_mem, MOTD_O);
	loadfile2buf(&motdc_mem, MOTD_C);
	loadfile2buf(&motda_mem, MOTD_A);
}

static void CreateMOTDFile(const char *content, const char *filename){
	int ret;
	if( (ret=write_file((char *)content,(char *)filename, 0)) == -1) {
		logprintf(LOG_EMERG, "Failed creating %s file! Exiting",filename);
		exit(EXIT_FAILURE);
	}else if (ret==1) logprintf(LOG_NOTICE, "Created %s file", filename);
}

void CreateAllMOTDs(void){
	CreateMOTDFile("Welcome to the DB Hub.\r\nProject page: http://www.dbhub.org", "motd");	
	CreateMOTDFile(" ", "motdn");
	CreateMOTDFile(" ", "motdp");
	CreateMOTDFile(" ", "motdr");
	CreateMOTDFile(" ", "motdv");
	CreateMOTDFile(" ", "motdk");
	CreateMOTDFile(" ", "motdo");
	CreateMOTDFile(" ", "motdc");
	CreateMOTDFile(" ", "motda");
}

void FreeAllMOTDs(void){
	free(motd_mem);motd_mem=NULL;
	free(motdn_mem);motdn_mem=NULL;
	free(motdp_mem);motdp_mem=NULL;
	free(motdr_mem);motdr_mem=NULL;
	free(motdv_mem);motdv_mem=NULL;
	free(motdk_mem);motdk_mem=NULL;
	free(motdo_mem);motdo_mem=NULL;
	free(motdc_mem);motdc_mem=NULL;
	free(motda_mem);motda_mem=NULL;
}

