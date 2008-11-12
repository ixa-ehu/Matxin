#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "config.h"

#include <XML_reader.h>
#include <data_manager.h>

using namespace std;


string lem(string nodo) {
  unsigned int lemma_end = nodo.find('<');
  if (lemma_end == string::npos) return nodo;

  return nodo.substr(0,lemma_end);
}

string pos(string nodo) {
  unsigned int pos_begin, pos_end;
  pos_begin = nodo.find('<');
  pos_end = nodo.find('>');

  if (pos_begin == string::npos or pos_end == string::npos) return nodo;

  return "[" + nodo.substr(pos_begin+1, pos_end-pos_begin-1) + "]";
}

string mi(string nodo) {
  unsigned int mi_begin = nodo.find('>');
  if (mi_begin == string::npos) return nodo;

  return nodo.substr(mi_begin+1);
}

string translate_si(string sub, string obj, string zobj) {
  string si;

  si = "[sub";
  if (sub == "[NUMS]") si += "3s]";
  else if (sub == "[NUMP]") si += "3p]";
  else if (sub == "[NI]") si += "1s]";
  else if (sub == "[GU]") si += "1p]";
  else if (sub == "[ZU]") si += "2s]";
  else if (sub == "[ZUEK]") si += "2p]";
  else if (sub == "[HURA]") si += "3s]";
  else if (sub == "[HAIEK]") si += "3p]";
  else si += "00]";

  si += "[obj";
  if (obj == "[NUMS]") si += "3s]";
  else if (obj == "[NUMP]") si += "3p]";
  else if (obj == "[NI]") si += "1s]";
  else if (obj == "[GU]") si += "1p]";
  else if (obj == "[ZU]") si += "2s]";
  else if (obj == "[ZUEK]") si += "2p]";
  else if (obj == "[HURA]") si += "3s]";
  else if (obj == "[HAIEK]") si += "3p]";
  else si += "00]";

  si += "[dat";
  if (zobj == "[NUMS]") si += "3s]";
  else if (zobj == "[NUMP]") si += "3p]";
  else if (zobj == "[NI]") si += "1s]";
  else if (zobj == "[GU]") si += "1p]";
  else if (zobj == "[ZU]") si += "2s]";
  else if (zobj == "[ZUEK]") si += "2p]";
  else if (zobj == "[HURA]") si += "3s]";
  else if (zobj == "[HAIEK]") si += "3p]";
  else si += "00]";

  return si;
}

string get_AS_source(vector<string> patterns, string sub, string obj, string zobj, string trans, string pos_eus) {
  string AS_source = "#";
  string si, lemma_eus;
  unsigned int lemma_start, lemma_end;
  si = translate_si(sub, obj, zobj);

  if (trans == "") trans = "DU";

  for (int i=0; i < patterns.size(); i++) {
    lemma_start = patterns[i].find("_");
    if (lemma_start != string::npos) {
      lemma_end = patterns[i].rfind("_");
      lemma_eus = patterns[i].substr(lemma_start+1, lemma_end-lemma_start-1);
    }

    if (AS_source == "#") {
      AS_source += patterns[i];
    }
    else {
      AS_source += "+" + patterns[i];
    }
  }

  AS_source += "&" + si + "&[" + trans + "]&" + lemma_eus + "$";
  //AS_source += "&" + si + "&" + pos_eus + "&[" + trans + "]&" + lemma_eus + "$";
  return AS_source;
}

