#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
static void put32(unsigned char *p, uint32_t v) {
    for (int i=0; i<4; ++i) p[i] = (unsigned char)(v >> (8*i));
}
static int decode(unsigned char *b, size_t size, BmpImage *out) {
    FILE *f=tmpfile(); assert(f);
    assert(fwrite(b,1,size,f)==size); rewind(f);
    int r=bmp_read(f,320,240,out); fclose(f); return r;
}
int main(void) {
    /* 1x2: three BGR bytes plus padding per row. */
    unsigned char b[62]={0}; b[0]='B'; b[1]='M';
    put32(b+2,sizeof b); put32(b+10,54); put32(b+14,40);
    put32(b+18,1); put32(b+22,2); b[26]=1; b[28]=24;
    b[56]=255; b[57]=0xa5; b[59]=255;
    BmpImage image={0};
    assert(decode(b,sizeof b,&image)==0);
    assert(image.width==1 && image.height==2);
    assert(image.pixels[0]==0xf800 && image.pixels[1]==0x07e0); free(image.pixels);
    put32(b+22,UINT32_MAX-1);
    assert(decode(b,sizeof b,&image)==0);
    assert(image.pixels[1]==0xf800 && image.pixels[0]==0x07e0); free(image.pixels);
    for (size_t i=0; i<sizeof b; ++i) assert(decode(b,i,&image)==-1);
    put32(b+22,2);
    const int offsets[]={0,10,14,18,22,26,28,30};
    const uint32_t values[]={0,20,12,UINT32_MAX,0,2,32,1};
    for (size_t i=0;i<sizeof offsets/sizeof offsets[0];++i) {
        unsigned char bad[sizeof b]; memcpy(bad,b,sizeof b);
        put32(bad+offsets[i],values[i]); assert(decode(bad,sizeof bad,&image)==-1);
    }
    put32(b+18,321); assert(decode(b,sizeof b,&image)==-1);
    put32(b+18,1); put32(b+22,241); assert(decode(b,sizeof b,&image)==-1);
    const char *files[]={"images/chipy.bmp","images/trudels.bmp","images/vorp.bmp"};
    for(size_t i=0;i<3;++i) {
        FILE *f=fopen(files[i],"rb"); assert(f);
        assert(bmp_read(f,320,240,&image)==0); free(image.pixels); fclose(f);
    }
    return 0;
}
