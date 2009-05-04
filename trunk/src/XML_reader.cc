#include <XML_reader.h>

#include <lttoolbox/xml_parse_util.h>

// Converts xmlChar strings used by libxml2 to ordinary strings
string xmlc2s(xmlChar const *entrada) {
  if (entrada == NULL) {
    return "";
  }

  return reinterpret_cast<char const *>(entrada);
}


wstring write_xml(wstring s) {
  unsigned int pos=0;
  while ((pos=s.find(L"&", pos)) != wstring::npos) {
    s.replace(pos, 1, L"&amp;");
    pos += 4;
  }

  while ((pos=s.find(L'"')) != wstring::npos) {
    s.replace(pos, 1, L"&quot;");
  }

  pos=0;
  while ((pos=s.find(L'\'', pos)) != wstring::npos) {
    if (s[pos-1] != L'=' && s[pos+1] != L' ' && pos != (s.size()-1))
      s.replace(pos, 1, L"&apos;");
    else pos++;
  }

  while ((pos=s.find(L"<")) != wstring::npos) {
    s.replace(pos, 1, L"&lt;");
  }

  while ((pos=s.find(L">")) != wstring::npos) {
    s.replace(pos, 1, L"&gt;");
  }

  return s;
}


wstring getTagName(xmlTextReaderPtr reader) {
//Momentuko etiketaren izena itzultzen du.
  xmlChar const *xname = xmlTextReaderConstName(reader);
  wstring tagName = XMLParseUtil::stows(xmlc2s(xname));
  return tagName;
}


int nextTag(xmlTextReaderPtr reader) {
//Hurrengo etiketara pasatzen da. Textua eta DTD definizioak saltatzen ditu.
  int ret = xmlTextReaderRead(reader);
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  while (ret == 1 and (tagType == XML_READER_TYPE_DOCUMENT_TYPE or tagName == L"#text")) {
    // DTD definizioekin zer egin behar den definitu behar dugu.

    ret = xmlTextReaderRead(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  //cerr << tagName << "\t" << tagType << endl;

  return ret;
}


wstring attrib(xmlTextReaderPtr reader, string const &nombre) {
  if (nombre[0] == '\'' && nombre[nombre.size()-1] == '\'')
    return XMLParseUtil::stows(nombre.substr(1, nombre.size()-2));

//atributu izen bat emanda, momentuko elementuaren izen bereko atributuaren balioa itzultzen du.
  xmlChar *nomatrib = xmlCharStrdup(nombre.c_str());
  xmlChar *atrib = xmlTextReaderGetAttribute(reader,nomatrib);

  wstring resultado = XMLParseUtil::stows(xmlc2s(atrib));
  
  xmlFree(atrib);
  xmlFree(nomatrib);
  
  return resultado;
}


wstring allAttrib(xmlTextReaderPtr reader) {
 //Momentuko etiketaren atributu guztiak itzultzen ditu.
  wstring output = L"";

  for (int hasAttrib=xmlTextReaderMoveToFirstAttribute(reader); hasAttrib > 0; hasAttrib=xmlTextReaderMoveToNextAttribute(reader)) {
    xmlChar const *xname = xmlTextReaderConstName(reader);
    xmlChar const *xvalue = xmlTextReaderConstValue(reader);
    output += L" " + XMLParseUtil::stows(xmlc2s(xname)) + L"='" +
              XMLParseUtil::stows(xmlc2s(xvalue)) + L"'";
  }

  xmlTextReaderMoveToElement(reader);
  return output;
}


wstring allAttrib_except(xmlTextReaderPtr reader, wstring attrib_no) {
 //Momentuko etiketaren atributu guztiak itzultzen ditu.
  wstring output = L"";

  for (int hasAttrib=xmlTextReaderMoveToFirstAttribute(reader); hasAttrib > 0; hasAttrib=xmlTextReaderMoveToNextAttribute(reader)) {
    xmlChar const *xname = xmlTextReaderConstName(reader);
    xmlChar const *xvalue = xmlTextReaderConstValue(reader);
    if (XMLParseUtil::stows(xmlc2s(xname)) != attrib_no)
      output += L" " + XMLParseUtil::stows(xmlc2s(xname)) + L"='" +
                XMLParseUtil::stows(xmlc2s(xvalue)) + L"'";
  }

  xmlTextReaderMoveToElement(reader);
  return output;
}


wstring text_attrib(wstring attributes, wstring const &nombre) {
  if (nombre[0] == L'\'' && nombre[nombre.size()-1] == L'\'')
    return nombre.substr(1, nombre.size()-2);

  wstring to_find = nombre + L"=";
  unsigned int startPos = attributes.find(to_find);
  if (startPos == wstring::npos) return L"";
  startPos = startPos+nombre.size()+2;

  unsigned int endPos = attributes.find(L"'", startPos);
  if (endPos == wstring::npos) endPos = attributes.size();

  return attributes.substr(startPos, endPos-startPos);
}


wstring text_whole_attrib(wstring attributes, wstring const &nombre) {
  wstring to_find = nombre + L"=";
  unsigned int startPos = attributes.find(to_find);
  if (startPos == wstring::npos) return L"";

  unsigned int endPos = attributes.find(L"'", startPos+nombre.size()+2);
  if (endPos == wstring::npos) endPos = attributes.size();

  return attributes.substr(startPos-1, endPos-startPos+2);
}


wstring text_allAttrib_except(wstring attributes, wstring const &nombre){
  wstring output = attributes;
  wstring to_find = nombre + L"='";
  unsigned int startPos = output.find(to_find);
  if (startPos == wstring::npos) return attributes;

  unsigned int endPos = output.find(L"'", startPos+nombre.size()+2);
  if (endPos == wstring::npos) endPos = output.size();

  output.erase(startPos-1, endPos-startPos+2);
  return output;
}

