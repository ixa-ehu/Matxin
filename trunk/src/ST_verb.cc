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
#include <locale>

#include "config.h"
#include "matxin_string_utils.h"
#include "IORedirectHandler.hpp"

#include <XML_reader.h>
#include <data_manager.h>

using namespace std;

wstring lem(wstring nodo)
{
  size_t lemma_end = nodo.find(L'<');
  if (lemma_end == wstring::npos)
    return nodo;

  return nodo.substr(0, lemma_end);
}


wstring pos(wstring nodo)
{
  size_t pos_begin, pos_end;
  pos_begin = nodo.find(L'<');
  pos_end = nodo.find(L'>');

  if (pos_begin == wstring::npos or pos_end == wstring::npos)
    return nodo;

  return L"[" + nodo.substr(pos_begin + 1, pos_end - pos_begin - 1) + L"]";
}


wstring mi(wstring nodo)
{
  size_t mi_begin = nodo.find(L'>');
  if (mi_begin == wstring::npos)
    return nodo;

  return nodo.substr(mi_begin + 1);
}


wstring translate_si(wstring sub, wstring obj, wstring zobj)
{
  wstring si;

  si = L"[sub";
  if (sub == L"[NUMS]")
    si += L"3s]";
  else if (sub == L"[NUMP]")
    si += L"3p]";
  else if (sub == L"[NI]")
    si += L"1s]";
  else if (sub == L"[GU]")
    si += L"1p]";
  else if (sub == L"[ZU]")
    si += L"2s]";
  else if (sub == L"[ZUEK]")
    si += L"2p]";
  else if (sub == L"[HURA]")
    si += L"3s]";
  else if (sub == L"[HAIEK]")
    si += L"3p]";
  else
    si += L"00]";

  si += L"[obj";
  if (obj == L"[NUMS]")
    si += L"3s]";
  else if (obj == L"[NUMP]")
    si += L"3p]";
  else if (obj == L"[NI]")
    si += L"1s]";
  else if (obj == L"[GU]")
    si += L"1p]";
  else if (obj == L"[ZU]")
    si += L"2s]";
  else if (obj == L"[ZUEK]")
    si += L"2p]";
  else if (obj == L"[HURA]")
    si += L"3s]";
  else if (obj == L"[HAIEK]")
    si += L"3p]";
  else
    si += L"00]";

  si += L"[dat";
  if (zobj == L"[NUMS]")
    si += L"3s]";
  else if (zobj == L"[NUMP]")
    si += L"3p]";
  else if (zobj == L"[NI]")
    si += L"1s]";
  else if (zobj == L"[GU]")
    si += L"1p]";
  else if (zobj == L"[ZU]")
    si += L"2s]";
  else if (zobj == L"[ZUEK]")
    si += L"2p]";
  else if (zobj == L"[HURA]")
    si += L"3s]";
  else if (zobj == L"[HAIEK]")
    si += L"3p]";
  else
    si += L"00]";

  return si;
}


wstring get_AS_source(vector<wstring> patterns, wstring sub, wstring obj,
                      wstring zobj, wstring trans, wstring pos_eus)
{
  wstring AS_source = L"#";
  wstring si, lemma_eus;
  size_t lemma_start, lemma_end;
  si = translate_si(sub, obj, zobj);

  if (trans == L"")
    trans = L"DU";

  for (size_t i = 0; i < patterns.size(); i++)
  {
    lemma_start = patterns[i].find(L"_");
    if (lemma_start != string::npos)
    {
      lemma_end = patterns[i].rfind(L"_");
      lemma_eus = patterns[i].substr(lemma_start + 1, lemma_end - lemma_start - 1);
    }

    if (AS_source == L"#")
    {
      AS_source += patterns[i];
    }
    else
    {
      AS_source += L"+" + patterns[i];
    }
  }

  AS_source += L"&" + si + L"&[" + trans + L"]&" + lemma_eus + L"$";

  return AS_source;
}


