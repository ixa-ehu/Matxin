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

#include <string>
#include <iostream>
#include <sstream>
#include <locale>

#include "config.h"
#include "IORedirectHandler.hpp"


#include <data_manager.h>
#include <XML_reader.h>

using namespace std;


wstring get_gen_case(wstring whole_case)
{
  if (whole_case.find(L"/"))
    return whole_case.substr(0, whole_case.find(L"/"));

  return whole_case;
}


wstring procSYN (xmlTextReaderPtr reader)
{
  wstring syns;

  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"SYN" and tagType == XML_READER_TYPE_ELEMENT)
  {
    syns += L"<SYN" + write_xml(allAttrib(reader));
    if (xmlTextReaderIsEmptyElement(reader) == 1)
    {
      syns += L"/>\n";
      return syns;
    }
    else
    {
      syns += L">\n";
    }
  }
  else
  {
    wcerr << L"ERROR (ST_inter): invalid tag: <" << tagName
          << L"> when <SYN> was expected..." << endl;
    exit(-1);
  }

  if (tagName == L"SYN" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    syns += L"</SYN>\n";
  }
  else
  {
    wcerr << L"ERROR (ST_inter): invalid document: found <" << tagName
          << L"> when </SYN> was expected..." << endl;
    exit(-1);
  }

  return syns;
}


