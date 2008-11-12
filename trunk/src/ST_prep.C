#include <string>
#include <iostream>
#include <sstream>

#include "config.h"

#include <data_manager.h>
#include <XML_reader.h>

using namespace std;


void merge_cases(vector<string> &subj_cases, vector<string> &cases) {

  if (subj_cases.size() == 0) {
    subj_cases = cases;
  }
  else {
    for (int i=0; i<subj_cases.size(); i++) {
      bool inIntersection = false;

      for (int j=0; j<cases.size(); j++) {
	if (subj_cases[i] == cases[j]) inIntersection=true;
      }

      if (not inIntersection && subj_cases.size()>1) {
	subj_cases.erase(subj_cases.begin() + i);
	i--;
      }
    }
  }

}

string get_gen_case(string whole_case) {
  if (whole_case.find("/")) 
    return whole_case.substr(0, whole_case.find("/"));

  return whole_case;
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

string procNODE(xmlTextReaderPtr reader, int &length){
  //NODE etiketak irakurri eta tratatzen ditu.
  //IN-OUT: Tratatu diren NODE kopurua; CHUNKaren luzera.
  // - NODEaren azpian dauden NODEak irakurri eta prozesatzen ditu.
  string nodes;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if (tagName == "NODE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    //Sarrerako atributu berdinekin idazten da irteerako NODE elementua.
    nodes += "<NODE" + write_xml(allAttrib(reader));
    length++;
    if (xmlTextReaderIsEmptyElement(reader) == 1) {
      //NODE hutsa bada (<NODE .../>), NODE hutsa sortzen da eta momentuko NODEarekin bukatzen dugu.
      nodes += "/>\n";
      return nodes;
    }
    else {
      //NODE hutsa bada (<NODE .../>), NODE hutsa sortzen da eta momentuko NODEarekin bukatzen dugu.
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
    //Menpeko NODEak tratatzen dira...
    nodes += procNODE(reader, length);

    int ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  //NODE bukaera etiketa tratatzen da.
  if (tagName == "NODE" and tagType == XML_READER_TYPE_END_ELEMENT) {
    nodes += "</NODE>\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}

vector<string> procCHUNK(xmlTextReaderPtr reader, string &attributesFromParent, vector<string> &chunk_attributes, string sentenceref, int sentencealloc, config &cfg) {
  //CHUNK etiketa irakurri eta tratatzen du.
  // IN:  attributesFromParent gurasotik jasotzen diren attributuak.
  // OUT: attributesToParent: gurasoari pasatzen zaizkion attributuak.
  //      my_attributes: momentuko Chunkaren attributu guztiak.
  // - Gramatika baten arabera attributuak CHUNK batetik bestera mugitzen dira.
  // - NODOrik gabe gelditzen diren CHUNKak desagertzen dira.
  vector<string> chunk_subTrees;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  string chunk_type, my_attributes, attributesToChild, attributesFromChild, tree;
  int length=0;

  //Irteeran sarrerako etiketa berdinak.
  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    chunk_type = attrib(reader, "type");
    my_attributes = allAttrib(reader);

  }
  else {
    cerr << "ERROR: invalid tag: <" << tagName << "> when <CHUNK> was expected..." << endl;
    exit(-1);
  }  

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  //Menpeko NODEak irakurri eta tratatzen dira.
  if (ret == 1 and tagName == "NODE" and tagType == XML_READER_TYPE_ELEMENT) {
    tree += procNODE(reader, length);
    
    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  ostringstream length_attrib;
  length_attrib << " length='" << length << "'";
  my_attributes += length_attrib.str();

  while (ret == 1 and tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    //string attributesFrom;
    //Menpeko CHUNKak irakurri eta tratatzen dira. Gamatika baten arabera zenbait attributu mugitzen dira CHUNK batetik bestera.
    vector<string> child_attributes;
    vector<string> child_subTree = procCHUNK(reader, my_attributes, child_attributes, sentenceref, sentencealloc, cfg);

    for (int i=0;i<child_attributes.size();i++) {
      chunk_subTrees.push_back(child_subTree[i]);
      chunk_attributes.push_back(child_attributes[i]);
    }

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if (length > 0) {
    vector<vector<string> > chunk_cases;
    //Preposizioen itzulpena.
    for (int i=0; i < chunk_attributes.size(); i++) {
      vector<string> cases;

      if (text_attrib(chunk_attributes[i], "cas") != "") {
	cases.push_back(text_attrib(chunk_attributes[i], "cas"));
	chunk_attributes[i] = text_allAttrib_except(chunk_attributes[i], "cas");
      }
      else if (text_attrib(chunk_attributes[i], "rel") != "") {
	cases.push_back(text_attrib(chunk_attributes[i], "rel"));
      }
      else 
	cases  = preposition_transference(my_attributes, chunk_attributes[i], sentenceref, sentencealloc, cfg);

      chunk_cases.push_back(cases);
    }

    if (chunk_type.substr(0,4) == "adi-") {
      //Aditz sintagma bada subkategorizazioa. (aditzaren transitibitatea eta menpekoen kasuak)
      string verb_lemma = text_attrib(my_attributes, "headlem");
      if (verb_lemma[0]=='_' && verb_lemma[verb_lemma.size()-1]=='_')
	verb_lemma = verb_lemma.substr(1, verb_lemma.size()-2);
      

      if (cfg.UseTripletes) {
	//0. Ratnaparki
	for (int i=0; i < chunk_attributes.size(); i++) {
	  string chunk_head = text_attrib(chunk_attributes[i], "headlem");
	  vector<string> new_cases = verb_noum_subcategoritation(verb_lemma, chunk_head, chunk_cases[i], chunk_attributes[i], sentenceref, sentencealloc, cfg);
	  chunk_cases.erase(chunk_cases.begin()+i);
	  chunk_cases.insert(chunk_cases.begin()+i, new_cases);
	}
      }


      if (cfg.UseSubcat) {
	//1. subjektua bereiztu.
	vector<string> subj_cases;

	vector<int> subj_index;
	vector<string> subj_attributes;
	vector<string> subj_subTree;
	for (int i=0; i < chunk_attributes.size(); i++) {
	  if (text_attrib(chunk_attributes[i], "si") == "subj") {
	    merge_cases(subj_cases, chunk_cases[i]);
	    chunk_cases.erase(chunk_cases.begin()+i);
	    
	    subj_index.push_back(i + subj_index.size());
	    
	    subj_attributes.push_back(chunk_attributes[i]);
	    chunk_attributes.erase(chunk_attributes.begin()+i);

	    subj_subTree.push_back(chunk_subTrees[i]);
	    chunk_subTrees.erase(chunk_subTrees.begin()+i);
	    
	    i--;
	  }
	}

	//2. aditz horren agerpenak begiratu menpekoen casuak bereizteko.
	string subj_attrib; if (subj_attributes.size() > 0) subj_attrib = subj_attributes[0];
	string trans = verb_subcategoritation(verb_lemma, chunk_cases, chunk_attributes, subj_cases, subj_attrib, sentenceref, sentencealloc, cfg);
	my_attributes += " trans='" + trans + "'";
	
	//3. subjektu beste menpekoekin batera jarri.
	for (int i=0; i<subj_index.size(); i++) {
	  chunk_cases.insert(chunk_cases.begin() + subj_index[i], subj_cases);
	  chunk_attributes.insert(chunk_attributes.begin() + subj_index[i], subj_attributes[i]);
	  chunk_subTrees.insert(chunk_subTrees.begin() + subj_index[i], subj_subTree[i]);
	}
      }
    }

    // Kasuak idazten dira (azpikategorizazioan ebatzi bada, dagoen bakarra, bestela dagoen lehenengoa.)
    for (int i=0; i < chunk_attributes.size(); i++) {
      string old_cas = text_attrib(chunk_attributes[i], "cas");
      int alloc = atoi(text_attrib(chunk_attributes[i], "alloc").c_str()) - sentencealloc;
      string prep = text_attrib(chunk_attributes[i], "prep");
      if (prep == "") prep = "-";

      if (cfg.first_case || chunk_cases[i].size() == 1) {
	if (chunk_cases[i][0] != "" && chunk_cases[i][0] != "[ZERO]") {

	  if (chunk_cases[i].size() != 1 && cfg.DoPrepTrace) {
	    cerr << sentenceref << ":" << alloc << ":" << prep << " ANBIGUOA GELDITU DA (";
	    for(int j=0;j<chunk_cases[i].size();j++) {
	      cerr << chunk_cases[i][j] << " ";
	    }
	    cerr <<") HIZTEGIKO LEHENENGOA AUKERATUKO DA: " << chunk_cases[i][0] << endl << endl;
	  }

	  chunk_attributes[i] = text_allAttrib_except(chunk_attributes[i], "cas") + " cas='";
	  if (old_cas != "")
	    chunk_attributes[i] += old_cas + "," + get_gen_case(chunk_cases[i][0]);
	  else
	    chunk_attributes[i] += get_gen_case(chunk_cases[i][0]);
	  chunk_attributes[i] +="'";
	}
      }
    }
  }

  //CHUNK bukaera etiketa tratatzen da.
  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT) {

    // Kasuak idazten dira (azpikategorizazioan ebatzi bada, dagoen bakarra, bestela dagoen lehenengoa.)
    for (int i=0; i < chunk_attributes.size(); i++) {
      tree += "<CHUNK" + write_xml(chunk_attributes[i]) + ">\n" + chunk_subTrees[i];
    }

    tree += "</CHUNK>\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </CHUNK> was expected..." << endl;
    exit(-1);
  }

  if (length > 0) {
    chunk_attributes.clear();
    chunk_attributes.push_back(my_attributes);

    chunk_subTrees.clear();
    chunk_subTrees.push_back(tree);
  }

  return chunk_subTrees;
}

string procSENTENCE (xmlTextReaderPtr reader, config &cfg) {
  //SENTENCE etiketa irakurri eta tratatzen du.
  string tree, ref;
  int sentencealloc;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  //Irteeran sarrerako etiketa berdina.
  if(tagName == "SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    ref = attrib(reader, "ref");
    sentencealloc = atoi(attrib(reader, "alloc").c_str());
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
    //SENTENCE barruan dauden CHUNK etiketak irakurri eta tratatzen ditu.
    string input, output;
    vector<string> child_attributes;
    vector<vector<string> > chunk_cases;

    vector<string> subTree = procCHUNK(reader, input, child_attributes, ref, sentencealloc, cfg);    
    
    //Preposizioen itzulpena.
    for (int i=0; i < child_attributes.size(); i++) {
      vector<string> cases;

      if (text_attrib(child_attributes[i], "cas") != "") {
	cases.push_back(text_attrib(child_attributes[i], "cas"));
	child_attributes[i] = text_allAttrib_except(child_attributes[i], "cas");
      }
      else if (text_attrib(child_attributes[i], "rel") != "") {
	cases.push_back(text_attrib(child_attributes[i], "rel"));
      }
      else 
	cases  = preposition_transference("", child_attributes[i], ref, sentencealloc, cfg);

      chunk_cases.push_back(cases);
    }
    
    //Kasu bakarra badago hori aukeratzen da, bestela hutsik uzten da.
    for (int i=0; i < child_attributes.size(); i++) {
      int alloc = atoi(text_attrib(child_attributes[i], "alloc").c_str()) - sentencealloc;
      string prep = text_attrib(child_attributes[i], "prep");
      if (prep == "") prep = "-";

      if ((!cfg.first_case && chunk_cases[i].size() != 1) || chunk_cases[i][0] == "" || chunk_cases[i][0] == "[ZERO]")
	tree += "<CHUNK" + write_xml(child_attributes[i]) + ">\n" + subTree[i];
      else {

	  if (chunk_cases[i].size() != 1 && cfg.DoPrepTrace) {
	    cerr << ref << ":" << alloc << ":" << prep << " ANBIGUOA GELDITU DA (";
	    for(int j=0;j<chunk_cases[i].size();j++) {
	      cerr << chunk_cases[i][j] << " ";
	    }
	    cerr <<") HIZTEGIKO LEHENENGOA AUKERATUKO DA: " << chunk_cases[i][0] << endl << endl;
	  }

	string old_cas = text_attrib(child_attributes[i], "cas");
	tree += "<CHUNK" + write_xml(text_allAttrib_except(child_attributes[i], "cas")) + " cas='";
	if (old_cas != "")
	  tree += write_xml(old_cas) + "," + write_xml(get_gen_case(chunk_cases[i][0]));
	else
	  tree += write_xml(get_gen_case(chunk_cases[i][0]));
	tree += "'>\n" + subTree[i];
      }
    }
          
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

  init_preposition_transference(cfg.PrepositionsFile, cfg);
  if (cfg.UseSubcat) init_verb_subcategoritation(cfg.SubcatFile, cfg);
  if (cfg.UseTripletes) init_verb_noum_subcategoritation(cfg.Noum_SubcatFile, cfg);

  //libXml liburutegiko reader hasieratzen da, sarrera estandarreko fitxategia irakurtzeko.
  xmlTextReaderPtr reader;
  reader = xmlReaderForFd(0,"", NULL, 0);

  int ret = nextTag(reader);
  string tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if(tagName == "corpus" and tagType != XML_READER_TYPE_END_ELEMENT) {
    cout << "<?xml version='1.0' encoding='iso-8859-1'?>" << endl;
    cout << "<corpus " << write_xml(allAttrib(reader)) << ">\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when <corpus> was expected..." << endl;
    exit(-1);
  }

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  // corpus barruan dauden SENTENCE guztietarako 
  while (ret == 1 and tagName == "SENTENCE") {

    //SENTENCE irakurri eta prozesatzen du.
    cout << procSENTENCE(reader, cfg) << endl;
    cout.flush();

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

