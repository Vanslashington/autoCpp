// autoCpp
// David Vaughan


#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>

using namespace std;


// A struct to hold function-line information
struct functionLine
{
    string returnType;
    string header;
};


// Extract the template line and template parameters
void getTemplateLine(string& line, vector<string>& params, ifstream& fin);

// Extract the class name
void getClassName(string& className, ifstream& fin);

// Extract the function lines
void getFunctionLines(vector<functionLine>& functionLines, string className,
                      ifstream& fin);

// Detect whether a line is a function line
bool isFunctionLine(const string& line);

// Print out the formatted .cpp file
void printCpp(string inputFileName, string templateLine,
              vector<string> templateParams, string className,
              vector<functionLine> functionLines);


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

    // Look for the class line
    string className;
    getClassName(className, fin);

    // Look for function lines
    vector<functionLine> functionLines;
    getFunctionLines(functionLines, className, fin);

    // Output the .cpp file
    printCpp(inputFileName, templateLine, templateParams,
             className, functionLines);

    // Done
    cout << "Done." << endl << endl;

    return 0;
}


/*
    getTemplateLine

    Searches through the header file until the class template line is found.
    The entire line (string) is passed back to line, and both of the template
    parameters (strings) are passed back into the params vector. The ifstream
    object's position is left at the end of the template line. 
*/
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


/*
    getClassName

    Searches through the .h file until the class line is found. Passes the
    class name back to the reference parameter. The ifstream object's position
    is left at the end of the class line.
*/
void getClassName(string& className, ifstream& fin)
{
    // Extract lines from the file until the class line is found
    string tempLine;
    while(getline(fin, tempLine))
    {
        // Make a stringstream of the line
        stringstream tempStream(tempLine);

        // Check if the first word is "class"
        string tempWord;
        tempStream >> tempWord;
        if(tempWord == "class")
        {
            char tempChar;

            // Ignore leading spaces
            do
                tempStream.get(tempChar);
            while(tempChar == ' ');

            // Extract the class name
            className = "";
            while(tempChar != ' ' && tempChar != '\n' && tempChar != '{')
            {
                className += tempChar;
                tempStream.get(tempChar);
            }

            // And we're done
            return;
        }
    }
}


/*
    getFunctionLines

    Searches through the .h file for all function lines, and passes back their
    return types and the rest of each header to a vector through the reference
    parameter.
*/
void getFunctionLines(vector<functionLine>& functionLines, string className,
                      ifstream& fin)
{
    // Extract lines from the file until EOF, looking for functions
    string tempLine;
    while(getline(fin, tempLine))
    {
        // Detect if this is a function line
        if(!isFunctionLine(tempLine))
            continue;
            
        // Make a stringstream of the line
        stringstream lineStream(tempLine);
        
        // Get to the first non-space character 
        char tempChar;
        int funcBeg = -1; 
        do
        {
            ++funcBeg;
            lineStream.get(tempChar);
        }
        while(tempChar == ' '); 

        // Is it a destructor?
        bool isDestructor = false;
        if(tempChar == '~')
            isDestructor = true;

        // If not a destructor, check for constructor
        bool isConstructor = false;
        if(!isDestructor)
        {
            lineStream.seekg(funcBeg, lineStream.beg);
            string firstFuncWord = "";
            lineStream.get(tempChar);
            while(tempChar != ' ' && tempChar != '(')
            {
                firstFuncWord += tempChar;
                lineStream.get(tempChar);
            }

            if(firstFuncWord == className)
                isConstructor = true;
        }
        
        /* If it's a constructor or destructor, just grab the whole line up to
           the semicolon */
        if(isConstructor || isDestructor)
        {
            functionLine retFunc;
            lineStream.seekg(funcBeg, lineStream.beg);
            getline(lineStream, retFunc.header, ';');
            retFunc.returnType = "";

            functionLines.push_back(retFunc);

            // And we're done
            continue;
        }

        // Otherwise, find the location of the function name
        lineStream.seekg(funcBeg, lineStream.beg);
        int nameBeg = funcBeg;
        lineStream.get(tempChar);
        while(tempChar != '(')
        {
            // If we've hit a space,
            if(tempChar == ' ')
                // ...then that word wasn't the function name
                nameBeg = lineStream.tellg();
            lineStream.get(tempChar);
        }

        // Extract the return type and header
        functionLine retFunc;
        lineStream.seekg(funcBeg, lineStream.beg);
        for(int i = funcBeg; i < nameBeg-1; ++i)
        {
            lineStream.get(tempChar);
            retFunc.returnType += tempChar;
        }

        lineStream.seekg(nameBeg, lineStream.beg);
        getline(lineStream, retFunc.header, ';');

        // Add the function to the list, and we're done
        functionLines.push_back(retFunc);
    }
}


/*
    isFunctionLine

    Return whether or not a given string is a function line.
*/
bool isFunctionLine(const string& line)
{
    // Make a stringstream of the line
    stringstream lineStream(line);

    // Search for an open paren
    char tempChar;
    while(lineStream.get(tempChar))
        if(tempChar == '(')
            return true;

    // If there were none, return false
    return false;
}


/*
    printCpp

    Print out the formatted .cpp file, with all the templated function stubs.
*/
void printCpp(string inputFileName, string templateLine,
              vector<string> templateParams, string className,
              vector<functionLine> functionLines)
{
    // Create the output file name from the input file name
    string outputFileName = "";
    int index = 0;
    while(inputFileName[index] != '.')
        outputFileName += inputFileName[index++];
    outputFileName += ".cpp";

    // Open the output file
    ofstream fout;
    fout.open(outputFileName.c_str());

    // Print the .h include
    fout << "#include \"" << inputFileName << "\"\n" << endl;

    // Iterate through the function list
    for(int i = 0; i < functionLines.size(); ++i)
    {
        // Print the template line
        fout << templateLine << endl;

        // Print the function header line
        if(functionLines[i].returnType != "")
            fout << functionLines[i].returnType << " ";
        fout << className << "<";
        for(int j = 0; j < templateParams.size(); ++j)
        {
            if(j != 0)
                fout << ", ";
            fout << templateParams[j];
        }
        fout << ">::" << functionLines[i].header;

        // Print the braces
        fout << "\n{\n}\n" << endl;
    }
}
