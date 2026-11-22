#include <cstdio>
#include <vector>
#include <sstream>
#include <string>

static void harmonic(char* out, unsigned long d, unsigned long n) {
std::vector<unsigned long> digits(d + 11, 0UL);
for (unsigned long i = 1; i <= n; ++i) {
unsigned long rem = 1;
for (unsigned long k = 0; k < d + 11 && rem; ++k) {
unsigned long div = rem / i;
unsigned long mod = rem % i;
digits[k] += div;
rem = mod * 10;
}
}
for (unsigned long k = d + 11 - 1; k > 0; --k) {
digits[k - 1] += digits[k] / 10;
digits[k] %= 10;
}
if (digits[d + 1] >= 5) ++digits[d];
for (unsigned long k = d; k > 0; --k) {
digits[k - 1] += digits[k] / 10;
digits[k] %= 10;
}
std::ostringstream os;
os << digits[0] << '.';
for (unsigned long k = 1; k <= d; ++k) os << digits[k];
std::string s = os.str();
s.copy(out, s.size());
out[s.size()] = '\0';
}

int main() {
unsigned long d = 0, n = 0;
if (std::scanf("%lu %lu", &d, &n) != 2) return 1;
std::vector<char> out(d + 32, 0);
harmonic(out.data(), d, n);
std::printf("%s\n", out.data());
return 0;
}
