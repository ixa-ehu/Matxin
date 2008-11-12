#include <string>
#include <vector>
#include <iostream>

#include "config.h"

#include <XML_reader.h>
#include <data_manager.h>

using namespace std;


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

string procNODE(xmlTextReaderPtr reader, string &attributesToChunk, string myNodeType) {
// NODE etiketa irakurri eta prozesatzen du, NODE hori AS motakoa ez den CHUNK baten barruan dagoela:
// OUT: attributesToChunk: Mugimenduen gramatikaren arabera CHUNKari pasa behar zaizkion attributuak.
// - Lema hutsa (lem=="") duten NODEak desagertzen dira eta besteek  sarrerako atributu berdinekin idazten da.
// - NODEaren azpian dauden NODE guztietarako, NODEa irakurri eta prozesatzen du, dagozkion atributuak jasoz

  string nodes, lem, myAttributes;
  
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  
  if (tagName == "NODE" and tagType == XML_READER_TYPE_ELEMENT) {
    lem = attrib(reader, "lem");
    myAttributes = allAttrib(reader);

    // Lema hutsa (lem=="") duten NODEak desagertzen dira.
    // eta besteak  sarrerako atributu berdinekin idazten da.
    if (lem != "") {
      nodes += "<NODE" + write_xml(allAttrib(reader));

      if (xmlTextReaderIsEmptyElement(reader) == 1) {
	nodes += "/>\n";
	return nodes;
      }
      else {
	nodes += ">\n";   
      }
    }


    //CHUNKari pasatu behar zaizkion atributuak jaso.
    vector<movement> attributes_to_move = get_node_movements_byfrom(myAttributes);
    for (int i=0; i < attributes_to_move.size(); i++) {
      if (text_attrib(myAttributes, attributes_to_move[i].from.attrib) != "") {
	if (text_attrib(attributesToChunk, attributes_to_move[i].to.attrib) == "") {
	  
	  attributesToChunk += " " + attributes_to_move[i].to.attrib + "='" + text_attrib(myAttributes, attributes_to_move[i].from.attrib) + "'";
	}
	else if (text_attrib(attributesToChunk, attributes_to_move[i].to.attrib) != "" and 
		 attributes_to_move[i].write_type == "overwrite") {
	  
	  attributesToChunk = text_allAttrib_except(attributesToChunk, attributes_to_move[i].to.attrib) + " " + attributes_to_move[i].to.attrib + "='" + text_attrib(myAttributes, attributes_to_move[i].from.attrib) + "'";
	}
	else if (text_attrib(attributesToChunk, attributes_to_move[i].to.attrib) != "" and
		 attributes_to_move[i].write_type == "concat") {
	  
	  string attribute = " " + attributes_to_move[i].to.attrib + "='" + text_attrib(attributesToChunk, attributes_to_move[i].to.attrib) + "," + text_attrib(myAttributes, attributes_to_move[i].from.attrib) + "'";
	  attributesToChunk = text_allAttrib_except(attributesToChunk, attributes_to_move[i].to.attrib) + attribute;
	}
      }
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


  // if there are, process the nodes depending of this node. 
  string NodeType = "Child";
  if ((myNodeType == "Head" || myNodeType == "NewHead") && lem == "")
    NodeType = "NewHead";

  // NODEaren azpian dauden NODE guztietarako
  while (tagName == "NODE" and tagType == XML_READER_TYPE_ELEMENT) {
    // NODEa irakurri eta prozesatzen du, dagozkion atributuak jasoz
    nodes += procNODE(reader, attributesToChunk, NodeType);
    
    if (NodeType != "NewHead" || nodes != "")
      NodeType = "Child";

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if (tagName == "NODE" and tagType == XML_READER_TYPE_END_ELEMENT) {
    //NODE bukaera etiketa tratatzen da.
    //Dagokion NODEaren lema hutsa ez bada bakarrik idazten da.
    if ((lem != "" && myNodeType == "Child") || (myNodeType == "Head" && nodes != "")) nodes += "</NODE>\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}

string procCHUNK(xmlTextReaderPtr reader) {
// CHUNK etiketa irakurri eta prozesatzen du:
//   - NODE etiketa tratatzen da, dagozkion atributuak jasoz.
//   - NODOetatikjasotzen diren atributuak idazten dira.

  string tree, attributes;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  // Atributu guztiak mantentzen dira
  if (tagName == "CHUNK" and tagType != XML_READER_TYPE_END_ELEMENT) {
    string my_attributes = allAttrib(reader);

    tree = "<CHUNK" + write_xml(allAttrib(reader));

    // Menpeko NODOak tratatzen dira.
    int ret = nextTag(reader);
    string attributesToChunk;
    string nodes = procNODE(reader, attributesToChunk, "Head"); 

    // NODOetatik goratutako attributuak idazten dira.
    vector<movement> attributes_to_move = get_node_movements_byto(my_attributes);
    for (int i=0; i<attributes_to_move.size();i++) {
      //cerr << attributesToChunk << "(" << attributes_to_move[i].to.attrib << ")";
      string attribute=text_whole_attrib(attributesToChunk, attributes_to_move[i].to.attrib);
      if (text_attrib(attributes, attributes_to_move[i].to.attrib) == ""  && attribute != "") {
	attributes += attribute;
	//cerr << "\t" << attribute << endl;
      }
      //else cerr << endl;
    }

    tree += write_xml(attributes) + ">\n" + nodes;
  }
  else {
    cerr << "ERROR: invalid tag: <"<< tagName << "> when <CHUNK> was expected..." << endl;
    exit(-1);
  }  

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  // CHUNK honen barruan dauden CHUNK guztietarako
  while (ret == 1 and tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    // CHUNK irakurri eta prozesatzen du.
    tree += procCHUNK(reader);
    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  //CHUNK bukaera etiketa tratatzen da.
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
// - Irteeran sarrerako etiketa berdina sortzen da.
// - SENTENCE barruan dauden CHUNKak irakurri eta prozesatzen ditu.

  string tree;
  string tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  // Irteeran sarrerako etiketa berdina sortzen da.
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
    // SENTENCE barruan dauden CHUNKak irakurri eta prozesatzen ditu.
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

  init_nodeMovement(cfg.Intra_MovementsFile);

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
