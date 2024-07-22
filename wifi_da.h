/*
 * resource set indices for wifi_da
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        31
 * Number of Bitblks:        8
 * Number of Iconblks:       1
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       3
 * Number of Free Strings:   8
 * Number of Free Images:    7
 * Number of Objects:        21
 * Number of Trees:          3
 * Number of Userblks:       0
 * Number of Images:         10
 * Total file size:          1918
 */

#ifdef RSC_NAME
#undef RSC_NAME
#endif
#ifndef __ALCYON__
#define RSC_NAME "wifi_da"
#endif
#ifdef RSC_ID
#undef RSC_ID
#endif
#ifdef wifi_da
#define RSC_ID wifi_da
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 31
#define NUM_FRSTR 8
#define NUM_UD 0
#define NUM_IMAGES 10
#define NUM_BB 8
#define NUM_FRIMG 7
#define NUM_IB 1
#define NUM_CIB 0
#define NUM_TI 3
#define NUM_OBS 21
#define NUM_TREE 3
#endif



#define ABOUT_DIALOG_ID                    0 /* form/dialog */
#define ICON                               1 /* ICON in tree ABOUT_DIALOG_ID */
#define VERSION                            3 /* STRING in tree ABOUT_DIALOG_ID */
#define ABOUT_OK                           8 /* BUTTON in tree ABOUT_DIALOG_ID */

#define PASSWORD_DIALOG_ID                 1 /* form/dialog */
#define PASSWORD_DIALOG_CANCEL             3 /* BUTTON in tree PASSWORD_DIALOG_ID */
#define PASSWORD_DIALOG_CONNECT            4 /* BUTTON in tree PASSWORD_DIALOG_ID */
#define PASSWORD_DIALOG_SSID               5 /* TEXT in tree PASSWORD_DIALOG_ID */
#define PASSWORD_DIALOG_PASSWORD           7 /* FTEXT in tree PASSWORD_DIALOG_ID */

#define WIN_ID                             2 /* form/dialog */
#define WIN_SIGNAL                         1 /* IMAGE in tree WIN_ID */
#define WIN_SCSI_ID                        2 /* BOXTEXT in tree WIN_ID */

#define VERSION_STR                        0 /* Free string */

#define AL_ASK                             1 /* Free string */

#define AL_WARN                            2 /* Free string */

#define WIN_TITLE                          3 /* Free string */

#define NO_SCSI_DRVR                       4 /* Free string */

#define NO_DEVICE_TEXT                     5 /* Free string */

#define NO_NETWORK_TEXT                    6 /* Free string */

#define STR_FINDING                        7 /* Free string */

#define SIGNAL_NO_DEVICE                   0 /* Free image */

#define SIGNAL_FINDING_DEVICE              1 /* Free image */

#define SIGNAL_NONE                        2 /* Free image */

#define SIGNAL_1                           3 /* Free image */

#define SIGNAL_2                           4 /* Free image */

#define SIGNAL_3                           5 /* Free image */

#define SIGNAL_4                           6 /* Free image */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD wifi_da_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD wifi_da_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD wifi_da_rsc_free(void);
#endif
