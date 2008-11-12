#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include <XML_reader.h>

using namespace std;

struct word {
  string form;
  string alloc;
  bool is_unknow;
};

struct format_tag {
  int alloc;
  int order;
  string tag;
};

bool operator<(const format_tag &a, const format_tag &b) {
  return a.order < b.order;
}

struct open_close_tag {
  bool opened;
  bool used;
  format_tag open;
  format_tag close;
};

struct format {
  vector<int> empty_tags;
  vector<format_tag> tags;
  vector<open_close_tag> open_close;
};

void load_empty_tag(xmlTextReaderPtr format_reader, format &file_format) {
  int offset = atoi(attrib(format_reader, "offset").c_str());
  file_format.empty_tags.push_back(offset);

  if (xmlTextReaderIsEmptyElement(format_reader) != 1) {
    int ret = xmlTextReaderRead(format_reader);
    string tagName = getTagName(format_reader);
    int tagType = xmlTextReaderNodeType(format_reader);
    
    while (ret == 1 && !(tagType == XML_READER_TYPE_END_ELEMENT && tagName == "empty-tag")) {
      // DTD definizioekin zer egin behar den definitu behar dugu.
      
      ret = xmlTextReaderRead(format_reader);
      tagName = getTagName(format_reader);
      tagType = xmlTextReaderNodeType(format_reader);
    }
  }
}

void load_format_tag(xmlTextReaderPtr format_reader, format &file_format) {
  format_tag tag;

  tag.alloc = atoi(attrib(format_reader, "offset").c_str());
  tag.order = atoi(attrib(format_reader, "order").c_str());
  tag.tag = "";

  if (xmlTextReaderIsEmptyElement(format_reader) != 1) {
    int ret = xmlTextReaderRead(format_reader);
    string tagName = getTagName(format_reader);
    int tagType = xmlTextReaderNodeType(format_reader);
    
    while (ret == 1 && !(tagType == XML_READER_TYPE_END_ELEMENT && tagName == "format-tag")) {
      // DTD definizioekin zer egin behar den definitu behar dugu.
      if (tagName== "#cdata-section") tag.tag += latin1(xmlTextReaderValue(format_reader));
      
      ret = xmlTextReaderRead(format_reader);
      tagName = getTagName(format_reader);
      tagType = xmlTextReaderNodeType(format_reader);
    }
  }

  file_format.tags.push_back(tag);
}

