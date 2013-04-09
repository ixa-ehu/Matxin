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
#include <sstream>
#include <data_manager.h>

#include "config.h"
#include "IORedirectHandler.hpp"

#include <XML_reader.h>

using namespace std;


wstring writeNODE(vector<wstring> &subTree, wstring &order)
{
  wostringstream nodes;
  int counter = 0;
  for (size_t i = 0; i < subTree.size(); i++)
  {
    if (subTree[i] == L"")
      nodes << L"</NODE>\n";
    else
    {
      nodes << L"<NODE";

      size_t pos = order.find(L"-");
      if (pos == wstring::npos)
      {
        if (order == L"")
          nodes << L" ord='" << counter << L"'";
        else
          nodes << L" ord='" << order << L"'";
      }
      else
      {
        nodes << L" ord='" << order.substr(0, pos) << L"'";
        order = order.substr(pos + 1);
      }

      nodes << subTree[i];
      counter++;
    }
  }

  return nodes.str();
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


// NODE etiketa irakurri eta prozesatzen du
// IN:  head: momentuko NODOa chunk bura den edo ez.
// OUT: tree: XML etiketen bektore bat.
// - Kategoriak jasotzen ditu NODOen ordena definitu ahal izateko. Zenbait kasutan benetako kategoria erabili beharrean moldaketa bate egin behar da. Adb. (hau[DET][ERKARR]) -> ([DET][IZO])
// - NODEaren azpian dauden NODE guztietarako, NODEa irakurri eta prozesatzen du.
wstring procNODE(xmlTextReaderPtr reader, vector<wstring> &tree, bool head)
{
  wstring pattern, attributes, synonyms;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"NODE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    wstring lemma = attrib(reader, "lem");
    wstring pos = attrib(reader, "pos");
    lemma = lemma + pos;
    if (get_lexInfo(L"orderPos", lemma) != L"")
    {
      pos = get_lexInfo(L"orderPos", lemma).substr(get_lexInfo(L"orderPos", lemma).find(L"["),
                                                   get_lexInfo(L"orderPos", lemma).size());
    }
    if (head)
      pattern = L"([BURUA])";
    else
      pattern = L"(" + pos + L")";

    attributes = write_xml(allAttrib(reader));

    if (xmlTextReaderIsEmptyElement(reader) == 1)
    {
      //Elementu hutsa bada (<NODE .../>) NODE hutsa sortzen da eta NODE honetkin bukatu dugu.
      attributes += L"/>\n";
      tree.push_back(attributes);
      return pattern;
    }
    else
    {
      //Ez bada NODE hutsa hasiera etiketa ixten da.
      attributes += L">\n";
      //      tree.push_back(attributes);
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
    synonyms += procSYN(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  attributes += synonyms;
  tree.push_back(attributes);

  // NODEaren azpian dauden NODE guztietarako:
  while (ret == 1 and tagName == L"NODE" and tagType == XML_READER_TYPE_ELEMENT)
  {
    // NODEa irakurri eta prozesatzen du. NODE hori ez da CHUNKaren burua izango (head=false)
    pattern += procNODE(reader, tree, false);

    nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  //NODE bukaera etiketaren tratamendua.
  if (tagName == L"NODE" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    tree.push_back(L"");
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return pattern;
}


// CHUNK etiketa irakurri eta prozesatzen du:
// - Bere barruko NODOak tratatu ondoren NODOei dagokien ordena definitzen da, eta XML zuahitza idazten du.
// - CHUNK honen barruan dauden beste CHUNKak irakurri eta prozesatzen ditu.
wstring procCHUNK(xmlTextReaderPtr reader)
{
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);
  wstring tree, pattern, chunkType;

  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    tree = L"<CHUNK" + write_xml(allAttrib(reader)) + L">\n";
    chunkType = attrib(reader, "type");
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

  //CHUNK barruko NODOak tratzen dira.
  vector<wstring> subTree;
  pattern = procNODE(reader, subTree, true);
  // CHUNKen ordena jasotzen da.
  wstring order = get_node_order(chunkType, pattern);
  // XML zuhaitza sortzen da etiketen bektoreekin eta ordenarekin.
  tree += writeNODE(subTree,order);

  // CHUNK barruko CHUNKak tratatzen dira.
  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);
  while (ret == 1 and tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    tree += procCHUNK(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  // CHUNK bukaera etiketaren tratamendua.
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
// - SENTENCE barruan dauden CHUNKak irakurri eta prozesatzen ditu.
wstring procSENTENCE (xmlTextReaderPtr reader)
{
  wstring tree;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if(tagName == L"SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    tree += L"<SENTENCE" + write_xml(allAttrib(reader)) + L">\n";
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
    tree += procCHUNK(reader);

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

  //ordena definitu ahal izateko kategoria (DET-en azpikategoria) aldaketen biltegia hasieratu...
  init_lexInfo(L"orderPos", cfg.POS_ToOrderFile);
  init_node_order(cfg.Node_OrderFile);

  while (true)
  {
    // redirect io
    Fd0WcoutRedirectHandler ioredirect(cfg);
    xmlTextReaderPtr reader;
    reader = xmlReaderForFd(0, "", NULL, 0);
  
    int ret = nextTag(reader);
    wstring tagName = getTagName(reader);
    int tagType = xmlTextReaderNodeType(reader);
  
    if(tagName == L"corpus" and tagType != XML_READER_TYPE_END_ELEMENT)
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
  
    int i = 0;
    // corpus barruan dauden SENTENCE guztietarako
    while (ret == 1 and tagName == L"SENTENCE")
    {
      //SENTENCE irakurri eta prozesatzen du.
      wstring tree = procSENTENCE(reader);
      wcout << tree << endl;
      wcout.flush();
  
      if (cfg.DoTrace)
      {
        ostringstream log_fileName_osoa;
        wofstream log_file;
  
	log_fileName_osoa.imbue(std::locale("C"));
        log_fileName_osoa << cfg.Trace_File << i++ << ".xml";
  
        log_file.open(log_fileName_osoa.str().c_str(), ofstream::out | ofstream::app);
	log_file << L"<!-- Syntactic generation -->" << endl;
        log_file << tree;
        log_file.close();
      }
  
      ret = nextTag(reader);
      tagName = getTagName(reader);
      tagType=xmlTextReaderNodeType(reader);
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