//NODE etiketak irakurri eta tratatzen ditu.
//IN-OUT: Tratatu diren NODE kopurua; CHUNKaren luzera.
// - NODEaren azpian dauden NODEak irakurri eta prozesatzen ditu.
wstring procNODE(xmlTextReaderPtr reader, int &length)
{
  wstring nodes;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"NODE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    //Sarrerako atributu berdinekin idazten da irteerako NODE elementua.
    nodes += L"<NODE" + write_xml(allAttrib(reader));
    length++;
    if (xmlTextReaderIsEmptyElement(reader) == 1)
    {
      // NODE hutsa bada (<NODE .../>), NODE hutsa sortzen da eta
      // momentuko NODEarekin bukatzen dugu.
      nodes += L"/>\n";
      return nodes;
    }
    else
    {
      // NODE hutsa bada (<NODE .../>), NODE hutsa sortzen da eta
      // momentuko NODEarekin bukatzen dugu.
      nodes += L">\n";
    }
  }
  else
  {
    wcerr << L"ERROR (ST_inter): invalid tag: <" << tagName
          << L"> when <NODE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  // if there are, process the posible synonyms
  while (ret == 1 and tagName == L"SYN" and tagType == XML_READER_TYPE_ELEMENT)
  {
    nodes += procSYN(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  while (ret == 1 and tagName == L"NODE" and tagType == XML_READER_TYPE_ELEMENT)
  {
    //Menpeko NODEak tratatzen dira...
    nodes += procNODE(reader, length);

    nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  //NODE bukaera etiketa tratatzen da.
  if (tagName == L"NODE" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    nodes += L"</NODE>\n";
  }
  else
  {
    wcerr << L"ERROR (ST_inter): invalid document: found <" << tagName
          << L"> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}


//CHUNK etiketa irakurri eta tratatzen du.
// IN:  attributesFromParent gurasotik jasotzen diren attributuak.
// OUT: attributesToParent: gurasoari pasatzen zaizkion attributuak.
//      my_attributes: momentuko Chunkaren attributu guztiak.
// - Gramatika baten arabera attributuak CHUNK batetik bestera mugitzen dira.
// - NODOrik gabe gelditzen diren CHUNKak desagertzen dira.
vector<wstring> procCHUNK(xmlTextReaderPtr reader, wstring &attributesFromParent,
                          vector<wstring> &chunk_attributes)
{
  vector<wstring> chunk_subTrees;
  wstring chunk_type, my_attributes, attributesToChild, attributesFromChild, tree;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);
  int length = 0;

  //Irteeran sarrerako etiketa berdinak.
  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    chunk_type = attrib(reader, "type");
    my_attributes = allAttrib(reader);

    // Aurreko atributuetaz gain CHUNK batetik bestera mugitzen direnak ere gehitzen dira.
    // Hemen gurasotik jasotako attributuak zuhaitzan gehitzen dira.
    vector<movement> parent_attributes = get_chunk_movements(attributesFromParent, my_attributes, L"down");
    for (size_t i = 0; i < parent_attributes.size(); i++)
    {
      if (text_attrib(attributesFromParent, parent_attributes[i].from.attrib) != L"")
      {
        if (text_attrib(my_attributes, parent_attributes[i].to.attrib) == L"")
          my_attributes += L" " + parent_attributes[i].to.attrib + L"='" +
                           text_attrib(attributesFromParent,
                                       parent_attributes[i].from.attrib) + L"'";
        else if (text_attrib(my_attributes, parent_attributes[i].to.attrib) != L""
                 and parent_attributes[i].write_type == L"overwrite")
          my_attributes = text_allAttrib_except(my_attributes, parent_attributes[i].to.attrib) + L" " +
                          parent_attributes[i].to.attrib + L"='" +
                          text_attrib(attributesFromParent, parent_attributes[i].from.attrib) + L"'";
        else if (text_attrib(my_attributes, parent_attributes[i].to.attrib) != L""
                 and parent_attributes[i].write_type == L"concat")
        {
          wstring attribute = L" " + parent_attributes[i].to.attrib + L"='" +
                              text_attrib(my_attributes, parent_attributes[i].to.attrib) +
                              L"," + text_attrib(attributesFromParent, parent_attributes[i].from.attrib) + L"'";
          my_attributes = text_allAttrib_except(my_attributes, parent_attributes[i].to.attrib) + attribute;
        }
      }
    }
  }
  else
  {
    wcerr << L"ERROR (ST_inter): invalid tag: <" << tagName
          << L"> when <CHUNK> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  //Menpeko NODEak irakurri eta tratatzen dira.
  if (ret == 1 and tagName == L"NODE" and tagType == XML_READER_TYPE_ELEMENT)
  {
    tree += procNODE(reader, length);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  while (ret == 1 and tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    // Menpeko CHUNKak irakurri eta tratatzen dira. Gamatika baten arabera
    // zenbait attributu mugitzen dira CHUNK batetik bestera.
    vector<wstring> child_attributes;
    vector<wstring> child_subTree = procCHUNK(reader, my_attributes, child_attributes);

    for (size_t i = 0; i < child_attributes.size(); i++)
    {
      chunk_subTrees.push_back(child_subTree[i]);
      chunk_attributes.push_back(child_attributes[i]);
    }

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  //Menpeko CHUNKetatik jaso behar diren elementuak jasotzen dira.
  for (size_t i = 0; i < chunk_attributes.size(); i++)
  {
    vector<movement> parent_attributes = get_chunk_movements(chunk_attributes[i], my_attributes, L"up");
    for (size_t j = 0; j < parent_attributes.size(); j++)
    {
      if (text_attrib(chunk_attributes[i], parent_attributes[j].from.attrib) != L"")
      {
        if (text_attrib(my_attributes, parent_attributes[j].to.attrib) == L"")
          my_attributes += L" " + parent_attributes[j].to.attrib + L"='" +
                           text_attrib(chunk_attributes[i],
                                       parent_attributes[j].from.attrib) + L"'";
        else if (text_attrib(my_attributes, parent_attributes[j].to.attrib) != L""
                 and parent_attributes[j].write_type == L"overwrite")
          my_attributes = text_allAttrib_except(my_attributes, parent_attributes[j].to.attrib) +
                          L" " + parent_attributes[j].to.attrib + L"='" +
                          text_attrib(chunk_attributes[i],
                                      parent_attributes[j].from.attrib) + L"'";
        else if (text_attrib(my_attributes, parent_attributes[j].to.attrib) != L""
                 and parent_attributes[j].write_type == L"concat")
        {
          wstring attribute = L" " + parent_attributes[j].to.attrib + L"='" +
                              text_attrib(chunk_attributes[i],
                                          parent_attributes[j].from.attrib) + L","
                              + text_attrib(my_attributes,
                                            parent_attributes[j].to.attrib) + L"'";
          my_attributes = text_allAttrib_except(my_attributes, parent_attributes[j].to.attrib) + attribute;
        }
      }
    }
  }

  //CHUNK bukaera etiketa tratatzen da.
  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    for (size_t i=0; i < chunk_attributes.size(); i++)
    {
      if (chunk_subTrees[i] != L"") tree += L"<CHUNK" + write_xml(chunk_attributes[i]) + L">\n" + chunk_subTrees[i];
    }
    tree += L"</CHUNK>\n";
  }
  else
  {
    wcerr << L"ERROR (ST_inter): invalid document: found <" << tagName
          << L"> when </CHUNK> was expected..." << endl;
    exit(-1);
  }

  if (length > 0)
  {
    chunk_attributes.clear();
    chunk_subTrees.clear();
  }
  else
  {
    tree = L"";
  }

  chunk_attributes.push_back(my_attributes);
  chunk_subTrees.push_back(tree);

  return chunk_subTrees;
}


// SENTENCE etiketa irakurri eta tratatzen du.
wstring procSENTENCE (xmlTextReaderPtr reader)
{
  wstring tree;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  // Irteeran sarrerako etiketa berdina.
  if (tagName == L"SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    tree = L"<SENTENCE" + write_xml(allAttrib(reader)) + L">\n";
  }
  else
  {
    wcerr << L"ERROR (ST_inter): invalid document: found <" << tagName
          << L"> when <SENTENCE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  while (ret == 1 and tagName == L"CHUNK")
  {
    //SENTENCE barruan dauden CHUNK etiketak irakurri eta tratatzen ditu.
    wstring input;
    vector<wstring> child_attributes;
    vector<vector<string> > chunk_cases;

    vector<wstring> subTree = procCHUNK(reader, input, child_attributes);

    for (size_t i = 0; i < child_attributes.size(); i++)
    {
      if (subTree[i] != L"") tree += L"<CHUNK" + write_xml(child_attributes[i]) + L">\n" + subTree[i];
    }

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  if (ret == 1 and tagName == L"SENTENCE" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    tree += L"</SENTENCE>\n";
  }
  else
  {
    wcerr << L"ERROR (ST_inter): invalid document: found <" << tagName
          << L"> when </SENTENCE> was expected..." << endl;
    exit(-1);
  }

  return tree;
}


int main(int argc, char *argv[])
{
  config cfg(argc, argv);

  // Output in the locale's encoding
  locale::global(locale(""));

  if (cfg.Inter_Phase == 1)
  {
    init_chunkMovement(cfg.Inter_Movements1File);
  }
  else if (cfg.Inter_Phase == 2)
  {
    init_chunkMovement(cfg.Inter_Movements2File);
  }
  else
  {
    init_chunkMovement(cfg.Inter_Movements3File);
  }
  while (true)
  {
    // redirect io
    Fd0WcoutRedirectHandler ioredirect(cfg);
    //libXml liburutegiko reader hasieratzen da, sarrera estandarreko fitxategia irakurtzeko.
    xmlTextReaderPtr reader;
    reader = xmlReaderForFd(0,"", NULL, 0);
    int ret = nextTag(reader);
    wstring tagName = getTagName(reader);
    int tagType = xmlTextReaderNodeType(reader);
  
    if (tagName == L"corpus" and tagType != XML_READER_TYPE_END_ELEMENT)
    {
      wcout << L"<?xml version='1.0' encoding='UTF-8' ?>" << endl;
      wcout << L"<corpus " << allAttrib(reader) << L">\n";
    }
    else
    {
      wcerr << L"ERROR (ST_inter): invalid document: found <" << tagName
            << L"> when <corpus> was expected..." << endl;
      exit(-1);
    }
  
    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  
    // corpus barruan dauden SENTENCE guztietarako
    while (ret == 1 and tagName == L"SENTENCE")
    {
      //SENTENCE irakurri eta prozesatzen du.
      wcout << procSENTENCE(reader) << endl;
      wcout.flush();
  
      ret = nextTag(reader);
      tagName = getTagName(reader);
      tagType = xmlTextReaderNodeType(reader);
    }
    xmlFreeTextReader(reader);
    xmlCleanupParser();
  
    if (ret == 1 and tagName == L"corpus" and tagType == XML_READER_TYPE_END_ELEMENT)
    {
      wcout << L"</corpus>\n";
    }
    else
    {
      wcerr << L"ERROR (ST_inter): invalid document: found <" << tagName
            << L"> when </corpus> was expected..." << endl;
      exit(-1);
    }
    if (!ioredirect.serverOK())
      break;

  }
}

