#include "screens.h"

#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"
#include "soogh-debug.h"
#include <treemenu.h>

// C-style callbacks

MenuScreen::MenuScreen(SooghGUI& g) : Screen(g)
{
};

void MenuScreen::load()
{

};

bool MenuScreen::loop()
{
    
    return false;
};

bool MenuScreen::handle(soogh_event_t e)
{
	if(e>KEY_C)
		DBG("e = %s", soogh_event_name(e));
	return false;
};
