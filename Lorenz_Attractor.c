#include "cw3dgrph.h"
#include "excelreport.h"
#include "excel.h"
#include "toolbox.h"
#include <math.h>
#include <cvirte.h>		
#include <userint.h>
#include "Lorenz_Attractor.h"

#ifdef NAN
/* NAN is supported */
#endif
#ifdef INFINITY
/* INFINITY is supported */
#endif

#define PANEL_panel2 1
#define PANEL_EX_PANEL 8
#define PANEL_OpenEX_PANEL 3
#define MAXNUM 1000

typedef struct // point struct
{
    double x;
    double y;
    double z;
    double t;
} point;

typedef struct // thread struct
{
	point *res;
	double sigma;
	double rho;
	double beta;
	CmtThreadFunctionID threadid;
} thread_data,*functionData;

typedef struct
{
	double* x1,*x2,*y1,*y2,*z1,*z2,*distans,*t;
}excelData;

static int panelHandle2, panelHandle, panelHandle3,tabHandleMain,tabHandleDiff;
static int clk = 0, CorserOnOff=0, bitmapid = 0;
static double dt = 0.01 ;
static int stepsNum = 100;
char Save_File_Path[500];
thread_data *tdm1,*tdm2;
double MaxPointX, MaxPointY;
static excelData finitData;
static CAObjHandle objectHandle1, objectHandle2 , workbookHandle , applicationHandle, rawDataWorkSheetHandle;
double calc2PointDistance(point point1, point point2);
int CVICALLBACK tr1 (void *functionData);
int CVICALLBACK tr2 (void *functionData);

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle2 = LoadPanel (0, "Lorenz_Attractor.uir", PANEL_2)) < 0)
		return -1;
	if ((panelHandle = LoadPanel (0, "Lorenz_Attractor.uir", PANEL)) < 0)
		return -1;
	
	// Get the handles for the tabbed panels
	GetPanelHandleFromTabPage (panelHandle, PANEL_TAB, 0, &tabHandleMain);
	GetPanelHandleFromTabPage (panelHandle, PANEL_TAB, 1, &tabHandleDiff);
	if ((panelHandle3 = LoadPanel (0, "Lorenz_Attractor.uir", PANEL_3)) < 0)
		return -1;
	tdm1 = malloc(sizeof(thread_data));
	tdm2 = malloc(sizeof(thread_data));
	
	// Setup 3D-graph objects and initialize:
	GetObjHandleFromActiveXCtrl (tabHandleMain, MainTAB_CWGRAPH3D, &objectHandle1);
	CW3DGraphLib_CWPlots3DItem (objectHandle1, NULL, CA_VariantInt(1),&objectHandle1);
	GetObjHandleFromActiveXCtrl (tabHandleMain, MainTAB_CWGRAPH3D_2, &objectHandle2);
	CW3DGraphLib_CWPlots3DItem (objectHandle2, NULL, CA_VariantInt(1),&objectHandle2);
	
	DisplayPanel (panelHandle); // Display only the main panel
	RunUserInterface (); // Start the User Interface
	// discard all pan:
	DiscardPanel (panelHandle2);
	DiscardPanel (panelHandle);
	DiscardPanel (panelHandle3);
	return 0;
}

int CVICALLBACK CloseWin (int panel, int control, int event,void *callbackData, int eventData1, int eventData2) // close extra paneles [Done]
{
	switch (event)
	{
		case EVENT_COMMIT:
		switch (panel)
		{
			case PANEL_panel2:
				HidePanel (panelHandle2); 
			break;	
		
			case PANEL_EX_PANEL:
				HidePanel (panelHandle3); 
			break;
		}
	}
	return 0;
}