wstring writeNODE_AS(wstring ref, wstring alloc, wstring &length, wstring rel,
                     wstring AS_target, wstring synonyms)
{
  size_t separator1, separator2;
  int len = 1;

  // size_t pos_separator = AS_target.rfind(L"/");
  // size_t pos_erl = AS_target.rfind(L"+n[ERL][MEN]");

  // if (rel != L"" && pos_erl != wstring::npos &&
  //     (pos_separator == wstring::npos || pos_separator < pos_erl))
  // {
  //   AS_target = AS_target.substr(0, pos_erl);
  // }

  // if (rel[0] == L'+' && AS_target.find(L"<") != wstring::npos)
  //   AS_target += rel;


  separator2 = AS_target.find(L"/");
  if (separator2 == wstring::npos)
    separator2 = AS_target.size();

  wstring nodo = AS_target.substr(0, separator2);

  wstring nodes;
  nodes = L"<NODE ref='" + write_xml(ref) + L"' alloc='" + write_xml(alloc) + L"'" +
          L" lem='" + write_xml(lem(nodo)) + L"' pos='" + write_xml(pos(nodo)) +
          L"' mi='" + write_xml(mi(nodo)) + L"'>\n" + synonyms;

  separator1 = separator2;
  separator2 = AS_target.find(L"/", separator1 + 1);
  if (separator2 == wstring::npos)
    separator2 = AS_target.size();

  //AS_target-en nodo gehiago dagoen bitartean
  while (separator1 != AS_target.size())
  {
    len++;
    nodo = AS_target.substr(separator1 + 1, separator2 - separator1 - 1);

    nodes += L"<NODE ref='" + write_xml(ref) + L"' alloc='" + write_xml(alloc) + L"'" +
             L" lem='" + write_xml(lem(nodo)) + L"' pos='" + write_xml(pos(nodo)) +
             L"' mi='" + write_xml(mi(nodo)) + L"'/>\n";

    separator1 = separator2;
    separator2 = AS_target.find(L"/", separator1 + 1);
    if (separator2 == wstring::npos)
      separator2 = AS_target.size();
  }

  nodes += L"</NODE>\n";
  wostringstream dummy;
  dummy << (len - 1);
  length = dummy.str();

  return nodes;
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


wstring procNODE_AS(xmlTextReaderPtr reader, wstring &ref, wstring &alloc,
                    vector<wstring> &patterns, wstring &synonyms)
{
  wstring pattern, pos;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"NODE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    for (int i = patterns.size(); i <= watoi(attrib(reader, "ref").c_str()); i++)
      patterns.push_back(L"");

    pattern = attrib(reader, "lem") + L"[" + attrib(reader, "mi") + L"]";
    patterns[watoi(attrib(reader, "ref").c_str())] = pattern;
    pos = attrib(reader, "pos");

    if (ref == L"")
      ref = attrib(reader, "ref");
    else
      ref += L"," + attrib(reader, "ref");

    if (alloc == L"")
      alloc = attrib(reader, "alloc");
    else
      alloc += L"," + attrib(reader, "alloc");

    if (xmlTextReaderIsEmptyElement(reader) == 1)
      return pos;
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

  while (ret == 1 and tagName == L"NODE" and tagType == XML_READER_TYPE_ELEMENT)
  {
    wstring subsyns;
    procNODE_AS(reader, ref, alloc, patterns, subsyns);

    nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  if (!(tagName == L"NODE" and tagType == XML_READER_TYPE_END_ELEMENT))
  {
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return pos;
}


wstring procNODE_notAS(xmlTextReaderPtr reader)
{
  wstring nodes;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"NODE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    //Preposizioa bada tratamendu berezi bat egin beharko zaio (definitzeke oraindik)
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
    nodes += procNODE_notAS(reader);

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
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}


wstring procCHUNK(xmlTextReaderPtr reader)
{
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);
  wstring tree, length, pattern, chunkType, chunkSUB, chunkOBJ, chunkZOBJ, chunkTrans, rel;

  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    length = attrib(reader, "length");
    chunkType = attrib(reader, "type");
    chunkSUB = attrib(reader, "subMi");
    chunkOBJ = attrib(reader, "objMi");
    chunkZOBJ = attrib(reader, "datMi");
    chunkTrans = attrib(reader, "trans");
    rel = attrib(reader, "rel");

    wstring attributes = allAttrib_except(reader, L"length");
    tree = L"<CHUNK" + write_xml(text_allAttrib_except(attributes, L"trans"));
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

  if (chunkType.substr(0, 4) == L"adi-")
  {
    vector<wstring> patterns;
    wstring ref, alloc, synonyms;
    int separator;
    wstring head_pos = procNODE_AS(reader, ref, alloc, patterns, synonyms);
    wstring AS_source = get_AS_source(patterns, chunkSUB, chunkOBJ,
                                      chunkZOBJ, chunkTrans, head_pos);
    wstring AS_target = apply_verbTransference(AS_source);
    //wcerr << AS_source << endl << AS_target << endl << endl;

    if ((separator = AS_target.find(L"&")) != wstring::npos)
    {
      chunkTrans = AS_target.substr(1, separator - 2);
      AS_target = AS_target.substr(separator + 1);
    }

    wstring subtree = writeNODE_AS(ref, alloc, length, rel, AS_target, synonyms);
    tree += L" trans='" + write_xml(chunkTrans) + L"' length='" + write_xml(length) + L"'>\n" + subtree;
  }
  else
  {
    tree += L" length='" + write_xml(length) + L"'>\n";
    tree += procNODE_notAS(reader);
  }

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


wstring procSENTENCE (xmlTextReaderPtr reader)
{
  wstring tree;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

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

  init_verbTrasference(cfg.Verb_TransferFile, cfg.DoVerbTrace);

  while (true)
  {
    // redirect io
    Fd0WcoutRedirectHandler ioredirect(cfg);
    // libXml liburutegiko reader hasieratzen da, sarrera estandarreko fitxategia irakurtzeko.
    xmlTextReaderPtr reader;
    reader = xmlReaderForFd(0, "", NULL, 0);
  
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
  
    int i = 0;
    // corpus barruan dauden SENTENCE guztietarako
    while (ret == 1 and tagName == L"SENTENCE")
    {
      // SENTENCE irakurri eta prozesatzen du.
      wstring tree = procSENTENCE(reader);
      wcout << tree << endl;
      wcout.flush();
  
      if (cfg.DoTrace)
      {
        ostringstream log_fileName_osoa;
        wofstream log_file;
  
	log_fileName_osoa.imbue(std::locale("C"));
        log_fileName_osoa << cfg.Trace_File << i++ << ".xml";
  
        log_file.open(log_fileName_osoa.str().c_str(), wofstream::out | wofstream::app);
	log_file << L"<!-- verb-transference -->" << endl;
        log_file << tree;
        log_file.close();
      }
  
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

