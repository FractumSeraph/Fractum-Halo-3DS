/* New3DS integration additions by Kevin Boggs. */
#include "halo_pc_model.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Kevin Boggs: main - decode streamed and resident model data with identical results. */
int main(int argc,char **argv)
{
 if(argc!=2)return 2;
 for(int streamed=0;streamed<2;++streamed){
  struct halo_cache cache;char error[160];unsigned models=0,parts=0;
  if(!(streamed?halo_cache_open_preview(&cache,argv[1],error,sizeof(error)):halo_cache_open(&cache,argv[1],error,sizeof(error)))){puts(error);return 1;}
  for(unsigned i=0;i<cache.tag_count;++i){
   struct halo_cache_tag tag;
   if(!halo_cache_tag_at(&cache,i,&tag,error,sizeof(error)))continue;
   if(tag.group!=(cache.kind==5?0x6d6f6465u:0x6d6f6432u))continue;
   unsigned short geometries[128];unsigned count=0;
   if(!halo_pc_model_default_geometries(&cache,&tag,4,geometries,128,&count,error,sizeof(error))){puts(error);return 1;}
   if(!strcmp(tag.name,"fixture"))assert(count==2 && geometries[0]==0 && geometries[1]==1);
   for(unsigned g=0;g<count;++g){unsigned n=0;
    if(!halo_pc_model_part_count(&cache,&tag,geometries[g],&n,error,sizeof(error))){puts(error);return 1;}
    for(unsigned p=0;p<n;++p){
     struct halo_pc_model_part part;
     if(!halo_pc_model_part_at(&cache,&tag,geometries[g],p,&part,error,sizeof(error))){printf("%s: %s\n",tag.name,error);return 1;}
     float *v=malloc(part.vertex_count*8*sizeof(float));unsigned short *indices=malloc((part.strip_triangle_count+2)*2);
     assert(v && indices);
     if(!halo_pc_model_read_part_uv_normal(&cache,&part,v,part.vertex_count*8,indices,part.strip_triangle_count+2,error,sizeof(error))){printf("%s: %s\n",tag.name,error);return 1;}
     for(unsigned j=0;j<part.vertex_count;++j){
      for(int a=0;a<8;++a)assert(isfinite(v[j*8+a]));
      if(!strcmp(tag.name,"fixture")){
       assert(fabsf(v[j*8]-(float)j)<.001f);
       assert(fabsf(v[j*8+3]-.25f)<.001f && fabsf(v[j*8+4]+.5f)<.001f);
       assert(fabsf(v[j*8+5]-1.f)<.002f && fabsf(v[j*8+6])<.002f && fabsf(v[j*8+7])<.002f);
      }
     }
     free(v);free(indices);++parts;
    }
   }
   ++models;
  }
  halo_cache_close(&cache);assert(models && parts);
  printf("PASS: %s model reader: %u models / %u parts\n",streamed?"streamed":"resident",models,parts);
 }
 return 0;
}
