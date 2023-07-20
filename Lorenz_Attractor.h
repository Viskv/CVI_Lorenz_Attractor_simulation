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

#define  PANEL                            1
#define  PANEL_QUITBUTTON                 2       /* control type: command, callback function: QuitCallback */
#define  PANEL_TAB                        3       /* control type: tab, callback function: (none) */
#define  PANEL_LED                        4       /* control type: LED, callback function: (none) */
#define  PANEL_TIMER                      5       /* control type: timer, callback function: TIMER */

#define  PANEL_2                          2
#define  PANEL_2_Close_save_photo_win     2       /* control type: command, callback function: CloseWin */
#define  PANEL_2_CANVAS                   3       /* control type: canvas, callback function: (none) */
#define  PANEL_2_Save_3dgraph_photo       4       /* control type: command, callback function: Save_3dgraph_photo */

#define  PANEL_3                          3
#define  PANEL_3_STRING                   2       /* control type: string, callback function: (none) */
#define  PANEL_3_exel_save_bottom         3       /* control type: command, callback function: Save_run_2_exel */
#define  PANEL_3_TEXTMSG                  4       /* control type: textMsg, callback function: (none) */

     /* tab page panel controls */
#define  Dist_G_TAB_GRAPH                 2       /* control type: graph, callback function: diff_GRAPH_function */
#define  Dist_G_TAB_Corser_TOGGLEBUTTON   3       /* control type: textButton, callback function: Corser_on_off_TOGGLEBUTTON */
#define  Dist_G_TAB_NUMERIC_2             4       /* control type: numeric, callback function: (none) */
#define  Dist_G_TAB_NUMERIC               5       /* control type: numeric, callback function: (none) */
#define  Dist_G_TAB_log_x_RADIOBUTTON     6       /* control type: radioButton, callback function: log_x_RADIOBUTTON */
#define  Dist_G_TAB_Find_max_point_bottom 7       /* control type: command, callback function: Find_max_point_bottom */
#define  Dist_G_TAB_TEXTMSG               8       /* control type: textMsg, callback function: (none) */

     /* tab page panel controls */
#define  MainTAB_CWGRAPH3D                2       /* control type: activeX, callback function: (none) */
#define  MainTAB_CWGRAPH3D_2              3       /* control type: activeX, callback function: (none) */
#define  MainTAB_calc_LA                  4       /* control type: command, callback function: calc_LA */
#define  MainTAB_Open_Save_run_2_exel     5       /* control type: command, callback function: Save_run_2_exel */
#define  MainTAB_TEXTMSG_2                6       /* control type: textMsg, callback function: (none) */
#define  MainTAB_TEXTMSG                  7       /* control type: textMsg, callback function: (none) */
#define  MainTAB_TEXTMSG_3                8       /* control type: textMsg, callback function: (none) */
#define  MainTAB_Open_Save_right_Graph    9       /* control type: command, callback function: Save_3dgraph_photo */
#define  MainTAB_Open_Save_left_Graph     10      /* control type: command, callback function: Save_3dgraph_photo */
#define  MainTAB_Parametric_init_TABLE    11      /* control type: table, callback function: (none) */
#define  MainTAB_Init_place_TABLE         12      /* control type: table, callback function: (none) */
#define  MainTAB_Time_Step_RINGSLIDE      13      /* control type: slide, callback function: (none) */
#define  MainTAB_Number_Of_Steps_ring     14      /* control type: slide, callback function: (none) */
#define  MainTAB_Z_Y_GraphViewBUTTON      15      /* control type: command, callback function: GraphView_Function */
#define  MainTAB_X_Z_GraphViewBUTTON      16      /* control type: command, callback function: GraphView_Function */
#define  MainTAB_X_Y_GraphViewBUTTON      17      /* control type: command, callback function: GraphView_Function */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK calc_LA(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CloseWin(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Corser_on_off_TOGGLEBUTTON(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK diff_GRAPH_function(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Find_max_point_bottom(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK GraphView_Function(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK log_x_RADIOBUTTON(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Save_3dgraph_photo(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Save_run_2_exel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TIMER(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif