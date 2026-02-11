#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

/*
Inteface to store XML tags in a tree like data structure
*/
struct XMLTag
{
    bool is_text;
    std::string name;
    std::map<std::string, std::string> properties;
    std::vector<XMLTag> children;

    // name denotes the tag name or the text content of its parent tag in case is_text is set to be true
    XMLTag(bool is_text=false);
};


struct XMLParser
{
    // Parses a file and populates its xml content to the root XMLTag. Returns false in case the file is not valid.
    static bool ParseFile(const std::string& content, XMLTag& root);
};

std::ostream& operator<<(std::ostream& os, const XMLTag& tag);  // To output it to standard std streams such as cout and stringstream

#endif // XMLPARSER_H
