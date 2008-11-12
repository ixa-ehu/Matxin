#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <data_manager.h>

#include "config.h"

#include <XML_reader.h>

using namespace std;


string writeNODE(vector<string> &subTree, string &order) {
  ostringstream nodes;
  int counter=0;
  for (int i=0; i<subTree.size(); i++) {

    if (subTree[i] == "") nodes << "</NODE>\n";
    else {
      nodes << "<NODE";

      int pos = order.find("-");
      if (pos == string::npos)
	if (order == "") 
	  nodes << " ord='" << counter << "'";
	else 
	  nodes << " ord='" << order << "'";
      else {
	nodes << " ord='" << order.substr(0,pos) << "'";
	order = order.substr(pos+1);
      }

      nodes << subTree[i];
      counter++;
    }
  }
  return nodes.str();
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

string procNODE(xmlTextReaderPtr reader, vector<string> &tree, bool head){
// NODE etiketa irakurri eta prozesatzen du
// IN:  head: momentuko NODOa chunk bura den edo ez.
// OUT: tree: XML etiketen bektore bat.
// - Kategoriak jasotzen ditu NODOen ordena definitu ahal izateko. Zenbait kasutan benetako kategoria erabili beharrean moldaketa bate egin behar da. Adb. (hau[DET][ERKARR]) -> ([DET][IZO])
// - NODEaren azpian dauden NODE guztietarako, NODEa irakurri eta prozesatzen du.
  string pattern, attributes, synonyms;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if (tagName == "NODE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    string lemma = attrib(reader, "lem");
    string pos = attrib(reader, "pos");
    lemma = lemma + pos;
    if (get_lexInfo("orderPos", lemma) != "") {
      pos = get_lexInfo("orderPos", lemma).substr(get_lexInfo("orderPos", lemma).find("["), get_lexInfo("orderPos", lemma).size());
    }
    if (head) pattern = "([BURUA])";
    else pattern = "(" + pos + ")";

    attributes = write_xml(allAttrib(reader));

    if (xmlTextReaderIsEmptyElement(reader) == 1) {
      //Elementu hutsa bada (<NODE .../>) NODE hutsa sortzen da eta NODE honetkin bukatu dugu.
      attributes += "/>\n";
      tree.push_back(attributes);
      return pattern;
    }
    else {
      //Ez bada NODE hutsa hasiera etiketa ixten da.
      attributes += ">\n";   
      //      tree.push_back(attributes);
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

  attributes += synonyms;
  tree.push_back(attributes);

  // NODEaren azpian dauden NODE guztietarako:
  while (ret == 1 and tagName == "NODE" and tagType == XML_READER_TYPE_ELEMENT) {
    // NODEa irakurri eta prozesatzen du. NODE hori ez da CHUNKaren burua izango (head=false)
    pattern += procNODE(reader, tree, false);

    int ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  //NODE bukaera etiketaren tratamendua.
  if (tagName == "NODE" and tagType == XML_READER_TYPE_END_ELEMENT) {
    tree.push_back("");
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return pattern;
}

string procCHUNK(xmlTextReaderPtr reader) {
// CHUNK etiketa irakurri eta prozesatzen du:
// - Bere barruko NODOak tratatu ondoren NODOei dagokien ordena definitzen da, eta XML zuahitza idazten du.
// - CHUNK honen barruan dauden beste CHUNKak irakurri eta prozesatzen ditu.
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  string tree, pattern, chunkType;

  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    tree = "<CHUNK" + write_xml(allAttrib(reader)) + ">\n";
    chunkType = attrib(reader, "type");
  }
  else {
    cerr << "ERROR: invalid tag: <" << tagName << "> when <CHUNK> was expected..." << endl;
    exit(-1);
  }  

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  //CHUNK barruko NODOak tratzen dira.
  vector<string> subTree;
  pattern = procNODE(reader, subTree, true);
  // CHUNKen ordena jasotzen da.
  string order = get_node_order(chunkType, pattern);
  // XML zuhaitza sortzen da etiketen bektoreekin eta ordenarekin.
  tree += writeNODE(subTree,order);

  // CHUNK barruko CHUNKak tratatzen dira.
  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);
  while (ret == 1 and tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    tree += procCHUNK(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  // CHUNK bukaera etiketaren tratamendua.
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
// SENTENCE etiketa irakurri eta prozesatzen du:
// - SENTENCE barruan dauden CHUNKak irakurri eta prozesatzen ditu.
  string tree;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if(tagName == "SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    tree += "<SENTENCE" + write_xml(allAttrib(reader)) + ">\n";
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

  //ordena definitu ahal izateko kategoria (DET-en azpikategoria) aldaketen biltegia hasieratu...
  init_lexInfo("orderPos", cfg.POS_ToOrderFile);
  init_node_order(cfg.Node_OrderFile);

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

  int i = 0;
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
