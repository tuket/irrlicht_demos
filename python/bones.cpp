#include <iostream>
#include <irrlicht/irrlicht.h>

using namespace std;
using namespace irr;

int main()
{
	
	IrrlichtDevice* device =
	createDevice
	(
		video::EDT_OPENGL,
		core::dimension2d<u32>(800, 600),
		16,
		false,	// fullscreen
		false,	// stencyl buffer
		false,	// vsync
		0		// IEventReceiver*
	);
	
	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver* driver = device->getVideoDriver();
	
	smgr->setAmbientLight( video::SColorf(0.1, 0.1, 0.1, 1) );
		
	// add camera
    scene::ICameraSceneNode* camera =
    smgr->addCameraSceneNodeFPS
    (
		0,	// parent
		15,	// speed rotate
		0.004	// speed move
    );
    camera->setPosition(core::vector3df(-5, 3, 0));
    camera->setRotation(core::vector3df(-90, 0, 0));
    
	
	scene::IAnimatedMesh* mainCharMesh =
	smgr->getMesh("models/dcu_jimmy_olsen/Jimmy.ms3d");
	
	scene::IAnimatedMeshSceneNode* mainChar =
	smgr->addAnimatedMeshSceneNode( mainCharMesh );
	
	if( mainChar )
	{
		
		mainChar->setMaterialTexture
		(
			0,
			driver->getTexture( "models/dcu_jimmy_olsen/CHRNPCICOASC102_DIFFUSE.tga" )
		);
		
		
		video::ITexture* bumpTexture = driver->getTexture( "models/dcu_jimmy_olsen/CHRNPCICOASC102_NORMAL.tga" );
		driver->makeNormalMapTexture( bumpTexture, 1.5f );
		
		mainChar->setMaterialTexture
		(
			1,
			bumpTexture
		);
		mainChar->setMaterialType(video::EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA);
		mainChar->setMaterialFlag( video::EMF_LIGHTING, true );
		
		// mainChar->getMaterial(0).EmissiveColor.set(255, 255, 255, 255);
		mainChar->getMaterial(0).AmbientColor.set(255, 255, 255, 255);
		
	}
	
	scene::ILightSceneNode* light1 = 
	smgr->addLightSceneNode
	(
		0,
		core::vector3df(0, 0, -10),
		video::SColorf(1, 0.5, 0.5, 0),
		800
	);
	
	light1->getLightData().AmbientColor = video::SColorf(0.3, 0.3, 0.3, 1.0);
	
	light1->setDebugDataVisible( scene::EDS_BBOX );
	
	// Important tell irrlicht we want to control the position of the bones
	mainChar->setJointMode(scene::EJUOR_CONTROL);
	mainChar->setLoopMode( false );
	
	scene::IBoneSceneNode* leftElbow = mainChar->getJointNode("LEFTELBOW");
	scene::IBoneSceneNode* eyeBrow = mainChar->getJointNode("BROW");
	
	if( !leftElbow )
	{
		cerr << "No Left Elbow" << endl;
	}
	
	if( !eyeBrow )
	{
		cerr << "No Eye Brow" << endl;
	}
	
	leftElbow->setPosition( leftElbow->getPosition() + core::vector3df(10, 0, 0) );
	// eyeBrow->setPosition( eyeBrow->getPosition() - core::vector3df(0, 0, 0.15) );
	// mainChar->setTransitionTime(3);
	
	/*
	mainChar->setFrameLoop(1, 15);
	mainChar->setLoopMode(true);
	mainChar->setAnimationSpeed(5);
	*/
	
	
	
	while( device->run() )
	{
		
		leftElbow->setRotation( leftElbow->getRotation() + core::vector3df(1, 0, 0) );
		//mainChar->animateJoints();
		
		driver->beginScene
		(
			true,
			true,
			video::SColor(255, 222, 255, 255)
		);
				
		smgr->drawAll();
				
		driver->endScene();
		
	}
	
}
