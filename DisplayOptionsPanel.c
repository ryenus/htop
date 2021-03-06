/*
htop - DisplayOptionsPanel.c
(C) 2004-2011 Hisham H. Muhammad
Released under the GNU GPLv2, see the COPYING file
in the source distribution for its full text.
*/

#include "config.h" // IWYU pragma: keep

#include "DisplayOptionsPanel.h"

#include <stdbool.h>
#include <stdlib.h>

#include "CheckItem.h"
#include "CRT.h"
#include "FunctionBar.h"
#include "Header.h"
#include "Object.h"
#include "ProvideCurses.h"
#include "XUtils.h"


static const char* const DisplayOptionsFunctions[] = {"      ", "      ", "      ", "      ", "      ", "      ", "      ", "      ", "      ", "Done  ", NULL};

static void DisplayOptionsPanel_delete(Object* object) {
   Panel* super = (Panel*) object;
   DisplayOptionsPanel* this = (DisplayOptionsPanel*) object;
   Panel_done(super);
   free(this);
}

static HandlerResult DisplayOptionsPanel_eventHandler(Panel* super, int ch) {
   DisplayOptionsPanel* this = (DisplayOptionsPanel*) super;

   HandlerResult result = IGNORED;
   CheckItem* selected = (CheckItem*) Panel_getSelected(super);

   switch(ch) {
   case 0x0a:
   case 0x0d:
   case KEY_ENTER:
   case KEY_MOUSE:
   case KEY_RECLICK:
   case ' ':
      CheckItem_set(selected, !CheckItem_get(selected));
      result = HANDLED;
   }

   if (result == HANDLED) {
      this->settings->changed = true;
      Header* header = this->scr->header;
      Header_calculateHeight(header);
      Header_reinit(header);
      Header_draw(header);
      ScreenManager_resize(this->scr, this->scr->x1, header->height, this->scr->x2, this->scr->y2);
   }
   return result;
}

const PanelClass DisplayOptionsPanel_class = {
   .super = {
      .extends = Class(Panel),
      .delete = DisplayOptionsPanel_delete
   },
   .eventHandler = DisplayOptionsPanel_eventHandler
};

DisplayOptionsPanel* DisplayOptionsPanel_new(Settings* settings, ScreenManager* scr) {
   DisplayOptionsPanel* this = AllocThis(DisplayOptionsPanel);
   Panel* super = (Panel*) this;
   FunctionBar* fuBar = FunctionBar_new(DisplayOptionsFunctions, NULL, NULL);
   Panel_init(super, 1, 1, 1, 1, Class(CheckItem), true, fuBar);

   this->settings = settings;
   this->scr = scr;

   Panel_setHeader(super, "Display options");
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Tree view"), &(settings->treeView)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Shadow other users' processes"), &(settings->shadowOtherUsers)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Hide kernel threads"), &(settings->hideKernelThreads)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Hide userland process threads"), &(settings->hideUserlandThreads)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Display threads in a different color"), &(settings->highlightThreads)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Show custom thread names"), &(settings->showThreadNames)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Show program path"), &(settings->showProgramPath)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Highlight program \"basename\""), &(settings->highlightBaseName)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Highlight large numbers in memory counters"), &(settings->highlightMegabytes)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Leave a margin around header"), &(settings->headerMargin)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Detailed CPU time (System/IO-Wait/Hard-IRQ/Soft-IRQ/Steal/Guest)"), &(settings->detailedCPUTime)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Count CPUs from 1 instead of 0"), &(settings->countCPUsFromOne)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Update process names on every refresh"), &(settings->updateProcessNames)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Add guest time in CPU meter percentage"), &(settings->accountGuestInCPUMeter)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Also show CPU percentage numerically"), &(settings->showCPUUsage)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Also show CPU frequency"), &(settings->showCPUFrequency)));
   #ifdef HAVE_LIBSENSORS
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Also show CPU temperature"), &(settings->showCPUTemperature)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Show temperature in degree Fahrenheit"), &(settings->degreeFahrenheit)));
   #endif
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Enable the mouse"), &(settings->enableMouse)));
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Highlight new and old processes"), &(settings->highlightChanges)));
   #ifdef HAVE_LIBHWLOC
   Panel_add(super, (Object*) CheckItem_newByRef(xStrdup("Show topology when selecting affinity by default"), &(settings->topologyAffinity)));
   #endif
   return this;
}
