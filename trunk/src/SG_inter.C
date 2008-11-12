#include <string>
#include <iostream>

#include "config.h"

#include <XML_reader.h>
#include <data_manager.h>

using namespace std;


int get_order(vector<string> order, string attributes){
  string ref = text_attrib(attributes, "ref");

  for (int i=0; i<order.size(); i++) {
    if (order[i] == ref)
      return i;
  }

  return order.size();
}

string write_CHUNK(vector<string> tree, vector<string> order) {
  ostringstream XMLtree;
  for (int i=0; i<tree.size(); i++) {
    if (tree[i] != "") {
      int ord = get_order(order, tree[i]);
      XMLtree << "<CHUNK ord='" << ord << "'" << tree[i];
    }
    else 
      XMLtree << "</CHUNK>\n";
  }

  return XMLtree.str();
}

vector<string> merge(vector<string> order, int &head_index, vector<string> child_order, string relative_order) {
  /*
for (int i=0; i<order.size();i++)
  cerr << order[i] << " ";
 cerr << "(" << head_index << ")" << endl;

for (int i=0; i<child_order.size();i++)
  cerr << child_order[i] << " ";
 cerr << endl << endl;
  */

  if (relative_order == "x1.x2") {
    for (int i=0; i<child_order.size();i++) {
      order.push_back(child_order[i]);
    }
    /*
for (int i=0; i<order.size();i++)
  cerr << order[i] << " ";
 cerr << "(" << head_index << ")" << endl << endl;
    */
    return order;
  }
  else if (relative_order == "x2.x1.x2") {
    vector<string> output_order;
    int output_head;
    for (int i=0; i<head_index;i++) {
      output_order.push_back(order[i]);
    }

    for (int i=0; i<child_order.size()-1;i++) {
      output_order.push_back(child_order[i]);
    }

    output_order.push_back(order[head_index]);
    output_head = output_order.size()-1;
    output_order.push_back(child_order[child_order.size()-1]);

    for (int i=head_index+1; i<order.size();i++) {
      output_order.push_back(order[i]);
    }
    head_index = output_head;
    /*
for (int i=0; i<output_order.size();i++)
  cerr << output_order[i] << " ";
 cerr << "(" << head_index << ")" << endl << endl;
    */
    return output_order;
  }
  else if (relative_order == "x2+x1") {
    vector<string> output_order;
    int output_head;
    for (int i=0; i<head_index;i++) {
      output_order.push_back(order[i]);
    }

    output_head = output_order.size();

    for (int i=0; i<child_order.size();i++) {
      output_order.push_back(child_order[i]);
    }

    for (int i=head_index; i<order.size();i++) {
      output_order.push_back(order[i]);
    }
    head_index = output_head;
    /*
for (int i=0; i<output_order.size();i++)
  cerr << output_order[i] << " ";
 cerr << "(" << head_index << ")" << endl << endl;
    */
    return output_order;
  }
//  else if (relative_order == "x2.x1") {
  else {
    vector<string> output_order;
    int output_head;
    for (int i=0; i<head_index;i++) {
      output_order.push_back(order[i]);
    }

    for (int i=0; i<child_order.size();i++) {
      output_order.push_back(child_order[i]);
    }

    output_order.push_back(order[head_index]);
    output_head = output_order.size()-1;

    for (int i=head_index+1; i<order.size();i++) {
      output_order.push_back(order[i]);
    }
    head_index = output_head;
    /*
for (int i=0; i<output_order.size();i++)
  cerr << output_order[i] << " ";
 cerr << endl << "(" << head_index << ")" << endl << endl;
    */
    return output_order;

  }
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

string procNODE(xmlTextReaderPtr reader){

  string nodes;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if (tagName == "NODE" and tagType != XML_READER_TYPE_END_ELEMENT) {
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
    cout << nodes;
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
    nodes += procNODE(reader);

    int ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if (tagName == "NODE" and tagType == XML_READER_TYPE_END_ELEMENT) {
    nodes += "</NODE>\n";
  }
  else {
    cout << nodes;
    cerr << "ERROR: invalid document: found <" << tagName << "> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}

vector<string> procCHUNK(xmlTextReaderPtr reader, vector<string> &tree, string &type, int &ref) {
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  string subtree;
  vector<string> order;
  int head_index;

  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    order.push_back(attrib(reader, "ref"));
    head_index=0;

    type = attrib(reader, "type");
    ref = atoi(attrib(reader, "ref").c_str());

    subtree = write_xml(allAttrib(reader)) + ">\n";
  }
  else {
    cerr << "ERROR: invalid tag: <" << tagName << "> when <CHUNK> was expected..." << endl;
    exit(-1);
  }  

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  subtree += procNODE(reader);
  tree.push_back(subtree);

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);
  while (ret == 1 and tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    string child_type;
    int child_ref;
    vector<string> suborder = procCHUNK(reader, tree, child_type, child_ref);

    string relative_order = get_chunk_order(type, child_type, child_ref - ref);
    //cerr << type << "," << child_type << "," << child_ref - ref << ":" << relative_order << endl;
    order= merge(order, head_index, suborder, relative_order);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT) {
    tree.push_back("");
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </CHUNK> was expected..." << endl;
    exit(-1);
  }

  return order;
}

string procSENTENCE (xmlTextReaderPtr reader) {
  string tree;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if(tagName == "SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    tree = "<SENTENCE ord='" + write_xml(attrib(reader, "ref")) + "'" + write_xml(allAttrib(reader)) + ">\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when <SENTENCE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  vector<string> subtree;
  vector<string> order;

  while (ret == 1 and tagName == "CHUNK") {
    string child_type;
    int child_ref;
    vector<string> child_subtree;
    vector<string> child_order = procCHUNK(reader, child_subtree, child_type, child_ref);

    subtree.insert(subtree.end(), child_subtree.begin(), child_subtree.end());
    order.insert(order.end(), child_order.begin(), child_order.end());

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  tree += write_CHUNK(subtree, order);

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

  init_chunk_order(cfg.Chunk_OrderFile);

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
