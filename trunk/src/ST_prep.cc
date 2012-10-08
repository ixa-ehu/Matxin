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
#include "matxin_string_utils.h"
#include "IORedirectHandler.hpp"

#include <data_manager.h>
#include <XML_reader.h>

using namespace std;


void merge_cases(vector<wstring> &subj_cases, vector<wstring> &cases)
{

  if (subj_cases.size() == 0)
  {
    subj_cases = cases;
  }
  else
  {
    for (int i = 0; i < int(subj_cases.size()); i++)
    {
      bool inIntersection = false;

      for (size_t j = 0; j < cases.size(); j++)
      {
        if (subj_cases[i] == cases[j])
          inIntersection=true;
      }

      if (not inIntersection && subj_cases.size() > 1)
      {
        subj_cases.erase(subj_cases.begin() + i);
        i--;
      }
    }
  }

}


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
    //Menpeko NODEak tratatzen dira...
    nodes += procNODE(reader, length);

    nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  // NODE bukaera etiketa tratatzen da.
  if (tagName == L"NODE" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
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


//CHUNK etiketa irakurri eta tratatzen du.
// IN:  attributesFromParent gurasotik jasotzen diren attributuak.
// OUT: attributesToParent: gurasoari pasatzen zaizkion attributuak.
//      my_attributes: momentuko Chunkaren attributu guztiak.
// - Gramatika baten arabera attributuak CHUNK batetik bestera mugitzen dira.
// - NODOrik gabe gelditzen diren CHUNKak desagertzen dira.
vector<wstring> procCHUNK(xmlTextReaderPtr reader, wstring &attributesFromParent,
                          vector<wstring> &chunk_attributes, wstring sentenceref,
                          int sentencealloc, config &cfg)
{
  vector<wstring> chunk_subTrees;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);
  wstring chunk_type, my_attributes, attributesToChild, attributesFromChild, tree;
  int length = 0;

  // Irteeran sarrerako etiketa berdinak.
  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    chunk_type = attrib(reader, "type");
    my_attributes = allAttrib(reader);
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

  // Menpeko NODEak irakurri eta tratatzen dira.
  if (ret == 1 and tagName == L"NODE" and tagType == XML_READER_TYPE_ELEMENT)
  {
    tree += procNODE(reader, length);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  wostringstream length_attrib;
  length_attrib << L" length='" << (length-1) << L"'";
  my_attributes += length_attrib.str();

  while (ret == 1 and tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    // Menpeko CHUNKak irakurri eta tratatzen dira. Gamatika baten arabera
    // zenbait attributu mugitzen dira CHUNK batetik bestera.
    vector<wstring> child_attributes;
    vector<wstring> child_subTree = procCHUNK(reader, my_attributes, child_attributes, sentenceref, sentencealloc, cfg);

    for (size_t i = 0; i < child_attributes.size(); i++)
    {
      chunk_subTrees.push_back(child_subTree[i]);
      chunk_attributes.push_back(child_attributes[i]);
    }

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  if (length > 0)
  {
    vector<vector<wstring> > chunk_cases;
    // Preposizioen itzulpena.
    for (size_t i = 0; i < chunk_attributes.size(); i++)
    {
      vector<wstring> cases;

      if (text_attrib(chunk_attributes[i], L"cas") != L"")
      {
        cases.push_back(text_attrib(chunk_attributes[i], L"cas"));
        chunk_attributes[i] = text_allAttrib_except(chunk_attributes[i], L"cas");
      }
      // else if (text_attrib(chunk_attributes[i], L"rel") != L"")
      // {
      //   cases.push_back(text_attrib(chunk_attributes[i], L"rel"));
      // }
      else
        cases  = preposition_transference(my_attributes, chunk_attributes[i], sentenceref, sentencealloc, cfg);

      chunk_cases.push_back(cases);
    }

    if (chunk_type.substr(0, 4) == L"adi-")
    {
      // Aditz sintagma bada subkategorizazioa.
      // (aditzaren transitibitatea eta menpekoen kasuak)
      wstring verb_lemma = text_attrib(my_attributes, L"headlem");
      if (verb_lemma[0]=='_' && verb_lemma[verb_lemma.size() - 1]=='_')
        verb_lemma = verb_lemma.substr(1, verb_lemma.size() - 2);

      if (cfg.UseTripletes)
      {
        //0. Ratnaparki
        for (size_t i = 0; i < chunk_attributes.size(); i++)
        {
          wstring chunk_head = text_attrib(chunk_attributes[i], L"headlem");
          vector<wstring> new_cases =verb_noun_subcategorisation(verb_lemma, chunk_head, chunk_cases[i], chunk_attributes[i], sentenceref, sentencealloc, cfg);
          chunk_cases.erase(chunk_cases.begin() + i);
          chunk_cases.insert(chunk_cases.begin() + i, new_cases);
        }
      }

      if (cfg.UseSubcat)
      {
        //1. subjektua bereiztu.
        vector<wstring> subj_cases;

        vector<int> subj_index;
        vector<wstring> subj_attributes;
        vector<wstring> subj_subTree;
        for (int i = 0; i < int(chunk_attributes.size()); i++)
        {
          if (text_attrib(chunk_attributes[i], L"subject") == L"true")
          {
            merge_cases(subj_cases, chunk_cases[i]);
            chunk_cases.erase(chunk_cases.begin() + i);

            subj_index.push_back(i + subj_index.size());

            subj_attributes.push_back(chunk_attributes[i]);
            chunk_attributes.erase(chunk_attributes.begin() + i);

            subj_subTree.push_back(chunk_subTrees[i]);
            chunk_subTrees.erase(chunk_subTrees.begin() + i);

            i--;
          }
        }

        //2. aditz horren agerpenak begiratu menpekoen casuak bereizteko.
        wstring subj_attrib;
        if (subj_attributes.size() > 0)
          subj_attrib = subj_attributes[0];
        wstring trans = verb_subcategorisation(verb_lemma, chunk_cases, chunk_attributes, subj_cases, subj_attrib, sentenceref, sentencealloc, cfg);
        my_attributes += L" trans='" + trans + L"'";

        //3. subjektu beste menpekoekin batera jarri.
        for (size_t i = 0; i < subj_index.size(); i++)
        {
          chunk_cases.insert(chunk_cases.begin() + subj_index[i], subj_cases);
          chunk_attributes.insert(chunk_attributes.begin() + subj_index[i], subj_attributes[i]);
          chunk_subTrees.insert(chunk_subTrees.begin() + subj_index[i], subj_subTree[i]);
        }
      }

    }

    // Kasuak idazten dira (azpikategorizazioan ebatzi bada, dagoen bakarra, bestela dagoen lehenengoa.)
    for (size_t i = 0; i < chunk_attributes.size(); i++)
    {
      wstring old_cas = text_attrib(chunk_attributes[i], L"cas");
      int alloc = watoi(text_attrib(chunk_attributes[i], L"alloc").c_str()) - sentencealloc;
      wstring prep = text_attrib(chunk_attributes[i], L"prep");
      if (prep == L"")
        prep = L"-";

      if (cfg.first_case || chunk_cases[i].size() == 1)
      {
        if (chunk_cases[i][0] != L"" && chunk_cases[i][0] != L"[ZERO]")
        {
          if (chunk_cases[i].size() != 1 && cfg.DoPrepTrace)
          {
            wcerr << sentenceref << L":" << alloc << L":"
                  << prep << L" ANBIGUOA GELDITU DA (";
            for (size_t j = 0; j < chunk_cases[i].size(); j++)
            {
              wcerr << chunk_cases[i][j] << L" ";
            }
            wcerr << L") HIZTEGIKO LEHENENGOA AUKERATUKO DA: "
                  << chunk_cases[i][0] << endl << endl;
          }

          chunk_attributes[i] = text_allAttrib_except(chunk_attributes[i], L"cas") + L" cas='";
          if (old_cas != L"")
            chunk_attributes[i] += old_cas + L"," + get_gen_case(chunk_cases[i][0]);
          else
            chunk_attributes[i] += get_gen_case(chunk_cases[i][0]);
          chunk_attributes[i] += L"'";
        }
      }
    }

  }

  //CHUNK bukaera etiketa tratatzen da.
  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    // Kasuak idazten dira (azpikategorizazioan ebatzi bada, dagoen bakarra, bestela dagoen lehenengoa.)
    for (size_t i = 0; i < chunk_attributes.size(); i++)
    {
      tree += L"<CHUNK" + write_xml(chunk_attributes[i]) + L">\n" + chunk_subTrees[i];
    }
    tree += L"</CHUNK>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </CHUNK> was expected..." << endl;
    exit(-1);
  }

  if (length > 0)
  {
    chunk_attributes.clear();
    chunk_attributes.push_back(my_attributes);

    chunk_subTrees.clear();
    chunk_subTrees.push_back(tree);
  }

  return chunk_subTrees;
}


wstring procSENTENCE (xmlTextReaderPtr reader, config &cfg)
{
  //SENTENCE etiketa irakurri eta tratatzen du.
  wstring tree, ref;
  int sentencealloc;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  // Irteeran sarrerako etiketa berdina.
  if (tagName == L"SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    ref = attrib(reader, "ref");
    sentencealloc = watoi(attrib(reader, "alloc").c_str());
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
    //SENTENCE barruan dauden CHUNK etiketak irakurri eta tratatzen ditu.
    wstring input;
    vector<wstring> child_attributes;
    vector<vector<wstring> > chunk_cases;

    vector<wstring> subTree = procCHUNK(reader, input, child_attributes, ref, sentencealloc, cfg);

    //Preposizioen itzulpena.
    for (size_t i = 0; i < child_attributes.size(); i++)
    {
      vector<wstring> cases;

      if (text_attrib(child_attributes[i], L"cas") != L"")
      {
        cases.push_back(text_attrib(child_attributes[i], L"cas"));
        child_attributes[i] = text_allAttrib_except(child_attributes[i], L"cas");
      }
      // else if (text_attrib(child_attributes[i], L"rel") != L"")
      // {
      //   cases.push_back(text_attrib(child_attributes[i], L"rel"));
      // }
      else
        cases  = preposition_transference(L"", child_attributes[i], ref, sentencealloc, cfg);

      chunk_cases.push_back(cases);
    }

    // Kasu bakarra badago hori aukeratzen da, bestela hutsik uzten da.
    for (size_t i = 0; i < child_attributes.size(); i++)
    {
      int alloc = watoi(text_attrib(child_attributes[i], L"alloc").c_str()) - sentencealloc;
      wstring prep = text_attrib(child_attributes[i], L"prep");
      if (prep == L"")
        prep = L"-";

      if ((!cfg.first_case && chunk_cases[i].size() != 1) or 
          chunk_cases[i][0] == L"" || chunk_cases[i][0] == L"[ZERO]")
        tree += L"<CHUNK" + write_xml(child_attributes[i]) + L">\n" + subTree[i];
      else
      {
        if (chunk_cases[i].size() != 1 && cfg.DoPrepTrace)
        {
          wcerr << ref << L":" << alloc << L":"
                << prep << L" ANBIGUOA GELDITU DA (";
          for (size_t j = 0; j < chunk_cases[i].size(); j++)
          {
            wcerr << chunk_cases[i][j] << L" ";
          }
          wcerr << L") HIZTEGIKO LEHENENGOA AUKERATUKO DA: "
                << chunk_cases[i][0] << endl << endl;
        }

        wstring old_cas = text_attrib(child_attributes[i], L"cas");
        tree += L"<CHUNK" + write_xml(text_allAttrib_except(child_attributes[i], L"cas")) + L" cas='";
        if (old_cas != L"")
          tree += write_xml(old_cas) + L"," + write_xml(get_gen_case(chunk_cases[i][0]));
        else
          tree += write_xml(get_gen_case(chunk_cases[i][0]));
        tree += L"'>\n" + subTree[i];
      }
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

  init_preposition_transference(cfg.PrepositionsFile);
  if (cfg.UseSubcat)
    init_verb_subcategorisation(cfg.SubcatFile);
  if (cfg.UseTripletes)
    init_verb_noun_subcategorisation(cfg.Noun_SubcatFile);

  
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
      // SENTENCE irakurri eta prozesatzen du.
      wcout << procSENTENCE(reader, cfg) << endl;
      cout.flush();
  
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

