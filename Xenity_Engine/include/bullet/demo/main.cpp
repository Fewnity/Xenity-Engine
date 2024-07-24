// #include <pspgu.h>
// #include <pspgum.h>
// #include <pspctrl.h>
// #include <psppower.h>
// #include <stdlib.h> // needed in order to have "exit" function @@@
// #include <math.h>
// #include <psptypes.h>

// #include <vector>

// #include "CTimer.h"
// #include "Render.h"
// #include <math.h>

// #define BUF_WIDTH (512)
// #define SCR_WIDTH (480)
// #define SCR_HEIGHT (272)



// //////////////////////////////////////////////////////////////////////////
// #include "LinearMath/btVector3.h"
// #include "LinearMath/btMatrix3x3.h"
// #include "LinearMath/btTransform.h"
// #include "LinearMath/btQuickprof.h"
// #include "LinearMath/btAlignedObjectArray.h"
// #include "btBulletDynamicsCommon.h"

// //Bullet physics - Main objects
// btDynamicsWorld* physDynamicsWorld;
// btBroadphaseInterface* physBroadphase;
// btCollisionDispatcher* physDispatcher;
// btConstraintSolver*	physSolver;
// btDefaultCollisionConfiguration* physCollisionConfiguration;

// //Bullet physics - Scene objects
// btCollisionShape* physBoxShape;
// btRigidBody* physBoxBody;
// btCollisionShape* physGroundShape;
// btRigidBody* physGroundBody;

// std::vector<btRigidBody*> mBodies;
// int boxPosY = 10;



// // Prototypes
// //void GetBulletColObjTransform(btRigidBody* body, D3DXMATRIX &World);

// void InitPhysics(void);
// void DeleteCollisionShape(btCollisionShape* shape);
// void Cleanup(void);

// //////////////////////////////////////////////////////////////////////////


// void FPS( void );				// Display Frames Per Second 
 
// //Graphics related functions:
// void InitGU( void );			        // Initialize the Graphics Subsystem
// void DrawScene( void );			        // Render Geometry
// void Update( void );			        // Update 

// #include "main.h"


// PSP_MODULE_INFO("Lesson1", 0, 1, 1);
// PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
// PSP_HEAP_SIZE_KB(12*1024);

 
// // for calculating the frames per second
// char fpsDisplay[100];
// float dt = (1.0f/60.0f);
// float msec = 1.0f;

// CTimer timer;

// unsigned int colors[4] =
// {
// 	0xffff0000,
// 	0xff00ff00,
// 	0xff0000ff,
// 	0xffffffff
// };

// //for camera
// float awayFrom  = 14.0f;
// float pointX;
// float pointY;
// float cangle;

// float rotX = 50.0f,oldRotX = 0.0f,oldpointX = 0.0f,oldpointY = 0.0f;
// ScePspFVector3 m_vPosition;

// ///////////////////////////////////////////////////////
// // Initialize the physics
// ///////////////////////////////////////////////////////
// void InitPhysics( void )
// {
// 	btVector3 worldAabbMin(-1000,-1000,-1000);
// 	btVector3 worldAabbMax(1000,1000,1000);
// 	int maxProxies = 32766;


// 	//Collision configuration contains default setup for memory, collision setup
	
// 	physCollisionConfiguration = new btDefaultCollisionConfiguration();

	
// 	//Use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
// 	physDispatcher = new btCollisionDispatcher(physCollisionConfiguration);

// 	physBroadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);


// 	//The default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
// 	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
// 	physSolver = solver;

// 	//Create the main physics object
// 	physDynamicsWorld = new btDiscreteDynamicsWorld(physDispatcher,physBroadphase,physSolver,physCollisionConfiguration);
// 	physDynamicsWorld->setGravity(btVector3(0,-10,0));

	
// 	//Create the ground object
// 	{
// 		// Create the ground shape
// 		physGroundShape = new btBoxShape(btVector3(btScalar(50.0f),btScalar(1.0f),btScalar(50.0f)));

// 		// Set the position for the ground shape
// 		btTransform groundTransform;
// 		groundTransform.setIdentity();
// 		groundTransform.setOrigin(btVector3(0, -0.5f, 0));

// 		// Create MotionState and RigidBody object for the ground shape
// 		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);

// 		float mass = 0;
// 		bool isDynamic = (mass != 0.f);

// 		btVector3 localInertia(0,0,0);
// 		if (isDynamic)
// 			physGroundShape->calculateLocalInertia(mass,localInertia);

// 		physGroundBody = new btRigidBody(mass,myMotionState,physGroundShape,localInertia);

// 		// Add ground body to physics object
// 		physDynamicsWorld->addRigidBody(physGroundBody);
// 	}

