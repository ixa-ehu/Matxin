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

#include <lttoolbox/fst_processor.h>
#include <lttoolbox/ltstr.h>

#include <string>
#include <iostream>
#include <sstream>
#include <locale>

#include "config.h"
#include "matxin_string_utils.h"

#include <data_manager.h>
#include <XML_reader.h>

using namespace std;

FSTProcessor fstp;


wstring upper_type(wstring form, wstring mi, wstring ord) {
  wstring case_type = L"none";
  size_t form_begin, form_end;
  int upper_case, lower_case;

  //cerr << form;

  form_begin = 0;
  if (form.find(L"_", form_begin+1) == wstring::npos)
    form_end = form.size();
  else
    form_end = form.find(L"_", form_begin+1);

  upper_case = lower_case = 0;
  while (form_begin != form.size()) {
    //cerr << ":" << form.substr(form_begin, form_end-form_begin) << "(" << form_begin << "," << form_end << ")[" << ord << "," << mi.substr(0,2) << "]";
    if (tolower(form[form_begin]) != form[form_begin] && (ord != L"1" || mi.substr(0,2) == L"NP")) {
      if (form_begin == 0)
        case_type = L"first";
      else
        case_type = L"title";
    }

    for (size_t i=form_begin; i<form_end; i++) {
      if (form[i] != tolower(form[i]))
        upper_case++;
      else
        lower_case++;
    }

    if (form.size() > form_end)
      form_begin = form_end + 1;
    else
      form_begin = form.size();

    if (form.find(L"_", form_begin+1) == wstring::npos)
      form_end = form.size();
    else
      form_end = form.find(L"_", form_begin+1);
  }
  if (upper_case > lower_case)
    case_type = L"all";

  //cerr << ":" << case_type << endl;

  return case_type;
}


wstring lema(wstring const &full) {
// Hiztegi elebidunaren euskarazko ordainetik lema lortzen du.
// IN:  Euskarazko ordain bat ( lema<key1>VALUE1<key2>VALUE2 )
// OUT: lema                  ( lema                         )

  wstring lemma = full.substr(0, full.find(L'<'));

  return lemma;
}

wstring get_dict_attributes(wstring const &full) {
// Hiztegi elebidunaren euskarazko ordainetik atributuak lortzen ditu.
// IN:  Euskarazko ordain bat                        ( lema<key1>VALUE1<key2>VALUE2...<keyn>VALUEn )
// OUT: ezaugarriak XML zuhaitzan txertatzeko moduan ( key1='VALUE1' key2='VALUE2'...keyn='VALUEn' )

  if (full.find(L'<') == wstring::npos)
    return L"";

  wstring output = L"lem='" + full + L"'";

  for(uint i = 0; i < output.size(); i++) {
    if (output[i] == L'<') {
      output.replace(i, 1, L"' ");
      i++;
    }
    else if (output[i] == L'>') {
      output.replace(i, 1, L"='");
      i++;
    }
  }

  if (output.substr(output.size()-3) == L"=''")
    output = output.substr(0, output.rfind(L" "));

  return output;
}

wstring getsyn(vector<wstring> translations) {
  wstring output;

  for (uint i=0; i< translations.size(); i++) {
    output += L"<SYN " + translations[i] + L"/>\n";
  }

  return output;
}

void order_ordainak(vector<wstring> &ordainak) {
  vector<wstring> ordered_ordain;
  int sense;
  bool zero_sense = false;

  vector<wstring>::iterator it;
  for (it = ordainak.begin(); it != ordainak.end(); it++)
  {
    sense = 0;
    size_t pos, pos2;
    if ((pos = (*it).find(L" sense='")) != wstring::npos)
    {
      if ((pos2 = (*it).find(L"'", pos+8)) != wstring::npos)
        sense = watoi((*it).substr(pos+8, pos2-pos-8).c_str());
    }

    if (sense == 0)
    {
      zero_sense = true;
      ordered_ordain.insert(ordered_ordain.begin(), *it);
    }
    else
    {
      if (!zero_sense)
        sense--;
      if (ordered_ordain.size() < sense+1)
        ordered_ordain.resize(sense+1);
      ordered_ordain[sense] = *it;
    }
  }

  ordainak = ordered_ordain;
}


