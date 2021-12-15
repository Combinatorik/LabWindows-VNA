/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  CAL                              1
#define  CAL_INPUT_PHYS_CHANNEL           2       /* control type: string, callback function: updateVNAControllsCallback */
#define  CAL_DECORATION_7                 3       /* control type: deco, callback function: (none) */
#define  CAL_DECORATION_6                 4       /* control type: deco, callback function: (none) */
#define  CAL_TEXTMSG_6                    5       /* control type: textMsg, callback function: (none) */
#define  CAL_DECORATION                   6       /* control type: deco, callback function: (none) */
#define  CAL_TEXTMSG_5                    7       /* control type: textMsg, callback function: (none) */
#define  CAL_OUT_PHYS_CHANNEL             8       /* control type: string, callback function: updateVNAControllsCallback */
#define  CAL_TEXTMSG_3                    9       /* control type: textMsg, callback function: (none) */
#define  CAL_INPUT_CONFIG                 10      /* control type: ring, callback function: saveVNAPanelSettings */
#define  CAL_QUIT                         11      /* control type: command, callback function: quitCalPanel */
#define  CAL_DECORATION_5                 12      /* control type: deco, callback function: (none) */
#define  CAL_MAG_CHART                    13      /* control type: graph, callback function: (none) */
#define  CAL_PHASE_CHART                  14      /* control type: graph, callback function: (none) */
#define  CAL_FREQ_MSG                     15      /* control type: textMsg, callback function: (none) */
#define  CAL_START                        16      /* control type: command, callback function: startCal */
#define  CAL_STATUS_MSG                   17      /* control type: textMsg, callback function: (none) */
#define  CAL_STOP                         18      /* control type: command, callback function: stopCal */
#define  CAL_D_2                          19      /* control type: numeric, callback function: (none) */
#define  CAL_C_2                          20      /* control type: numeric, callback function: (none) */
#define  CAL_B_2                          21      /* control type: numeric, callback function: (none) */
#define  CAL_A_2                          22      /* control type: numeric, callback function: (none) */
#define  CAL_TEXTMSG_4                    23      /* control type: textMsg, callback function: (none) */
#define  CAL_SAVE                         24      /* control type: command, callback function: saveCal */
#define  CAL_D                            25      /* control type: numeric, callback function: (none) */
#define  CAL_C                            26      /* control type: numeric, callback function: (none) */
#define  CAL_B                            27      /* control type: numeric, callback function: (none) */
#define  CAL_A                            28      /* control type: numeric, callback function: (none) */

#define  MAIN                             2
#define  MAIN_VNA_BUTTON                  2       /* control type: command, callback function: launchVNA */
#define  MAIN_STATIC_RESPONSE             3       /* control type: command, callback function: launchStaticResponse */
#define  MAIN_DEVICE_CAL                  4       /* control type: command, callback function: launchCalPanel */
#define  MAIN_QUITBUTTON                  5       /* control type: command, callback function: QuitCallback */

#define  STATIC_RES                       3       /* callback function: staticResponsePanelCallback */
#define  STATIC_RES_R_INPUT_PHYS_CHANNEL  2       /* control type: string, callback function: updateSRDevices */
#define  STATIC_RES_INPUT_PHYS_CHANNEL    3       /* control type: string, callback function: updateSRDevices */
#define  STATIC_RES_TRIG_PHYS_CHANNEL     4       /* control type: string, callback function: updateSRDevices */
#define  STATIC_RES_OUT_PHYS_CHANNEL      5       /* control type: string, callback function: updateSRDevices */
#define  STATIC_RES_INPUT_VOLTAGE         6       /* control type: numeric, callback function: adjustSRVoltageBounds */
#define  STATIC_RES_OUTPUT_FREQ           7       /* control type: numeric, callback function: saveSRPanel */
#define  STATIC_RES_FIT_POS               8       /* control type: ring, callback function: saveSRPanel */
#define  STATIC_RES_INPUT_CONFIG          9       /* control type: ring, callback function: saveSRPanel */
#define  STATIC_RES_SIGNAL_START_V        10      /* control type: numeric, callback function: adjustSRVoltageBounds */
#define  STATIC_RES_SIGNAL_STOP_V         11      /* control type: numeric, callback function: adjustSRVoltageBounds */
#define  STATIC_RES_STEPS                 12      /* control type: numeric, callback function: saveSRPanel */
#define  STATIC_RES_START                 13      /* control type: command, callback function: startStaticResponse */
#define  STATIC_RES_STOP                  14      /* control type: command, callback function: stopStaticResponse */
#define  STATIC_RES_SAVE                  15      /* control type: command, callback function: saveStaticResponse */
#define  STATIC_RES_QUIT                  16      /* control type: command, callback function: quitStaticResp */
#define  STATIC_RES_DECORATION_5          17      /* control type: deco, callback function: (none) */
#define  STATIC_RES_DECORATION_4          18      /* control type: deco, callback function: (none) */
#define  STATIC_RES_DECORATION_7          19      /* control type: deco, callback function: (none) */
#define  STATIC_RES_DECORATION            20      /* control type: deco, callback function: (none) */
#define  STATIC_RES_TEXTMSG               21      /* control type: textMsg, callback function: (none) */
#define  STATIC_RES_TEXTMSG_5             22      /* control type: textMsg, callback function: (none) */
#define  STATIC_RES_TEXTMSG_3             23      /* control type: textMsg, callback function: (none) */
#define  STATIC_RES_MAG_CHART             24      /* control type: graph, callback function: (none) */
#define  STATIC_RES_PHASE_CHART           25      /* control type: graph, callback function: (none) */
#define  STATIC_RES_VOLT_MSG              26      /* control type: textMsg, callback function: (none) */
#define  STATIC_RES_STATUS_MSG            27      /* control type: textMsg, callback function: (none) */
#define  STATIC_RES_TEXTMSG_4             28      /* control type: textMsg, callback function: (none) */
#define  STATIC_RES_USE_TRIG_CHAN         29      /* control type: radioButton, callback function: saveSRPanel */
#define  STATIC_RES_SETTLE_TIME           30      /* control type: numeric, callback function: saveSRPanel */
#define  STATIC_RES_DELAY_TIME            31      /* control type: numeric, callback function: saveSRPanel */
#define  STATIC_RES_SIGNALS_CHART         32      /* control type: graph, callback function: (none) */
#define  STATIC_RES_RATIO                 33      /* control type: numeric, callback function: adjustSRVoltageBounds */
#define  STATIC_RES_GAIN                  34      /* control type: numeric, callback function: adjustSRVoltageBounds */
#define  STATIC_RES_TRIALS                35      /* control type: numeric, callback function: (none) */

