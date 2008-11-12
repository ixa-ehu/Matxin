#include <lttoolbox/FSTProcessor.H>
#include <lttoolbox/Ltstr.H>

#include <string>
#include <iostream>
#include <sstream>

#include "config.h"

#include <data_manager.h>
#include <XML_reader.h>

using namespace std;

FSTProcessor fstp;


string upper_type(string form, string mi, string ord) {
  string case_type = "none";
  int form_begin, form_end, upper_case, lower_case;

  //cerr << form;
  
  form_begin = 0;
  if (form.find("_", form_begin+1) == string::npos) form_end = form.size();
  else form_end = form.find("_", form_begin+1);

  upper_case = lower_case = 0;
  while (form_begin != form.size()) {
    //cerr << ":" << form.substr(form_begin, form_end-form_begin) << "(" << form_begin << "," << form_end << ")[" << ord << "," << mi.substr(0,2) << "]";
    if (tolower(form[form_begin]) != form[form_begin] && (ord != "1" || mi.substr(0,2) == "NP")) {
      if (form_begin == 0) case_type = "first";
      else case_type = "title";
    }

    for (int i=form_begin; i<form_end; i++) {
      if (form[i] != tolower(form[i])) upper_case++;
      else lower_case++;
    }

    if (form.size() > form_end) form_begin = form_end + 1;
    else form_begin = form.size();

    if (form.find("_", form_begin+1) == string::npos) form_end = form.size();
    else form_end = form.find("_", form_begin+1);
  }
  if (upper_case > lower_case) case_type="all";

  //cerr << ":" << case_type << endl;

  return case_type;
}


string lema(string const &full) {
// Hiztegi elebidunaren euskarazko ordainetik lema lortzen du.
// IN:  Euskarazko ordain bat ( lema<key1>VALUE1<key2>VALUE2 )
// OUT: lema                  ( lema                         )

  string lemma = full.substr(0, full.find('<'));

  return lemma;
}

string get_dict_attributes(string const &full) {
// Hiztegi elebidunaren euskarazko ordainetik atributuak lortzen ditu.
// IN:  Euskarazko ordain bat                        ( lema<key1>VALUE1<key2>VALUE2...<keyn>VALUEn )
// OUT: ezaugarriak XML zuhaitzan txertatzeko moduan ( key1='VALUE1' key2='VALUE2'...keyn='VALUEn' )

  if (full.find('<') == string::npos) return "";

  string output = full.substr(full.find('<')) + "'";
     
  for(uint i = 0; i < output.size(); i++) {
    if (output[i] == '<' && i == 0) {
      output.erase(i, 1);i--;}
    else if (output[i] == '<') {
      output.replace(i, 1, "' ");i++;}
    else if (output[i] == '>') {
      output.replace(i, 1, "='");i++;}
  }

  if (output.substr(output.size()-3) == "=''") output = output.substr(0, output.rfind(" "));

  return output;
}

string getsyn(vector<string> translations) {
  string output;

  for (uint i=0; i< translations.size(); i++) {
    output += "<SYN lem='" + lema(translations[i]) + "' " + get_dict_attributes(translations[i]) + "/>\n";
  }

  return output;
}

void order_ordainak(vector<string> &ordainak) {
  vector<string> ordered_ordain;
  int sense;
  bool zero_sense = false;
  ordered_ordain.insert(ordered_ordain.begin(), ordainak.size(), "");
  
  vector<string>::iterator it;
  for (it=ordainak.begin(); it!=ordainak.end(); it++) {
    sense = 0;
    unsigned int pos, pos2;
    if ((pos = (*it).find("<sense>")) != string::npos) {
      if ((pos2 = (*it).find("<", pos+1)) != string::npos) {
	sense = atoi((*it).substr(pos+7, pos2-pos-7).c_str());
	//(*it).erase(pos, pos2-pos);
      }
      else {
	sense = atoi((*it).substr(pos+7).c_str());
	//(*it).erase(pos);
      }
    }
    
    if (sense == 0) {
      zero_sense = true;
      ordered_ordain.insert(ordered_ordain.begin(), *it);
      ordered_ordain.pop_back();
    }
    else if (zero_sense)
      ordered_ordain[sense] = *it;
    else 
      ordered_ordain[sense-1] = *it;
    
  }
  
  ordainak = ordered_ordain;
}