string writeNODE_AS(string ref, string alloc, string &length, string rel, string AS_target, string synonyms) {
  unsigned int separator1, separator2;
  int len = 1;
  
  unsigned int pos_separator = AS_target.rfind("/");
  unsigned int pos_erl = AS_target.rfind("+n[ERL][MEN]");

  if (rel != "" && pos_erl != string::npos && (pos_separator == string::npos || pos_separator < pos_erl)) {
    AS_target = AS_target.substr(0, pos_erl);
  }

  if (rel[0] == '+')
    AS_target += rel;


  separator2 = AS_target.find("/");
  if (separator2 == string::npos) separator2 = AS_target.size();

  string nodo = AS_target.substr(0,separator2);

  string nodes = "<NODE ref='" + write_xml(ref) + "' alloc='" + write_xml(alloc) + "'";
  nodes += " lem='" + write_xml(lem(nodo)) + "' pos='" + write_xml(pos(nodo)) + "' mi='" + write_xml(mi(nodo)) + "'";
  nodes += ">\n" + synonyms;

  separator1 = separator2;
  separator2 = AS_target.find("/", separator1+1);
  if (separator2 == string::npos) separator2 = AS_target.size();

  while (separator1 != AS_target.size()) { //AS_target-en nodo gehiago dagoen bitartean
    len++;
    nodo = AS_target.substr(separator1+1, separator2 - separator1 -1);

    nodes += "<NODE ref='" + write_xml(ref) + "' alloc='" + write_xml(alloc) + "'";
    nodes += " lem='" + write_xml(lem(nodo)) + "' pos='" + write_xml(pos(nodo)) + "' mi='" + write_xml(mi(nodo)) + "'";
    nodes += "/>\n";

    separator1 = separator2;
    separator2 = AS_target.find("/", separator1+1);
    if (separator2 == string::npos) separator2 = AS_target.size();
  }

  nodes += "</NODE>\n";
  ostringstream dummy;
  dummy << len;
  length = dummy.str();
  return nodes;
}

string procSYN (xmlTextReaderPtr reader) {
  string syns;

  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  
  if (tagName == "SYN" and tagType == XML_READER_TYPE_ELEMENT) {

    syns += "<SYN" + write_xml(allAttrib(reader));
    if (xmlTextReaderIsEmptyElement(reader) == 1) {
      syns += "/>\n";
      return syns;
    }
    else {
      syns += ">\n";   
    }
  }
  else {
    cerr << "ERROR: invalid tag: <"<< tagName << "> when <SYN> was expected..." << endl;
    exit(-1);
  }

  if (tagName == "SYN" and tagType == XML_READER_TYPE_END_ELEMENT) {
    syns += "</SYN>\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </SYN> was expected..." << endl;
    exit(-1);
  }

  return syns;
}

