#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
//
//#define  dprintf   printf
//#define  dfopen    fopen
//#define  dfclose   fclose
//#define  dfread    fread
//#define  dfwrite   fwrite

FILE *fp_debug;

#define PROC_IMAGE_WIDTH   2304
#define PROC_IMAGE_HEIGHT  1744
#define PROC_IMAGE_SIZE    (PROC_IMAGE_WIDTH * PROC_IMAGE_HEIGHT*3/2)

unsigned char YUVUV[PROC_IMAGE_SIZE];

//#define L2LENGTH      (10*1024)
//#define L2TMPLENGTH   (80*1024)
//#define SDRAMLENGTH   ((PROC_IMAGE_WIDTH/2+1)*(PROC_IMAGE_HEIGHT/2+1)*20+ 10*1024*1024)
//#define SDRAMCACHELEN ((PROC_IMAGE_WIDTH/2+1)*(PROC_IMAGE_HEIGHT/2+1)*8+(PROC_IMAGE_WIDTH/2+256)*(PROC_IMAGE_HEIGHT/2+2)*2+1*1024*1024+10*1024)
//#define SDRAMTMP_LEN  ((PROC_IMAGE_WIDTH/4+1)*(PROC_IMAGE_HEIGHT/8+2)*80)
//
//#define SDRAMTMPLENGTH      ((PROC_IMAGE_WIDTH/4+1)*(PROC_IMAGE_HEIGHT/8+2)*30 + 2*1024*1024) //( (PROC_IMAGE_WIDTH/2+1)*(PROC_IMAGE_HEIGHT/2+1)*14+ 6*1024*1024)
//#define SDRAMTMPCACHELENGTH (8*1024*1024) //( (PROC_IMAGE_WIDTH/2+1)*(PROC_IMAGE_HEIGHT/2+1)*14+ 6*1024*1024)

//unsigned char  L2[L2LENGTH];
//unsigned char  L2TMP[L2TMPLENGTH];
//unsigned char  SDRAM[SDRAMLENGTH];
//unsigned char  SDRAMCACHE[SDRAMCACHELEN];
//unsigned char  SDRAMTmp[SDRAMTMP_LEN];
//unsigned char  STREAM[PROC_IMAGE_SIZE*2];


#define CHANNELS        3     //通道数     
#define YUV_STEP        2     //yuv420格式上下两个y对应一个u和v 
#define YUV_RATIO       0.25  //yuv420格式四个y对应一个u和v

/*YUV到RGB转换中的运算*/
#define GET_R(sy,su,sv) ((297 * sy + 407 * sv - 56845) >> 8)                        
#define GET_G(sy,su,sv) ((297 * sy - 100 * su - 207 * sv + 34574) >> 8) 
#define GET_B(sy,su,sv) ((297 * sy + 514 * su - 70591) >> 8)  

#define CLIP(value)     (value) < 0 ? 0 : ((value) > 255 ? 255 : (value))

int yuv_to_rgb(unsigned char *p_yuv, unsigned char *p_rgb, int width, int height)
{
	unsigned char  *y1;     //Y分量奇数行指针
	unsigned char  *y2;     //Y分量偶数行指针
	unsigned char  *des1;   //RGB奇数行指针
	unsigned char  *des2;   //RGB偶数行指针
	unsigned char  *p_u;    //U分量指针
	unsigned char  *p_v;    //V分量指针

	int   src_y;            //Y分量值
	int   src_u;            //U分量值
	int   src_v;            //V分量值
	int   i, j;

	if (p_yuv == 0 || p_rgb == 0)
	{
		return 0;
	}

	y1   = p_yuv;            
	y2   = p_yuv   + width;          //yuv下行指针
	des1 = p_rgb;
	des2 = p_rgb + CHANNELS * width; //rgb下行指针

	p_u = p_yuv + width * height;
	p_v = p_u   + (int)(width * height * YUV_RATIO);	

	for(i = 0; i < height; i += YUV_STEP)
	{
		for (j = 0; j < width; j += YUV_STEP) 
		{
			src_y     = *y1;
			src_u     = *p_u;
			src_v     = *p_v;
			des1[0] = CLIP(GET_B(src_y, src_u, src_v));
			des1[1] = CLIP(GET_G(src_y, src_u, src_v));
			des1[2] = CLIP(GET_R(src_y, src_u, src_v));			
			des1 += CHANNELS;
			y1++;

			src_y     = *y1;
			des1[0] = CLIP(GET_B(src_y,src_u,src_v));
			des1[1] = CLIP(GET_G(src_y,src_u,src_v));
			des1[2] = CLIP(GET_R(src_y,src_u,src_v));
			des1 += CHANNELS;
			y1++;

			src_y     = *y2;
			des2[0] = CLIP(GET_B(src_y,src_u,src_v));
			des2[1] = CLIP(GET_G(src_y,src_u,src_v));
			des2[2] = CLIP(GET_R(src_y,src_u,src_v));
			des2 += CHANNELS;
			y2++;

			src_y     = *y2;
			des2[0] = CLIP(GET_B(src_y,src_u,src_v));
			des2[1] = CLIP(GET_G(src_y,src_u,src_v));
			des2[2] = CLIP(GET_R(src_y,src_u,src_v));
			des2 += CHANNELS;
			y2++;

			p_u++;
			p_v++;
		}

		y1   += width;
		y2   += width;
		des2 += CHANNELS * width;
		des1 += CHANNELS * width;
	}	

	return 1;
}