int CVICALLBACK QuitCallback (int panel, int control, int event,void *callbackData, int eventData1, int eventData2) // Close Program and free pointer
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(bitmapid) DiscardBitmap(bitmapid);
			if (tdm1->res != NULL) free(tdm1);
			if (tdm2->res != NULL) free(tdm2);
			if (finitData.distans != NULL) free(finitData.distans);
			if (finitData.t != NULL) free(finitData.t);
			if (finitData.x1 != NULL) free(finitData.x1);
			if (finitData.x2 != NULL) free(finitData.x2);
			if (finitData.y1 != NULL) free(finitData.y1);
			if (finitData.y2 != NULL) free(finitData.y2);
			if (finitData.z1 != NULL) free(finitData.z1);
			if (finitData.z2 != NULL) free(finitData.z2);
			CA_DiscardObjHandle(objectHandle1);
			CA_DiscardObjHandle(objectHandle2);
			CA_DiscardObjHandle(workbookHandle);
			CA_DiscardObjHandle(applicationHandle);
			CA_DiscardObjHandle(rawDataWorkSheetHandle);
			QuitUserInterface (0);
		break;
	}
	return 0;
}

int CVICALLBACK Save_run_2_exel (int panel, int control, int event,void *callbackData, int eventData1, int eventData2) // Save run data to exel file [Done]
{
	switch (event)
	{
		case EVENT_COMMIT:
			if (panel == PANEL_OpenEX_PANEL) DisplayPanel (panelHandle3);
			
			if(panel == PANEL_EX_PANEL)
			{
				char FileName[5000];
				double x1[stepsNum], y1[stepsNum], z1[stepsNum], x2[stepsNum], y2[stepsNum], z2[stepsNum],t[stepsNum],d[stepsNum];
				
				// Get Data From Graph Tab Panel
				for(int i = 0; i < stepsNum ; i++)
				{
					 x1[i] = finitData.x1[i];
					 y1[i] = finitData.y1[i];
					 z1[i] = finitData.z1[i];
					 x2[i] = finitData.x2[i];
					 y2[i] = finitData.y2[i];
					 z2[i] = finitData.z2[i];
					 t[i] =finitData.t[i];
					 d[i] = finitData.distans[i];
				}
				GetCtrlVal (panelHandle3, PANEL_3_STRING, FileName);
				
				ExcelRpt_ApplicationNew(VTRUE,&applicationHandle);
				ExcelRpt_WorkbookNew(applicationHandle,&workbookHandle);
				
				//Get the worksheet handle. You can also get the worksheet handle by the name of the worksheet using ExcelRpt_GetWorksheetFromName():
				ExcelRpt_GetWorksheetFromIndex(workbookHandle,1,&rawDataWorkSheetHandle);
				
				//Rename the worksheet to “Analysis Data”:
				ExcelRpt_SetWorksheetAttribute(rawDataWorkSheetHandle,ER_WS_ATTR_NAME,"Analysis Data");
				
				//define the channel header text:
				char* channelHeaders[] = {"P1_X_Axis","P1_Y_Axis","P1_Z_Axis", "P2_X_Axis","P2_Y_Axis","P2_Z_Axis", "Two_Points_Distance", "Time"};
				ExcelRpt_WriteData (rawDataWorkSheetHandle, "A1:H1", CAVT_CSTRING, 1, 8, channelHeaders);
				ExcelRpt_WriteData (rawDataWorkSheetHandle, "A2:A1001", CAVT_DOUBLE, stepsNum, 1, x1);
				ExcelRpt_WriteData (rawDataWorkSheetHandle,"B2:B1001",CAVT_DOUBLE ,stepsNum,1, y1);
				ExcelRpt_WriteData (rawDataWorkSheetHandle,"C2:C1001",CAVT_DOUBLE ,stepsNum,1, z1);
				ExcelRpt_WriteData (rawDataWorkSheetHandle,"D2:D1001",CAVT_DOUBLE ,stepsNum,1, x2);
				ExcelRpt_WriteData (rawDataWorkSheetHandle,"E2:E1001",CAVT_DOUBLE ,stepsNum,1, y2);
				ExcelRpt_WriteData (rawDataWorkSheetHandle,"F2:F1001", CAVT_DOUBLE, stepsNum, 1, z2);
				ExcelRpt_WriteData (rawDataWorkSheetHandle,"G2:G1001",CAVT_DOUBLE ,stepsNum,1,d);
				ExcelRpt_WriteData (rawDataWorkSheetHandle,"H2:H1001",CAVT_DOUBLE ,stepsNum,1,t);
				
				// Save data:
				VARIANT vFilename;
				CA_VariantSetCString(&vFilename, FileName);
				Excel_WorkbookSaveAs (workbookHandle, NULL,vFilename , CA_DEFAULT_VAL, CA_DEFAULT_VAL, CA_DEFAULT_VAL, CA_DEFAULT_VAL, CA_DEFAULT_VAL,
								  ExcelConst_xlNoChange, CA_DEFAULT_VAL, CA_DEFAULT_VAL, CA_DEFAULT_VAL, CA_DEFAULT_VAL, CA_DEFAULT_VAL, CA_DEFAULT_VAL);
				// Close EXCEL & popup messege:
				ExcelRpt_ApplicationQuit (applicationHandle);
				MessagePopup ("Complte", "------------The Data Was Saved To Excel------------\n The File is Saved at 'Documents' Folder on your PC");
				HidePanel (panelHandle3);
			}
		break;
	}
	return 0;
}