string procNODE_AS(xmlTextReaderPtr reader, string &ref, string &alloc, vector<string> &patterns, string &synonyms){
  string pattern, pos;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if (tagName == "NODE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    for (int i=patterns.size(); i <= atoi(attrib(reader, "ref").c_str()); i++)
      patterns.push_back("");

    pattern = attrib(reader, "lem") + "[" + attrib(reader, "mi") + "]";
    patterns[atoi(attrib(reader, "ref").c_str())] = pattern;
    pos = attrib(reader, "pos");

    if (ref == "") ref = attrib(reader, "ref");
    else ref += "," + attrib(reader, "ref");

    if (alloc == "") alloc = attrib(reader, "alloc");
    else alloc += "," + attrib(reader, "alloc");

    if (xmlTextReaderIsEmptyElement(reader) == 1) {
      return pos;
    }
  }
  else {
    cerr << "ERROR: invalid tag: <"<< tagName << "> when <NODE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);
  
  // if there are, process the posible synonyms 
  while (ret == 1 and tagName == "SYN" and tagType == XML_READER_TYPE_ELEMENT) {
    synonyms += procSYN(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  while (ret == 1 and tagName == "NODE" and tagType == XML_READER_TYPE_ELEMENT) {
    string subsyns;
    procNODE_AS(reader, ref, alloc, patterns, subsyns);

    int ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if (!(tagName == "NODE" and tagType == XML_READER_TYPE_END_ELEMENT)) {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return pos;
}

string procNODE_notAS(xmlTextReaderPtr reader){

  string nodes;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if (tagName == "NODE" and tagType != XML_READER_TYPE_END_ELEMENT) {

    //Preposizioa bada tratamendu berezi bat egin beharko zaio (definitzeke oraindik)

    nodes += "<NODE" + write_xml(allAttrib(reader));
    if (xmlTextReaderIsEmptyElement(reader) == 1) {
      nodes += "/>\n";
      return nodes;
    }
    else {
      nodes += ">\n";   
    }
  }
  else {
    cerr << "ERROR: invalid tag: <"<< tagName << "> when <NODE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);
  
  // if there are, process the posible synonyms 
  while (ret == 1 and tagName == "SYN" and tagType == XML_READER_TYPE_ELEMENT) {
    nodes += procSYN(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  while (ret == 1 and tagName == "NODE" and tagType == XML_READER_TYPE_ELEMENT) {
    nodes += procNODE_notAS(reader);

    int ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if (tagName == "NODE" and tagType == XML_READER_TYPE_END_ELEMENT) {
    nodes += "</NODE>\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}

string procCHUNK(xmlTextReaderPtr reader) {
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  string tree, length, pattern, chunkType, chunkSUB, chunkOBJ, chunkZOBJ, chunkTrans, rel;

  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    length = attrib(reader, "length");
    chunkType = attrib(reader, "type");
    chunkSUB = attrib(reader, "subMi");
    chunkOBJ = attrib(reader, "objMi");
    chunkZOBJ = attrib(reader, "datMi");
    chunkTrans = attrib(reader, "trans");
    rel = attrib(reader, "rel");

    string attributes = allAttrib_except(reader, "length");
    tree = "<CHUNK" + write_xml(text_allAttrib_except(attributes, "trans"));
  }
  else {
    cerr << "ERROR: invalid tag: <" << tagName << "> when <CHUNK> was expected..." << endl;
    exit(-1);
  }  

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  if (chunkType.substr(0,4) == "adi-") {
    vector<string> patterns;
    string ref, alloc, synonyms;
    int separator;
    string head_pos = procNODE_AS(reader, ref, alloc, patterns, synonyms);
    string AS_source = get_AS_source(patterns, chunkSUB, chunkOBJ, chunkZOBJ, chunkTrans, head_pos);
    //cerr << "AT: " << AS_source << endl;
    string AS_target = apply_verbTransference(AS_source);
    if ((separator = AS_target.find("&")) != string::npos) {
      chunkTrans = AS_target.substr(1, separator-2);
      AS_target = AS_target.substr(separator+1);
    } 
    //cerr << "AT: " << AS_target << endl;

    string subtree = writeNODE_AS(ref, alloc, length, rel, AS_target, synonyms);
    tree += " trans='" + write_xml(chunkTrans) + "' length='" + write_xml(length) + "'>\n" + subtree;
  }
  else {
    tree += " length='" + write_xml(length) + "'>\n";
    tree += procNODE_notAS(reader);
  }

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);
  while (ret == 1 and tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    tree += procCHUNK(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }


  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT) {
    tree += "</CHUNK>\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </CHUNK> was expected..." << endl;
    exit(-1);
  }

  return tree;
}

string procSENTENCE (xmlTextReaderPtr reader) {
  string tree;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if(tagName == "SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    tree = "<SENTENCE" + write_xml(allAttrib(reader)) + ">\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when <SENTENCE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  while (ret == 1 and tagName == "CHUNK") {
    tree += procCHUNK(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if(ret == 1 and tagName == "SENTENCE" and tagType == XML_READER_TYPE_END_ELEMENT) {
      tree += "</SENTENCE>\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </SENTENCE> was expected..." << endl;
    exit(-1);
  }

  return tree;
}

int main(int argc, char *argv[])
{
  config cfg(argv);

  init_verbTrasference(cfg.Verb_TransferFile, cfg.DoVerbTrace);

  xmlTextReaderPtr reader;
  reader = xmlReaderForFd(0,"", NULL, 0);

  int ret = nextTag(reader);
  string tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if(tagName == "corpus" and tagType != XML_READER_TYPE_END_ELEMENT) {
    cout << "<?xml version='1.0' encoding='iso-8859-1'?>" << endl;
    cout << "<corpus " << allAttrib(reader) << ">\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when <corpus> was expected..." << endl;
    exit(-1);
  }

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  int i=0;
  // corpus barruan dauden SENTENCE guztietarako 
  while (ret == 1 and tagName == "SENTENCE") {

    //SENTENCE irakurri eta prozesatzen du.
    string tree = procSENTENCE(reader);
    cout << tree << endl;
    cout.flush();

    if (cfg.DoTrace) {
      ostringstream log_fileName_osoa;
      ofstream log_file;

      log_fileName_osoa << cfg.Trace_File << i++ << ".xml";

      log_file.open(log_fileName_osoa.str().c_str(), ofstream::out | ofstream::app);
      log_file << tree;
      log_file.close();
    }

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if(ret == 1 and tagName == "corpus" and tagType == XML_READER_TYPE_END_ELEMENT) {
    cout << "</corpus>\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </corpus> was expected..." << endl;
    exit(-1);
  }
}