#define  VNA                              4       /* callback function: vnaPanelCallback */
#define  VNA_TRIG_PHYS_CHANNEL            2       /* control type: string, callback function: updateVNAControllsCallback */
#define  VNA_R_INPUT_PHYS_CHANNEL         3       /* control type: string, callback function: updateVNAControllsCallback */
#define  VNA_INPUT_PHYS_CHANNEL           4       /* control type: string, callback function: updateVNAControllsCallback */
#define  VNA_OUT_PHYS_CHANNEL             5       /* control type: string, callback function: updateVNAControllsCallback */
#define  VNA_INPUT_VOLTAGE                6       /* control type: numeric, callback function: adjustVoltageBounds */
#define  VNA_OUTPUT_VOLTAGE               7       /* control type: numeric, callback function: adjustVoltageBounds */
#define  VNA_FIT_POS                      8       /* control type: ring, callback function: saveVNAPanelSettings */
#define  VNA_INPUT_CONFIG                 9       /* control type: ring, callback function: saveVNAPanelSettings */
#define  VNA_SIGNAL_START_FREQ            10      /* control type: numeric, callback function: vnaFreqBoundsCallback */
#define  VNA_SIGNAL_STOP_FREQ             11      /* control type: numeric, callback function: vnaFreqBoundsCallback */
#define  VNA_STEPS                        12      /* control type: numeric, callback function: saveVNAPanelSettings */
#define  VNA_SIG_TYPE                     13      /* control type: ring, callback function: saveVNAPanelSettings */
#define  VNA_START                        14      /* control type: command, callback function: startVNACallback */
#define  VNA_STOP                         15      /* control type: command, callback function: stopVNACallback */
#define  VNA_SAVE                         16      /* control type: command, callback function: saveVNADataCallback */
#define  VNA_QUIT                         17      /* control type: command, callback function: quitVNACallback */
#define  VNA_DECORATION_5                 18      /* control type: deco, callback function: (none) */
#define  VNA_DECORATION_4                 19      /* control type: deco, callback function: (none) */
#define  VNA_DECORATION_6                 20      /* control type: deco, callback function: (none) */
#define  VNA_DECORATION                   21      /* control type: deco, callback function: (none) */
#define  VNA_TEXTMSG                      22      /* control type: textMsg, callback function: (none) */
#define  VNA_TEXTMSG_5                    23      /* control type: textMsg, callback function: (none) */
#define  VNA_TEXTMSG_3                    24      /* control type: textMsg, callback function: (none) */
#define  VNA_MAG_CHART                    25      /* control type: graph, callback function: (none) */
#define  VNA_PHASE_CHART                  26      /* control type: graph, callback function: (none) */
#define  VNA_FREQ_MSG                     27      /* control type: textMsg, callback function: (none) */
#define  VNA_STATUS_MSG                   28      /* control type: textMsg, callback function: (none) */
#define  VNA_TEXTMSG_4                    29      /* control type: textMsg, callback function: (none) */
#define  VNA_USE_TRIG_CHAN                30      /* control type: radioButton, callback function: saveVNAPanelSettings */
#define  VNA_SIGNALS_CHART                31      /* control type: graph, callback function: (none) */
#define  VNA_USE_REF_CHAN                 32      /* control type: radioButton, callback function: updateVNAControllsCallback */
#define  VNA_DELAY_TIME                   33      /* control type: numeric, callback function: saveVNAPanelSettings */
#define  VNA_RATIO                        34      /* control type: numeric, callback function: adjustVoltageBounds */
#define  VNA_AVERAGES                     35      /* control type: numeric, callback function: saveVNAPanelSettings */
#define  VNA_GAIN                         36      /* control type: numeric, callback function: adjustVoltageBounds */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK adjustSRVoltageBounds(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK adjustVoltageBounds(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK launchCalPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK launchStaticResponse(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK launchVNA(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quitCalPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quitStaticResp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quitVNACallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK saveCal(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK saveSRPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK saveStaticResponse(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK saveVNADataCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK saveVNAPanelSettings(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK startCal(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK startStaticResponse(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK startVNACallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK staticResponsePanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK stopCal(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK stopStaticResponse(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK stopVNACallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK updateSRDevices(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK updateVNAControllsCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK vnaFreqBoundsCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK vnaPanelCallback(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