// 	//Create the box object
// 	{
// 		//Create the box shape
// 		physBoxShape = new btBoxShape(btVector3(1,1,1));

// 		//Set mass, initial inertia and position for the box
// 		float mass = 1.0f;
// 		btVector3 inertia(0, 0, 0);
// 		btTransform startTransform;
// 		startTransform.setIdentity();
// 		startTransform.setOrigin(btVector3(0, 10, 0));

// 		//Calculate the inertia
// 		physBoxShape->calculateLocalInertia(mass, inertia);
			
// 		// Create MotionState and RigidBody object for the box shape
// 		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
// 		physBoxBody = new btRigidBody(mass, myMotionState, physBoxShape, inertia);

// 		// Add box body to physics object & activate it
// 		physDynamicsWorld->addRigidBody(physBoxBody);
// 		physBoxBody->activate();

// 		//add to contener
// 		mBodies.push_back(physBoxBody);
// 	}
// }

// void GetBulletColObjPosition(btRigidBody* body, ScePspFVector3 &pos)
// {
// 	if(body && body->getMotionState())
// 	{
// 		btPoint3 p = body->getCenterOfMassPosition();
// 		pos.x = p.getX();
// 		pos.y = p.getY();
// 		pos.z = p.getZ();
// 	}
// }

// void QuaternionToEulerXYZ(const btQuaternion &quat,btVector3 &euler)
// {
// 	float w=quat.getW();	float x=quat.getX();	float y=quat.getY();	float z=quat.getZ();
// 	float sqw = w*w; float sqx = x*x; float sqy = y*y; float sqz = z*z; 
// 	euler.setZ((atan2f(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw))));
// 	euler.setX((atan2f(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw))));
// 	euler.setY((asinf(-2.0 * (x*z - y*w))));
// }

// void GetRotation(btRigidBody* body, ScePspFVector3 &rot)
// {
// 	btVector3 btv;
// 	btQuaternion btq=body->getOrientation();
// 	QuaternionToEulerXYZ(btq, btv);
// 	rot.x = btv.getX();
// 	rot.y = btv.getY();
// 	rot.z = btv.getZ();
// }

// void FPS( void )
// {
// 	msec = (float)timer.GetDeltaTime( );
	
// 	sprintf( fpsDisplay, "Fps: %f",(1.0f/msec) );

// 	pspDebugScreenSetOffset( (int)Render::Instance()->fbp0 );
// 	pspDebugScreenSetXY( 0, 0 );
// 	pspDebugScreenPrintf( fpsDisplay );
 
// }

// void InitGU( void )
// {
// 	//init render engine
// 	Render::Instance()->Init();
// 	Render::Instance()->SetPerspective();
// }

// void SetupProjection( void )
// {
// 	// setup matrices for the triangle
// 	sceGumMatrixMode(GU_PROJECTION);
// 	sceGumLoadIdentity();
	
// 	sceGumPerspective( 75.0f, 16.0f/9.0f, 0.5f, 1000.0f);
 
//     sceGumMatrixMode(GU_VIEW);
// 	sceGumLoadIdentity();
 
// 	sceGuClearColor( GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f ) ); 
// 	sceGuClearDepth(0);	
// }

// void DrawScene( void )
// {
// 	Render::Instance()->Start();

// 	//for camera
// 	sceGumMatrixMode(GU_PROJECTION);
// 	sceGumLoadIdentity();
// 	sceGumPerspective(50.0f,16.0f/9.0f,4.0f,100.0f);

// 	sceGumMatrixMode(GU_VIEW);
// 	sceGumLoadIdentity();
// 	{
// 		ScePspFVector3 center;
// 		GetBulletColObjPosition(physBoxBody,center);

// 		ScePspFVector3 cameraPos = { center.x, center.y + 4, center.z -10};
// 		ScePspFVector3 lookAtPos = { center.x, center.y, center.z};
// 		ScePspFVector3 upVec = { 0, 1, 0};
// 		upVec.y = 1;

// 		if(rotX != oldRotX)
// 		{
// 			cangle = (rotX/50)*6.28;

// 			pointX = sin(cangle)*awayFrom;
// 			pointY = cos(cangle)*awayFrom;

// 			oldRotX = rotX;
// 			oldpointX = pointX;
// 			oldpointY = pointY;
// 		}else
// 		{
// 			pointX = oldpointX;
// 			pointY = oldpointY;
// 		}		

// 		cameraPos.x = pointX + lookAtPos.x;
// 		cameraPos.y += 7.0f;
// 		cameraPos.z = pointY + lookAtPos.z;

// 		m_vPosition = cameraPos;

