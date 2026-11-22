#include <stdio.h>
#include <stdlib.h>
int main(void) {
int W, H, Imax;
if (scanf("%d %d %d", &W, &H, &Imax) != 3) return 1;
int *grid = malloc(sizeof(int) * (long)W * H);
if (!grid) return 1;
for (int y = 0; y < H; y++)
for (int x = 0; x < W; x++) {
double cr = -2.0 + 3.0 * x / W;
double ci = -1.5 + 3.0 * y / H;
double zr = 0, zi = 0; int it = 0;
while (it < Imax && zr*zr + zi*zi <= 4.0) {
double t = zr*zr - zi*zi + cr;
zi = 2*zr*zi + ci; zr = t; it++;
}
grid[(long)y*W+x] = it;
}
for (long i = 0; i < (long)W*H; i++) printf("%d\n", grid[i]);
free(grid);
return 0;
}
