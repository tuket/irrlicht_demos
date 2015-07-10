#include <irrlicht/irrlicht.h>
#include <iostream>

#ifndef TEXTBOX
#define TEXTBOX

namespace irr
{
namespace gui
{

/**
 * This widget is a textbox with scrollbar
 **/

class TextBox : public IGUIElement
{

public:
	
	TextBox
	(
		IGUIFont* font,					//< font of the text
		const core::stringw& text,		//< displayed text
		IGUIEnvironment* environment,	//< gui environment
		core::rect<s32> rectangle,		//< position and dimensions
		IGUIElement* parent = 0,		//< parent
		s32 id = -1						//< id
	);
	
	/**
	 * \brief handle events
	 * remember to return false if the event is not treated
	 **/
	virtual bool OnEvent( const SEvent& event );
	
	/**
	 * \brief set the content of the TextBox, erasing the previous \
	 * contents
	 **/
	virtual void setText( const core::stringw& text );
	
	/**
	 * \brief append content to the TextBox.
	 **/
	virtual void addText( const core::stringw& text );
	
	/**
	 * Set the position of this TextBox relative to its parent
	 */
	void setRelativePosition( const core::rect<s32> rectangle );
	
private:
	
	/**
	 * \brief call this function when the text is changed
	 * updates the size of the scroll bar and sets the text position
	 * to the end
	 **/
	void reset();
	
	/**
	 * \brief set the first line the can be seen.
	 * Specify how many lines are skept until the first one shown.
	 * For example: 0 -> the text is displayed from the first line,
	 * 1 -> the text is displayed from the second line
	 **/
	void setTextDisplacement(s32 line);
	
	IGUIScrollBar* scrollBar;
	IGUIStaticText* staticText;
	core::stringw text;
	IGUIFont* font;
	
};


}
}

#endif
