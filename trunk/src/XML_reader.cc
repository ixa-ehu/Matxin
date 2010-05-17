/*
 *  Copyright (C) 2005 IXA Research Group / IXA Ikerkuntza Taldea.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 *  02110-1301  USA
 */

#include <XML_reader.h>


// Converts xmlChar strings used by libxml2 to ordinary strings
string xmlc2s(xmlChar const *entrada)
{
  if (entrada == NULL)
    return "";

  return reinterpret_cast<char const *>(entrada);
}


wstring write_xml(wstring s)
{
  size_t pos = 0;
  while ((pos = s.find(L"&", pos)) != wstring::npos)
  {
    s.replace(pos, 1, L"&amp;");
    pos += 4;
  }

  while ((pos = s.find(L'"')) != wstring::npos)
  {
    s.replace(pos, 1, L"&quot;");
  }

  pos = 0;
  while ((pos = s.find(L'\'', pos)) != wstring::npos)
  {
    if (s.find(L"='") == wstring::npos || (s[pos - 1] != L'=' && s[pos + 1] != L' ' && pos != (s.size() - 1)))
      s.replace(pos, 1, L"&apos;");
    else
      pos++;
  }

  while ((pos = s.find(L"<")) != wstring::npos)
  {
    s.replace(pos, 1, L"&lt;");
  }

  while ((pos = s.find(L">")) != wstring::npos)
  {
    s.replace(pos, 1, L"&gt;");
  }

  return s;
}


//Momentuko etiketaren izena itzultzen du.
wstring getTagName(xmlTextReaderPtr reader)
{
  xmlChar const *xname = xmlTextReaderConstName(reader);
  wstring tagName = XMLParseUtil::stows(xmlc2s(xname));
  return tagName;
}


//Hurrengo etiketara pasatzen da. Textua eta DTD definizioak saltatzen ditu.
int nextTag(xmlTextReaderPtr reader)
{
  int ret = xmlTextReaderRead(reader);
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  while (ret == 1 and (tagType == XML_READER_TYPE_DOCUMENT_TYPE or tagName == L"#text"))
  {
    // DTD definizioekin zer egin behar den definitu behar dugu.
    ret = xmlTextReaderRead(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  return ret;
}


wstring attrib(xmlTextReaderPtr reader, string const &nombre)
{
  if (nombre[0] == '\'' && nombre[nombre.size() - 1] == '\'')
    return XMLParseUtil::stows(nombre.substr(1, nombre.size() - 2));

  //atributu izen bat emanda, momentuko elementuaren izen bereko
  // atributuaren balioa itzultzen du.
  xmlChar *nomatrib = xmlCharStrdup(nombre.c_str());
  xmlChar *atrib = xmlTextReaderGetAttribute(reader,nomatrib);

  wstring result = XMLParseUtil::stows(xmlc2s(atrib));
  
  xmlFree(atrib);
  xmlFree(nomatrib);
  
  return result;
}


//Momentuko etiketaren atributu guztiak itzultzen ditu.
wstring allAttrib(xmlTextReaderPtr reader)
{
  wstring output = L"";

  for (int hasAttrib = xmlTextReaderMoveToFirstAttribute(reader);
       hasAttrib > 0;
       hasAttrib = xmlTextReaderMoveToNextAttribute(reader))
  {
    xmlChar const *xname = xmlTextReaderConstName(reader);
    xmlChar const *xvalue = xmlTextReaderConstValue(reader);
    output += L" " + XMLParseUtil::stows(xmlc2s(xname)) + L"='" +
              XMLParseUtil::stows(xmlc2s(xvalue)) + L"'";
  }

  xmlTextReaderMoveToElement(reader);
  return output;
}


//Momentuko etiketaren atributu guztiak itzultzen ditu.
wstring allAttrib_except(xmlTextReaderPtr reader, wstring attrib_no)
{
  wstring output = L"";

  for (int hasAttrib=xmlTextReaderMoveToFirstAttribute(reader);
       hasAttrib > 0;
       hasAttrib = xmlTextReaderMoveToNextAttribute(reader))
  {
    xmlChar const *xname = xmlTextReaderConstName(reader);
    xmlChar const *xvalue = xmlTextReaderConstValue(reader);
    if (XMLParseUtil::stows(xmlc2s(xname)) != attrib_no)
      output += L" " + XMLParseUtil::stows(xmlc2s(xname)) + L"='" +
                XMLParseUtil::stows(xmlc2s(xvalue)) + L"'";
  }

  xmlTextReaderMoveToElement(reader);
  return output;
}


wstring text_attrib(wstring attributes, const wstring& nombre)
{
  vector<wstring> tokens;
  wstring value = L"";

  if (nombre[0] == L'\'' && nombre[nombre.size()-1] == L'\'') return nombre.substr(1, nombre.size()-2);

  Tokenize(attributes, tokens);
  for (size_t i = 0; i < tokens.size(); i++)
  {
    vector<wstring> attribs;

    Tokenize(tokens[i], attribs, L"=");
    if (attribs[0] == nombre)
    {
      Tokenize2(attribs[1], value, L"'");
      break;
    }
  }

  return value;
}


wstring text_allAttrib_except(wstring attributes, const wstring &nombre)
{
  vector<wstring> tokens;

  Tokenize(attributes, tokens);
  for (size_t i = 0; i < tokens.size(); i++)
  {
    vector<wstring> attribs;

    Tokenize(tokens[i], attribs, L"=");
    if (attribs[0] == nombre)
    {
      tokens.erase(tokens.begin() + i);
      break;
    }
  }

  return v2s(tokens);

}

