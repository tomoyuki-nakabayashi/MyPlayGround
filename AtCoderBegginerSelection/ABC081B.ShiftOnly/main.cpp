#include <iostream>
#include <vector>
using namespace std;

static bool hasOnlyEven(const vector<int>& a) {
  for (const auto& i : a) {
    if ((i%2) != 0) return false;
  }

  return true;
}

int main() {
  int n;
  cin >> n;
  vector<int> a(n);
  for (auto& i : a) {
    cin >> i;
  }
}