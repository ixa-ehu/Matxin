#include <libxml/xmlreader.h>
#include <libxml/encoding.h>

#include <string>
#include <iostream>

using namespace std;

string xmlc2s(xmlChar const * entrada);

wstring write_xml(wstring input);

wstring getTagName(xmlTextReaderPtr reader);

int nextTag(xmlTextReaderPtr reader);

wstring attrib(xmlTextReaderPtr reader, string const &nombre);

wstring allAttrib(xmlTextReaderPtr reader);

wstring allAttrib_except(xmlTextReaderPtr reader, wstring attrib_no);

wstring text_attrib(wstring attributes, wstring const &nombre);

wstring text_whole_attrib(wstring attributes, wstring const &nombre);

wstring text_allAttrib_except(wstring attributes, wstring const &nombre);
