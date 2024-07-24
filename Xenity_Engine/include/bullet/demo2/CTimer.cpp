// //**************************************************************************
// //		PSPGU Tutorial: 'Lesson3' - CTimer.cpp
// //**************************************************************************
// #include "CTimer.h"
 
// CTimer::CTimer( void )
// {
// 	sceRtcGetCurrentTick( &timeLastAsk );
// 	tickResolution = sceRtcGetTickResolution();
// }
 
// double CTimer::GetDeltaTime( void )
// {
// 	sceRtcGetCurrentTick( &timeNow );
// 	double dt = ( timeNow - timeLastAsk ) / ((float) tickResolution );
// 	timeLastAsk = timeNow;
 
// 	return dt;
// }