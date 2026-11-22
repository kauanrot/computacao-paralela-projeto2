#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using Symbol = std::string;
using SymbolString = std::vector<Symbol>;
using Production = std::pair<SymbolString, SymbolString>;

struct Grammar {
std::set<Symbol> nonterminals, terminals;
std::set<Production> productions;
Symbol start;
};

static SymbolString split(const std::string &s) {
std::istringstream iss(s); SymbolString v; std::string t;
while (iss >> t) v.push_back(t);
return v;
}

static bool is_terminal(const Grammar &g, const Symbol &s) {
return g.terminals.count(s) > 0;
}

static SymbolString replace_once(const SymbolString &text,
const SymbolString &old_s,
const SymbolString &new_s) {
auto it = std::search(text.begin(), text.end(), old_s.begin(), old_s.end());
SymbolString out;
out.insert(out.end(), text.begin(), it);
out.insert(out.end(), new_s.begin(), new_s.end());
out.insert(out.end(), it + old_s.size(), text.end());
return out;
}

static bool parse(const Grammar &g, SymbolString current, SymbolString input) {
size_t i = 0;
while (i < current.size() && i < input.size() && current[i] == input[i]) i++;
current.erase(current.begin(), current.begin() + i);
input.erase(input.begin(), input.begin() + i);
if (current.empty() && input.empty()) return true;
if (current.empty() && !input.empty()) return false;
if (!current.empty() && is_terminal(g, current[0])) return false;
Symbol nt;
for (auto &x : current) if (!is_terminal(g, x)) { nt = x; break; }
if (nt.empty()) return false;
for (auto &p : g.productions) {
if (p.first.size() == 1 && p.first[0] == nt) {
SymbolString next = replace_once(current, p.first, p.second);
if (parse(g, next, input)) return true;
}
}
return false;
}

int main() {
Grammar g;
std::string line;
std::getline(std::cin, line); { auto v = split(line); g.terminals = std::set<Symbol>(v.begin(), v.end()); }
std::getline(std::cin, line); { auto v = split(line); g.nonterminals = std::set<Symbol>(v.begin(), v.end()); }
std::getline(std::cin, line); { auto v = split(line); if (!v.empty()) g.start = v[0]; }
while (std::getline(std::cin, line)) {
if (line == "-") break;
auto pos = line.find(':');
if (pos == std::string::npos) continue;
auto lhs = split(line.substr(0, pos));
auto rhs = split(line.substr(pos + 1));
g.productions.insert({lhs, rhs});
}
std::string w;
std::getline(std::cin, w);
SymbolString input = split(w);
SymbolString start = {g.start};
std::cout << (parse(g, start, input) ? "SUCCESS" : "FAILED") << "\n";
return 0;
}
