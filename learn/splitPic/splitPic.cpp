#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <atlimage.h>  // CImage
//#include <WinGDI.h>  // BITMAPFILEHEADER
#define __BMFH__ \
"typedef struct tagBITMAPFILEHEADER { \r\n"\
"	WORD    bfType      = %d;\r\n"\
"	DWORD   bfSize      = %d;\r\n"\
"	WORD    bfReserved1 = %d;\r\n"\
"	WORD    bfReserved2 = %d;\r\n"\
"	DWORD   bfOffBits   = %d;\r\n"\
"} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;\r\n\r\n"
#define __BMIH__ \
"typedef struct tagBITMAPINFOHEADER{\r\n"\
"	DWORD      biSize          = %d;\r\n"\
"	LONG       biWidth         = %d;\r\n"\
"	LONG       biHeight        = %d;\r\n"\
"	WORD       biPlanes        = %d;\r\n"\
"	WORD       biBitCount      = %d;\r\n"\
"	DWORD      biCompression   = %d;\r\n"\
"	DWORD      biSizeImage     = %d;\r\n"\
"	LONG       biXPelsPerMeter = %d;\r\n"\
"	LONG       biYPelsPerMeter = %d;\r\n"\
"	DWORD      biClrUsed       = %d;\r\n"\
"	DWORD      biClrImportant  = %d;\r\n"\
"} BITMAPINFOHEADER, FAR *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;\r\n\r\n"

//#define __BMFH__ \
//"typedef struct tagBITMAPFILEHEADER { \r\n"\
//"	WORD    bfType      = %d;\r\n"\  // (int)19778 -> "BM" ->BMP
//"	DWORD   bfSize      = %d;\r\n"\  // 文件大小
//"	WORD    bfReserved1 = %d;\r\n"\  // 0
//"	WORD    bfReserved2 = %d;\r\n"\  // 0
//"	DWORD   bfOffBits   = %d;\r\n"\  // 像素信息偏移
//"} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;\r\n\r\n"
//
// BITMAPINFOHEADER
// https://docs.microsoft.com/en-us/previous-versions//dd183376(v=vs.85)
//
// The bits in the array are packed together, but each scan line must be padded with zeros to end on a LONG data-type boundary.
// https://docs.microsoft.com/zh-cn/windows/win32/api/wingdi/ns-wingdi-bitmapinfo?redirectedfrom=MSDN
//#define __BMIH__ \
//"typedef struct tagBITMAPINFOHEADER{\r\n"\
//"	DWORD      biSize          = %d;\r\n"\  // 当前结构体大小
//"	LONG       biWidth         = %d;\r\n"\  // 图片宽度像素
//"	LONG       biHeight        = %d;\r\n"\  // 图片高度像素(为正表示图片自底向上坐标为左下角，为负自顶向下坐标为左上角)...
//"	WORD       biPlanes        = %d;\r\n"\  // 目标设备的平面数。该值必须设置为 1
//"	WORD       biBitCount      = %d;\r\n"\  // 每像素位数(0\1\4\8\16\24\32)...
//"	DWORD      biCompression   = %d;\r\n"\  // 图片压缩方法  // BI_RGB
//"	DWORD      biSizeImage     = %d;\r\n"\  // 0则自动计算，可存储像素及空白字节
//"	LONG       biXPelsPerMeter = %d;\r\n"\  // 位图的目标设备的水平分辨率（以像素/米为单位）
//"	LONG       biYPelsPerMeter = %d;\r\n"\  // 位图的目标设备的垂直分辨率（以像素/米为单位）
//"	DWORD      biClrUsed       = %d;\r\n"\  // 0
//"	DWORD      biClrImportant  = %d;\r\n"\  // 0
//"} BITMAPINFOHEADER, FAR *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;\r\n\r\n"
//
//typedef struct tagBITMAPCOREINFO {
//	BITMAPCOREHEADER bmciHeader;
//	RGBTRIPLE        bmciColors[1];
//} BITMAPCOREINFO, *LPBITMAPCOREINFO, *PBITMAPCOREINFO;
//
//typedef struct tagBITMAPCOREHEADER {
//	DWORD bcSize;
//	WORD  bcWidth;
//	WORD  bcHeight;
//	WORD  bcPlanes;
//	WORD  bcBitCount;
//} BITMAPCOREHEADER, *LPBITMAPCOREHEADER, *PBITMAPCOREHEADER;
//pColor = ((LPBYTE) pBitmapCoreInfo + (WORD) (pBitmapCoreInfo -> bcSize)) 
//
//typedef struct tagRGBTRIPLE {
//BYTE    rgbtBlue;
//BYTE    rgbtGreen;
//BYTE    rgbtRed;
//} RGBTRIPLE, *PRGBTRIPLE, NEAR *NPRGBTRIPLE, FAR *LPRGBTRIPLE;
//#include <poppack.h>
//
//typedef struct tagRGBQUAD {
//BYTE    rgbBlue;
//BYTE    rgbGreen;
//BYTE    rgbRed;
//BYTE    rgbReserved;
//} RGBQUAD;
//typedef RGBQUAD FAR* LPRGBQUAD;
//  逐行扫描时要在每行数据后添加空白以便4字节对齐

