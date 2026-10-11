/* Host preview reader test. Pass owner-supplied maps or generated fixtures. */

#include "halo_cache_reader.h"
#include "halo_pc_bsp.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 for(int i=1;i<argc;++i)for(int streamed=0;streamed<2;++streamed){
  struct halo_cache c={0};struct halo_pc_bsp b={0};char error[160];
  if(!(streamed?halo_cache_open_preview(&c,argv[i],error,sizeof(error)):halo_cache_open(&c,argv[i],error,sizeof(error)))){printf("FAIL open %s: %s\n",argv[i],error);return 1;}
  if(!halo_pc_bsp_open(&b,&c,0,error,sizeof(error))){printf("FAIL BSP %s: %s\n",argv[i],error);return 1;}
  unsigned faces=0,bitmaps=0;
  for(unsigned lm=0;lm<b.lightmap_count;++lm){unsigned n=0;assert(halo_pc_bsp_material_count(&b,lm,&n,error,sizeof(error)));
   for(unsigned m=0;m<n;++m){struct halo_pc_bsp_material mat;assert(halo_pc_bsp_material_at(&b,lm,m,&mat,error,sizeof(error)));
    float *v=malloc(mat.vertex_count*5*sizeof(float));unsigned short *tri=malloc(mat.surface_count*3*sizeof(short));
    assert(v && tri && halo_pc_bsp_read_material_uv(&b,&mat,v,mat.vertex_count*5,tri,mat.surface_count*3,error,sizeof(error)));
    for(unsigned j=0;j<mat.vertex_count*5;++j)assert(isfinite(v[j]));
    faces+=mat.surface_count;free(v);free(tri);
   }
  }
  for(unsigned j=0;j<c.tag_count;++j){struct halo_cache_tag t;
   if(!halo_cache_tag_at(&c,j,&t,error,sizeof(error)) || t.group!=0x6269746d)continue;
   struct halo_cache_bitmap bm;
   if(!halo_cache_bitmap_at(&c,&t,0,&bm,error,sizeof(error)) || bm.external)continue;
   void *p=malloc(bm.first_mip_bytes);assert(p);
   assert(halo_cache_read_bitmap(&c,&bm,NULL,p,bm.first_mip_bytes,error,sizeof(error)));free(p);++bitmaps;
  }
  printf("PASS %s: faces=%u materials=%u bitmap_reads=%u BSP=%u directory=%zu nav=%u\n",argv[i],faces,b.material_count,bitmaps,b.size,c.directory_offset+c.tag_count*32,b.nav_surface_count);
  halo_pc_bsp_close(&b);halo_cache_close(&c);
 }
 return 0;
}
