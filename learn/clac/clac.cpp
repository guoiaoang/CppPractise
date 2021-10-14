#include <stdio.h>  // printf
#include <stdlib.h> // malloc
#include <string.h> // strlen

// �ο�:https://github.com/kothariji/BhimIntegers/blob/master/BhimInteger.h
char*p = NULL;  // ָ�����ַ���
char* multi(const char *szA, const char*szB) {
	// �������
	// ������ڴ�(����int��ֹԽ��)
	int ia = strlen(szA), ib = strlen(szB), len = ia + ib, iLeadZero = 0;
	int* ans = (int*)malloc(len * sizeof(int));
	memset(ans, 0, len * sizeof(int));
	// ������ // �Ӻ���ǰ��  // ������λ����˽������
	for (int i = ia - 1; i >= 0; i--) {
		for (int j = ib - 1; j >= 0; j--) {
			ans[i + j + 1] += (szA[i] - '0')*(szB[j] - '0');  // �����������Ӧλ��
			ans[i + j] += ans[i + j + 1] / 10;  // ǰ��λ�ý�λ�洢  // �ۼӺ����Խ�磿��
			ans[i + j + 1] = ans[i + j + 1] % 10;  // ����λ��ֻ�洢��λ����
		}
	}
	// ���ҷ�0
	char *p = (char*)malloc(len * sizeof(char*));
	memset(p, 0, len * sizeof(char*));
	p[0] = '0';
	for (int i = 0; i < len; i++) {
		if (ans[i]) {
			iLeadZero = i;
			break;
		}
	}
	// ���������
	for (int i = 0; i < len - iLeadZero; i++) {
		*(p + i) = ans[iLeadZero + i] + '0';
	}
	free(ans); ans = NULL;
	return p;
}
/*  output:
muiti:10123456789 * 19876543210 = 201219326301126352690
muiti:99909101233423456789 * 99993198765433422106 = 9990230618109521189181023257816288377634
*/
int main() {
	char*szA = "10123456789";
	char*szB = "19876543210";
	// 1.219326311126353e+17  // 201219326301126352690
	printf("muiti:%s * %s = %s\r\n", szA, szB, multi(szA, szB));
	if (p) { free(p); p = NULL; }

	szA = "99909101233423456789";
	szB = "99993198765433422106";
	printf("muiti:%s * %s = %s\r\n", szA, szB, multi(szA, szB));
	if (p) { free(p); p = NULL; }

	getchar();
	return 0;
}
