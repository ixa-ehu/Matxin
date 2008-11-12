#include <XML_reader.h>

string latin1(xmlChar const * entrada) {
//libxml itzultzen dituen string-ak UTF8-tik ISO-8859-1 encoding-era aldatzeko funtzioa.
  if(entrada == NULL) {
    return "";
  }

  int lensalida=xmlStrlen(entrada)+1;
  int lenentrada=xmlStrlen(entrada);

  unsigned char salida[lensalida];
  
  if(UTF8Toisolat1(salida, &lensalida, entrada, &lenentrada) != 0) {
//    cerr << "Error: Cannot convert encoding from UTF-8 to ISO-8859-1." << endl;
//    exit(EXIT_FAILURE);
  }

  salida[lensalida] = 0;
  string output = reinterpret_cast<char *>(salida);

  return output;  
}

string write_xml(string s) {
  unsigned int pos=0;
  while ((pos=s.find("&", pos))!=string::npos) {
     s.replace(pos,1,"&amp;");
     pos += 4;
  }

  while ((pos=s.find('"'))!=string::npos) {
    s.replace(pos,1,"&quot;");
  }

  pos=0;
  while ((pos=s.find('\'', pos))!=string::npos) {
    if (s[pos-1] != '=' && s[pos+1] != ' ' && pos != (s.size()-1)) 
      s.replace(pos,1,"&apos;");
    else pos++;
  }

  while ((pos=s.find("<"))!=string::npos) {
    s.replace(pos,1,"&lt;");
  }

  while ((pos=s.find(">"))!=string::npos) {
    s.replace(pos,1,"&gt;");
  }

  return s;
}

string getTagName(xmlTextReaderPtr reader) {
//Momentuko etiketaren izena itzultzen du.
  xmlChar const *xname = xmlTextReaderConstName(reader);
  string tagName = latin1(xname);
  return tagName;
}

int nextTag(xmlTextReaderPtr reader) {
//Hurrengo etiketara pasatzen da. Textua eta DTD definizioak saltatzen ditu.
  int ret = xmlTextReaderRead(reader);
  string tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  while (ret == 1 and (tagType == XML_READER_TYPE_DOCUMENT_TYPE or tagName == "#text")) {
    // DTD definizioekin zer egin behar den definitu behar dugu.

    ret = xmlTextReaderRead(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  //cerr << tagName << "\t" << tagType << endl;

  return ret;
}

string attrib(xmlTextReaderPtr reader, string const &nombre) {
  if (nombre[0]=='\'' && nombre[nombre.size()-1]=='\'') return nombre.substr(1, nombre.size()-2);

//atributu izen bat emanda, momentuko elementuaren izen bereko atributuaren balioa itzultzen du.
  xmlChar *nomatrib = xmlCharStrdup(nombre.c_str());
  xmlChar *atrib = xmlTextReaderGetAttribute(reader,nomatrib);

  string resultado = latin1(atrib);
  
  xmlFree(atrib);
  xmlFree(nomatrib);
  
  return resultado;
}

string allAttrib(xmlTextReaderPtr reader) {
 //Momentuko etiketaren atributu guztiak itzultzen ditu.
  string output = "";

  for (int hasAttrib=xmlTextReaderMoveToFirstAttribute(reader); hasAttrib > 0; hasAttrib=xmlTextReaderMoveToNextAttribute(reader)) {
    xmlChar const *xname = xmlTextReaderConstName(reader);
    xmlChar const *xvalue = xmlTextReaderConstValue(reader);
    output += " " + latin1(xname) + "='" + latin1(xvalue) + "'";
  }

  xmlTextReaderMoveToElement(reader);
  return output;
}

string allAttrib_except(xmlTextReaderPtr reader, string attrib_no) {
 //Momentuko etiketaren atributu guztiak itzultzen ditu.
  string output = "";

  for (int hasAttrib=xmlTextReaderMoveToFirstAttribute(reader); hasAttrib > 0; hasAttrib=xmlTextReaderMoveToNextAttribute(reader)) {
    xmlChar const *xname = xmlTextReaderConstName(reader);
    xmlChar const *xvalue = xmlTextReaderConstValue(reader);
    if (latin1(xname) != attrib_no)
      output += " " + latin1(xname) + "='" + latin1(xvalue) + "'";
  }

  xmlTextReaderMoveToElement(reader);
  return output;
}

string text_attrib(string attributes, string const &nombre){
  if (nombre[0]=='\'' && nombre[nombre.size()-1]=='\'') return nombre.substr(1, nombre.size()-2);

  string to_find = nombre + "=";
  unsigned int startPos = attributes.find(to_find);
  if (startPos == string::npos) return "";
  startPos = startPos+nombre.size()+2;

  unsigned int endPos = attributes.find("'", startPos);
  if (endPos == string::npos) endPos = attributes.size();

  return attributes.substr(startPos, endPos-startPos);
}

string text_whole_attrib(string attributes, string const &nombre){
  string to_find = nombre + "=";
  unsigned int startPos = attributes.find(to_find);
  if (startPos == string::npos) return "";

  unsigned int endPos = attributes.find("'", startPos+nombre.size()+2);
  if (endPos == string::npos) endPos = attributes.size();

  return attributes.substr(startPos-1, endPos-startPos+2);
}

string text_allAttrib_except(string attributes, string const &nombre){
  string output = attributes;
  string to_find = nombre + "='";
  unsigned int startPos = output.find(to_find);
  if (startPos == string::npos) return attributes;

  unsigned int endPos = output.find("'", startPos+nombre.size()+2);
  if (endPos == string::npos) endPos = output.size();

  output.erase(startPos-1, endPos-startPos+2);
  return output;
}