void  YUV420ToYUVUV(unsigned char *pInBuffer, int width,int height, unsigned char *pOutBuffer)
{
	int i;
	unsigned char *pInY,*pInU,*pInV,*pOutY,*pOutUV;
	pInY = pInBuffer;
	pInU = pInBuffer + width*height;
	pInV = pInU + width*height/4;

	pOutY = pOutBuffer;
	pOutUV = pOutBuffer + width*height;

	memcpy(pOutY, pInY,width*height);
	for (i=0; i<width*height/4; i++)
	{
		pOutUV[i*2] = pInU[i];
		pOutUV[i*2+1] = pInV[i];
	}

}


void  YUVUVToYUV420(unsigned char *pInBuffer, int width,int height, unsigned char *pOutBuffer)
{
	int i;
	unsigned char *pInY,*pOutU,*pOutV,*pOutY,*pInUV;
	pInY = pInBuffer;
	pInUV = pInBuffer + width*height;

	pOutY = pOutBuffer;
	pOutU = pOutBuffer + width*height;
	pOutV = pOutU + width*height/4;

	memcpy( pOutY, pInY, width*height);
	for (i=0; i<width*height/4; i++)
	{
		pOutU[i] = pInUV[i*2] ;
		pOutV[i] = pInUV[i*2+1]  ;
	}

}

void  ScaleImg(unsigned char BufIn[], unsigned char BufOut[], int width,   int startY, int endY)
{
	int i, j;
	short Y;
	unsigned char *pY0, *pY1;
	unsigned char *pDstY;

	int  ww = width/2;

	pY0 = BufIn +  startY*2*width;
	pY1 = BufIn +  startY*2*width;

	pDstY = BufOut + startY*width/2 ;

	for(j = 0; j < (endY-startY); j++)
	{
		for(i = 0; i < width/2; i++)
		{
			Y = *(pY0 + 2*i+0) + *(pY0 + 2*i+1)+*(pY1 + 2*i+0) + *(pY1 + 2*i+1)  ;


			*(pDstY + i) = Y >> 2;

		}

		pY0 += width*2;
		pY1 += width*2;
		pDstY += width/2;
	}
}


void  Scale4Img(unsigned char BufIn[], unsigned char BufOut[], int width, int height   )
{

	int i, j;
	short Y;
	unsigned char *pY0, *pY1;
	unsigned char *pDstY;
	int  ww = width/4;
	int sum=0;

	pY0 = BufIn + width*1;
	pY1 = BufIn + width*2;

	pDstY = BufOut  ;

	for(j = 0; j < height/4; j++)
	{
		for(i = 0; i < width/4; i++)
		{
			Y = *(pY0 + 4*i+0) + *(pY0 + 4*i+1)  + *(pY0 + 4*i+2) + *(pY0 + 4*i+3) +
				*(pY1 + 4*i+0) + *(pY1 + 4*i+1)  + *(pY1 + 4*i+2) + *(pY1 + 4*i+3);


			*(pDstY + i) = Y >> 3;
			sum += (Y>>3);
		}

		pY0 += width*4;
		pY1 += width*4;
		pDstY += width/4;
	}
}

int main()
{

}