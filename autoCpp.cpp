// autoCpp
// David Vaughan

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// Extract the template line and template parameters
void getTemplateLine(string& line, vector<string>& params, ifstream& fin);

// Main
int main()
{
    // Prompt for header file name
    cout << endl << "Header File Name: ";
    string inputFileName;
    cin >> inputFileName;

    // Open the file for input
    ifstream fin;
    fin.open(inputFileName.c_str());

    // Look for the template line
    string templateLine;
    vector<string> templateParams;
    getTemplateLine(templateLine, templateParams, fin);

    // **For testing purposes**
    // Print out the template line and parameters
    cout << endl << "\"" << templateLine << "\"" << endl
         << "Template parameters: ";
    for(int i = 0; i < templateParams.size(); ++i)
        cout << templateParams[i] << " ";
    cout << endl << endl;
    
    // Look for the class line
        // Store class name (for function scope and finding constrs/destrs)
    // Look for function lines
        // Store return type (if any),
        //  and the rest of the function header separately

    return 0;
}

void getTemplateLine(string& line, vector<string>& params, ifstream& fin)
{
    // Extract lines from the file until the template line is found
    string tempLine;
    while(getline(fin, tempLine))
    {
        // Make a stringstream of the line
        stringstream tempStream(tempLine);

        // Check if the first word is "template"
        string tempWord;
        tempStream >> tempWord;
        if(tempWord == "template")
        {
            // If the line was found, store the entire template line
            line = tempLine;

            // Then grab the template parameters
            // Extract characters until '<' is found, to find the first param
            while(tempStream.get() != '<') {}

            // Grab param words until the end of the template line 
            char tempChar;
            do
            {
                // Ignore the "typename" or "class" string
                string ignore;
                tempStream >> ignore;

                // Ignore leading spaces
                do
                    tempStream.get(tempChar);
                while(tempChar == ' ');

                // Extract chars until ',' or '>' is found
                string param = "";
                while(tempChar != ',' && tempChar != '>')
                {
                    param += tempChar;
                    tempStream.get(tempChar);
                }

                // Push the parameter onto the list
                params.push_back(param);
            }
            while(tempChar != '>');

            // Done with the template line. Return.
            return;
        }
    }
}