BYTE*pByte = nullptr;
char *read() {
	FILE *fp = fopen("../[image]/resource/Wallpaper_PC.bmp","r");
	if (!fp) {
		printf("图片文件未找到！\r\n");
		system("pause");
		exit(0);
	}
	fseek(fp,0,SEEK_END);
	long lSize = ftell(fp);
	rewind(fp);
	pByte = (BYTE*)malloc(lSize);
	fread(pByte,lSize,1,fp);
	fclose(fp);
	fp = nullptr;
	return "";
}

RGBTRIPLE**rgbTriple = nullptr;
long height = 0;
long width = 0;

BITMAPFILEHEADER *bmfi = nullptr;
BITMAPINFOHEADER *bmih = nullptr;
char* analysis() {
	if(!pByte) read();

	// BITMAP 文件头
	BYTE *pCur = pByte;
	bmfi = (BITMAPFILEHEADER *)pCur;
	if(bmfi->bfType != 0x4d42) return "BM";  // bmfi.bfType 0x4d42

	// BITMAP 图像头
	pCur = pByte+sizeof(BITMAPFILEHEADER);
	bmih = (BITMAPINFOHEADER*)pCur;
	if(bmih->biSize != sizeof(BITMAPINFOHEADER)) return "biSize";
	if(bmih->biWidth < 0) return "biWidth";
	if(bmih->biHeight < 0) return "biHeight";
	if(bmih->biBitCount != sizeof(RGBTRIPLE) * 8) return "biBitCount";  // 一个字节占用8个比特位
	if(bmih->biCompression != BI_RGB) return "BI_RGB";

	// 保存解析出来的RGB像素内容
	rgbTriple = (RGBTRIPLE **)malloc(bmih->biHeight*sizeof(RGBTRIPLE*)); 

	// 设置行首指针
	pCur = pByte + bmfi->bfOffBits;
	rgbTriple[0] = (RGBTRIPLE*)pCur; // 指向第一行图像的起始位置
	int iLineEnd = 4 - sizeof(RGBTRIPLE)*bmih->biWidth % 4;  // 行尾空白
	long lineSize = sizeof(RGBTRIPLE)*bmih->biWidth + iLineEnd;  // 一行图像占用的空间
	for(long line = 1; line < bmih->biHeight; line++)
		rgbTriple[line] = (RGBTRIPLE*)((BYTE*)rgbTriple[line - 1] + lineSize);  // 每行起始=前一行行首+前一行行大小;
	return "";
}

