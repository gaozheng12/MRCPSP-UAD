#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define max 120
int res[max], used[max] = { 0 }, num[max] = { 0 }, len = 0;
char s[120];
void output(int list[], int n) {
	int i = 1;
	for (i = 0; i < n; i++) {
		if (list[i] != 0)
			printf("%c", s[list[i]]);
	}
	putchar('\n');
}
void perm(int k, int n) {
	int i;
	if (k == n) {
		output(res, n);
		return;
	}
	for (i = 1; i <= len; i++) {
		if (used[i] >= num[i])
			continue;
		used[i]++;
		res[k] = i;
		perm(k + 1, n);
		used[i]--;
	}
}
int main()
{
	int i, j, n, as_len[255] = { 0 };
	char ascll[255], temp[255];
	gets_s(temp);
	n = strlen(temp);
	for (i = 0; i < n; i++) {
		j = temp[i];
		if (as_len[j] == 0) {
			ascll[j] = temp[i];
			as_len[j]++;
		}
		else
			as_len[temp[i]]++;
	}
	for (i = 0; i < 255; i++) {
		if (as_len[i] != 0) {
			s[++len] = ascll[i];
			num[len] = as_len[i];
		}
	}
	perm(0, n);
}