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
//"	DWORD   bfSize      = %d;\r\n"\  // �ļ���С
//"	WORD    bfReserved1 = %d;\r\n"\  // 0
//"	WORD    bfReserved2 = %d;\r\n"\  // 0
//"	DWORD   bfOffBits   = %d;\r\n"\  // ������Ϣƫ��
//"} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;\r\n\r\n"
//
// BITMAPINFOHEADER
// https://docs.microsoft.com/en-us/previous-versions//dd183376(v=vs.85)
//
// The bits in the array are packed together, but each scan line must be padded with zeros to end on a LONG data-type boundary.
// https://docs.microsoft.com/zh-cn/windows/win32/api/wingdi/ns-wingdi-bitmapinfo?redirectedfrom=MSDN
//#define __BMIH__ \
//"typedef struct tagBITMAPINFOHEADER{\r\n"\
//"	DWORD      biSize          = %d;\r\n"\  // ��ǰ�ṹ���С
//"	LONG       biWidth         = %d;\r\n"\  // ͼƬ�������
//"	LONG       biHeight        = %d;\r\n"\  // ͼƬ�߶�����(Ϊ����ʾͼƬ�Ե���������Ϊ���½ǣ�Ϊ���Զ���������Ϊ���Ͻ�)...
//"	WORD       biPlanes        = %d;\r\n"\  // Ŀ���豸��ƽ��������ֵ��������Ϊ 1
//"	WORD       biBitCount      = %d;\r\n"\  // ÿ����λ��(0\1\4\8\16\24\32)...
//"	DWORD      biCompression   = %d;\r\n"\  // ͼƬѹ������  // BI_RGB
//"	DWORD      biSizeImage     = %d;\r\n"\  // 0���Զ����㣬�ɴ洢���ؼ��հ��ֽ�
//"	LONG       biXPelsPerMeter = %d;\r\n"\  // λͼ��Ŀ���豸��ˮƽ�ֱ��ʣ�������/��Ϊ��λ��
//"	LONG       biYPelsPerMeter = %d;\r\n"\  // λͼ��Ŀ���豸�Ĵ�ֱ�ֱ��ʣ�������/��Ϊ��λ��
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
//  ����ɨ��ʱҪ��ÿ�����ݺ���ӿհ��Ա�4�ֽڶ���

