// autoCpp
// David Vaughan

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main()
{
    // Prompt for header file name
    cout << "Header File Name: ";
    string inputFileName;
    cin >> inputFileName;

    // Open the file for input
    ifstream fin;
    fin.open(inputFileName.c_str());

    /*
        Look for:
        template
        class
        functions
        braces/parenthases/etc
    */

    return 0;
}
