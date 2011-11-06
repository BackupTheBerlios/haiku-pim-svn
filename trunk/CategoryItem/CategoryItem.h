/*
 * Copyright 2011 Alexey Burshtein <aburst02@campus.haifa.ac.il>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _CATEGORY_ITEM_H_
#define _CATEGORY_ITEM_H_

#include <iostream>
#include <ostream>
#include <stdlib.h>

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

/*!	\struct 	Category
 *	\brief		Represents a category with its name and color.
 *	\details	
 */
struct Category {
	rgb_color	categoryColor;
	BString 	categoryName;
	
	//! \brief		Constructor without a color - assigns random color.
	Category( const BString &nameIn );
	Category( const BString &nameIn, rgb_color colorIn );
	Category( const Category &in );
	Category( const Category *in );
	
	/*!	\note	Operators on categories
	 *			In all of the following operators, except for output and assignment,
	 *			the color is not important. Only the name matters.
	 */
	inline bool operator== ( const Category& in ) const {
		return ( this->categoryName == in.categoryName );
	}
	inline bool operator!= ( const Category& in ) const {
		return ( this->categoryName != in.categoryName );
	}
	inline bool operator< ( const Category& in ) const {
		return ( this->categoryName < in.categoryName );
	}
	inline bool operator> ( const Category& in ) const {
		return ( this->categoryName > in.categoryName );
	}
	inline bool operator<= ( const Category& in ) const {
		return ( ! ( this->categoryName > in.categoryName ) );
	}
	inline bool operator>= ( const Category& in ) const {
		return ( ! ( this->categoryName < in.categoryName ) );
	}
	inline Category operator= ( const Category& in ) {
		this->categoryName = in.categoryName;
		this->categoryColor = in.categoryColor;
		return *this;
	}
	
};



extern BList global_ListOfCategories;	//!< List that holds all categories in the system.

Category*	FindCategory( const BString& name );
Category*	FindDefaultCategory();
void	AddCategoryToGlobalList( const Category *toAdd );
bool	MergeCategories( BString& source, BString& target );

/*!	\brief		Just a shortcut for another function.
 *	\details	Creates a temporary object of type Category and calls the other function
 *				synchronously.
 */
inline void	AddCategoryToGlobalList( const BString &name, rgb_color color )
{
	Category* toAdd = new Category( name, color );
	AddCategoryToGlobalList( toAdd );
}

/*!	\brief		Just a shortcut for another function.
 */
inline void AddCategoryToGlobalList( const Category &toAdd )
{
	AddCategoryToGlobalList( &toAdd );
}

/*!	\brief		Just a shortcut for another function.
 *	\details	Creates a temporary object of type Category, assigns it a random color
 *				and calls the other function synchronously.
 */
inline void AddCategoryToGlobalList( const BString &name )
{
	Category toAdd( name );
	AddCategoryToGlobalList( toAdd );	
}


void		DeleteCategoryFromGlobalList( const BString& toDelete );


/*!	\brief		Compares two categories using BString's operator<.
 */
inline int		CategoriesCompareFunction( const void* in1, const void* in2 )
{
	return ( ( (const Category*)in1)->categoryName < ( ( const Category* )in2)->categoryName );
}



/*!
 *	\brief		Creates a random color.
 *	\note		What color can be used?
 *				Actually, all colors except for black (0, 0, 0) are allowed.
 *				The color is fully opaque. Values for Red, Green and Blue
 *				are selected randomly.
 */
inline rgb_color	CreateRandomColor()
{
	rgb_color toReturn;
	toReturn.red	= rand() % 0xFF;
	toReturn.green	= rand() % 0xFF;
	toReturn.blue	= rand() % 0xFF;
	toReturn.alpha  = 0xFF;		// The color is always opaque!
	
	// If the color is black, we'll modify it to become not black.
	if ( !toReturn.red && !toReturn.blue && !toReturn.green )
	{
		toReturn.blue = 1;
	}
	
	return toReturn;
}	// <-- end of function CreateRandomColor.