vector<string> disanbiguate(string &full) {
// Hiztegi elebidunaren euskarazko ordainetik lehenengoa lortzen du.
// IN:  Euskarazko ordainak ( ordain1[/ordain2]* )
// OUT: lehenengoa          ( oradin1            )
  string output = full;
  vector<string> ordainak;

  for(uint i = 0; i < output.size(); i++) {
    if (output[i] == '/') {
      ordainak.push_back(output.substr(0,i));

      output = output.substr(i+1);
      i=0;
    }

    if (output[i] == '\\') 
      output.erase(i, 1);
  }

  ordainak.push_back(output);

  order_ordainak(ordainak);
  return ordainak;
}

vector<string> get_translation(string lem, string mi, bool &unknown){
  vector<string> translation;

  string input = "^" + lem + "<parol>" + mi + "$";
  //cerr << endl << input << endl;
  string trad = fstp.biltrans(input);
  //cerr << trad << endl;
  trad = trad.substr(1, trad.size()-2);        
  //cerr << trad << endl;
  
  unknown =false;
  if (trad[0] == '@' || trad.find(">") < trad.find("<")) {
    input = "^" + lem + "<parol>noKAT$";
    //cerr << "NoKAT:" << input << endl;
    trad = fstp.biltrans(input);
    trad = trad.substr(1, trad.size()-2);
    //if (trad[0] == '@') trad.erase(0,1);
    //cerr << "NoKAT:" << trad << endl;
    
    if (trad[0] == '@' || trad.find(">") < trad.find("<")) {
      input = "^@" + lem + "<parol>" + mi + "$";
      //cerr << "NoLex:" << input << endl;
      trad = fstp.biltrans(input);
      trad = trad.substr(3, trad.size()-4);
      if (trad[0] == '@') trad.erase(0,1);
      //cerr << "NoLex:" << trad << endl;
      
      if (trad[0] == '@' || trad.find(">") < trad.find("<")) {
	trad = lem + "<pos>" +  mi.substr(0,2); 
      }
    }
    unknown =true;
  }

  translation = disanbiguate(trad);
  //cerr << translation[0] << endl << endl;

  return translation;
}

string multiNodes (xmlTextReaderPtr reader, string &full) {
// Hiztegi elebidunaren euskarazko ordaina NODO bat baino gehiagoz osaturik badago.
// Adb. oso<ADB><ADOARR><+><MG><+>\eskas<ADB><ADJ><+><MG><+>. 
// Azken NODOa ezik besteak tratatzen ditu 
// IN:  Euskarazko ordain bat, NODO bat baino gehiago izan ditzake.
// OUT: Lehen NODOei dagokien XML zuhaitza.
  string output="";

  unsigned int aux = full.find("\\");
  while (aux != string::npos) {

    string trad = full.substr(0,aux);
    string dict_attributes = get_dict_attributes(trad);

    output += "<NODE ref='" + attrib(reader,"ord") + "' alloc='" + attrib(reader,"alloc") + "'";
    output += " lem='" + lema(trad) + "' " + dict_attributes;

    if (text_attrib(dict_attributes, "pos") == "[IZE][ARR]" && get_lexInfo("noumSem", lema(trad)) != "" ) 
      output += " sem='" + get_lexInfo("noumSem", lema(trad)) + "'";

    output += "/>\n";
      
    full.erase(0,aux + 1);
    aux = full.find("\\");
  }

  return output;
}