vector<wstring> disanbiguate(wstring &full) {
// Hiztegi elebidunaren euskarazko ordainetik lehenengoa lortzen du.
// IN:  Euskarazko ordainak ( ordain1[/ordain2]* )
// OUT: lehenengoa          ( oradin1            )
  wstring output = full;
  vector<wstring> ordainak;

  for(uint i = 0; i < output.size(); i++) {
    if (output[i] == L'/') {
      ordainak.push_back(get_dict_attributes(output.substr(0, i)));

      output = output.substr(i+1);
      i=0;
    }

    if (output[i] == L'\\')
      output.erase(i, 1);
  }

  ordainak.push_back(get_dict_attributes(output));
  order_ordainak(ordainak);

  return ordainak;
}

vector<wstring> get_translation(wstring lem, wstring mi, bool fromAS, bool &unknown) {
  vector<wstring> translation;

  wstring input = L"^" + lem + L"<parol>" + mi + L"$";
  wstring trad = fstp.biltrans(input);
  trad = trad.substr(1, trad.size()-2);

  unknown = false;
  if (trad[0] == L'@' || trad.find(L">") < trad.find(L"<")) {
    input = L"^" + lem + L"<parol>noKAT$";
    //cerr << "NoKAT:" << input << endl;
    trad = fstp.biltrans(input);
    trad = trad.substr(1, trad.size()-2);
    //if (trad[0] == '@') trad.erase(0,1);
    //cerr << "NoKAT:" << trad << endl;

    if (trad[0] == L'@' || trad.find(L">") < trad.find(L"<")) {
      input = L"^@" + lem + L"<parol>" + mi + L"$";
      //cerr << "NoLex:" << input << endl;
      trad = fstp.biltrans(input);
      trad = trad.substr(3, trad.size()-4);
      if (trad[0] == L'@') trad.erase(0,1);
      //cerr << "NoLex:" << trad << endl;

      if (trad[0] == L'@' || trad.find(L">") < trad.find(L"<")) {
        trad = lem + L"<pos>" +  mi.substr(0,2);
      }
    }
    unknown = true;
  }

  if (fromAS) {
    trad = L"_" + trad;
    trad.replace(trad.find(L"<"), 1, L"_<");
  }

  translation = disanbiguate(trad);
  //cerr << translation[0] << endl << endl;

  return translation;
}

wstring multiNodes (xmlTextReaderPtr reader, wstring &full, wstring attributes) {
// Hiztegi elebidunaren euskarazko ordaina NODO bat baino gehiagoz osaturik badago.
// Adb. oso<ADB><ADOARR><+><MG><+>\eskas<ADB><ADJ><+><MG><+>.
// Azken NODOa ezik besteak tratatzen ditu
// IN:  Euskarazko ordain bat, NODO bat baino gehiago izan ditzake.
// OUT: Lehen NODOei dagokien XML zuhaitza.
  wstring output = L"";
  vector<wstring> tmp;

  tmp = split_multiattrib(full);

  for (size_t i = 1; i < tmp.size(); i++)
  {
    output += L"<NODE " + attributes;
    output += L" " + tmp[i] + L" />\n";
  }

  return output;
}


