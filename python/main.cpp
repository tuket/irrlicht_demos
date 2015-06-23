#include <iostream>
#include <irrlicht/irrlicht.h>
#include <boost/python.hpp>
#include <cstdlib>

using namespace std;
using namespace irr;

class MainCharacter
{
	
	static const unsigned int keyFrames[3];
	
public:

	scene::IAnimatedMeshSceneNode* node;
	
	int getAnimationsCount()const
	{
		return sizeof(keyFrames)/sizeof(unsigned int) - 1;
	}
	
	void setAnimation(int n)
	{
		node->setFrameLoop( keyFrames[n], keyFrames[n+1] );
	}
	
};

const unsigned int MainCharacter::keyFrames[3] = {0, 16, 34};

MainCharacter mainCharacter;

MainCharacter getMainCharacter()
{
	return mainCharacter;
}

BOOST_PYTHON_MODULE(module)
{
	
	using namespace boost::python;
	
	class_ <MainCharacter>("MainCharacter")
		.def( "getAnimationsCount", &MainCharacter::getAnimationsCount )
		.def( "setAnimation", &MainCharacter::setAnimation )
	;
	
	def( "getMainCharacter", getMainCharacter );
	
}

class MyEventReceiver : public IEventReceiver
{
	
	IrrlichtDevice* device;
	
public:

	MyEventReceiver(IrrlichtDevice* device) : device(device) { }
	
	virtual bool OnEvent(const SEvent& event)
	{
		
		if( event.EventType == EET_GUI_EVENT )
		{

			if( event.GUIEvent.EventType == gui::EGET_EDITBOX_ENTER )
			{
				
				boost::python::object main_module = boost::python::import("__main__");
				boost::python::object main_namespace = main_module.attr("__dict__");
				
				core::stringw wtext = ((gui::IGUIEditBox*)event.GUIEvent.Caller)->getText();
				char* text = new char[wtext.size()+1];
				wcstombs
				(
					text,			//dest
					wtext.c_str(),			// src
					wtext.size()	// max size
				);
					
				boost::python::exec
				(
					text,
					main_namespace
				);
				((gui::IGUIEditBox*)event.GUIEvent.Caller)->setText(L"");
				
				delete text;
				
				return true;
				
			}
			
		}
		
		return false;
		
	}
	
};

int main()
{
	
	Py_Initialize();
	
	boost::python::object main_module = boost::python::import("__main__");
	boost::python::object main_namespace = main_module.attr("__dict__");

	initmodule();
	boost::python::exec( "from module import *", main_namespace );
	
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
	
	MyEventReceiver eventReceiver( device );
	device->setEventReceiver( &eventReceiver );
	
	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver* driver = device->getVideoDriver();
	gui::IGUIEnvironment* gui = device->getGUIEnvironment();
	
	smgr->setAmbientLight( video::SColorf(0.5, 0.5, 0.5, 1) );
		
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
	smgr->getMesh("models/dcu_jimmy_olsen/Jimmy.b3d");
	
	mainCharacter.node =
	smgr->addAnimatedMeshSceneNode( mainCharMesh );
	
	if( mainCharacter.node )
	{
		
		mainCharacter.node->setMaterialTexture
		(
			0,
			driver->getTexture( "models/dcu_jimmy_olsen/CHRNPCICOASC102_DIFFUSE.tga" )
		);
		
		
		video::ITexture* bumpTexture = driver->getTexture( "models/dcu_jimmy_olsen/CHRNPCICOASC102_NORMAL.tga" );
		driver->makeNormalMapTexture( bumpTexture, 1.5f );
		
		mainCharacter.node->setMaterialTexture
		(
			1,
			bumpTexture
		);
		
		mainCharacter.node->setMaterialType(video::EMT_NORMAL_MAP_SOLID);
		
		for
		(
			int i=0;
			i < mainCharacter.node->getMaterialCount();
			i++
		)
		{
			mainCharacter.node->getMaterial(i).AmbientColor = video::SColor(255, 255, 255, 255);
		}
		// mainCharacter.node->setMaterialFlag( video::EMF_LIGHTING, true );
		
		// mainChar->getMaterial(0).EmissiveColor.set(255, 255, 255, 255);
		// mainCharacter.node->getMaterial(0).AmbientColor.set(255, 255, 255, 255);
		
	}
	
	
	scene::ILightSceneNode* light1 = 
	smgr->addLightSceneNode
	(
		0,
		core::vector3df(0, 0, -10),
		video::SColorf(1, 0.5, 0.5, 0),
		800
	);
	
	//light1->getLightData().AmbientColor = video::SColorf(0.1, 0.1, 0.1, 1.0);
	
	light1->setDebugDataVisible( scene::EDS_BBOX );
	
	
	// Important tell irrlicht we want to control the position of the bones
	//mainCharacter.node->setJointMode(scene::EJUOR_CONTROL);
	//mainChar->setLoopMode( false );
	
	mainCharacter.node->setFrameLoop(1, 100);
	mainCharacter.node->setLoopMode(true);
	mainCharacter.node->setAnimationSpeed(5);
	
	//mainCharacter.setAnimation(1);
	boost::python::exec("getMainCharacter().setAnimation(0)", main_namespace);
	
	//GUI
	
	gui::IGUIEditBox *editBox = gui->addEditBox(L"", core::rect<s32>(0, 0, 32, 10));
	
	while( device->run() )
	{
		
		mainCharacter.node->animateJoints();
		
		driver->beginScene
		(
			true,
			true,
			video::SColor(255, 222, 255, 255)
		);
				
		gui->drawAll();
		smgr->drawAll();
				
		driver->endScene();
		
	}
	
	Py_Finalize();
	
}
