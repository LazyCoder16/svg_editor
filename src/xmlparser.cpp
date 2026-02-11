#include "xmlparser.h"
#include <iostream>
#include <stack>
#include <cctype>
#include <string>


XMLTag::XMLTag(bool is_text)
    : is_text(is_text)
{}


int SkipXmlHeader(const std::string &content) {
    // Returns position of first character after skipping xml header and whitespaces
    // Returns content.size() if there is an error in parsing the header
    int pos = 0;
    int len = content.size();
    while(pos < len && std::isspace(content[pos])) { ++pos; }
    if(pos >= len || content[pos] != '<') return len;
    if(pos+1 < len && content[pos+1] == '?') {
        // We ignore any further errors and simply search for ?>
        int header_end = content.find("?>", pos);
        if(header_end == std::string::npos) return len;
        pos = header_end + 2;
        while(pos < len && std::isspace(content[pos])) { ++pos; }
        if(pos >= len || content[pos] != '<') return len;
    }
    return pos;
}

bool XMLParser::ParseFile(const std::string &content, XMLTag &root)
{
    std::stack<XMLTag*> st;
    st.push(&root);
    int pos = SkipXmlHeader(content) + 1;
    
    enum State { StartTag, Attr, CloseTag };
    State state = StartTag;
    std::string buffer;
    size_t len = content.size();

    // Assumes tag names and attribute name only contain lower case alphabets and hypens as typical of SVG xml files.
    // Use a state machine to know what we are currently processing
    while(pos < len && !st.empty()) {
        switch(state)
        {
            case StartTag: 
            {
                if(st.top()->is_text)
                {
                    while(pos < len && content[pos] != '<')
                    {
                        buffer.push_back(content[pos]);
                        ++pos;
                    }
                    st.top()->name = buffer;
                    st.pop();
                    buffer.clear();
                    if(pos+1 >= len || content[pos+1] != '/') return false;
                    state = CloseTag;
                    break;
                }
                while(pos < len && std::islower(content[pos]))
                {
                    buffer.push_back(content[pos]);
                    ++pos;
                }
                if(buffer.empty()) return false;
                st.top()->name = buffer;
                buffer.clear();
                while(pos < len && std::isspace(content[pos])) { ++pos; }
                state = Attr;
                break;
            }
            
            case Attr:
            {
                if(pos < len && content[pos] == '/')
                {
                    state = CloseTag;
                    break;
                }
                else if (pos < len && content[pos] == '>')
                {
                    ++pos;
                    while(pos < len && std::isspace(content[pos])) { ++pos; }
                    if(pos >= len) return false;
                    if(content[pos] == '<')
                    {
                        st.top()->children.push_back(XMLTag(false));
                        st.push(&st.top()->children.back());
                        ++pos;
                    }
                    else {
                        st.top()->children.push_back(XMLTag(true));
                        st.push(&st.top()->children.back());
                    }
                    state = StartTag;
                    break;
                }
                std::string attr_name, attr_val;
                while(pos < len && (content[pos]=='-' || std::isalnum(content[pos])))
                {
                    attr_name.push_back(content[pos]);
                    ++pos;
                }
                if(attr_name.empty()) return false;
                while(pos < len && std::isspace(content[pos])) { ++pos; }
                if(pos >= len || content[pos] != '=') return false;
                ++pos;
                while(pos < len && std::isspace(content[pos])) { ++pos; }
                if(pos >= len || content[pos] != '"') return false;
                ++pos;
                while(pos < len && content[pos] != '"')
                {
                    attr_val.push_back(content[pos]);
                    ++pos;
                }
                if(pos >= len || content[pos] != '"') return false;
                st.top()->properties[attr_name] = attr_val;
                ++pos;
                while(pos < len && std::isspace(content[pos])) { ++pos; }
                break;
            }

            case CloseTag: 
            {   
                if(pos < len && content[pos] == '/')
                {
                    // Self closing
                    if(pos+1 >= len || content[pos+1] != '>') return false;
                    st.pop();
                    pos += 2;
                }
                else
                {
                    if(pos+1 >= len || content[pos+1] != '/') return false;
                    pos += 2;
                    while(pos < len && content[pos] != '>') 
                    {
                        if(!std::islower(content[pos])) return false;
                        buffer.push_back(content[pos]);
                        ++pos;
                    }
                    if(pos >= len || content[pos] != '>' || buffer != st.top()->name) return false;
                    buffer.clear();
                    st.pop();
                    ++pos;
                }

                while(pos < len && std::isspace(content[pos])) { ++pos; }
                if(pos < len)
                {
                    if(content[pos] != '<') return false;
                    if(pos+1 < len && content[pos+1] == '/')
                    {
                        state = CloseTag;
                    }
                    else 
                    {
                        ++pos;
                        st.top()->children.push_back(XMLTag(false));
                        st.push(&st.top()->children.back());
                        state = StartTag;
                    }
                }
                break;
            }
        }
    }
    return (root.name=="svg" && st.empty());
}


std::ostream& operator<<(std::ostream& os, const XMLTag& tag)
{
    // Outputs the current xml tag recursively to a standard output stream
    if(tag.is_text)
    {
        os << tag.name << "\n";
        return os;
    }
    os << "<" << tag.name << " ";
    for(const std::pair<std::string, std::string>& attr : tag.properties)
    {
        os << attr.first << "=" << '"' << attr.second << '"' << " ";
    }
    if(tag.children.size() > 0) {
        os << ">\n";
        for(const XMLTag& child : tag.children)
        {
            os << child;
        }
        os << "</" << tag.name << ">\n";
    }
    else {
        os << " />\n";
    }
    return os;
}