BYTE*pByte = nullptr;
char *read() {
	FILE *fp = fopen("../[image]/resource/Wallpaper_PC.bmp","r");
	if (!fp) {
		printf("ͼƬ�ļ�δ�ҵ���\r\n");
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

	// BITMAP �ļ�ͷ
	BYTE *pCur = pByte;
	bmfi = (BITMAPFILEHEADER *)pCur;
	if(bmfi->bfType != 0x4d42) return "BM";  // bmfi.bfType 0x4d42

	// BITMAP ͼ��ͷ
	pCur = pByte+sizeof(BITMAPFILEHEADER);
	bmih = (BITMAPINFOHEADER*)pCur;
	if(bmih->biSize != sizeof(BITMAPINFOHEADER)) return "biSize";
	if(bmih->biWidth < 0) return "biWidth";
	if(bmih->biHeight < 0) return "biHeight";
	if(bmih->biBitCount != sizeof(RGBTRIPLE) * 8) return "biBitCount";  // һ���ֽ�ռ��8������λ
	if(bmih->biCompression != BI_RGB) return "BI_RGB";

	// �������������RGB��������
	rgbTriple = (RGBTRIPLE **)malloc(bmih->biHeight*sizeof(RGBTRIPLE*)); 

	// ��������ָ��
	pCur = pByte + bmfi->bfOffBits;
	rgbTriple[0] = (RGBTRIPLE*)pCur; // ָ���һ��ͼ�����ʼλ��
	int iLineEnd = 4 - sizeof(RGBTRIPLE)*bmih->biWidth % 4;  // ��β�հ�
	long lineSize = sizeof(RGBTRIPLE)*bmih->biWidth + iLineEnd;  // һ��ͼ��ռ�õĿռ�
	for(long line = 1; line < bmih->biHeight; line++)
		rgbTriple[line] = (RGBTRIPLE*)((BYTE*)rgbTriple[line - 1] + lineSize);  // ÿ����ʼ=ǰһ������+ǰһ���д�С;
	return "";
}

// ��������Ҫ������ָ�����ʹ��"T[i][j].size=0;"
BITMAPINFOHEADER ***destbmih = nullptr;
RGBTRIPLE***destRgb = nullptr;
int divx = 4;
int divy = 3;
char* split() {
	if(!rgbTriple) analysis();  // �����β�հ�
	destbmih = (BITMAPINFOHEADER***)malloc(sizeof(BITMAPINFOHEADER**) * divx);
	destRgb = (RGBTRIPLE***)malloc(sizeof(RGBTRIPLE**) * divx);
	// �Ǳ߽�ͼƬͼƬ�Ŀ�Ⱥ͸߶�
	unsigned long normalWidth = bmih->biWidth   % divx ? bmih->biWidth  / divx + 1 : bmih->biWidth  / divx;
	unsigned long normalHeight = bmih->biHeight % divy ? bmih->biHeight / divy + 1 : bmih->biHeight / divy;
	// �߽�ͼƬ�Ŀ�Ⱥ͸߶�
	unsigned long lastWidth  = bmih->biWidth  - (divx - 1) * normalWidth;
	unsigned long lastHeight = bmih->biHeight - (divy - 1) * normalHeight;
	// ����ռ估��ʼ��
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
			destbmih[i][j]->biBitCount = sizeof(RGBTRIPLE) * 8;  // һ���ֽ�ռ��8������λ
			destbmih[i][j]->biCompression = BI_RGB;  // ��ѹ��
			destbmih[i][j]->biSizeImage = 0;
			destbmih[i][j]->biXPelsPerMeter = bmih->biXPelsPerMeter;  // ����ԭ�ȵ�ֵ
			destbmih[i][j]->biYPelsPerMeter = bmih->biYPelsPerMeter;  // ����ԭ�ȵ�ֵ
			destbmih[i][j]->biClrUsed = 0;
			destbmih[i][j]->biClrImportant = 0;

			// ÿ�����ӷ���RGB���ؿռ�
			unsigned long lineSize = destbmih[i][j]->biWidth * sizeof(RGBTRIPLE);  // ԭ���г�
			destRgb[i][j] = (RGBTRIPLE*)malloc(destbmih[i][j]->biHeight * lineSize);  // д�ļ�ʱ�Ų��ո�
			memset(destRgb[i][j], 0, destbmih[i][j]->biHeight * lineSize);
			// 

			// ����ռ���ܿռ�
			lineSize += lineSize % 4 ? 4 - lineSize % 4 : 0;  // ����϶
			destbmih[i][j]->biSizeImage = destbmih[i][j]->biHeight * lineSize; // BYTEλ��(�Ƕ�����λ)
		}
	}
	//RGBTRIPLE*p=nullptr;
	//// ����RGB����
	//// �ȴ������ң��ٴ������£�ÿ���д�������
	//unsigned long rowStart = 0;
	//unsigned long colStart = 0;
	//for (int i=0;i<divx;i++) {
	//	if(i)  // ������һ�� 
	//		rowStart += destbmih[i][0]->biWidth;  // ÿ����Ŀ�Ⱦ���ͬ
	//	for (int j=0;j<divy;j++) {
	//		//lineStart += destbmih[i][j]->biWidth;
	//		j == 0 ? colStart = 0 : colStart += destbmih[i][j-1]->biHeight;
	//		unsigned long lineSize = destbmih[i][j]->biWidth * sizeof(RGBTRIPLE);  // ԭ���г�
	//		lineSize += lineSize % 4 ? 4 - lineSize % 4 : 0;  // ����϶
	//		for (int k=0;k<destbmih[i][j]->biHeight;k++) { // ���и���
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
			// ����д��
			// ÿ�����ӷ���RGB���ؿռ�
			unsigned long lineSize = destbmih[i][j]->biWidth * sizeof(RGBTRIPLE);  // ԭ���г�
			lineSize += lineSize % 4 ? 4 - lineSize % 4 : 0;  // ����϶
			for (int k =0; k<destbmih[i][j]->biHeight;k++) {
				// ����һ�л����
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
	//save(); // δ���
	printf("�����У�δ���\r\n");
	system("pause");
	return 0;
}
