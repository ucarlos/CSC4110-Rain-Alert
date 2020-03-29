#include <iostream>
#include <string>
#include <vector>
using namespace std;
int main() {
    vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    string temp = "Hello World!";
    cout << temp << endl;
    cout << "Values of vector:\n";
    for (int &i : vec)
        cout << i << " ";
    cout << endl;


    return 0;
}
