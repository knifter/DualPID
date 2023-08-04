#ifndef __SCREENS_BOOT_H
#define __SCREENS_BOOT_H

#include <stdlib.h>
#include <stdint.h>

#include <soogh.h>

#include "gui.h"

/*** BOOT ************************************************************************************/
class BootScreen : public Screen
{
    public:
		BootScreen(SooghGUI &g);
		virtual ScreenType type() { return ScreenType::BOOT; };

		virtual void loop();
		virtual void load();
		virtual bool handle(soogh_event_t);

    private:
        uint32_t    _start = 0;
};

#endif // __SCREENS_BOOT_H
