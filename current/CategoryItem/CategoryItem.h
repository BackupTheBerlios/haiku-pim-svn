/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _CATEGORY_ITEM_H_
#define _CATEGORY_ITEM_H_

#include <InterfaceKit.h>
#include <GraphicsDefs.h>
#include <SupportDefs.h>
#include <Handler.h>
#include <Looper.h>

#include "IconListItem.h"

const uint32	kColorSelected		= 'CSEL';
const uint32	kColorChanged		= 'CCHG';
const uint32 	kColorReverted 		= 'CREV';

const uint32	kCategorySelected	= 'CATS';
const uint32	kCategoryInvoked	= 'CATI';


/*!	\class		ColorUpdateWindow
 *	\brief		A control that allows editing color and optionally text label.
 *	\details	Opens in a new modal window that can't be closed except by clicking "Ok".
 */
class 	ColorUpdateWindow
	:
	public BWindow
{
	protected:
		rgb_color currentColor, originalColor;
		BString originalString;
		BColorControl* colorControl;
		BView* labelView;
		BButton* okButton;
		BButton* revertButton;
		BMessage* messageToSend;
		BHandler* target;
		bool dirty;		//!< Defines the settings were changed.
		bool enableEditingLabel;	//!< Defines the label can be edit.
		
		

	public:
		ColorUpdateWindow( BPoint corner,
						   BString &label,
						   rgb_color	&defaultColor,
						   bool enableEditingLabel = false,
						   BString title = "Color selector",
						   BHandler *target = NULL,
						   screen_id currentScreen = B_MAIN_SCREEN_ID,
						   BMessage *message = NULL );
		virtual ~ColorUpdateWindow();
		virtual void MessageReceived(BMessage* message);
};


/*!	\class		CategoryListItem
 *	\brief		Represents a category in BListView and BOutlineListView.
 *	\details	Based on "IconListItem" sample class by Fabien Fulhaber.
 */
class CategoryListItem
	:
	public BListItem
{
	public:
		CategoryListItem( rgb_color color, BString& label, int level = 0, bool expanded = false);
		virtual ~CategoryListItem( );
		
		virtual void DrawItem( BView *owner, BRect frame, bool touchEverything = false );
		virtual void Update( BView *owner, const BFont *font );	
		
	protected:
		rgb_color	currentColor;
		BRect 		bounds;
		BString 	currentLabel;
		BBitmap*	icon;
		
		bool 		thereAreItemsInCategory;
		
		BBitmap*	CreateIcon( const rgb_color color, BBitmap* toChange = NULL );
		
};



/*!	\class 		CategoryListView
 *	\brief		Extention of the BListView, especially built for handling categories.
 *	\details	Internally manages the scrollbars. Provides support for invocation
 *				on an item (which represent a Category), opening an edit window.
 */
class CategoryListView
	:
	public BListView
{
	public:
		CategoryListView( BRect frame, const char* name );
		virtual ~CategoryListView();
		
		virtual void FrameResized( float width, float height );
		virtual void GetPreferredSize( float* width, float* height );
		
		virtual void RefreshContents( BMessage* preferences ) {};
		virtual void SetScroller( BScrollView* scrollView = NULL );
		virtual BScrollView* GetScroller() const;
		
	protected:
		BScrollView*	scrollView;
		
		virtual void FixupScrollbars();
};	// <-- end of class CategoryListView



/*!	\class		CategoryMenuItem
 *	\brief		Represents a category in the menu.
 *	\details	Based on IconMenuItem by Fabien Fulhaber.
 */
class	CategoryMenuItem
	:
	public BMenuItem
{
	public:
		CategoryMenuItem( BString& label,
						  const rgb_color color,
						  BMessage* message );
		virtual ~CategoryMenuItem();
		
		inline virtual void SetShortcut( char shortcut,
										 uint32 modifiers ) { }
		virtual void GetContentSize( float *width, float* height );
		virtual void DrawContent( void );
		
	protected:
		BBitmap* icon;
		
		virtual BBitmap*	CreateIcon( const rgb_color color, BBitmap* toChange = NULL );
};


/*!	\class		CategoryMenu
 *	\brief		Represents a menu filled with categories.
 *	\details	The only difference from ordinary BMenu with "items in column" layout
 *				is that this menu populates itself with pre-defined Categories.
 */
class	CategoryMenu
	:
	public BMenu
{
	public:
		CategoryMenu( const char* name, BMessage* preferences = NULL );
		virtual ~CategoryMenu();
	
		virtual void RefreshMenu( BMessage* preferences );
	
	protected:
	
};


#endif // _CATEGORY_ITEM_H_
