/* Native loading overlay. The renderer owns its shared shader/vertex storage;
 * no Xbox background textures, file copies, audio drone or worker priority. */
#include "cseries.h"
#include "progress_bar.h"
#include "engine_renderer.h"
static boolean initialized,active,enabled=TRUE,prepared;
void progress_bar_initialize(void) { initialized=TRUE; active=FALSE; prepared=FALSE; enabled=TRUE; }
void progress_bar_dispose(void) { initialized=active=prepared=FALSE; }
void progress_bar_begin(boolean skip_frame_capture)
{
    assert(initialized);
    /* Draw over the current window; this reduced-fidelity backend does not
     * capture or animate a previous frame for either capture mode. */
    (void)skip_frame_capture;
    active=TRUE;
}
void progress_bar_end(void) { active=FALSE; }
boolean progress_bar_is_active(void) { return active; }
void progress_bar_enable(boolean value) { enabled=value; }
boolean progress_bar_is_stuff_ready(void) { return prepared && n3ds_gpu_loading_ready(); }
void progress_bar_eachframe(void) { /* No privately owned textures or audio to retire. */ }
void progress_bar_display(real progress)
{
    assert(progress>=0.f && progress<=1.f);
    if(!active || !enabled || progress<=0.f) return;
    assert(initialized && n3ds_engine_window_active());
    assert(n3ds_gpu_loading_draw(progress));
    prepared=TRUE;
}
/* port: a map's download (port/linux/game/download_screen.c): its screen
 * draws its own text over the menus; no loading screen is drawn over them
 * (the fork's draws its loading screen at the download's progress) */
static real download_progress;
void progress_bar_download_display(real progress) { download_progress=progress; }
void progress_bar_download_end(void) { download_progress=0.f; }
