#include <string>
#include <iostream>
#include <sstream>

#include "config.h"

#include <data_manager.h>
#include <XML_reader.h>

using namespace std;


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

vector<string> procCHUNK(xmlTextReaderPtr reader, string &attributesFromParent, vector<string> &chunk_attributes) {
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

    //Aurreko atributuetaz gain CHUNK batetik bestera mugitzen direnak ere gehitzen dira.
    //Hemen gurasotik jasotako attributuak zuhaitzan gehitzen dira.
//cerr << attributesFromParent << ":" <<  my_attributes << ":down" << endl;
    vector<movement> parent_attributes = get_chunk_movements(attributesFromParent, my_attributes, "down");
    for (int i=0; i<parent_attributes.size();i++) {
      /*
cerr << attributesFromParent << endl;
cerr << my_attributes << endl;
cerr << parent_attributes[i].write_type << "//" << parent_attributes[i].to.attrib << "=" << text_attrib(attributesFromParent, parent_attributes[i].from.attrib) << endl << endl;
      */
      if (text_attrib(attributesFromParent, parent_attributes[i].from.attrib) != "") {
	if (text_attrib(my_attributes, parent_attributes[i].to.attrib) == "")
	  my_attributes += " " + parent_attributes[i].to.attrib + "='" + text_attrib(attributesFromParent, parent_attributes[i].from.attrib) + "'";
	else if (text_attrib(my_attributes, parent_attributes[i].to.attrib) != "" && parent_attributes[i].write_type == "overwrite") 
	  my_attributes = text_allAttrib_except(my_attributes, parent_attributes[i].to.attrib) + " " + parent_attributes[i].to.attrib + "='" + text_attrib(attributesFromParent, parent_attributes[i].from.attrib) + "'";
	else if (text_attrib(my_attributes, parent_attributes[i].to.attrib) != "" && parent_attributes[i].write_type == "concat") {
	  string attribute = " " + parent_attributes[i].to.attrib + "='" + text_attrib(my_attributes, parent_attributes[i].to.attrib) + "," + text_attrib(attributesFromParent, parent_attributes[i].from.attrib) + "'";
	  my_attributes = text_allAttrib_except(my_attributes, parent_attributes[i].to.attrib) + attribute;
	}
      }
    }
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

  while (ret == 1 and tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    //string attributesFrom;
    //Menpeko CHUNKak irakurri eta tratatzen dira. Gamatika baten arabera zenbait attributu mugitzen dira CHUNK batetik bestera.
    vector<string> child_attributes;
    vector<string> child_subTree = procCHUNK(reader, my_attributes, child_attributes);

    for (int i=0;i<child_attributes.size();i++) {
      chunk_subTrees.push_back(child_subTree[i]);
      chunk_attributes.push_back(child_attributes[i]);
    }

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  //Menpeko CHUNKetatik jaso behar diren elementuak jasotzen dira.
  for (int i=0; i < chunk_attributes.size(); i++) {
    vector<movement> parent_attributes = get_chunk_movements(chunk_attributes[i], my_attributes, "up");
    for (int j=0;j<parent_attributes.size();j++) {
      /*
cerr << chunk_attributes[i] << "//" << endl;
cerr << my_attributes << "//" << endl;
cerr << parent_attributes[j].write_type << "//" << parent_attributes[j].to.attrib << "=" << text_attrib(chunk_attributes[i], parent_attributes[j].from.attrib) << endl << endl;
      */
      if (text_attrib(chunk_attributes[i], parent_attributes[j].from.attrib) != "") {
	if (text_attrib(my_attributes, parent_attributes[j].to.attrib) == "")
	  my_attributes += " " + parent_attributes[j].to.attrib + "='" + text_attrib(chunk_attributes[i], parent_attributes[j].from.attrib) + "'";
	else if (text_attrib(my_attributes, parent_attributes[j].to.attrib) != "" && parent_attributes[j].write_type == "overwrite")
	  my_attributes = text_allAttrib_except(my_attributes, parent_attributes[j].to.attrib) + " " + parent_attributes[j].to.attrib + "='" + text_attrib(chunk_attributes[i], parent_attributes[j].from.attrib) + "'";
	else if (text_attrib(my_attributes, parent_attributes[j].to.attrib) != "" && parent_attributes[j].write_type == "concat") {
	  string attribute = " " + parent_attributes[j].to.attrib + "='" + text_attrib(chunk_attributes[i], parent_attributes[j].from.attrib) + "," + text_attrib(my_attributes, parent_attributes[j].to.attrib) + "'";
	  my_attributes = text_allAttrib_except(my_attributes, parent_attributes[j].to.attrib) + attribute;
	}
      }
    }
  }
  
  //CHUNK bukaera etiketa tratatzen da.
  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT) {

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

string procSENTENCE (xmlTextReaderPtr reader) {
  //SENTENCE etiketa irakurri eta tratatzen du.
  string tree;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  //Irteeran sarrerako etiketa berdina.
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
    //SENTENCE barruan dauden CHUNK etiketak irakurri eta tratatzen ditu.
    string input, output;
    vector<string> child_attributes;
    vector<vector<string> > chunk_cases;

    vector<string> subTree = procCHUNK(reader, input, child_attributes);    
    
    for (int i=0; i < child_attributes.size(); i++) {
      tree += "<CHUNK" + write_xml(child_attributes[i]) + ">\n" + subTree[i];
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

  if (cfg.Inter_Phase == 1) {
    init_chunkMovement(cfg.Inter_Movements1File);
  }
  else if (cfg.Inter_Phase == 2) {
    init_chunkMovement(cfg.Inter_Movements2File);
  }
  else {
    init_chunkMovement(cfg.Inter_Movements3File);
  }

  //libXml liburutegiko reader hasieratzen da, sarrera estandarreko fitxategia irakurtzeko.
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

  // corpus barruan dauden SENTENCE guztietarako 
  while (ret == 1 and tagName == "SENTENCE") {

    //SENTENCE irakurri eta prozesatzen du.
    cout << procSENTENCE(reader) << endl;
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

