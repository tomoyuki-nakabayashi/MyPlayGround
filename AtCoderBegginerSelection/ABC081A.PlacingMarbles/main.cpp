#include <iostream>
using namespace std;

int main(){
  int s;
  cin >> s;

  int count{};
  count += s & 1;
  count += (s/10) & 1;
  count += (s/100) & 1;

  cout << count << endl;

  return 0;
}