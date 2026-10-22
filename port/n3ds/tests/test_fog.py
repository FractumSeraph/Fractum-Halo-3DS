# New3DS integration additions by Kevin Boggs.
"""Exercise the production distance controller with deterministic frame costs."""
from pathlib import Path
import re
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[3]

# Kevin Boggs: main
def main():
    compiler = sys.argv[sys.argv.index('--') + 1:]
    policy = (ROOT / 'port/n3ds/include/engine_fog_policy.inl').read_text()
    policy = re.sub(r'^#include .*\n', '', policy, flags=re.M)
    gpu = (ROOT / 'port/n3ds/include/engine_gpu_fog.inl').read_text()
    clip = gpu[gpu.index('static int distance_fog_clip('):gpu.index('/* Kevin Boggs: n3ds_gpu_fog_backdrop */')]
    policy += clip
    render = (ROOT / 'port/n3ds/src/engine_mission_render.c').read_text()
    begin = render.index('static int batch_outside_plane(')
    bounds = render[begin:render.index('/* Reuse the largest', begin)]
    policy += r'''
typedef float real;
typedef struct {float i,j,k;} real_vector3d;
typedef struct {float x,y,z;} real_point3d;
typedef struct {real_vector3d n;float d;} real_plane3d;
struct render_frustum {real_plane3d world_planes[6];};
struct mission_batch {real_point3d center;real_vector3d extent;};
#define NONE -1
float dot_product3d(const real_vector3d *a,const real_vector3d *b){return a->i*b->i+a->j*b->j+a->k*b->k;}
''' + bounds
    source = r'''
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
struct render_camera {float vertical_field_of_view,z_near,z_far;};
struct render_fog {float atmospheric_maximum_density,atmospheric_maximum_distance;struct {float n[3];} atmospheric_color;};
static int original,menu,cinematic;static unsigned int generation=1;
static const char *map="levels\\a30\\a30";
static float fog_end;
unsigned int n3ds_engine_cache_generation(void){return generation;}
int n3ds_input_platform_original_model(void){return original;}
int main_menu_is_active(void){return menu;}
int cinematic_in_progress(void){return cinematic;}
const char *main_get_map_name(void){return map;}
void n3ds_gpu_fog_config(float start,float end,unsigned int rgb){fog_end=end;}
void n3ds_log(const char *s){}
/* POLICY */
static void reset(int old){
 original=old;menu=cinematic=0;++generation;
 struct render_camera c={1,.01f,1000},r=c;struct render_fog f={0};
 n3ds_engine_fog_prepare(&c,&r,&f,1);
 assert(fog_end==(old?60:110));assert(c.z_near==.01f && c.z_far==r.z_far);
}
int main(void){
 struct render_frustum frustum={0};frustum.world_planes[5]=(real_plane3d){{1,0,0},60};
 struct mission_batch batch={{61,0,0},{.5f,.5f,.5f}};
 assert(batch_outside_plane(&batch,&frustum)==5);
 batch.center.x=59.8f;assert(batch_outside_plane(&batch,&frustum)==NONE);
 batch.center.x=65;batch.extent.i=10;assert(batch_outside_plane(&batch,&frustum)==NONE);
 for(int roll=-10;roll<=10;++roll)for(int pitch=-10;pitch<=10;++pitch){
  float q[8][3]={{0,0,0},{400,0,0},{400,240,0},{0,240,0}},a[8][3],b[8][3];
  for(int i=0;i<4;++i)q[i][2]=pitch*.1f+roll*.1f*(q[i][0]/200-1)+(q[i][1]/120-1);
  int count=distance_fog_clip(a,q,4,.18f,1);assert(count>=0 && count<=5);
  count=distance_fog_clip(b,a,count,0,0);assert(count>=0 && count<=6);
  for(int i=0;i<count;++i)assert(b[i][2]>=0 && b[i][2]<=.180001f);
 }
 for(int old=0;old<2;++old){
  reset(old);
  for(int i=0;i<1000;++i)n3ds_engine_fog_feedback(33,10,0,8,0);
  assert(distance_budget.level==0);
  for(int i=0;i<1000;++i)n3ds_engine_fog_feedback(60,10,0,8,0);
  assert(distance_budget.level==0); /* Simulation pressure alone. */
  n3ds_engine_fog_feedback(900,10,0,8,0);assert(distance_budget.level==0);
  for(int i=0;i<1000;++i)n3ds_engine_fog_feedback(60,35,12,18,1);
  assert(distance_budget.level==0); /* Resolution owns the current trial. */
  for(int i=0;i<1000 && !distance_budget.trial;++i)n3ds_engine_fog_feedback(60,35,12,18,0);
  assert(distance_budget.trial && distance_budget.level==1);
  for(int i=0;i<400 && distance_budget.trial;++i)n3ds_engine_fog_feedback(60,35,12,18,0);
  assert(!distance_budget.trial && distance_budget.level==0 && distance_budget.hold>=29000);
  reset(old);
  for(int i=0;i<1000 && !distance_budget.trial;++i)n3ds_engine_fog_feedback(60,35,12,18,0);
  for(int i=0;i<400 && distance_budget.trial;++i)n3ds_engine_fog_feedback(48,25,6,14,0);
  assert(distance_budget.level==1 && !distance_budget.trial);
  assert(distance_budget.distance>=distance_budget.base*.81f);
  for(int i=0;i<200;++i)n3ds_engine_fog_feedback(48,25,6,14,0);
  assert(distance_budget.level==1); /* Sustained hold prevents rapid pumping. */
  for(int i=0;i<1000;++i)n3ds_engine_fog_feedback(20,8,0,8,0);
  assert(distance_budget.level==0);
  struct render_camera c={.2f,.01f,1000},r=c;struct render_fog f={0};
  n3ds_engine_fog_prepare(&c,&r,&f,1);assert(!fog_end && c.z_far==1000);
  c.vertical_field_of_view=1;menu=1;n3ds_engine_fog_prepare(&c,&r,&f,1);assert(!fog_end);
  menu=0;n3ds_engine_fog_prepare(&c,&r,&f,0);assert(!fog_end);
  cinematic=1;n3ds_engine_fog_prepare(&c,&r,&f,1);assert(!distance_budget.active && fog_end==(old?100:150));
  map="levels\\d40\\d40";c.z_far=r.z_far=1000;n3ds_engine_fog_prepare(&c,&r,&f,1);
  assert(!fog_end && c.z_far==1000); /* Distant cinematic set pieces stay visible. */
  map="levels\\a30\\a30";
 }
 puts("PASS: fog controller model tiers, CPU-only pressure, hitch rejection, resolution coordination, rejected/accepted trials, slow recovery, menu/indoor/scope/cinematic exclusions");
}
'''.replace('/* POLICY */', policy)
    with tempfile.TemporaryDirectory(ignore_cleanup_errors=True) as directory:
        path = Path(directory)
        (path / 'fog.c').write_text(source)
        subprocess.run(compiler + [str(path / 'fog.c'), '-o', str(path / 'fog.exe')], check=True)
        subprocess.run([str(path / 'fog.exe')], check=True)

if __name__ == '__main__':
    main()