int CVICALLBACK Save_3dgraph_photo (int panel, int control, int event,void *callbackData, int eventData1, int eventData2) // save one of the graphs as image [Done]
{
	// This function is called when the user interacts with the "Save Photo" button on the GUI.
	switch (event)
	{
		case EVENT_COMMIT:
		switch (control)
		{
			case MainTAB_Open_Save_right_Graph: // The user clicked the "Save Photo" button for the right graph
				if(bitmapid) DiscardBitmap(bitmapid);
				// Save the display of the right 3D graph control to a temporary bitmap file
				SaveCtrlDisplayToFile (tabHandleMain, MainTAB_CWGRAPH3D_2, 0, -1, -1, "C:\\Users\\Public\\Documents\\dis_bitmapfile.bmp");
				// Get the bitmap from the saved file and store its ID in 'bitmapid'
				GetBitmapFromFile ("C:\\Users\\Public\\Documents\\dis_bitmapfile.bmp", &bitmapid);
				// Remove the temporary bitmap file
				RemoveFileIfExists ("C:\\Users\\Public\\Documents\\dis_bitmapfile.bmp");
				// Draw the saved bitmap onto the canvas of panelHandle2, which may be used to display images
				CanvasDrawBitmap (panelHandle2, PANEL_2_CANVAS, bitmapid, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT);
				DisplayPanel (panelHandle2);
			break;
			//======================================
			case MainTAB_Open_Save_left_Graph:
				if(bitmapid) DiscardBitmap(bitmapid);
				// Save the display of the right 3D graph control to a temporary bitmap file
				SaveCtrlDisplayToFile (tabHandleMain, MainTAB_CWGRAPH3D, 0, -1, -1, "C:\\Users\\Public\\Documents\\dis_bitmapfile.bmp");
				// Get the bitmap from the saved file and store its ID in 'bitmapid'
				GetBitmapFromFile ("C:\\Users\\Public\\Documents\\dis_bitmapfile.bmp", &bitmapid);
				// Remove the temporary bitmap file
				RemoveFileIfExists ("C:\\Users\\Public\\Documents\\dis_bitmapfile.bmp");
				// Draw the saved bitmap onto the canvas of panelHandle2, which may be used to display images
				CanvasDrawBitmap (panelHandle2, PANEL_2_CANVAS, bitmapid, VAL_ENTIRE_OBJECT, VAL_ENTIRE_OBJECT);
				DisplayPanel (panelHandle2);
			break;
		}
		// If the current panel is panelHandle2 (where the saved image is displayed), proceed to save the image
		if(panel == PANEL_panel2)
		{
		int f_error;
		// Display a file dialog box to prompt the user for the location to save the image
		f_error = FileSelectPopup ("", "Untiteled.png", "(*.png; *.jpg; *.tiff);", "Save Image", VAL_SAVE_BUTTON, 0, 0, 1, 1, Save_File_Path);
			if (f_error)
			{
				// Save the current content of the canvas (the displayed image) to the specified file path
				SaveCtrlDisplayToFile (panelHandle2, PANEL_2_CANVAS, 0, -1, -1, Save_File_Path);
			}
		}
		break;
	}
	return 0;
}