// 二级数组要用三级指针才能使用"T[i][j].size=0;"
BITMAPINFOHEADER ***destbmih = nullptr;
RGBTRIPLE***destRgb = nullptr;
int divx = 4;
int divy = 3;
char* split() {
	if(!rgbTriple) analysis();  // 填充行尾空白
	destbmih = (BITMAPINFOHEADER***)malloc(sizeof(BITMAPINFOHEADER**) * divx);
	destRgb = (RGBTRIPLE***)malloc(sizeof(RGBTRIPLE**) * divx);
	// 非边界图片图片的宽度和高度
	unsigned long normalWidth = bmih->biWidth   % divx ? bmih->biWidth  / divx + 1 : bmih->biWidth  / divx;
	unsigned long normalHeight = bmih->biHeight % divy ? bmih->biHeight / divy + 1 : bmih->biHeight / divy;
	// 边界图片的宽度和高度
	unsigned long lastWidth  = bmih->biWidth  - (divx - 1) * normalWidth;
	unsigned long lastHeight = bmih->biHeight - (divy - 1) * normalHeight;
	// 分配空间及初始化
	for (int i=0;i<divx;i++){
		destbmih[i] = (BITMAPINFOHEADER**)malloc(sizeof(BITMAPINFOHEADER*) * divy);
		destRgb[i] = (RGBTRIPLE**)malloc(sizeof(RGBTRIPLE*) * divy);
		for(int j=0;j<divy;j++){
			destbmih[i][j] = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
			memset(destbmih[i][j], 0, sizeof(BITMAPINFOHEADER));
			destbmih[i][j]->biSize = sizeof(BITMAPINFOHEADER);
			destbmih[i][j]->biWidth  = i == divx - 1 ? lastWidth  : normalWidth;
			destbmih[i][j]->biHeight = j == divy - 1 ? lastHeight : normalHeight;
			destbmih[i][j]->biPlanes = 1; // must
			destbmih[i][j]->biBitCount = sizeof(RGBTRIPLE) * 8;  // 一个字节占用8个比特位
			destbmih[i][j]->biCompression = BI_RGB;  // 无压缩
			destbmih[i][j]->biSizeImage = 0;
			destbmih[i][j]->biXPelsPerMeter = bmih->biXPelsPerMeter;  // 保存原先的值
			destbmih[i][j]->biYPelsPerMeter = bmih->biYPelsPerMeter;  // 保存原先的值
			destbmih[i][j]->biClrUsed = 0;
			destbmih[i][j]->biClrImportant = 0;

			// 每个格子分配RGB像素空间
			unsigned long lineSize = destbmih[i][j]->biWidth * sizeof(RGBTRIPLE);  // 原先行长
			destRgb[i][j] = (RGBTRIPLE*)malloc(destbmih[i][j]->biHeight * lineSize);  // 写文件时才补空格
			memset(destRgb[i][j], 0, destbmih[i][j]->biHeight * lineSize);
			// 

			// 像素占用总空间
			lineSize += lineSize % 4 ? 4 - lineSize % 4 : 0;  // 补空隙
			destbmih[i][j]->biSizeImage = destbmih[i][j]->biHeight * lineSize; // BYTE位数(非二进制位)
		}
	}
	//RGBTRIPLE*p=nullptr;
	//// 存入RGB数据
	//// 先从左往右，再从上往下，每格行从上往下
	//unsigned long rowStart = 0;
	//unsigned long colStart = 0;
	//for (int i=0;i<divx;i++) {
	//	if(i)  // 跳过第一块 
	//		rowStart += destbmih[i][0]->biWidth;  // 每竖块的宽度均相同
	//	for (int j=0;j<divy;j++) {
	//		//lineStart += destbmih[i][j]->biWidth;
	//		j == 0 ? colStart = 0 : colStart += destbmih[i][j-1]->biHeight;
	//		unsigned long lineSize = destbmih[i][j]->biWidth * sizeof(RGBTRIPLE);  // 原先行长
	//		lineSize += lineSize % 4 ? 4 - lineSize % 4 : 0;  // 补空隙
	//		for (int k=0;k<destbmih[i][j]->biHeight;k++) { // 按行复制
	//			p= (RGBTRIPLE*)(((char*)destRgb[i][j])+k*lineSize);
	//			memcpy(((char*)destRgb[i][j])+k*lineSize, rgbTriple[k+colStart]+rowStart, destbmih[i][j]->biWidth*sizeof(RGBTRIPLE));
	//		}
	//	}
	//}
	return "";
}

RGBTRIPLE ppp[768][1367] = {};
char *save() {
	if(!destRgb) split();
	memcpy(ppp,destRgb[0][0],destbmih[0][0]->biSizeImage);
	BITMAPFILEHEADER fbmih = { 19778,0, 0,0,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)};
	char name[128] = {};
	for (int i =0 ;i<divx;i++) {
		for (int j=0;j<divy;j++) {
			CreateDirectory(L"../[image]/result/", NULL);
			sprintf(name, "../[image]/result/dest_row%d-col%d_height%d-width%d.bmp",
				j,i,destbmih[i][j]->biHeight,destbmih[i][j]->biWidth);
			FILE*fp = fopen(name,"w");
			fbmih.bfSize = fbmih.bfOffBits + destbmih[i][j]->biSizeImage;
			fwrite(&fbmih, sizeof(BITMAPFILEHEADER), 1, fp);
			fwrite(destbmih[i][j], sizeof(BITMAPINFOHEADER), 1, fp);
			// 按行写入
			// 每个格子分配RGB像素空间
			unsigned long lineSize = destbmih[i][j]->biWidth * sizeof(RGBTRIPLE);  // 原先行长
			lineSize += lineSize % 4 ? 4 - lineSize % 4 : 0;  // 补空隙
			for (int k =0; k<destbmih[i][j]->biHeight;k++) {
				// 下面一行会崩溃
				fwrite(destRgb[i][j]+k*lineSize,destbmih[i][j]->biWidth*sizeof(RGBTRIPLE),1,fp);
				fwrite("\0\0",2,1,fp);
			}
			//fwrite((BYTE*)destRgb[i][j], destbmih[i][j]->biSizeImage, 1, fp);
			fclose(fp);
		}
	}

	return "";
}


int main(){
	//printf("%d\r\n",analysis());
	//split();
	//save(); // 未完成
	printf("测试中，未完成\r\n");
	system("pause");
	return 0;
}
