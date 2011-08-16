#include "HWindow.h"
#include "IconMenuItem.h"

HWindow::HWindow(BRect rect ,const char* name)
	:BWindow(rect,name,B_DOCUMENT_WINDOW,0)
{
	InitMenu();
}

HWindow::~HWindow()
{

}

void
HWindow::InitGUI()
{
}

void
HWindow::InitMenu()
{
	BMenuBar *menuBar = new BMenuBar(Bounds(),"MENUBAR");
	BMenu *menu;
	IconMenuItem *item;
	
	menu = new BMenu("File");
	item = new IconMenuItem("PoorMan",NULL,0,0,this->GetBitmapResource('BBMP',"BMP:POORMAN"));
	menu->AddItem(item);
	item = new IconMenuItem("CodyCam",NULL,0,0,this->GetBitmapResource('BBMP',"BMP:CODYCAM"));
	menu->AddItem(item);
	item = new IconMenuItem("BeMail",NULL,0,0,this->GetBitmapResource('BBMP',"BMP:MAIL"));
	menu->AddItem(item);
	item = new IconMenuItem("Camera",NULL,0,0,this->GetBitmapResource('BBMP',"BMP:CAMERA"));
	menu->AddItem(item);
	
	menuBar->AddItem(menu);
	this->AddChild(menuBar);
}

BBitmap*
HWindow::GetBitmapResource(type_code type,const char* name)
{
	size_t len = 0;
	BResources *rsrc = BApplication::AppResources();
	const void *data = rsrc->LoadResource(type, name, &len);

	if (data == NULL) {
		return NULL;
	}
	
	BMemoryIO stream(data, len);
	
	// Try to read as an archived bitmap.
	stream.Seek(0, SEEK_SET);
	BMessage archive;
	status_t err = archive.Unflatten(&stream);
	if (err != B_OK)
		return NULL;

	BBitmap* out = new BBitmap(&archive);
	if (!out)
		return NULL;

	err = (out)->InitCheck();
	if (err != B_OK) {
		delete out;
		out = NULL;
	}
	
	return out;
}


bool
HWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}