int CVICALLBACK calc_LA (int panel, int control, int event,void *callbackData, int eventData1, int eventData2) // L.A Calculation Main [Done]
{
	switch (event)
	{
		case EVENT_COMMIT:
		// Get number of steps + interval
		GetCtrlVal (tabHandleMain, MainTAB_Number_Of_Steps_ring, &stepsNum);
		GetCtrlVal (tabHandleMain, MainTAB_Time_Step_RINGSLIDE, &dt);
		//---------------------------------------------------------------------------------------
		// Get each graph Parameters:
		GetTableCellVal (tabHandleMain, MainTAB_Parametric_init_TABLE, MakePoint (1,1),&tdm1->sigma);
		GetTableCellVal (tabHandleMain, MainTAB_Parametric_init_TABLE, MakePoint (2,1),&tdm1->rho);
		GetTableCellVal (tabHandleMain, MainTAB_Parametric_init_TABLE, MakePoint (3,1),&tdm1->beta);
		GetTableCellVal (tabHandleMain, MainTAB_Parametric_init_TABLE, MakePoint (1,2),&tdm2->sigma);
		GetTableCellVal (tabHandleMain, MainTAB_Parametric_init_TABLE, MakePoint (2,2),&tdm2->rho);
		GetTableCellVal (tabHandleMain, MainTAB_Parametric_init_TABLE, MakePoint (3,2),&tdm2->beta);
		//---------------------------------------------------------------------------------------
		// Get starting position:
		point points1[stepsNum], points2[stepsNum];
		points1[0].t=0;
		points2[0].t=0;
		
		GetTableCellVal (tabHandleMain, MainTAB_Init_place_TABLE, MakePoint (1,1),&points1[0].x);
		GetTableCellVal (tabHandleMain, MainTAB_Init_place_TABLE, MakePoint (2,1),&points1[0].y);
		GetTableCellVal (tabHandleMain, MainTAB_Init_place_TABLE, MakePoint (3,1),&points1[0].z);
		GetTableCellVal (tabHandleMain, MainTAB_Init_place_TABLE, MakePoint (1,2),&points2[0].x);
		GetTableCellVal (tabHandleMain, MainTAB_Init_place_TABLE, MakePoint (2,2),&points2[0].y);
		GetTableCellVal (tabHandleMain, MainTAB_Init_place_TABLE, MakePoint (3,2),&points2[0].z);
		//---------------------------------------------------------------------------------------
		// Start Paralel Calc:
		tdm1->res = malloc(sizeof(point)*stepsNum);
		tdm2->res = malloc(sizeof(point)*stepsNum);
		memcpy(tdm1->res,points1,sizeof(points1));
		memcpy(tdm2->res,points2,sizeof(points2));
		CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, tr1, &tdm1, &tdm1->threadid);
		CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, tr2, &tdm2, &tdm2->threadid);
		if(tdm1->threadid && tdm2->threadid) //  tdm1->threadid && tdm2->threadid 
		{
			CmtWaitForThreadPoolFunctionCompletionEx (DEFAULT_THREAD_POOL_HANDLE, tdm1->threadid, OPT_TP_PROCESS_EVENTS_WHILE_WAITING, CMT_WAIT_FOREVER);
			CmtWaitForThreadPoolFunctionCompletionEx (DEFAULT_THREAD_POOL_HANDLE, tdm2->threadid, OPT_TP_PROCESS_EVENTS_WHILE_WAITING, CMT_WAIT_FOREVER);
			CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE, tdm1->threadid);
			CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE, tdm2->threadid);
			tdm1->threadid=0;
			tdm2->threadid=0;
		}
		//---------------------------------------------------------------------------------------
		// Get & Print The Distance between 2 point at any given time between 2 runs:
		double distance[stepsNum], timePoint[stepsNum];
		for(int i = 0; i < stepsNum ; i++)
		{
			distance[i] = calc2PointDistance(tdm1->res[i],tdm2->res[i]);
			timePoint[i] = i * dt;
			if(i == 0) {MaxPointY = distance[i]; MaxPointX = 0;}
			else if (distance[i] > MaxPointY) {MaxPointX = timePoint[i]; MaxPointY = distance[i];} // get abs Max Point
		}
		DeleteGraphPlot (tabHandleDiff, Dist_G_TAB_GRAPH, -1, VAL_IMMEDIATE_DRAW);
		PlotXY (tabHandleDiff, Dist_G_TAB_GRAPH, timePoint, distance, stepsNum, VAL_DOUBLE, VAL_DOUBLE, VAL_FAT_LINE, VAL_SOLID_CIRCLE, VAL_SOLID, (dt*10), 0x00FFCC00);
		//----------------------------------------------------------------------------------------
		// Show on 3D-Graph:
		double x1[stepsNum], y1[stepsNum], z1[stepsNum], x2[stepsNum], y2[stepsNum], z2[stepsNum];
		VARIANT var_x1, var_y1, var_z1, var_x2, var_y2, var_z2;
		for(int i = 0; i < stepsNum ; i++)
		{
		 x1[i] = tdm1->res[i].x;
		 y1[i] = tdm1->res[i].y;
		 z1[i] = tdm1->res[i].z;
		 x2[i] = tdm2->res[i].x;
		 y2[i] = tdm2->res[i].y;
		 z2[i] = tdm2->res[i].z;
		}
		
		finitData.x1 = malloc(sizeof(x1));
		finitData.x2 = malloc(sizeof(x2));
		finitData.y1 = malloc(sizeof(y1));
		finitData.y2 = malloc(sizeof(y2));
		finitData.z1 = malloc(sizeof(z1));
		finitData.z2 = malloc(sizeof(z2));
		finitData.distans = malloc(sizeof(distance));
		finitData.t = malloc(sizeof(timePoint));

		memcpy(finitData.x1,x1,stepsNum);
		memcpy(finitData.x2,x2,stepsNum);
		memcpy(finitData.y1,y1,stepsNum);
		memcpy(finitData.y2,y2,stepsNum);
		memcpy(finitData.z1,z1,stepsNum);
		memcpy(finitData.z2,z2,stepsNum);
		memcpy(finitData.distans,distance,stepsNum);
		memcpy(finitData.t,timePoint,stepsNum);
		//Show Graph 1:
		CA_VariantSet1DArray (&var_x1, CAVT_DOUBLE, stepsNum,x1);
		CA_VariantSet1DArray (&var_y1, CAVT_DOUBLE, stepsNum,y1);
		CA_VariantSet1DArray (&var_z1, CAVT_DOUBLE, stepsNum,z1);
		CW3DGraphLib_CWPlot3DSetMultiPlot (objectHandle1, NULL, VTRUE);
		CW3DGraphLib__DCWGraph3DPlot3DCurve (objectHandle1, NULL, var_x1, var_y1, var_z1, CA_DEFAULT_VAL);
		// Show graph 2:
		CA_VariantSet1DArray (&var_x2, CAVT_DOUBLE, stepsNum,x2);
		CA_VariantSet1DArray (&var_y2, CAVT_DOUBLE, stepsNum,y2);
		CA_VariantSet1DArray (&var_z2, CAVT_DOUBLE, stepsNum,z2);
		CW3DGraphLib_CWPlot3DSetMultiPlot (objectHandle2, NULL, VTRUE);
		CW3DGraphLib__DCWGraph3DPlot3DCurve (objectHandle2, NULL, var_x2, var_y2, var_z2, CA_DEFAULT_VAL);
		MessagePopup ("Complte", "The Lorenz Attraction was Calculated");
		SetCtrlAttribute (tabHandleMain, MainTAB_Open_Save_run_2_exel, ATTR_DIMMED, 0);
		break;
	}
	return 0;
}