// NODE etiketa irakurri eta prozesatzen du, NODE hori AS motakoa ez den CHUNK baten barruan dagoela:
// - ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du (helmugak jatorrizkoaren erreferentzia izateko postedizioan)
// - (S) preposizioen eta (F) puntuazio ikurren kasuan ez da transferentzia egiten.
// - Beste kasuetan:
//    - Transferentzia lexikoa egiten da, lem, pos, mi, cas eta sub attributuak sortuz.
//    - Hitz horren semantika begiratzen da
// - NODEaren azpian dauden NODEak irakurri eta prozesatzen ditu.
wstring procNODE_notAS(xmlTextReaderPtr reader, bool head,
                       wstring parent_attribs, wstring& attributes, config &cfg)
{
  wstring nodes;
  wstring subnodes;
  wstring synonyms;
  wstring child_attributes;
  wstring tagName = getTagName(reader);
  vector<wstring> trad;
  vector<wstring> select;
  int tagType = xmlTextReaderNodeType(reader);
  bool head_child = false;
  bool unknown = false;

  // ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du
  // alloc atributua mantentzen da
  if (tagName == L"NODE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    nodes = L"<NODE ";
    attributes = L"ref='" + write_xml(attrib(reader, "ord")) + L"'" +
                 L" alloc='" + write_xml(attrib(reader, "alloc")) + L"'" +
                 L" slem='" + write_xml(attrib(reader, "lem")) + L"'" +
                 L" smi='" + write_xml(attrib(reader, "mi")) + L"'" +
                 L" UpCase='" + write_xml(upper_type(attrib(reader, "form"),
                                                     attrib(reader, "mi"),
                                                     attrib(reader, "ord"))) +
                 L"'";

    if (attrib(reader, "mi").substr(0,1) == L"W" or
        attrib(reader, "mi").substr(0,1) == L"Z")
    {
      // Daten (W) eta zenbakien (Z) kasuan ez da transferentzia egiten.
      // lem eta mi mantentzen dira
      attributes += L" lem='" + write_xml(attrib(reader, "lem")) + L"'" +
                    L" pos='[" + write_xml(attrib(reader, "mi")) + L"]'";
    }
    else
    {
      // Beste kasuetan:
      // Transferentzia lexikoa egiten da, lem, pos, mi, cas eta sub attributuak sortuz.
      trad = get_translation(attrib(reader, "lem"),
                             attrib(reader, "mi"), false, unknown);

      if (trad.size() > 1)
        select = lexical_selection(parent_attribs, attributes, trad, cfg);
      else
        select = trad;

      if (trad.size() > 1)
        synonyms = getsyn(trad);

      subnodes = multiNodes(reader, select[0], attributes);
      attributes += L" " + select[0];

      // Hitz horren semantika begiratzen da
      if (head && text_attrib(select[0], L"pos") == L"[IZE][ARR]" and
          get_lexInfo(L"noumSem", text_attrib(select[0], L"lem")) != L"" )
        attributes += L" sem='" + write_xml(get_lexInfo(L"noumSem",
                                                        text_attrib(select[0], L"lem"))) + L"'";

      if (unknown)
        attributes += L" unknown='transfer'";

      head_child = head && (text_attrib(select[0], L"lem") == L"");
    }

    if (xmlTextReaderIsEmptyElement(reader) == 1 and
        subnodes == L"" && synonyms == L"")
    {
      //NODE hutsa bada (<NODE .../>), NODE hutsa sortzen da eta momentuko NODEarekin bukatzen dugu.
      nodes += attributes + L"/>\n";
      return nodes;
    }
    else if (xmlTextReaderIsEmptyElement(reader) == 1)
    {
      //NODE hutsa bada (<NODE .../>), NODE hutsa sortzen da eta momentuko NODEarekin bukatzen dugu.
      nodes += attributes + L">\n" + synonyms + subnodes + L"</NODE>\n";
      return nodes;
    }
    else
    {
      //bestela NODE hasiera etiketaren bukaera idatzi eta etiketa barrukoa tratatzera pasatzen gara.
      nodes += attributes + L">\n" + synonyms + subnodes;
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

  wstring attribs = attributes;
  if (text_attrib(attributes, L"lem") == L"") attribs = parent_attribs;

  // NODEaren azpian dauden NODE guztietarako
  while (ret == 1 and tagName == L"NODE" and
         tagType == XML_READER_TYPE_ELEMENT)
  {
    // NODEa irakurri eta prozesatzen du.
    //nodes += procNODE_notAS(reader, head_child, attributes, cfg);

    wstring NODOA = procNODE_notAS(reader, head_child, attribs, child_attributes, cfg);
    nodes += NODOA;

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  if (text_attrib(attributes, L"lem") == L"") attributes = child_attributes;

  //NODE bukaera etiketa tratatzen da.
  if (tagName == L"NODE" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    nodes += L"</NODE>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName << L"> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}


wstring procNODE_AS(xmlTextReaderPtr reader, bool head, wstring& attributes) {
// NODE etiketa irakurri eta prozesatzen du, NODE hori AS motako CHUNK baten barruan dagoela:
// IN: head ( NODEa CHUNKaren burua den ala ez )
// - ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du (helmugak jatorrizkoaren erreferentzia izateko postedizioan)
// - CHUNKaren burua bada:
//    - Transferentzia lexikoa egiten da. (lem eta pos atributuen balio berriak sortuz)
// - Burua ez bada jatorrizko hizkuntzaren lem puntuen artean markatuko da (.lem.) eta mi atributua mantentzen da.
// NODEaren azpian dauden NODEak irakurri eta prozesatzen ditu. NODE horiek ez dira CHUNKaren burua izango (head=false)

  wstring nodes, synonyms, child_attributes;
  wstring tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  bool unknown =false;


  if (tagName == L"NODE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    // ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du
    // alloc atributua mantentzen da
    nodes = L"<NODE";
    attributes = L" ref='" + write_xml(attrib(reader, "ord")) + L"' alloc='" + write_xml(attrib(reader, "alloc")) + L"'";
    attributes += L" UpCase='" + write_xml(upper_type(attrib(reader, "form"), attrib(reader, "mi"), attrib(reader, "ord"))) + L"'";
    attributes += L" mi='" + attrib(reader, "mi") + L"'";

    // CHUNKaren burua bada:
    if (head) {
      // Transferentzia lexikoa egiten da. (lem eta pos atributuen balio berriak sortuz)
      vector<wstring> trad = get_translation(attrib(reader, "lem"), attrib(reader, "mi"), true, unknown);

      if (trad.size() > 1)
        synonyms = getsyn(trad);

      attributes += L" " + trad[0];

      if (unknown)
        attributes += L" unknown='transfer'";
    }
    else {
      // Burua ez bada jatorrizko hizkuntzaren lem puntuen artean markatuko da (.lem.) eta mi atributua mantentzen da.
      attributes += L" lem='." + write_xml(attrib(reader, "lem")) + L".' mi='" + write_xml(attrib(reader, "mi")) + L"'";
    }

    if (xmlTextReaderIsEmptyElement(reader) == 1 && synonyms == L"") {
      //Elementu hutsa bada (<NODE .../>) NODE hutsa sortzen da eta NODE honetkin bukatu dugu.
      nodes += attributes + L"/>\n";
      return nodes;
    }
    else {
      //Ez bada NODE hutsa hasiera etiketa ixten da.
      nodes += attributes + L">\n" + synonyms;
    }
  }
  else {
    wcerr << L"ERROR: invalid tag: <" << tagName << L"> when <NODE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  // NODEaren azpian dauden NODE guztietarako:
  while (ret == 1 and tagName == L"NODE" and tagType == XML_READER_TYPE_ELEMENT) {
    // NODEa irakurri eta prozesatzen du. NODE hori ez da CHUNKaren burua izango (head=false)
    nodes += procNODE_AS(reader, false, child_attributes);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  //NODE bukaera etiketaren tratamendua.
  if (tagName == L"NODE" and tagType == XML_READER_TYPE_END_ELEMENT) {
    nodes += L"</NODE>\n";
  }
  else {
    wcerr << L"ERROR: invalid document: found <" << tagName << L"> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}


wstring procCHUNK(xmlTextReaderPtr reader, wstring parent_attribs, config cfg) {
// CHUNK etiketa irakurri eta prozesatzen du:
// - ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du (helmugak jatorrizkoaren erreferentzia izateko postedizioan)
// - type : CHUNKaren type atributua itzultzen da
// - CHUNK motaren arabera tratamendu desberdina egiten da (procNODE_AS edo procNODE_notAS)
// - CHUNK honen barruan dauden beste CHUNKak irakurri eta prozesatzen ditu.

  wstring tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  wstring tree, chunkType, head_attribs;

  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    // ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du
    // type : CHUNKaren type atributua itzultzen da
    // si atributua mantentzen da
    chunkType = get_lexInfo(L"chunkType", attrib(reader, "type"));

    if (chunkType == L"") chunkType = attrib(reader, "type");

    tree = L"<CHUNK ref='" + write_xml(attrib(reader, "ord")) + L"' type='" + write_xml(chunkType) + L"'";
    tree += write_xml(text_allAttrib_except(allAttrib_except(reader, L"ord"), L"type")) + L">\n";
 }
  else {
    wcerr << L"ERROR: invalid tag: <" << tagName << L"> when <CHUNK> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  // CHUNK motaren arabera tratamendu desberdina egiten da (procNODE_AS edo procNODE_notAS)
  if (chunkType.substr(0,4) == L"adi-")
    // NODEa irakurri eta prozesatzen du,  CHUNKaren burua izango da (head=true)
    tree += procNODE_AS(reader, true, head_attribs);
  else
  {
    // NODEa irakurri eta prozesatzen du
    wstring NODOA = procNODE_notAS(reader, true, parent_attribs, head_attribs, cfg);
    tree += NODOA;
  }

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  // CHUNK honen barruan dauden CHUNK guztietarako
  while (ret == 1 and tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    // CHUNK irakurri eta prozesatzen du.
    tree += procCHUNK(reader, head_attribs, cfg);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT) {
    tree += L"</CHUNK>\n";
  }
  else {
    wcerr << L"ERROR: invalid document: found <" << tagName << L"> when </CHUNK> was expected..." << endl;
    exit(-1);
  }

  return tree;
}


wstring procSENTENCE (xmlTextReaderPtr reader, config &cfg) {
// SENTENCE etiketa irakurri eta prozesatzen du:
// - ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du (helmugak jatorrizkoaren erreferentzia izateko postedizioan)
// - SENTENCE barruan dauden CHUNKak irakurri eta prozesatzen ditu.

  wstring tree;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if(tagName == L"SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    // ord -> ref : ord atributuan dagoen balioa, ref atributuan gordetzen du
    tree = L"<SENTENCE ref='" + write_xml(attrib(reader, "ord")) + L"'"
                              + write_xml(allAttrib_except(reader, L"ord"))
                              + L">\n";
  }
  else {
    wcerr << L"ERROR: invalid document: found <" << tagName << L"> when <SENTENCE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  // SENTENCE barruan dauden CHUNK guztietarako
  while (ret == 1 and tagName == L"CHUNK") {
    // CHUNKa irakurri eta prozesatzen du.
    tree += procCHUNK(reader, L"", cfg);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if(ret == 1 and tagName == L"SENTENCE" and tagType == XML_READER_TYPE_END_ELEMENT) {
    tree += L"</SENTENCE>\n";
  }
  else {
    wcerr << L"ERROR: invalid document: found <" << tagName << L"> when </SENTENCE> was expected..." << endl;
    exit(-1);
  }

  return tree;
}

int main(int argc, char *argv[])
{
  config cfg(argv);

  // This sets the C++ locale and affects to C and C++ locales.
  // wcout.imbue doesn't have any effect but the in/out streams use the proper encoding.
  locale::global(locale(""));

  // Hiztegi elebidunaren hasieraketa. Parametro moduan jasotzen den fitxagia erabiltzen da hasieraketarako.
  FILE *transducer = fopen(cfg.DictionaryFile, "r");
  fstp.load(transducer);
  fclose(transducer);
  fstp.initBiltrans();

  // Hasieraketa hauek konfigurazio fitxategi batetik irakurri beharko lirateke.
  init_lexInfo(L"noumSem", cfg.Noum_SemanticFile);
  init_lexInfo(L"chunkType", cfg.ChunkType_DictFile);
  // Init lexical selection reading the rules file
  init_lexical_selection(cfg.LexSelFile);

  // libXml liburutegiko reader hasieratzen da, sarrera estandarreko fitxategia irakurtzeko.
  xmlTextReaderPtr reader;
  reader = xmlReaderForFd(0, "", NULL, 0);

  int ret = nextTag(reader);
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if(tagName == L"corpus" and tagType != XML_READER_TYPE_END_ELEMENT) {
    wcout << L"<?xml version='1.0' encoding='UTF-8'?>" << endl;
    wcout << L"<corpus " << write_xml(allAttrib(reader)) << ">" << endl;
  }
  else {
    wcerr << L"ERROR: invalid document: found <" << tagName << L"> when <corpus> was expected..." << endl;
    exit(-1);
  }

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  int i=0;
  // corpus barruan dauden SENTENCE guztietarako
  while (ret == 1 and tagName == L"SENTENCE") {

    //SENTENCE irakurri eta prozesatzen du.
    wstring tree = procSENTENCE(reader, cfg);
    wcout << tree << endl;
    wcout.flush();

    if (cfg.DoTrace) {
      ostringstream log_fileName_osoa;
      wofstream log_file;

      log_fileName_osoa << cfg.Trace_File << i++ << ".xml";

      log_file.open(log_fileName_osoa.str().c_str(), wofstream::out | wofstream::app);
      log_file << tree;
      log_file.close();
    }

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }
  xmlFreeTextReader(reader);
  xmlCleanupParser();

  if(ret == 1 and tagName == L"corpus" and tagType == XML_READER_TYPE_END_ELEMENT) {
    wcout << L"</corpus>\n";
  }
  else {
    wcerr << L"ERROR: invalid document: found <" << tagName << L"> when </corpus> was expected..." << endl;
    exit(-1);
  }

}

