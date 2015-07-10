#include "text_box.hpp"
#include <iostream>

namespace irr
{
namespace gui
{


TextBox::TextBox
(
	IGUIFont* font,
	const core::stringw& text,
	IGUIEnvironment* environment,
	core::rect<s32> rectangle,
	IGUIElement* parent,
	s32 id
)
:
IGUIElement
(
	EGUIET_ELEMENT,
	environment,
	parent,
	id,
	rectangle
)
{
	
	if( !parent )
	{
		environment->getRootGUIElement()->addChild(this);
	}
	
	this->text = text;
	this->font = font;
	
	staticText =
	environment->addStaticText
	(
		L"console init",
		core::rect<s32>
		(
			0, 0,
			rectangle.getWidth()-10,
			rectangle.getHeight()
		),
		true,
		true,
		this
	);
	
	staticText->setOverrideFont(font);
	
	scrollBar =
	environment->addScrollBar
	(
		false,				// horizontal
		core::rect<s32>
		(
			rectangle.getWidth()-10, 0,
			rectangle.getWidth(), rectangle.getHeight()
		),
		this				// parent
	);
		
	staticText->setDrawBorder(true);
	staticText->setBackgroundColor( video::SColor(220, 220, 220, 250) );
	
	reset();
	
}


bool TextBox::OnEvent( const SEvent& event )
{
	
	if( Parent )
	{
		Parent->OnEvent( event );
	}
	
	// MOUSE WHEEL
	if
	(
		event.EventType == EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_MOUSE_WHEEL
	)
	{
		
		scrollBar->setPos
		(
			scrollBar->getPos() +
			(s32) event.MouseInput.Wheel * (-1)
		);
		
		s32 line = scrollBar->getPos();
		setTextDisplacement(line);
		
		std::cout << line << std::endl;
		
		return true;
		
	}
	
	// SCROLL BAR
	if
	(
		event.EventType == EET_GUI_EVENT &&
		event.GUIEvent.EventType == EGET_SCROLL_BAR_CHANGED
	)
	{
		
		s32 line = scrollBar->getPos();
		setTextDisplacement(line);
		
		return true;
		
	}
	
	return false;
	
}

void TextBox::setText( const core::stringw& text )
{
	
	this->text = text;
	
	staticText->setText( this->text.c_str() );
	
	staticText->setDrawBorder(true);
	staticText->setBackgroundColor( video::SColor(220, 220, 220, 250) );
	
	reset();
	
}

void TextBox::addText( const core::stringw& text )
{
	
	this->text += text;
	
	staticText->setText( this->text.c_str() );
	
	reset();
	
}
	
void TextBox::setRelativePosition( const core::rect<s32> rectangle )
{
	
	RelativeRect = rectangle;
	updateAbsolutePosition();
		
}

void TextBox::reset()
{
	
	const s32 textHeight = staticText->getTextHeight();
	const s32 rectHeight = RelativeRect.getHeight();
	
	s32 scrollBarSize =
		(textHeight < rectHeight) ?
			0 :	// scroll disabled if not enough text
			textHeight - rectHeight;
		
	scrollBar->setMax( scrollBarSize );
	
	if( textHeight < rectHeight )
	{
		scrollBar->setPos
		(
			0
		);
		setTextDisplacement( scrollBar->getPos() );
	}
	
	else
	{
		scrollBar->setPos
		(
			textHeight - rectHeight
		);
		setTextDisplacement( scrollBar->getPos() );
	}
	
	
	
}

void TextBox::setTextDisplacement( s32 line )
{
	
	staticText->setRelativePosition
	(
		core::rect<s32>
		(
			0, -line,
			RelativeRect.getWidth(), RelativeRect.getHeight()
		)
	);
	
}

}
}