int CVICALLBACK Corser_on_off_TOGGLEBUTTON (int panel, int control, int event,void *callbackData, int eventData1, int eventData2) // Togle Courser OM/OFF [Done]
{
	// This function is called when the user interacts with the "Toggle Cursor ON/OFF" toggle button on the GUI
	switch (event)
	{
		case EVENT_COMMIT:
		CorserOnOff = !CorserOnOff;
		SetCtrlAttribute (tabHandleDiff, Dist_G_TAB_GRAPH, ATTR_NUM_CURSORS, CorserOnOff);
		if (CorserOnOff) // If the cursor is turned ON
		{
			// Set cursor attributes for the graph control
			SetCursorAttribute (tabHandleDiff, Dist_G_TAB_GRAPH, 1, ATTR_CURSOR_COLOR, 0x00FF6600);
			SetCursorAttribute (tabHandleDiff, Dist_G_TAB_GRAPH, 1, ATTR_CURSOR_MODE, VAL_SNAP_TO_POINT);
			SetCursorAttribute (tabHandleDiff, Dist_G_TAB_GRAPH, 1, ATTR_CROSS_HAIR_STYLE, VAL_LONG_CROSS);
			// Enable the "Find Max Point" button, as the cursor is now ON and can be used
			SetCtrlAttribute (tabHandleDiff, Dist_G_TAB_Find_max_point_bottom, ATTR_DIMMED, 0);
		}
		else // If the cursor is turned OFF
		{ 
		// reset the cursor-related data to zero
		SetCtrlVal (tabHandleDiff, Dist_G_TAB_NUMERIC_2, 0.0); SetCtrlVal (tabHandleDiff, Dist_G_TAB_NUMERIC, 0.0);
		// Disable the "Find Max Point" button, as the cursor is OFF and cannot be used
		SetCtrlAttribute (tabHandleDiff, Dist_G_TAB_Find_max_point_bottom, ATTR_DIMMED, 1);
		}
		break;
	}
	return 0;
}

