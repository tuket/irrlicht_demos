#include <iostream>
#include <irrlicht/irrlicht.h>
#include <boost/python.hpp>
#include <cstdlib>
#include <sstream>
#include "text_box.hpp"

namespace bp = boost::python;

using namespace std;
using namespace irr;

/**
 * The main character class
 **/
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

// these are the starting frames of each animation
// (except the last number which is the last frame of the last 
// animation +1 )
// so there are n+1 keyframes, where n is the number of animations
const unsigned int MainCharacter::keyFrames[3] = {0, 16, 34};

// Our main character is a global varaible
MainCharacter mainCharacter;

// The python output console
gui::TextBox* console;

/**
 * \brief returns the main character
 **/
MainCharacter& getMainCharacter()
{
	return mainCharacter;
}


class MyEventReceiver : public IEventReceiver
{
	
	IrrlichtDevice* device;
	gui::TextBox* console;
	
public:

	MyEventReceiver(IrrlichtDevice* device) : device(device) { }
	
	void setConsole( gui::TextBox* console )
	{
		this->console = console;
	}
	
	virtual bool OnEvent(const SEvent& event)
	{
		
		// GUI
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
				text[ wtext.size() ] = '\0';
				
				try
				{
					
					boost::python::api::object result =
					boost::python::exec
					(
						text,
						main_namespace
					);
					
				}
				catch(bp::error_already_set& error)
				{
					
					PyObject *ptype = 0, *pvalue = 0, *ptraceback = 0;
					PyErr_Fetch(&ptype, &pvalue, &ptraceback);
										
					char *errorMsg = 0;
					if( pvalue )
					{
						errorMsg = PyString_AsString(pvalue);
						
						if( errorMsg == 0 )
						{
							errorMsg = (char *)"python interpreter error";
						}
					}
					else
					{
						errorMsg = (char *)"python interpreter error";
					}
					
					if( console )
					{
						std::wstring stdwc( strlen(errorMsg), L'#' );
						mbstowcs( &stdwc[0], errorMsg, strlen(errorMsg) );
						
						core::stringw wc( stdwc.c_str() );
						console->addText(wc + L"\n");
					}
					else
					{
						std::cerr << errorMsg << std::endl;
					}
					
				}
				catch(...)
				{
					
					/*
					PyObject *ptype, *pvalue, *ptraceback;
					PyErr_Fetch(&ptype, &pvalue, &ptraceback);
					
					char *errorMsg = PyString_AsString(pvalue);
					
					if( console )
					{
						std::wstring stdwc( strlen(errorMsg), L'#' );
						mbstowcs( &stdwc[0], errorMsg, strlen(errorMsg) );
						
						core::stringw wc( stdwc.c_str() );
						console->addText(wc + L"\n");
					}
					else
					{
						std::cerr << errorMsg << std::endl;
					}
					* */
					
				}
				
				((gui::IGUIEditBox*)event.GUIEvent.Caller)->setText(L"");
				
				delete text;
				
				return true;
				
			}
			
		}
		
		// KEYBOARD
		else if( event.EventType == EET_KEY_INPUT_EVENT )
		{
			
			if( event.KeyInput.Key == KEY_ESCAPE )
			{
				
				device->closeDevice();
				return true;
				
			}
			
		}
		
		return false;
		
	}
	
};

void printStr(std::wstring text)
{
	
	if( console )
	{
		core::stringw ws( text.c_str() );
		console->addText( ws + L"\n" );
	}
	else
	{
		std::string s( (char*) text.c_str() );
		std::cout << s << std::endl;
	}
	
}

void printInt(int x)
{
	
	std::stringstream ss;
	ss << x;
	string text = ss.str();
	
	if( console )
	{
		core::stringw ws( text.c_str() );
		console->addText( ws + L"\n" );
	}
	else
	{
		std::string s( (char*) text.c_str() );
		std::cout << s << std::endl;
	}
	
}

BOOST_PYTHON_MODULE(module)
{
	
	using namespace boost::python;
	
	class_ <MainCharacter>("MainCharacter")
		.def( "getAnimationsCount", &MainCharacter::getAnimationsCount )
		.def( "setAnimation", &MainCharacter::setAnimation )
	;
	
	def
	(
		"getMainCharacter", getMainCharacter,
		return_value_policy< reference_existing_object >()
	);
	
	def( "sprint", printInt );
	def( "sprint", printStr );
	
}


int main()
{
	
	Py_Initialize();
	
	bp::object main_module = bp::import("__main__");
	bp::object main_namespace = main_module.attr("__dict__");

	initmodule();
	bp::exec( "from module import *", main_namespace );
	
	// create a NULL device to detect screen resolution
	IrrlichtDevice *nulldevice = createDevice(video::EDT_NULL);

	core::dimension2d<u32> deskres = nulldevice->getVideoModeList()->getDesktopResolution();

	nulldevice -> drop();
	
	IrrlichtDevice* device =
	createDevice
	(
		video::EDT_OPENGL,
		deskres,
		16,
		true,	// fullscreen
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
    smgr->addCameraSceneNode
    (
		0	// parent
    );
    
    // place camera and point towards Jimmy's face
    camera->setPosition(core::vector3df(-2, 3.5, 0));
    camera->setTarget(core::vector3df(0, 3, 0));
    
	// load mesh
	scene::IAnimatedMesh* mainCharMesh =
	smgr->getMesh("models/dcu_jimmy_olsen/Jimmy.b3d");
	
	// add mesh to the scene
	mainCharacter.node =
	smgr->addAnimatedMeshSceneNode( mainCharMesh );
	
	if( mainCharacter.node )
	{
		
		mainCharacter.node->setRotation( core::vector3df(0, 90, 0) );
		
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
		
	}
	
	
	scene::ILightSceneNode* light1 = 
	smgr->addLightSceneNode
	(
		0,
		core::vector3df(0, 0, -10),
		video::SColorf(1, 0.5, 0.5, 0),
		800
	);
		
	light1->setDebugDataVisible( scene::EDS_BBOX );
	
	mainCharacter.node->setFrameLoop(1, 100);
	mainCharacter.node->setLoopMode(true);
	mainCharacter.node->setAnimationSpeed(5);
	
	bp::exec("getMainCharacter().setAnimation(0)", main_namespace);
	
	//GUI
	
	gui::IGUIFont* font = gui->getFont(io::path("font/myfont.xml"));
	
	gui::IGUIEditBox *editBox =
	gui->addEditBox
	(
		L"",
		core::rect<s32>
		(
			deskres.Width/4, 3*(deskres.Height/4),
			3*(deskres.Width/4), 3*(deskres.Height/4)+20
		)
	);
	
	editBox->setOverrideFont(font);
	
	
	console =
	new gui::TextBox
	(
		//gui->getBuiltInFont(),
		font,
		L"",
		gui,
		core::rect<s32>
		(
			deskres.Width/4, 3*(deskres.Height/4)+22,
			3*(deskres.Width/4), deskres.Height-4
		)
		
	);
	
	eventReceiver.setConsole( console );
	
	
	while( device->run() )
	{
		
		//mainCharacter.node->animateJoints();
		
		driver->beginScene
		(
			true,
			true,
			video::SColor(255, 222, 255, 255)
		);
		
		smgr->drawAll();
		gui->drawAll();
		
		driver->endScene();
		
	}
	
	Py_Finalize();
	
}
