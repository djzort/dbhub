#ifndef _MOTD_H_
#define _MOTD_H_

#define MOTD_FILE          "motd"          /*!< Name of file containing the motd */
#define MOTD_N          "motd_normal"          /* Name of file containing the motd */
#define MOTD_P          "motd_prot"          /* Name of file containing the motd */
#define MOTD_R          "motd_reg"          /* Name of file containing the motd */
#define MOTD_V          "motd_vip"          /* Name of file containing the motd */
#define MOTD_K          "motd_kvip"          /* Name of file containing the motd */
#define MOTD_O          "motd_op"          /* Name of file containing the motd */
#define MOTD_C          "motd_cheef"          /* Name of file containing the motd */
#define MOTD_A          "motd_admin"          /* Name of file containing the motd */
char *motd_mem;
char *motdn_mem;
char *motdp_mem;
char *motdr_mem;
char *motdv_mem;
char *motdk_mem;
char *motdo_mem;
char *motdc_mem;
char *motda_mem;

extern void LoadAllMOTDs(void);
extern void CreateAllMOTDs(void);
extern void FreeAllMOTDs(void);

#endif /* _MOTD_H_ */