// 		sceGumLookAt(&cameraPos, &lookAtPos, &upVec);
// 	}

// 	//light
// 	ScePspFVector3 pos = { 0,50,0};
// 	sceGuLight(0,GU_POINTLIGHT,GU_DIFFUSE_AND_SPECULAR,&pos);
// 	sceGuLightColor(0,GU_DIFFUSE,colors[3]);
// 	sceGuLightColor(0,GU_SPECULAR,0xffffffff);

// 	sceGuSpecular(12.0f);
// 	sceGuAmbient(0x00222222);

// 	sceGumMatrixMode(GU_MODEL);
// 	sceGumLoadIdentity();

// 	//draw floor box
// 	{
// 		ScePspFVector3 move;
// 		ScePspFVector3 scale = {20,2,20};
// 		GetBulletColObjPosition(physGroundBody,move);
// 		Render::Instance()->DrawBox(move,scale,1,0,0);
// 	}

// 	//draw box
// 	{
// 		ScePspFVector3 move;
// 		ScePspFVector3 scale = {2,2,2};
// 		GetBulletColObjPosition(physBoxBody,move);
// 		ScePspFVector3 rotate;
// 		GetRotation(physBoxBody,rotate);
// 		Render::Instance()->DrawBox(move,scale,rotate,0,0,1);
// 	}

// 	//draw rest of the boxes
// 	{
// 		for (unsigned int i = 0;i < mBodies.size();i++)
// 		{
// 			ScePspFVector3 move;
// 			ScePspFVector3 scale = {2,2,2};
// 			GetBulletColObjPosition(mBodies[i],move);
// 			ScePspFVector3 rotate;
// 			GetRotation(mBodies[i],rotate);
// 			Render::Instance()->DrawBox(move,scale,rotate,0,0,1);
// 		}
// 	}
	

// 	sceGuFinish();
// 	sceGuSync(0,0);

	
// }

// void Update( void )
// {
// 	//physic update
// 	if(physDynamicsWorld)
// 		physDynamicsWorld->stepSimulation(dt);
	
// }
//  extern "C"
// int main(void)
// {
// 	pspDebugScreenInit();
// 	SetupCallbacks();
	 
// 	InitGU();

// 	InitPhysics();

// 	SceCtrlData oldPad;

// 	bool running = true;
 
// 	while( running )
// 	{
// 		SceCtrlData pad;
// 		sceCtrlReadBufferPositive(&pad, 1);

// 		if (pad.Buttons != 0)
// 		{
// 			if (pad.Buttons & PSP_CTRL_LTRIGGER)
// 			{
// 				rotX -= 0.5f;
// 				if (rotX < -360)
// 				{
// 					rotX =	0.0f;
// 				}
// 			}
// 			if (pad.Buttons & PSP_CTRL_RTRIGGER)
// 			{
// 				rotX += 0.5f;
// 				if (rotX > 360)
// 				{
// 					rotX =	0.0f;
// 				}
// 			}
// 		}

// 		if (pad.Buttons != oldPad.Buttons)
// 		{
// 			if (pad.Buttons & PSP_CTRL_SQUARE)
// 			{
				
// 			}
// 			if (pad.Buttons & PSP_CTRL_CROSS)
// 			{
// 				//Create the box object
// 				{
// 					//Create the box shape
// 					physBoxShape = new btBoxShape(btVector3(1,1,1));

// 					//Set mass, initial inertia and position for the box
// 					float mass = 1.0f;
// 					btVector3 inertia(0, 0, 0);
// 					btTransform startTransform;
// 					startTransform.setIdentity();
// 					startTransform.setOrigin(btVector3(0, boxPosY, 0));

// 					//Calculate the inertia
// 					physBoxShape->calculateLocalInertia(mass, inertia);

// 					// Create MotionState and RigidBody object for the box shape
// 					btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
// 					btRigidBody *BoxBody = new btRigidBody(mass, myMotionState, physBoxShape, inertia);

// 					// Add box body to physics object & activate it
// 					physDynamicsWorld->addRigidBody(BoxBody);
// 					BoxBody->activate();

// 					//add to contener
// 					mBodies.push_back(BoxBody);
// 					boxPosY++;
// 				}
// 			}
// 			if (pad.Buttons & PSP_CTRL_TRIANGLE)
// 			{
// 				running = false;
// 			}
// 		}
// 		oldPad = pad;
		
// 		Update();
// 		DrawScene();
// 		FPS();
		
// 		sceDisplayWaitVblankStart();
// 		Render::Instance()->End();
//     }

// 	sceGuTerm();			// Terminating the Graphics System
 
// 	sceKernelExitGame();	// Quits Application
// 	return 0;
// }