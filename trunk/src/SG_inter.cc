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

#include "config.h"
#include "matxin_string_utils.h"
#include "IORedirectHandler.hpp"

#include <XML_reader.h>
#include <data_manager.h>

using namespace std;


int get_order(vector<wstring> order, wstring attributes)
{
  wstring ref = text_attrib(attributes, L"ref");

  for (size_t i = 0; i < order.size(); i++)
  {
    if (order[i] == ref)
      return i;
  }

  return order.size();
}


wstring write_CHUNK(vector<wstring> tree, vector<wstring> order)
{
  wostringstream XMLtree;
  for (size_t i = 0; i < tree.size(); i++)
  {
    if (tree[i] != L"")
    {
      int ord = get_order(order, tree[i]);
      XMLtree << L"<CHUNK ord='" << ord << L"'" << tree[i];
    }
    else
      XMLtree << L"</CHUNK>\n";
  }

  return XMLtree.str();
}


vector<wstring> merge(vector<wstring> order, int &head_index, int &head_length,
                      vector<wstring> child_order, wstring relative_order)
{
  if (relative_order == L"x1.x2")
  {
    for (size_t i = 0; i < child_order.size(); i++)
    {
      order.push_back(child_order[i]);
    }

    return order;
  }
  else if (relative_order == L"x2.x1.x2")
  {
    vector<wstring> output_order;
    int output_head;
    for (size_t i = 0; i < head_index; i++)
    {
      output_order.push_back(order[i]);
    }

    for (size_t i = 0; i < child_order.size() - 1; i++)
    {
      output_order.push_back(child_order[i]);
    }

    output_head = output_order.size();
    for (size_t i = head_index; i < head_index+head_length; i++) {
      output_order.push_back(order[i]);
    }
    output_order.push_back(child_order[child_order.size() - 1]);

    for (size_t i = head_index + head_length; i < order.size(); i++)
    {
      output_order.push_back(order[i]);
    }
    head_index = output_head;

    return output_order;
  }
  else if (relative_order == L"x2+x1")
  {
    vector<wstring> output_order;
    int output_head;
    for (size_t i = 0; i < head_index; i++)
    {
      output_order.push_back(order[i]);
    }

    output_head = output_order.size();

    for (size_t i = 0; i < child_order.size(); i++)
    {
      output_order.push_back(child_order[i]);
    }

    for (size_t i = head_index; i < order.size(); i++)
    {
      output_order.push_back(order[i]);
    }
    head_index = output_head;
    head_length = head_length + child_order.size();

    return output_order;
  }
  else if (relative_order == L"x1+x2")
  {
    vector<wstring> output_order;
    int output_head;
    for (size_t i = 0; i < head_index+head_length; i++)
    {
      output_order.push_back(order[i]);
    }

    for (size_t i = 0; i < child_order.size(); i++)
    {
      output_order.push_back(child_order[i]);
    }

    for (size_t i = head_index; i < order.size(); i++)
    {
      output_order.push_back(order[i]);
    }

    head_length = head_length + child_order.size();

    return output_order;
  }
  else
  {
    vector<wstring> output_order;
    int output_head;
    for (size_t i = 0; i < head_index; i++)
    {
      output_order.push_back(order[i]);
    }

    for (size_t i = 0; i < child_order.size(); i++)
    {
      output_order.push_back(child_order[i]);
    }

    output_order.push_back(order[head_index]);
    output_head = output_order.size() - 1;

    for (size_t i = head_index + 1; i < order.size(); i++)
    {
      output_order.push_back(order[i]);
    }
    head_index = output_head;

    return output_order;
  }
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


wstring procNODE(xmlTextReaderPtr reader)
{
  wstring nodes;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"NODE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    nodes += L"<NODE" + write_xml(allAttrib(reader));
    if (xmlTextReaderIsEmptyElement(reader) == 1)
    {
      nodes += L"/>\n";
      return nodes;
    }
    else
    {
      nodes += L">\n";
    }
  }
  else
  {
    wcout << nodes;
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

  while (ret == 1 and tagName == L"NODE" and tagType == XML_READER_TYPE_ELEMENT)
  {
    nodes += procNODE(reader);

    nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  if (tagName == L"NODE" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    nodes += L"</NODE>\n";
  }
  else
  {
    wcout << nodes;
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}


vector<wstring> procCHUNK(xmlTextReaderPtr reader, vector<wstring> &tree,
                          wstring &attribs, int &ref)
{
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);
  wstring subtree;
  vector<wstring> order;
  int head_index;
  int head_length;

  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    order.push_back(attrib(reader, "ref"));
    head_index = 0;
    head_length = 1;

    attribs = allAttrib(reader);
    ref = watoi(attrib(reader, "ref").c_str());

    subtree = write_xml(allAttrib(reader)) + L">\n";
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

  subtree += procNODE(reader);
  tree.push_back(subtree);

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);
  while (ret == 1 and tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    wstring child_attribs;
    int child_ref;
    vector<wstring> suborder = procCHUNK(reader, tree, child_attribs, child_ref);

    wstring relative_order = get_chunk_order(attribs, child_attribs, child_ref - ref);
    order = merge(order, head_index, head_length, suborder, relative_order);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    tree.push_back(L"");
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </CHUNK> was expected..." << endl;
    exit(-1);
  }

  return order;
}


wstring procSENTENCE (xmlTextReaderPtr reader)
{
  wstring tree;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    tree = L"<SENTENCE ord='" + write_xml(attrib(reader, "ref")) + L"'"
           + write_xml(allAttrib(reader)) + L">\n";
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

  vector<wstring> subtree;
  vector<wstring> order;

  while (ret == 1 and tagName == L"CHUNK")
  {
    wstring child_attribs;
    int child_ref;
    vector<wstring> child_subtree;
    vector<wstring> child_order = procCHUNK(reader, child_subtree, child_attribs, child_ref);

    subtree.insert(subtree.end(), child_subtree.begin(), child_subtree.end());
    order.insert(order.end(), child_order.begin(), child_order.end());

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  tree += write_CHUNK(subtree, order);

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

  init_chunk_order(cfg.Chunk_OrderFile);

  while (true)
  {
    // redirect io
    Fd0WcoutRedirectHandler ioredirect(cfg);
    xmlTextReaderPtr reader;
    reader = xmlReaderForFd(0,"", NULL, 0);
  
    int ret = nextTag(reader);
    wstring tagName = getTagName(reader);
    int tagType = xmlTextReaderNodeType(reader);
  
    if (tagName == L"corpus" and tagType != XML_READER_TYPE_END_ELEMENT)
    {
      wcout << L"<?xml version='1.0' encoding='UTF-8'?>" << endl;
      wcout << L"<corpus " << write_xml(allAttrib(reader)) << L">\n";
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
      wcout << "</corpus>\n";
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