string procNODE_notAS(xmlTextReaderPtr reader, bool head) {
// NODE etiketa irakurri eta prozesatzen du, NODE hori AS motakoa ez den CHUNK baten barruan dagoela:
// - ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du (helmugak jatorrizkoaren erreferentzia izateko postedizioan)
// - (S) preposizioen eta (F) puntuazio ikurren kasuan ez da transferentzia egiten.
// - Beste kasuetan:
//    - Transferentzia lexikoa egiten da, lem, pos, mi, cas eta sub attributuak sortuz.
//    - Hitz horren semantika begiratzen da
// - NODEaren azpian dauden NODEak irakurri eta prozesatzen ditu.

  string nodes, subnodes, synonyms;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  bool head_child=false;
  bool unknown=false;

  // ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du 
  // alloc atributua mantentzen da
  if (tagName == "NODE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    nodes = "<NODE ref='" + write_xml(attrib(reader,"ord")) + "' alloc='" + write_xml(attrib(reader,"alloc")) + "'";
    nodes += " UpCase='" + write_xml(upper_type(attrib(reader, "form"), attrib(reader, "mi"), attrib(reader, "ord"))) + "'";
    
    if (attrib(reader,"mi").substr(0,1) == "W" || attrib(reader,"mi").substr(0,1) == "Z") {
      // Daten (W) eta zenbakien (Z) kasuan ez da transferentzia egiten.
      // lem eta mi mantentzen dira
      nodes+= " lem='" + write_xml(attrib(reader, "lem")) + "' pos='[" + write_xml(attrib(reader,"mi")) + "]'";
    }
    else {
      // Beste kasuetan:
      // Transferentzia lexikoa egiten da, lem, pos, mi, cas eta sub attributuak sortuz.
      vector<string> trad = get_translation(attrib(reader, "lem"), attrib(reader,"mi"), unknown);

      if (trad.size() > 1)
	synonyms = getsyn(trad);

      subnodes = multiNodes(reader, trad[0]);
      string dict_attributes = get_dict_attributes(trad[0]);
      nodes += write_xml(" lem='" + lema(trad[0]) + "' " + dict_attributes);

      // Hitz horren semantika begiratzen da
      if (head && text_attrib(dict_attributes, "pos") == "[IZE][ARR]" && get_lexInfo("noumSem", lema(trad[0])) != "" ) 
	nodes += " sem='" + write_xml(get_lexInfo("noumSem", lema(trad[0]))) + "'";

      if (unknown) 
	nodes += " unknown='transfer'";

      head_child = head && (lema(trad[0]) == "");
    }

    if (xmlTextReaderIsEmptyElement(reader) == 1 && subnodes == "" && synonyms == "") {
      //NODE hutsa bada (<NODE .../>), NODE hutsa sortzen da eta momentuko NODEarekin bukatzen dugu.
      nodes += "/>\n";
      return nodes;
    }
    else if (xmlTextReaderIsEmptyElement(reader) == 1) {
      //NODE hutsa bada (<NODE .../>), NODE hutsa sortzen da eta momentuko NODEarekin bukatzen dugu.
      nodes += ">\n" + synonyms + subnodes + "</NODE>\n";
      return nodes;
    }
    else {
      //bestela NODE hasiera etiketaren bukaera idatzi eta etiketa barrukoa tratatzera pasatzen gara.
      nodes += ">\n" + synonyms + subnodes;   
    }
  }
  else {
    cerr << "ERROR: invalid tag: <"<< tagName << "> when <NODE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);
  
  // NODEaren azpian dauden NODE guztietarako
  while (ret == 1 and tagName == "NODE" and tagType == XML_READER_TYPE_ELEMENT) {
    // NODEa irakurri eta prozesatzen du.
    nodes += procNODE_notAS(reader, head_child);

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

string procNODE_AS(xmlTextReaderPtr reader, bool head) {
// NODE etiketa irakurri eta prozesatzen du, NODE hori AS motako CHUNK baten barruan dagoela:
// IN: head ( NODEa CHUNKaren burua den ala ez )
// - ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du (helmugak jatorrizkoaren erreferentzia izateko postedizioan)
// - CHUNKaren burua bada:
//    - Transferentzia lexikoa egiten da. (lem eta pos atributuen balio berriak sortuz)
// - Burua ez bada jatorrizko hizkuntzaren lem puntuen artean markatuko da (.lem.) eta mi atributua mantentzen da.
// NODEaren azpian dauden NODEak irakurri eta prozesatzen ditu. NODE horiek ez dira CHUNKaren burua izango (head=false)

  string nodes, synonyms;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  bool unknown =false;


  if (tagName == "NODE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    // ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du 
    // alloc atributua mantentzen da
    nodes = "<NODE ref='" + write_xml(attrib(reader,"ord")) + "' alloc='" + write_xml(attrib(reader,"alloc")) + "'";
    nodes += " UpCase='" + write_xml(upper_type(attrib(reader, "form"), attrib(reader, "mi"), attrib(reader, "ord"))) + "'";

    // CHUNKaren burua bada:
    if (head) {
      // Transferentzia lexikoa egiten da. (lem eta pos atributuen balio berriak sortuz)
      vector<string> trad = get_translation(attrib(reader, "lem"), attrib(reader,"mi"), unknown);

      if (trad.size() > 1)
	synonyms = getsyn(trad);

      nodes += " lem='_" + write_xml(lema(trad[0])) + "_' mi='" + write_xml(attrib(reader,"mi")) + "' " + write_xml(text_allAttrib_except(get_dict_attributes(trad[0]), "mi"));
      if (unknown) nodes += " unknown='transfer'";
    }
    else {
      // Burua ez bada jatorrizko hizkuntzaren lem puntuen artean markatuko da (.lem.) eta mi atributua mantentzen da.
      nodes+= " lem='." + write_xml(attrib(reader, "lem")) + ".' mi='" + write_xml(attrib(reader,"mi")) + "'";   
    }

    if (xmlTextReaderIsEmptyElement(reader) == 1 && synonyms == "") {
      //Elementu hutsa bada (<NODE .../>) NODE hutsa sortzen da eta NODE honetkin bukatu dugu.
      nodes += "/>\n";
      return nodes;
    }
    else {
      //Ez bada NODE hutsa hasiera etiketa ixten da.
      nodes += ">\n" + synonyms;   
    }
  }
  else {
    cerr << "ERROR: invalid tag: <"<< tagName << "> when <NODE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);
  
  // NODEaren azpian dauden NODE guztietarako:
  while (ret == 1 and tagName == "NODE" and tagType == XML_READER_TYPE_ELEMENT) {
    // NODEa irakurri eta prozesatzen du. NODE hori ez da CHUNKaren burua izango (head=false)
    nodes += procNODE_AS(reader,false);

    int ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType=xmlTextReaderNodeType(reader);
  }

  //NODE bukaera etiketaren tratamendua.
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
// CHUNK etiketa irakurri eta prozesatzen du:
// - ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du (helmugak jatorrizkoaren erreferentzia izateko postedizioan)
// - type : CHUNKaren type atributua itzultzen da
// - CHUNK motaren arabera tratamendu desberdina egiten da (procNODE_AS edo procNODE_notAS)
// - CHUNK honen barruan dauden beste CHUNKak irakurri eta prozesatzen ditu.

  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  string tree, chunkType;

  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    // ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du 
    // type : CHUNKaren type atributua itzultzen da
    // si atributua mantentzen da
    chunkType = get_lexInfo("chunkType", attrib(reader,"type"));

    if (chunkType == "") chunkType = attrib(reader,"type");

    tree = "<CHUNK ref='" + write_xml(attrib(reader,"ord")) + "' type='" + write_xml(chunkType) + "'";
    tree += write_xml(text_allAttrib_except(allAttrib_except(reader, "ord"), "type")) + ">\n";
 }
  else {
    cerr << "ERROR: invalid tag: <" << tagName << "> when <CHUNK> was expected..." << endl;
    exit(-1);
  }  

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  // CHUNK motaren arabera tratamendu desberdina egiten da (procNODE_AS edo procNODE_notAS)
  if (chunkType.substr(0,4) == "adi-")
    // NODEa irakurri eta prozesatzen du,  CHUNKaren burua izango da (head=true)
    tree += procNODE_AS(reader,true);
  else 
    // NODEa irakurri eta prozesatzen du
    tree += procNODE_notAS(reader,true);

  ret = nextTag(reader);
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
// - ord -> ref : ord atributuan dagoen balioa, ref atributuan idazten du (helmugak jatorrizkoaren erreferentzia izateko postedizioan)
// - SENTENCE barruan dauden CHUNKak irakurri eta prozesatzen ditu.

  string tree;
  string tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if(tagName == "SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    // ord -> ref : ord atributuan dagoen balioa, ref atributuan gordetzen du
    tree = "<SENTENCE ref='" + write_xml(attrib(reader, "ord")) + "'" + write_xml(allAttrib_except(reader, "ord")) + ">\n";
  }
  else {
    cerr << "ERROR: invalid document: found <" << tagName << "> when <SENTENCE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  // SENTENCE barruan dauden CHUNK guztietarako 
  while (ret == 1 and tagName == "CHUNK") {
    // CHUNKa irakurri eta prozesatzen du.
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

  // Hiztegi elebidunaren hasieraketa. Parametro moduan jasotzen den fitxagia erabiltzen da hasieraketarako.
  FILE *transducer = fopen(cfg.DictionaryFile, "r");  
  fstp.load(transducer);
  fclose(transducer);
  fstp.initBiltrans();
  
  // Hasieraketa hauek konfigurazio fitxategi batetik irakurri beharko lirateke.
  init_lexInfo("noumSem", cfg.Noum_SemanticFile);
  init_lexInfo("chunkType", cfg.ChunkType_DictFile);

  // libXml liburutegiko reader hasieratzen da, sarrera estandarreko fitxategia irakurtzeko.
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