int CVICALLBACK TIMER (int panel, int control, int event,void *callbackData, int eventData1, int eventData2) // Led Blink [Done]
{
	// This function is a timer callback and is called at regular intervals defined by the timer settings for led blinking
	switch (event)
	{
		case EVENT_TIMER_TICK:
		clk = !clk;
		SetCtrlVal (panelHandle, PANEL_LED, clk);
		break;
	}
	return 0;
}

int CVICALLBACK diff_GRAPH_function (int panel, int control, int event,void *callbackData, int eventData1, int eventData2) // If curser is on Get point [Done]
{
	/* This function is called when the mouse pointer moves over the graph (Dist_G_TAB_GRAPH) and
		is used to retrieve and display the current coordinates (xCur, yCur) of the graph cursor */
	double xCur,yCur;
	switch (event)
	{
		case EVENT_MOUSE_POINTER_MOVE:
			if (CorserOnOff)
			{
			GetGraphCursor (tabHandleDiff, Dist_G_TAB_GRAPH, 1, &xCur, &yCur);
			SetCtrlVal (tabHandleDiff, Dist_G_TAB_NUMERIC_2, xCur);
			SetCtrlVal (tabHandleDiff, Dist_G_TAB_NUMERIC, yCur);
			}
		break;
	}
	return 0;
}

int CVICALLBACK log_x_RADIOBUTTON (int panel, int control, int event,void *callbackData, int eventData1, int eventData2) //  set X axis to log scale [Done]
{
	/* This function is called when the user interacts with the "log_x_RADIOBUTTON" control.
		The control is used to set the X-axis of the graph to either linear or logarithmic scale */
	int L;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (tabHandleDiff, Dist_G_TAB_log_x_RADIOBUTTON, &L);
			SetCtrlAttribute (tabHandleDiff, Dist_G_TAB_GRAPH, ATTR_XMAP_MODE, L);
		break;
	}
	return 0;
}

int CVICALLBACK tr1 (void *functionData) // calc the next point of Lorenz Attractor thered 1 [Done]
{
	for (int i = 0; i < stepsNum; i++)
	{
		if(i != 0) 
			{
				double dx = tdm1->sigma * (tdm1->res[i-1].y - tdm1->res[i-1].x);
	    		double dy = (tdm1->res[i-1].x * (tdm1->rho - tdm1->res[i-1].z)) - tdm1->res[i-1].y;
	    		double dz = (tdm1->res[i-1].x * tdm1->res[i-1].y) - (tdm1->beta * tdm1->res[i-1].z);
			
				tdm1->res[i].x = tdm1->res[i-1].x + (dx * dt);
				tdm1->res[i].y = tdm1->res[i-1].y + (dy * dt);
				tdm1->res[i].z = tdm1->res[i-1].z + (dz * dt);
				tdm1->res[i].t = i * dt;
		 	}
		else tdm1->res[i].t = dt;
	}
	return 0;
}

