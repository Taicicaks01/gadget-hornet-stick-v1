#include "OSDApp.h"
#include "OSDManager.h"

// Map AppInterface callbacks to existing osdManager functions
static void osd_onEnter() { osdManager.onEnterMenu(); }
static void osd_update() { osdManager.update(); }
static void osd_render() { osdManager.render(); }
static void osd_onExit() { osdManager.onExit(); }

const AppInterface osdApp = {
    .id = APP_OSD,
    .name = "Onscreen Display",
    .onEnter = osd_onEnter,
    .update = osd_update,
    .render = osd_render,
    .onExit = osd_onExit
};