void load_open_close_tag(xmlTextReaderPtr format_reader, format &file_format) {
  open_close_tag tag;
  tag.opened = false;
  tag.used = false;

  int ret = xmlTextReaderRead(format_reader);
  string tagName = getTagName(format_reader);
  int tagType = xmlTextReaderNodeType(format_reader);
  
  while (ret == 1 && !(tagType == XML_READER_TYPE_ELEMENT && tagName == "open-tag")) {
    
    ret = xmlTextReaderRead(format_reader);
    tagName = getTagName(format_reader);
    tagType = xmlTextReaderNodeType(format_reader);
  }

  tag.open.alloc = atoi(attrib(format_reader, "offset").c_str());
  tag.open.order = atoi(attrib(format_reader, "order").c_str());
  tag.open.tag = latin1(xmlTextReaderValue(format_reader));

  if (xmlTextReaderIsEmptyElement(format_reader) != 1) {
    ret = xmlTextReaderRead(format_reader);
    tagName = getTagName(format_reader);
    tagType = xmlTextReaderNodeType(format_reader);
    
    while (ret == 1 && !(tagType == XML_READER_TYPE_END_ELEMENT && tagName == "open-tag")) {
      if (tagName== "#cdata-section") tag.open.tag += latin1(xmlTextReaderValue(format_reader));
      
      ret = xmlTextReaderRead(format_reader);
      tagName = getTagName(format_reader);
      tagType = xmlTextReaderNodeType(format_reader);
    }
  }

  ret = xmlTextReaderRead(format_reader);
  tagName = getTagName(format_reader);
  tagType = xmlTextReaderNodeType(format_reader);
  
  while (ret == 1 && !(tagType == XML_READER_TYPE_ELEMENT && tagName == "close-tag")) {
    ret = xmlTextReaderRead(format_reader);
    tagName = getTagName(format_reader);
    tagType = xmlTextReaderNodeType(format_reader);
  }

  tag.close.alloc = atoi(attrib(format_reader, "offset").c_str());
  tag.close.order = atoi(attrib(format_reader, "order").c_str());
  tag.close.tag = latin1(xmlTextReaderValue(format_reader));

  if (xmlTextReaderIsEmptyElement(format_reader) != 1) {
    ret = xmlTextReaderRead(format_reader);
    tagName = getTagName(format_reader);
    tagType = xmlTextReaderNodeType(format_reader);
    
    while (ret == 1 && !(tagType == XML_READER_TYPE_END_ELEMENT && tagName == "close-tag")) {
      if (tagName== "#cdata-section") tag.close.tag += latin1(xmlTextReaderValue(format_reader));
      
      ret = xmlTextReaderRead(format_reader);
      tagName = getTagName(format_reader);
      tagType = xmlTextReaderNodeType(format_reader);
    }
  }
  
  ret = xmlTextReaderRead(format_reader);
  tagName = getTagName(format_reader);
  tagType = xmlTextReaderNodeType(format_reader);
    
  while (ret == 1 && !(tagType == XML_READER_TYPE_END_ELEMENT && tagName == "open-close-tag")) {
    ret = xmlTextReaderRead(format_reader);
    tagName = getTagName(format_reader);
    tagType = xmlTextReaderNodeType(format_reader);
  }

  file_format.open_close.push_back(tag);
}

format load_format(xmlTextReaderPtr format_reader){
  format file_format;

  int ret = xmlTextReaderRead(format_reader);
  string tagName = getTagName(format_reader);
  int tagType = xmlTextReaderNodeType(format_reader);

  while (ret == 1 and (tagType == XML_READER_TYPE_DOCUMENT_TYPE or tagName == "#text")) {
    // DTD definizioekin zer egin behar den definitu behar dugu.

    ret = xmlTextReaderRead(format_reader);
    tagName = getTagName(format_reader);
    tagType = xmlTextReaderNodeType(format_reader);
  }

  if (!(ret ==1 && tagName == "format" && tagType == XML_READER_TYPE_ELEMENT)) {
    cerr << "ERROR: invalid document: found <" << tagName << "> when <format> was expected..." << endl;
    exit(-1);
  }

  ret = xmlTextReaderRead(format_reader);
  tagName = getTagName(format_reader);
  tagType = xmlTextReaderNodeType(format_reader);

  while (ret == 1 && tagName != "format") {
    if (tagName == "empty-tag" && tagType == XML_READER_TYPE_ELEMENT) load_empty_tag(format_reader, file_format);
    if (tagName == "format-tag" && tagType == XML_READER_TYPE_ELEMENT) load_format_tag(format_reader, file_format);
    if (tagName == "open-close-tag" && tagType == XML_READER_TYPE_ELEMENT) load_open_close_tag(format_reader, file_format);

    ret = xmlTextReaderRead(format_reader);
    tagName = getTagName(format_reader);
    tagType = xmlTextReaderNodeType(format_reader);
  }

  if (!(ret ==1 && tagName == "format" && tagType == XML_READER_TYPE_END_ELEMENT)) {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </format> was expected..." << endl;
    exit(-1);
  }

  return file_format;
}

int get_min_alloc(string alloc) {
  if (alloc.find(",") == string::npos) return atoi(alloc.c_str());

  int min_alloc = -1;
  while (alloc.find(",") != string::npos) {
    int current_alloc = atoi(alloc.substr(0, alloc.find(",")).c_str());
    alloc = alloc.substr(alloc.find(",")+1);

    if (current_alloc < min_alloc || min_alloc == -1)
      min_alloc = current_alloc;
  }
  return min_alloc;
}