/*!
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
		BLooper* looper;
		bool dirty;		//!< Defines the settings were changed.
		bool enableEditingLabel;	//!< Defines the label can be edit.
		
		

	public:
		ColorUpdateWindow( const Category& catIn,
						   bool enableEditingLabelIn,
						   const char* title,
						   BHandler *target = NULL,
						   BLooper* looper = NULL,
						   BMessage* message = NULL );
		ColorUpdateWindow( BPoint corner,
						   BString &label,
						   rgb_color	&defaultColor,
						   bool enableEditingLabel = false,
						   const char* title = "Color selector",
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
		CategoryListItem( rgb_color color, BString& label, int level = 0, bool expanded = false );
		CategoryListItem( const Category &cat, int level = 0, bool expanded = false );
		CategoryListItem( const Category *cat, int level = 0, bool expanded = false );
		virtual ~CategoryListItem( );
		
		virtual void DrawItem( BView *owner, BRect frame, bool touchEverything = false );
		virtual void Update( BView *owner, const BFont *font );	
		virtual inline rgb_color GetColor() const { return currentColor; }
		virtual inline BString GetLabel() const { return currentLabel; }
		virtual inline void UpdateColor( rgb_color newColor ) {
			currentColor = newColor;
			icon = CreateIcon( newColor, icon );
		}
		
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
		
		virtual bool AddItem( BListItem* toAdd ) {
			bool toReturn = BListView::AddItem( toAdd );
			FixupScrollbars();
			return toReturn;
		}
		
		virtual bool AddItem( BListItem* toAdd, int index ) {
			bool toReturn = BListView::AddItem( toAdd, index );
			FixupScrollbars();
			return toReturn;
		}
		
		virtual bool AddItem( CategoryListItem* toAdd );
		virtual void FrameResized( float width, float height );
		virtual void GetPreferredSize( float* width, float* height );
		
		virtual void RefreshList( BMessage* preferences = NULL );
		virtual void TargetedByScrollView( BScrollView* scroller );
		
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
		CategoryMenuItem( const BString& label,
						  const rgb_color color,
						  BMessage* message );
		CategoryMenuItem( const Category &categoryIn, BMessage *message );
		CategoryMenuItem( const Category *categoryIn, BMessage *message );
		virtual ~CategoryMenuItem();
		
		virtual void   UpdateColor( rgb_color newColor );
		inline virtual BString GetLabel() const { return BString( Label() ); }
		inline virtual rgb_color GetColor() const { return currentColor; }
		
		inline virtual void SetShortcut( char shortcut,
										 uint32 modifiers ) { }
		virtual void GetContentSize( float *width, float* height );
		virtual void DrawContent( void );
		
	protected:
		BBitmap* icon;
		rgb_color currentColor;
		
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
		CategoryMenu( const char* name,
					  bool withSeparator = false,
					  BMessage* templateMessage = NULL,
					  BMessage* preferences = NULL );
		virtual ~CategoryMenu();
	
		virtual void RefreshMenu( BMessage* preferences );
		virtual void UpdateItem( BString& oldLabel, BString& newLabel, rgb_color newColor ) {};
		virtual bool AddItem( CategoryMenuItem* item );

		virtual bool GetWithSeparator( void ) const {
			return bWithSeparator;
		}
		virtual void SetWithSeparator( bool ws ) {
			bWithSeparator = ws;
		}
	
	protected:
	
		/*!	\details		If this variable is "true", the menu upon creation or refresh
		 *				will contain a separator item which will be selected by default.
		 *				If this variable is "false", the "Default" category will be
		 *				selected, or the first one, if there's no "Default" category.
		 */
		bool bWithSeparator;
		
		/*!	\details		This message is the template message to be sent by every
		 *						invoked item. If item does not have a message, a copy of
		 *						template message is assigned to it. Additional data assigned
		 *						to message of every item in the menu is its Category Name.
		 */
		BMessage* fTemplateMessage;
};


#endif // _CATEGORY_ITEM_H_
