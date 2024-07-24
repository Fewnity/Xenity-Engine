// #ifndef Render_H
// #define Render_H

// #include <pspgu.h>
// #include <pspgum.h>
// #include <pspctrl.h>
// #include <stdlib.h> // needed in order to have "exit" function @@@
// #include <vector>
// #include <pspkernel.h>
// #include <pspdisplay.h>
// #include <pspdebug.h>

// #include "vram.h"

// #define BUF_WIDTH (512)
// #define SCR_WIDTH (480)
// #define SCR_HEIGHT (272)

// namespace
// {
// 	struct Vertex2
// 	{
// 		float nx,ny,nz;
// 		//unsigned int color;
// 		float x,y,z;
// 	};

// 	//static unsigned int __attribute__((aligned(16))) circlelList2[1024]; 
// 	static unsigned int __attribute__((aligned(16))) boxList[1024]; 

// 	static unsigned int __attribute__((aligned(16))) list[2][262144];//uncomment if you want use double calllist
// 	static unsigned int __attribute__((aligned(16))) callList[64];//uncomment if you want use double calllist

// 	int listNum = 0;//uncomment if you want use double calllist

// 	struct Vertex2 __attribute__((aligned(16))) boxVerts[12*3] =
// 	{
// 		{0, -2, -0,-0.5f, -0.5f, 0.5f}, //1
// 		{0, -1, -0,-0.5f, -0.5f, -0.5f}, //3
// 		{0, -2, -0,0.5f, -0.5f, -0.5f}, //4

// 		{0, -2, -0,0.5f, -0.5f, -0.5f}, //4
// 		{0, -1, -0,0.5f, -0.5f, 0.5f}, //2
// 		{0, -2, -0,-0.5f, -0.5f, 0.5f}, //1

// 		{0, 2, -0,-0.5f, 0.5f, 0.5f}, //5
// 		{0, 1, -0,0.5f, 0.5f, 0.5f}, //6
// 		{0, 2, -0,0.5f, 0.5f, -0.5f}, //8

// 		{0, 2, -0,0.5f, 0.5f, -0.5f}, //8
// 		{0, 1, -0,-0.5f, 0.5f, -0.5f}, //7
// 		{0, 2, -0,-0.5f, 0.5f, 0.5f}, //5

// 		{0, -2, -0,-0.5f, -0.5f, 0.5f}, //1
// 		{0, -1, -0,0.5f, -0.5f, 0.5f}, //2
// 		{0, 1, -0,0.5f, 0.5f, 0.5f}, //6

// 		{0, 1, -0,0.5f, 0.5f, 0.5f}, //6
// 		{0, 2, -0,-0.5f, 0.5f, 0.5f}, //5
// 		{0, -2, -0,-0.5f, -0.5f, 0.5f}, //1

// 		{0, -1, -0,0.5f, -0.5f, 0.5f}, //2
// 		{0, -2, -0,0.5f, -0.5f, -0.5f}, //4
// 		{0, 2, -0,0.5f, 0.5f, -0.5f}, //8

// 		{0, 2, -0,0.5f, 0.5f, -0.5f}, //8
// 		{0, 1, -0,0.5f, 0.5f, 0.5f}, //6
// 		{0, -1, -0,0.5f, -0.5f, 0.5f}, //2

// 		{0, -2, -0,0.5f, -0.5f, -0.5f}, //4
// 		{0, -1, -0,-0.5f, -0.5f, -0.5f}, //3
// 		{0, 1, -0,-0.5f, 0.5f, -0.5f}, //7

// 		{0, 1, -0,-0.5f, 0.5f, -0.5f}, //7
// 		{0, 2, -0,0.5f, 0.5f, -0.5f}, //8
// 		{0, -2, -0,0.5f, -0.5f, -0.5f}, //4

// 		{0, -1, -0,-0.5f, -0.5f, -0.5f}, //3
// 		{0, -2, -0,-0.5f, -0.5f, 0.5f}, //1
// 		{0, 2, -0,-0.5f, 0.5f, 0.5f}, //5

// 		{0, 2, -0,-0.5f, 0.5f, 0.5f}, //5
// 		{0, 1, -0,-0.5f, 0.5f, -0.5f}, //7
// 		{0, -1, -0,-0.5f, -0.5f, -0.5f}, //3
// 	}; 
// }

// class Render
// {
// public:

// 	typedef struct {
// 		float u,v; // tex coords
// 		unsigned int color;
// 		float nx,ny,nz;
// 		float x, y, z;
// 	} Vertex4;

// 	void *dList;		// display List, used by sceGUStart
// 	void *fbp0;
// 	void* fbp1;
// 	void* zbp;

	

// 	static Render* Instance() {
// 		return &m_Render;
// 	}

// 	static Render m_Render;

// 	void Init()
// 	{
// 		//dList = malloc( 262144 ); //comment if you want use double calllist
// 		//fbp0  = 0;
// 		fbp0 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
// 		fbp1 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
// 		zbp = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);

// 		// Init GU
// 		sceGuInit();
// 		gumInit();
// 		//sceGuStart( GU_DIRECT, dList ); //comment if you want use double calllist
// 		sceGuStart( GU_DIRECT, list ); //uncomment if you want use double calllist
// 		// Set Buffers
// 		//sceGuDrawBuffer( GU_PSM_8888, fbp0, BUF_WIDTH );
// 		//sceGuDispBuffer( SCR_WIDTH, SCR_HEIGHT, (void*)0x88000, BUF_WIDTH);
// 		//sceGuDepthBuffer( (void*)0x110000, BUF_WIDTH);