int CVICALLBACK tr2 (void *functionData) // calc the next point of Lorenz Attractor thered 2 [Done]
{
	for (int i = 0; i < stepsNum; i++)
	{
		if(i != 0) 
			{
				double dx = tdm2->sigma * (tdm2->res[i-1].y - tdm2->res[i-1].x);
	    		double dy = (tdm2->res[i-1].x * (tdm2->rho - tdm2->res[i-1].z)) - tdm2->res[i-1].y;
	    		double dz = (tdm2->res[i-1].x * tdm2->res[i-1].y) - (tdm2->beta * tdm2->res[i-1].z);
			
				tdm2->res[i].x = tdm2->res[i-1].x + (dx * dt);
				tdm2->res[i].y = tdm2->res[i-1].y + (dy * dt);
				tdm2->res[i].z = tdm2->res[i-1].z + (dz * dt);
				tdm2->res[i].t = i * dt;
		 	}
		else tdm2->res[i].t = dt;
	}
	return 0;
}

double calc2PointDistance(point point1, point point2) // calc the distans between 2 point [done]
{
    double dx = point1.x - point2.x;
    double dy = point1.y - point2.y;
    double dz = point1.z - point2.z;
	if( dx != dx || dy != dy || dz != dz) return 0.0;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

int CVICALLBACK Find_max_point_bottom (int panel, int control, int event, void *callbackData, int eventData1, int eventData2) // Set Corser To Max Point In Distance Graph [Done]
{
	 // This function is called when the user interacts with the "Find Max Point" button
	switch (event)
	{
		case EVENT_COMMIT:
			if(MaxPointX != MaxPointX || MaxPointY != MaxPointY)
			{
				// If MaxPointX or MaxPointY is NaN, show an error message indicating that no max point is available
				MessagePopup ("ERROR", "No Max Point Avalable");
			}
			else
			{
				// If valid MaxPointX and MaxPointY are available, set the graph cursor to the max point
				SetGraphCursor (tabHandleDiff, Dist_G_TAB_GRAPH, 1, MaxPointX, MaxPointY);
				SetCtrlVal (tabHandleDiff, Dist_G_TAB_NUMERIC_2, MaxPointX); SetCtrlVal (tabHandleDiff, Dist_G_TAB_NUMERIC, MaxPointY);
			}
		break;
	}
	return 0;
}

int CVICALLBACK GraphView_Function (int panel, int control, int event, void *callbackData, int eventData1, int eventData2) // Set Graph View fo both 3D graphes [Done]
{
	// This function is called when the user interacts with the graph view buttons
	switch (event)
	{
		case EVENT_COMMIT:
			switch (control)
			{
				case MainTAB_Z_Y_GraphViewBUTTON: // Set the view mode of both 3D graphs to YZ plane view
					CW3DGraphLib__DCWGraph3DSetViewMode (objectHandle1, NULL, CW3DGraphLibConst_cwViewYZPlane);
					CW3DGraphLib__DCWGraph3DSetViewMode (objectHandle2, NULL, CW3DGraphLibConst_cwViewYZPlane);
				break;
				//--------------------------------------------------
				case MainTAB_X_Z_GraphViewBUTTON:  // Set the view mode of both 3D graphs to XZ plane view
					CW3DGraphLib__DCWGraph3DSetViewMode (objectHandle1, NULL, CW3DGraphLibConst_cwViewXZPlane);
					CW3DGraphLib__DCWGraph3DSetViewMode (objectHandle2, NULL, CW3DGraphLibConst_cwViewXZPlane);
				break;
				//--------------------------------------------------
				case MainTAB_X_Y_GraphViewBUTTON: // Set the view mode of both 3D graphs to XY plane view
					CW3DGraphLib__DCWGraph3DSetViewMode (objectHandle1, NULL, CW3DGraphLibConst_cwViewXYPlane);
					CW3DGraphLib__DCWGraph3DSetViewMode (objectHandle2, NULL, CW3DGraphLibConst_cwViewXYPlane);
				break;
			}
		break;
	}
	return 0;
}