int get_max_alloc(string alloc) {
  if (alloc.find(",") == string::npos) return atoi(alloc.c_str());

  int min_alloc = -1;
  while (alloc.size() != 0) {
    int current_alloc;
    if (alloc.find(",") != string::npos) {
      current_alloc = atoi(alloc.substr(0, alloc.find(",")).c_str());
      alloc = alloc.substr(alloc.find(",")+1);
    }
    else {
      current_alloc = atoi(alloc.c_str());
      alloc= "";
    }

    if (current_alloc > min_alloc)
      min_alloc = current_alloc;
  }
  return min_alloc;
}

void procSYN (xmlTextReaderPtr reader) {
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  
  if (tagName == "SYN" and tagType == XML_READER_TYPE_ELEMENT) {
    if (xmlTextReaderIsEmptyElement(reader) == 1) {
      return ;
    }
  }
  else {
    cerr << "ERROR: invalid tag: <"<< tagName << "> when <SYN> was expected..." << endl;
    exit(-1);
  }

  if (tagName != "SYN" or tagType != XML_READER_TYPE_END_ELEMENT) {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </SYN> was expected..." << endl;
    exit(-1);
  }
}

void procNODE(xmlTextReaderPtr reader, map<int, map<int,word> > &sentence, int chunkOrd, int &min_alloc, int &max_alloc){
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if (tagName == "NODE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    int alloc = get_min_alloc(attrib(reader, "alloc"));
    if (min_alloc == -1 || min_alloc > alloc) {
      min_alloc = alloc;
    }

    alloc = get_max_alloc(attrib(reader, "alloc"));
    if (max_alloc < alloc) {
      max_alloc = alloc;
    }

    sentence[chunkOrd][atoi(attrib(reader, "ord").c_str())].form = attrib(reader, "form");
    sentence[chunkOrd][atoi(attrib(reader, "ord").c_str())].alloc = attrib(reader, "alloc");
    if (attrib(reader, "unknown") != "")
      sentence[chunkOrd][atoi(attrib(reader, "ord").c_str())].is_unknow = true;
    else 
      sentence[chunkOrd][atoi(attrib(reader, "ord").c_str())].is_unknow = false;

    if (xmlTextReaderIsEmptyElement(reader) == 1) {
      return;
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
    procSYN(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  while (ret == 1 and tagName == "NODE" and tagType == XML_READER_TYPE_ELEMENT) {
    procNODE(reader, sentence, chunkOrd, min_alloc, max_alloc);

    int ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if (!(tagName == "NODE" and tagType == XML_READER_TYPE_END_ELEMENT)) {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </NODE> was expected..." << endl;
    exit(-1);
  }
}

void procCHUNK(xmlTextReaderPtr reader, map<int, map<int,word> > &sentence, int &min_alloc, int &max_alloc) {
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  int ord;

  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    ord = atoi(attrib(reader, "ord").c_str());
  }
  else {
    cerr << "ERROR: invalid tag: <" << tagName << "> when <CHUNK> was expected..." << endl;
    exit(-1);
  }  

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  procNODE(reader, sentence, ord, min_alloc, max_alloc);

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);
  while (ret == 1 and tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    procCHUNK(reader, sentence, min_alloc, max_alloc);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }


  if (!(tagName == "CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT)) {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </CHUNK> was expected..." << endl;
    exit(-1);
  }
}

void procSENTENCE (xmlTextReaderPtr reader, map<int, map<int,word> > &sentence, int &min_alloc, int &max_alloc) {
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if (!(tagName == "SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT)) {
    cerr << "ERROR: invalid document: found <" << tagName << "> when <SENTENCE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  while (ret == 1 and tagName == "CHUNK") {
    procCHUNK(reader, sentence, min_alloc, max_alloc);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if(!(ret == 1 and tagName == "SENTENCE" and tagType == XML_READER_TYPE_END_ELEMENT)) {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </SENTENCE> was expected..." << endl;
    exit(-1);
  }
}

bool empty_tag(format &file_format, vector<int> allocs){
  vector<int>::iterator iter;

  for (iter=file_format.empty_tags.begin(); iter < file_format.empty_tags.end(); iter++) {
    bool is_empty=true;

    for (int i=0; i<allocs.size(); i++) {
      is_empty = is_empty && *iter == allocs[i];
      if (*iter == allocs[i]) {
	iter = file_format.empty_tags.erase(iter);iter--;
      }
    }
    if (allocs.size()>0 && is_empty) return is_empty;
  }

  return false;
}

string get_tags(format &file_format, vector<int> allocs){
  vector<format_tag> output_tags;

  vector<format_tag>::iterator iter_format;
  for (iter_format=file_format.tags.begin(); iter_format < file_format.tags.end(); iter_format++) {
    for (int i=0; i< allocs.size(); i++) {
      if (iter_format->alloc == allocs[i]) {
	output_tags.push_back(*iter_format);
	iter_format = file_format.tags.erase(iter_format);iter_format--;
      }
    }
  }

  vector<open_close_tag>::iterator iter_open_close;
  for (iter_open_close=file_format.open_close.begin(); iter_open_close < file_format.open_close.end(); iter_open_close++) {
    bool is_inside=false;

    for (int i=0; i<allocs.size(); i++) {
      if (!iter_open_close->opened && iter_open_close->open.alloc <= allocs[i] && iter_open_close->close.alloc > allocs[i]) {
	iter_open_close->opened=true;
	iter_open_close->used=true;
	output_tags.push_back(iter_open_close->open);
      }

      is_inside = is_inside || (iter_open_close->open.alloc <= allocs[i] && iter_open_close->close.alloc > allocs[i]);
    }

    if (iter_open_close->opened && !is_inside) {
      output_tags.push_back(iter_open_close->close);
      iter_open_close->opened=false;
    }
  }

  sort(output_tags.begin(), output_tags.end());
  /*
  cerr << "ordenatu: begin(";
  for (int i=0; i<allocs.size(); i++)
    cerr << allocs[i] << " ";
  cerr << ")" << endl;
  */
  string output;
  for (int i=0; i<output_tags.size(); i++) {
    output += output_tags[i].tag;
    //cerr << "\t" << output_tags[i].alloc << "," << output_tags[i].order << endl;
  }
  //cerr << "ordenatu: end -->" << endl << endl;
  int pos;
  while ((pos = output.find("\\]\\]\\>")) != string::npos)
    output.replace(pos, 6, "]]>");

  return output;
}

string clear_tags(format &file_format, int alloc, bool document_end) {
  vector<format_tag> output_tags;
  
  vector<format_tag>::iterator iter_format;
  for (iter_format=file_format.tags.begin(); iter_format < file_format.tags.end(); iter_format++) {
    if (iter_format->alloc <= alloc || document_end) {
      output_tags.push_back(*iter_format);
      iter_format = file_format.tags.erase(iter_format);iter_format--;
    }
  }

  vector<open_close_tag>::iterator iter_open_close;
  for (iter_open_close=file_format.open_close.begin(); iter_open_close < file_format.open_close.end(); iter_open_close++) {
    //if (!iter_open_close->opened && (iter_open_close->open.alloc < alloc || document_end)) {
    if (!iter_open_close->used && !iter_open_close->opened && (iter_open_close->open.alloc <= alloc || document_end)) {
      iter_open_close->opened=true;
      iter_open_close->used =true;
      output_tags.push_back(iter_open_close->open);
    }

    if (iter_open_close->opened && (iter_open_close->close.alloc <=alloc || document_end)) {
      output_tags.push_back(iter_open_close->close);
      iter_open_close->opened=false;
    }

    if (iter_open_close->close.alloc < alloc) {
      iter_open_close = file_format.open_close.erase(iter_open_close);iter_open_close--;
    }
  }

  sort(output_tags.begin(), output_tags.end());
  //cerr << "<!-- ordenatu: begin(" << alloc << ")" << endl;
  string output;
  for (int i=0; i<output_tags.size(); i++) {
    output += output_tags[i].tag;
    //cerr << "\t" << output_tags[i].alloc << "," << output_tags[i].order << endl;
  }
  //cerr << "ordenatu: end -->" << endl << endl;
  int pos;
  while ((pos = output.find("\\]\\]\\>")) != string::npos)
    output.replace(pos, 6, "]]>");

  return output;
}

void usage(char *program) {
  cerr << "USAGE: " << program << " [<format-file>] [-u]" << endl;
  exit(-1);
}

int main(int argc, char *argv[])
{
  format file_format;
  bool mark_unknow=false;

  if (argc > 1 and strcmp(argv[1], "-u")!=0) {
    xmlTextReaderPtr format_reader;
    format_reader = xmlReaderForFile(argv[1], NULL, 0);  
    file_format = load_format(format_reader);

    if (argc > 2 and strcmp(argv[2], "-u")==0)
      mark_unknow = true;
  }
  else if (argc > 1 and strcmp(argv[1], "-u")==0)
    mark_unknow = true;

  map<int, map<int,word> > sentence;
  int min_alloc, max_alloc;
  bool upperCase = true;

  xmlTextReaderPtr reader;
  reader = xmlReaderForFd(0,"", NULL, 0);

  int ret = nextTag(reader);
  string tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if(tagName != "corpus" or tagType == XML_READER_TYPE_END_ELEMENT) {
    cerr << "ERROR: invalid document: found <" << tagName << "> when <corpus> was expected..." << endl;
    exit(-1);
  }

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  // corpus barruan dauden SENTENCE guztietarako 
  while (ret == 1 and tagName == "SENTENCE") {

    //SENTENCE irakurri eta prozesatzen du.
    min_alloc = max_alloc = -1;
    procSENTENCE(reader, sentence, min_alloc, max_alloc);
    
    if (argc > 1) 
      cout << clear_tags(file_format, min_alloc, false);

    map<int, map<int,word> >::iterator curSentence = sentence.begin();
    while (curSentence != sentence.end()) {
      int chunkOrd = (*curSentence).first;
      //vector<FSId> anaLista = link_lista[katAzpikat];
      map<int,word> chunk = (*curSentence).second;
      
      map<int,word>::iterator curChunk = chunk.begin();
      while (curChunk != chunk.end()) {
	int nodoOrd = (*curChunk).first;
	string form = (*curChunk).second.form;
	string alloc = (*curChunk).second.alloc;
	bool is_unknow = (*curChunk).second.is_unknow;

	vector<int> allocs;
	while (alloc.size() != 0) {
	  if (alloc.find(",") == string::npos) {
	    allocs.push_back(atoi(alloc.c_str()));
	    alloc = "";
	  }
	  else {
	    allocs.push_back(atoi(alloc.substr(0, alloc.find(",")).c_str()));
	    alloc = alloc.substr(alloc.find(",")+1);
	  }
	}	

	if (!empty_tag(file_format, allocs)) {

	  cout << get_tags(file_format, allocs);
	  if (upperCase) {
	    if (argc <= 1) cout << endl;
	    form[0] = toupper(form[0]);
	  }
	  else if (form != "." && form != "," && form != ":" && form != ";" && form != "?" && form != "!" && form != ")") 
	    cout << " ";

	  if (mark_unknow && is_unknow)
	    cout << "*";

	  cout << form;

	}
	if (form == "." || form == "?" || form == "!") upperCase=true;
	else upperCase=false;

	curChunk++;
      }

      curSentence++;
    }

    sentence.clear();

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  if (argc > 1)
    cout << clear_tags(file_format, max_alloc, true);
  else 
    cout << endl;

  if(ret != 1 or tagName != "corpus" or tagType != XML_READER_TYPE_END_ELEMENT) {
    cerr << "ERROR: invalid document: found <" << tagName << "> when </corpus> was expected..." << endl;
    exit(-1);
  }
}
