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

// typedef struct {
// 	unsigned int color;
// 	float nx,ny,nz;
// 	float x, y, z;
// } Vertex;



// //////////////////////////////////////////////////////////////////////////
// #include "LinearMath/btVector3.h"
// #include "LinearMath/btMatrix3x3.h"
// #include "LinearMath/btTransform.h"
// #include "LinearMath/btQuickprof.h"
// #include "LinearMath/btAlignedObjectArray.h"
// #include "btBulletDynamicsCommon.h"

// #include "BulletSoftBody/btSoftBody.h"
// #include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
// #include "BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.h"
// #include "BulletCollision/NarrowPhaseCollision/btGjkEpa2.h"
// #include "LinearMath/btQuickprof.h"
// #include "LinearMath/btIDebugDraw.h"
// #include "LinearMath/btConvexHull.h"
// #include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
// #include "BulletSoftBody/btSoftBodyHelpers.h"

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

// //for soft bodies
// btSoftBody::btSoftBodyWorldInfo	m_softBodyWorldInfo;
// btSoftBody::Node*					m_node;
// btSoftBody*	psb;


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
// PSP_HEAP_SIZE_KB(10*1024);

 
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

// 	///register some softbody collision algorithms on top of the default btDefaultCollisionConfiguration
// 	physCollisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();

	
// 	//Use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
// 	physDispatcher = new btCollisionDispatcher(physCollisionConfiguration);
// 	m_softBodyWorldInfo.m_dispatcher = physDispatcher;

// 	physBroadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);
// 	m_softBodyWorldInfo.m_broadphase = physBroadphase;

// 	//The default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
// 	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
// 	physSolver = solver;

// 	//Create the main physics object
// 	physDynamicsWorld = new btSoftRigidDynamicsWorld(physDispatcher,physBroadphase,physSolver,physCollisionConfiguration);
// 	physDynamicsWorld->setGravity(btVector3(0,-10,0));
// 	m_softBodyWorldInfo.m_gravity.setValue(0,-10,0);
// 	m_softBodyWorldInfo.m_sparsesdf.Initialize();
	
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

// 	//init pressure soft body ^^
// 	{
// 		psb=btSoftBodyHelpers::CreateEllipsoid(m_softBodyWorldInfo,btVector3(0,10,0),btVector3(1,1,1),256);
		
// 		psb->m_materials[0]->m_kLST	=	0.1;
// 		psb->m_cfg.kDF				=	1;
// 		psb->m_cfg.kDP				=	0.001; // fun factor...
// 		psb->m_cfg.kPR				=	2500;
// 		psb->setTotalMass(30,true);

// 		/*
// 		psb->m_materials[0]->m_kLST	=	0.45;
// 		psb->m_cfg.kVC				=	20;
// 		psb->setTotalMass(30,true);
// 		psb->setPose(true,false);*/

// 		((btSoftRigidDynamicsWorld*)physDynamicsWorld)->addSoftBody(psb);
// 	}
// }

// void DrawSoftBody(btSoftBody *softbody,float R,float  G,float  B)
// {
// 	btSoftBody::tFaceArray&   faces(softbody->m_faces);

// 	Vertex *points;
// 	points = (Vertex*)sceGuGetMemory((faces.size() * 3) * sizeof(Vertex));

// 	int j =0;

// 	for (int i = 0; i < faces.size(); i++)
// 	{
// 		btSoftBody::Node*   node_0=faces[i].m_n[0];
// 		btSoftBody::Node*   node_1=faces[i].m_n[1];
// 		btSoftBody::Node*   node_2=faces[i].m_n[2];

// 		btVector3	n=cross(node_1->m_x - node_0->m_x,node_2->m_x - node_0->m_x).normalized();

// 		points[j].color =  GU_COLOR( R, G, B, 1.0f );
// 		points[j].x = node_0->m_x.getX();
// 		points[j].y = node_0->m_x.getY();
// 		points[j].z = node_0->m_x.getZ();
// 		points[j].nx = n.getX();
// 		points[j].ny = n.getY();
// 		points[j].nz = n.getZ();
// 		j++;

// 		points[j].color =  GU_COLOR( R, G, B, 1.0f );
// 		points[j].x = node_1->m_x.getX();
// 		points[j].y = node_1->m_x.getY();
// 		points[j].z = node_1->m_x.getZ();
// 		points[j].nx = n.getX();
// 		points[j].ny = n.getY();
// 		points[j].nz = n.getZ();
// 		j++;

// 		points[j].color =  GU_COLOR( R, G, B, 1.0f );
// 		points[j].x = node_2->m_x.getX();
// 		points[j].y = node_2->m_x.getY();
// 		points[j].z = node_2->m_x.getZ();
// 		points[j].nx = n.getX();
// 		points[j].ny = n.getY();
// 		points[j].nz = n.getZ();
// 		j++;
// 	}

// 	sceGumDrawArray( GU_TRIANGLES, GU_COLOR_8888|GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,	(faces.size() * 3), 0, points );

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

// 		/*
// 		btVector3 center,aabMin,aabMax;
// 		psb->getAabb(aabMin,aabMax);
// 		center = (aabMax - aabMin)/2;
// 		ScePspFVector3 cameraPos = { center.getX(),center.getY() + 4,center.getZ() -10};
// 		ScePspFVector3 lookAtPos = { center.getX(), center.getY(), center.getZ()};*/
		
// 		ScePspFVector3 cameraPos = { psb->getWorldTransform().getOrigin().getX(),psb->getWorldTransform().getOrigin().getY() + 4,psb->getWorldTransform().getOrigin().getZ() -10};
// 		ScePspFVector3 lookAtPos = { psb->getWorldTransform().getOrigin().getX(), psb->getWorldTransform().getOrigin().getY(), psb->getWorldTransform().getOrigin().getZ()};
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

// 	//render soft body
// 	{
// 		DrawSoftBody(psb,0,1,0);
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
// 			if (pad.Buttons & PSP_CTRL_CIRCLE)
// 			{
				
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