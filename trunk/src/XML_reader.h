#include <libxml/xmlreader.h>
#include <libxml/encoding.h>

#include <string>
#include <iostream>

using namespace std;

string latin1(xmlChar const * entrada);

string write_xml(string input);

string getTagName(xmlTextReaderPtr reader);

int nextTag(xmlTextReaderPtr reader);

string attrib(xmlTextReaderPtr reader, string const &nombre);

string allAttrib(xmlTextReaderPtr reader);

string allAttrib_except(xmlTextReaderPtr reader, string attrib_no);

string text_attrib(string attributes, string const &nombre);

string text_whole_attrib(string attributes, string const &nombre);

string text_allAttrib_except(string attributes, string const &nombre);
