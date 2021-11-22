#include "screens.h"

#include <memory>
#include <soogh.h>

#include "gui.h"

#include "config.h"
#include "tools-log.h"
#include "globals.h"

// C-style callbacks
#define KEY_SLOW 				500
#define KEY_FAST 				100
#define KEY_VERY_FAST 			50
#define KEY_STEP_SLOW 			0.1
#define KEY_STEP_FAST 			0.5

MenuScreen::MenuScreen(SooghGUI& g) : Screen(g)
{

};

bool MenuScreen::loop()
{
    
    return false;
};

bool MenuScreen::handle(event_t key)
{
	// DBG("KEY: %x", key);
	return false;
};
