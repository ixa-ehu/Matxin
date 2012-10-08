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
#include <vector>
#include <iostream>
#include <locale>

#include "config.h"
#include "IORedirectHandler.hpp"


#include <XML_reader.h>
#include <data_manager.h>

using namespace std;


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
    wcerr << L"ERROR: invalid tag: <" << tagName
          << L"> when <SYN> was expected..." << endl;
    exit(-1);
  }

  if (tagName == L"SYN" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    syns += L"</SYN>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </SYN> was expected..." << endl;
    exit(-1);
  }

  return syns;
}


wstring procNODE(xmlTextReaderPtr reader, wstring chunkAttributes)
{
  wstring nodes, lem, myAttributes;

  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"NODE" and tagType == XML_READER_TYPE_ELEMENT)
  {
    myAttributes = allAttrib(reader);
    
    // CHUNKetik jaso behar diren atributuak hartu.
    vector<movement> attributes_to_move = get_node_movements_bypair(chunkAttributes, myAttributes, L"down");
    for (size_t i = 0; i < attributes_to_move.size(); i++)
    {
      if (text_attrib(chunkAttributes, attributes_to_move[i].from.attrib) != L"")
	{
	  if (text_attrib(myAttributes, attributes_to_move[i].to.attrib) == L"")
	  {
	    myAttributes += L" " + attributes_to_move[i].to.attrib + L"='" +
	                  text_attrib(chunkAttributes, attributes_to_move[i].from.attrib) +
                          L"'";
	  }
	  else if (attributes_to_move[i].write_type == L"overwrite")
	  {
	    myAttributes = text_allAttrib_except(myAttributes, attributes_to_move[i].to.attrib) +
	                 L" " + attributes_to_move[i].to.attrib + L"='" +
	                 text_attrib(chunkAttributes, attributes_to_move[i].from.attrib) + L"'";
	  }
	  else if (attributes_to_move[i].write_type == L"concat")
          {
	    wstring attribute = L" " + attributes_to_move[i].to.attrib + L"='" +
	                        text_attrib(chunkAttributes, attributes_to_move[i].from.attrib) +
                                L"," + text_attrib(myAttributes, attributes_to_move[i].to.attrib) + L"'";
	    myAttributes = text_allAttrib_except(myAttributes, attributes_to_move[i].to.attrib) + attribute;
	  }
	}
    }

    if (xmlTextReaderIsEmptyElement(reader) == 1)
    {
      nodes = L"<NODE" + write_xml(myAttributes) + L"/>\n";
      return nodes;
    }
    else
    {
      nodes = L"<NODE" + write_xml(myAttributes) + L">\n";
    }
  }

  else
  {
    wcerr << L"ERROR: invalid tag: <" << tagName
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


  // NODEaren azpian dauden NODE guztietarako
  while (tagName == L"NODE" and tagType == XML_READER_TYPE_ELEMENT)
  {
    // NODEa irakurri eta prozesatzen du, dagozkion atributuak jasoz
    nodes += procNODE(reader, chunkAttributes);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  if (tagName == L"NODE" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    //NODE bukaera etiketa tratatzen da.
    //Dagokion NODEaren lema hutsa ez bada bakarrik idazten da.
    nodes += L"</NODE>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}


// CHUNK etiketa irakurri eta prozesatzen du:
//   - NODE etiketa tratatzen da, dagozkion atributuak jasoz.
//   - NODOetatikjasotzen diren atributuak idazten dira.
wstring procCHUNK(xmlTextReaderPtr reader)
{
  wstring tree, attributes;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  // Atributu guztiak mantentzen dira
  if (tagName == L"CHUNK" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    attributes = allAttrib(reader);

    tree = L"<CHUNK" + write_xml(allAttrib(reader));

    // Menpeko NODOak tratatzen dira.
    nextTag(reader);
    wstring nodes = procNODE(reader, attributes);

    tree += L">\n" + nodes;
  }
  else
  {
    wcerr << L"ERROR: invalid tag: <" << tagName
          << L"> when <CHUNK> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  // CHUNK honen barruan dauden CHUNK guztietarako
  while (ret == 1 and tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    // CHUNK irakurri eta prozesatzen du.
    tree += procCHUNK(reader);
    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  //CHUNK bukaera etiketa tratatzen da.
  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    tree += L"</CHUNK>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </CHUNK> was expected..." << endl;
    exit(-1);
  }

  return tree;
}


// SENTENCE etiketa irakurri eta prozesatzen du:
// - Irteeran sarrerako etiketa berdina sortzen da.
// - SENTENCE barruan dauden CHUNKak irakurri eta prozesatzen ditu.
wstring procSENTENCE (xmlTextReaderPtr reader)
{
  wstring tree;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  // Irteeran sarrerako etiketa berdina sortzen da.
  if (tagName == L"SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    tree = L"<SENTENCE" + write_xml(allAttrib(reader)) + L">\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when <SENTENCE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  while (ret == 1 and tagName == L"CHUNK")
  {
    // SENTENCE barruan dauden CHUNKak irakurri eta prozesatzen ditu.
    tree += procCHUNK(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  if(ret == 1 and tagName == L"SENTENCE" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
      tree += L"</SENTENCE>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName
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

  init_nodeMovement(cfg.Generation_MovementsFile);

  while (true)
  {
    // redirect io
    Fd0WcoutRedirectHandler ioredirect(cfg);
    // libXml liburutegiko reader hasieratzen da, sarrera estandarreko fitxategia irakurtzeko.
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
      wcerr << L"ERROR: invalid document: found <" << tagName
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
      wcerr << L"ERROR: invalid document: found <" << tagName
            << L"> when </corpus> was expected..." << endl;
      exit(-1);
    }
    if (!ioredirect.serverOK())
      break;
  }
}

