#include <stdio.h>
#include <stdlib.h>
int main(void) {
int W, H, L;
if (scanf("%d %d %d", &W, &H, &L) != 3) return 1;
long n = (long)W * H;
int *img = malloc(sizeof(int) * n);
if (!img) return 1;
for (long i = 0; i < n; i++) if (scanf("%d", &img[i]) != 1) return 1;
for (int i = 0; i < H; i++)
for (int j = 0; j < W; j++) {
long idx = (long)i * W + j;
img[idx] = (img[idx] > L) ? 255 : 0;
}
for (long i = 0; i < n; i++) printf("%d\n", img[i]);
free(img);
return 0;
}
