#include <iostream>
using namespace std;

int main() {
  int a, b;
  cin >> a >> b;

  string str = ((a*b) % 2) == 0 ? "Even" : "Odd";
  cout << str << endl;

  return 0;
}