// 		sceGuDrawBuffer( GU_PSM_8888, fbp0, BUF_WIDTH );
// 		sceGuDispBuffer( SCR_WIDTH, SCR_HEIGHT, fbp1, BUF_WIDTH);
// 		sceGuDepthBuffer( zbp, BUF_WIDTH);

// 		sceGuOffset( 2048 - (SCR_WIDTH/2), 2048 - (SCR_HEIGHT/2));
// 		sceGuViewport( 2048, 2048, SCR_WIDTH, SCR_HEIGHT);
// 		sceGuDepthRange( 65535, 0);

// 		// Set Render States
// 		sceGuScissor( 0, 0, SCR_WIDTH, SCR_HEIGHT);
// 		sceGuEnable( GU_SCISSOR_TEST );
// 		sceGuDepthFunc( GU_GEQUAL );
// 		sceGuEnable( GU_DEPTH_TEST );
// 		sceGuShadeModel( GU_SMOOTH );
// 		sceGuFrontFace( GU_CCW );

// 		//culling
// 		sceGuEnable(GU_CULL_FACE);
// 		sceGuEnable(GU_CLIP_PLANES);
		
// 		sceGuEnable(GU_LIGHTING);
// 		sceGuEnable(GU_LIGHT0);
// 		//sceGuEnable(GU_LIGHT1);
// 		//sceGuEnable(GU_LIGHT2);
// 		//sceGuEnable(GU_LIGHT3);
		
// 		sceGuAlphaFunc(GU_GREATER,0,0xff);
// 		sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

// 		sceGuTexMode(GU_PSM_8888,0,0,0);
// 		sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
// 		sceGuTexFilter(GU_NEAREST,GU_NEAREST);
// 		sceGuTexWrap(GU_REPEAT,GU_REPEAT);

// 		sceGuTexScale(1,1);
// 		sceGuTexOffset(0,0);

// 		sceGuFinish();
// 		sceGuSync(0,0);

// 		sceDisplayWaitVblankStart();
// 		sceGuDisplay(GU_TRUE);

		
// 		{
// 			//init box list
// 			sceGuStart(GU_CALL, boxList);
// 			sceGumDrawArray( GU_TRIANGLES, GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,12*3,0,boxVerts);
// 			sceGuFinish();
// 			sceGuSync(0,0);
// 		} 

// 		//for analog pad
// 		sceCtrlSetSamplingCycle(0);
// 		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
// 		// finish

// 		pspDebugScreenInit();

// 		//new
// 		sceGuStart(GU_CALL,list[listNum&1]);//uncomment if you want use double calllist

		
// 	}

	
// 	void SetOrtho()
// 	{
// 		sceGumMatrixMode(GU_PROJECTION);
// 		sceGumLoadIdentity();

// 		sceGumOrtho(0,480,272,0, -250, 250); 

// 		sceGumMatrixMode(GU_VIEW);
// 		sceGumLoadIdentity();

// 		sceGuClearColor( GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f ) ); 
// 		sceGuClearDepth(0);	

// 		sceGumMatrixMode(GU_MODEL);
// 		sceGumLoadIdentity();
// 	}

// 	void SetPerspective()
// 	{
// 		sceGumMatrixMode(GU_PROJECTION);
// 		sceGumLoadIdentity();

// 		sceGumPerspective( 75.0f, 16.0f/9.0f, 2.0f, 100.0f);

		
// 		sceGumMatrixMode(GU_VIEW);
// 		sceGumLoadIdentity();

// 		sceGuClearColor( GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f ) ); 
// 		sceGuClearDepth(0);	

// 		sceGumMatrixMode(GU_MODEL);
// 		sceGumLoadIdentity();
// 	}

// 	void Start()
// 	{
// 		//sceGuStart( GU_DIRECT, dList );//comment if you want use double calllist
// 		sceGuClearColor(0xff554433);
// 		sceGuClearDepth(0);
// 		sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
// 		sceGumLoadIdentity();
		
// 	}

// 	void ShowFPS(char fpsDisplay[100])
// 	{
// 		pspDebugScreenSetOffset( (int)fbp0 );
// 		pspDebugScreenSetXY( 0, 0 );
// 		pspDebugScreenPrintf( fpsDisplay );

// 	}

// 	void End()
// 	{
// 		fbp1 = sceGuSwapBuffers();
// 		//sceGuSwapBuffers();

// 		///*
// 		sceGuStart(GU_DIRECT,callList);
// 		sceGuCallList(list[listNum&1]);
// 		sceGuFinish();

// 		listNum ^= 1;
// 		sceGuStart(GU_CALL,list[listNum&1]);//*/
// 	}


// 	void DrawBox(ScePspFVector3 move, ScePspFVector3 mScale,float r,float g,float b)
// 	{
// 		//move
// 		sceGumPushMatrix();

// 		sceGumTranslate( &move );
// 		sceGumScale(&mScale);

// 		//update matrixes
// 		sceGumUpdateMatrix();
// 		//draw
// 		sceGuColor( GU_COLOR(r,g,b,1) );
// 		sceGuCallList(boxList);

// 		sceGumPopMatrix();
// 	}

// 	void DrawBox(ScePspFVector3 move, ScePspFVector3 mScale,ScePspFVector3 mRotattion,float r,float g,float b)
// 	{
// 		//move
// 		sceGumPushMatrix();

// 		sceGumTranslate( &move );
// 		sceGumScale(&mScale);
// 		sceGumRotateXYZ(&mRotattion);

// 		//update matrixes
// 		sceGumUpdateMatrix();
// 		//draw
// 		sceGuColor( GU_COLOR(r,g,b,1) );
// 		sceGuCallList(boxList);

// 		sceGumPopMatrix();
// 	}

// };


// #endif

