#include <String.h>
#include <Alert.h>
#include <Point.h>

#include "clsMainWindow.h"
#include "CalendarControl.h"
#include "GregorianCalendarModule.h"


 
MainView::MainView(BRect frame) 
	:
	BView(frame, 
			NULL, 
			B_FOLLOW_TOP|B_FOLLOW_LEFT, 
			B_FRAME_EVENTS|B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE)
{
	this->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	BRect tempRect = this->Bounds();
	tempRect.InsetBy(5, 5);
	tempRect.bottom = tempRect.top+20;
	
	GregorianCalendar* gregCal = new GregorianCalendar();
	if (!gregCal) { exit(1); }
	listOfCalendarModules.AddItem((void*)gregCal);
	
	CalendarControl* cont = new CalendarControl(tempRect,
				"calendar",
				"Date:");
				
	this->AddChild(cont);
	AttachedToWindow();
	cont->AttachedToWindow();
/*
	tempRect.right -= B_V_SCROLL_BAR_WIDTH;
	tempRect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	
	BListView* listView = new BListView(tempRect, "list");
	if (!listView) { exit(1); }
	
	this->scrollView = new BScrollView("scroll",
			listView,
			B_FOLLOW_LEFT | B_FOLLOW_TOP,
			B_FRAME_EVENTS,
			true, true);

	if (!scrollView) { exit(1); }
	((BScrollBar*)(scrollView->ScrollBar(B_VERTICAL)))->SetSteps(5, 20);
	((BScrollBar*)(scrollView->ScrollBar(B_HORIZONTAL)))->SetSteps(5, 20);

	 
	BBitmap *icon = NULL;
	icon = CreateIcon(kBlue);
	IconListItem *item = new IconListItem(icon,
										  "Test1",
										  0,
										  false);											  
	if (!item) { exit(1); }
	
	listView->AddItem(item);
	
	icon = CreateIcon(kMagen);
	item = new IconListItem(icon,
						  "Test2",
						  0,
						  false);											  
	if (!item) { exit(1); }
	listView->AddItem(item);
	
	icon = CreateIcon(kWhite);
	item = new IconListItem(icon,
						  "White icon",
						  0,
						  false);											  
	if (!item) { exit(1); }
	listView->AddItem(item);
	
	icon = CreateIcon(kMedGray);
	item = new IconListItem(icon,
						  "Категория на русском",
						  0,
						  false);											  
	if (!item) { exit(1); }
	listView->AddItem(item);

	BString categoryName("Категория с именем из BString");
	icon = CreateIcon(kBlue);	
	item = new IconListItem(icon,
				categoryName.String(),
				0,
				false);
	if (!item) { exit(1); }
	listView->AddItem(item);
	
	
	this->AddChild(scrollView);
	FixupScrollbars();
*/
}

void MainView::FrameResized(float width, float height) {
	BView::ResizeTo(width, height);
	BView::FrameResized(width, height);
	for (int i = 0; i < this->CountChildren(); i++) {
		((BView*)(this->ChildAt(i)))->FrameResized(width, height);
	}
	
	
/*	if (scrollView) {
		scrollView->ResizeTo(width-5, height-5);
	}
	width  -= (B_H_SCROLL_BAR_HEIGHT+9);
	height -= (B_V_SCROLL_BAR_WIDTH+9);
	BView* list = FindView("list");
	if (list) {
		list->ResizeTo(width, height);
	}

	FixupScrollbars();
*/
}
	
void MainView::AttachedToWindow(void) {
	BView::AttachedToWindow();
	BView* child = this->ChildAt(0);
	while (!child) {
		child->AttachedToWindow();
		child->NextSibling();
	}
}


void MainView::FixupScrollbars()
{
/*	BRect bounds=(Bounds()).InsetBySelf(5, 5);
	bounds.right -= B_V_SCROLL_BAR_WIDTH;
	bounds.bottom -= B_H_SCROLL_BAR_HEIGHT;
	BScrollBar *sb;
	BListView* listView = (BListView*)this->FindView("list");
	float ratio=1, realRectWidth=1, realRectHeight=1;
	if (!listView || !scrollView) { return; }

	listView->GetPreferredSize(&realRectWidth, &realRectHeight);
	
	realRectHeight = listView->CountItems() * 18;
	realRectWidth += 15 + B_V_SCROLL_BAR_WIDTH;
	
	sb = scrollView->ScrollBar(B_HORIZONTAL);
	if (sb) {
		ratio = bounds.Width() / (float)realRectWidth;		

		sb->SetRange(0, realRectWidth-bounds.Width());	
		if (ratio >= 1) {
			sb->SetProportion(1);
		} else {
			sb->SetProportion(ratio);
		}
	}

	sb = scrollView->ScrollBar(B_VERTICAL);
	if (sb) {
		ratio = bounds.Height() / (float)realRectHeight;
		
		sb->SetRange(0, realRectHeight+2);
		if (ratio >= 1) {
			sb->SetProportion(1);
		} else {
			sb->SetProportion(ratio);
		}
	}
*/
}

clsMainWindow::clsMainWindow(const char *uWindowTitle)
:
	BWindow(
		BRect(64, 64, 320, 256),
		uWindowTitle,
		B_TITLED_WINDOW,
		0	)
{
	mainView = new MainView(BWindow::Bounds());
			
	if (mainView != NULL)
	{
		mainView->AttachedToWindow();
		
		BWindow::AddChild(mainView);
	}
	else
	{
		AppReturnValue = B_NO_MEMORY;
		be_app->PostMessage(B_QUIT_REQUESTED);
	}
}

clsMainWindow::~clsMainWindow()
{
}

BBitmap* MainView::CreateIcon(const rgb_color colorIn)
{
	BRect rect(0, 0, 15, 15);
	BBitmap* toReturn = new BBitmap(rect, B_CMAP8, true);
	BView* drawing = new BView(rect, 
								"drawer", 
								B_FOLLOW_LEFT | B_FOLLOW_TOP,
								B_WILL_DRAW);
	if (!drawing || !toReturn) { return NULL; }	
	toReturn->AddChild(drawing);
	if (toReturn->Lock()) {
		drawing->SetHighColor(kWhite);
		drawing->FillRect(rect);
		drawing->SetHighColor(kBlack);
		drawing->SetPenSize(1);
		drawing->StrokeRect(rect);
		drawing->SetHighColor(colorIn);
		drawing->FillRect(rect.InsetBySelf(1, 1));
		drawing->Sync();
		toReturn->Unlock();			
	}
	toReturn->RemoveChild(drawing);
	delete drawing;
	return toReturn;
}

void clsMainWindow::MessageReceived(BMessage * Message)
{
	BAlert* al;
	BString sb;
	BPoint pt;
	switch(Message->what)
	{
/*		case (B_MOUSE_DOWN):
			pt = Message->FindPoint("where");
			sb << "Mouse down at " << pt.x << " and  " << pt.y;
			al = new BAlert("AAA", sb.String(), "Ok");
			if (al) al->Go();
*/	
		default:
		  BWindow::MessageReceived(Message);
		  break;
	}
}

bool clsMainWindow::QuitRequested()
{
	be_app->PostMessage( B_QUIT_REQUESTED);
	return BWindow::QuitRequested();
}

void clsMainWindow::FrameResized(float w, float h) {
	BWindow::FrameResized(w, h);
	mainView->FrameResized(w, h);
//	BRect frame = this->Bounds();
//	mainView->FrameResized(frame.Width(), frame.Height());	
}
