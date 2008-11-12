#include <lttoolbox/FSTProcessor.H>
#include <lttoolbox/Ltstr.H>

#include <string>
#include <iostream>

#include "config.h"

#include <data_manager.h>
#include <XML_reader.h>

using namespace std;

FSTProcessor fstp_generation;
FSTProcessor fstp_measures;
FSTProcessor fstp_pre_generation;
FSTProcessor fstp_nolex_generation;


string disanbiguate(string &full) {
// Hiztegi elebidunaren euskarazko ordainetik lehenengoa lortzen du.
// IN:  Euskarazko ordainak ( ordain1[/ordain2]* )
// OUT: lehenengoa          ( oradin1            )
  string output = full;

  for(uint i = 0; i < output.size(); i++) {
    if (output[i] == '/')
      output = output.substr(0,i);

    if (output[i] == '\\') 
      output.erase(i, 1);
  }

  if (output[output.size()-1] == '-')
    output.erase(output.size()-1, 1);

  return output;
}

string keep_case(string form, string UpCase) {
  string out_form = form;

  if (UpCase == "first" || UpCase == "title" || UpCase == "all" )
    out_form[0] = toupper(out_form[0]);

  for (int i = 1; i<form.size(); i++) {
    if (UpCase == "all" || UpCase == "title" && form[i-1] == ' ') 
      out_form[i] = toupper(out_form[i]);
  }

  return out_form;
}

string verb_generation(string lemma, string pos, string suf, string cas, string mi, string chunk_mi, string head_sem, bool is_last, bool &flexioned, config cfg) {
  string analisys, form, prefix, postposizio, pre_lemma, lemma_osoa;
  lemma_osoa = lemma;

  if (cfg.DoGenTrace) cerr << lemma << " " << pos << " " << mi << " " << cas << endl;

  for (int i=0; i<lemma.size(); i++) {
    if (lemma[i] == '_') lemma[i]= ' '; 
  }

  if (cas.find("++") != string::npos) {
    postposizio = " " + cas.substr(cas.find("++") + 2, cas.size());
    cas = cas.substr(0, cas.find("++")); 
  }

  if (cas.find('+') != 0 && cas.find('+') != string::npos) {
    prefix = cas.substr(0,cas.find('+'));
    cas = cas.substr(cas.find('+')+1, cas.size());

    if (prefix.find("[") != string::npos) 
      prefix.insert(prefix.find("["), "+");
  }

  unsigned int position1 = lemma.rfind(' ');
  unsigned int position2 = lemma.rfind('-');
  if (position1 != string::npos or position2 != string::npos) {
    if (position1 == string::npos or (position2 != string::npos and position2 > position1)) {
      position1 = position2;
    }

    pre_lemma = lemma.substr(0, position1+1);
    lemma = lemma.substr(position1+1, lemma.size());
  }

  if (mi.substr(0,10) == "[ADI][SIN]") {pos = "[ADI][SIN]";mi.erase(0,10);}
  if (mi.substr(0,10) == "[ADI][ADP]") {pos = "[ADI][ADP]";mi.erase(0,10);}
  if (mi.substr(0,5) == "[ADL]") {pos = "[ADL]";mi.erase(0,5);}
  if (mi.substr(0,5) == "[ADT]") {pos = "[ADT]";mi.erase(0,5);}

  string pre_gen = "^" + lemma_osoa + pos + "$";
  if (cfg.DoGenTrace) cerr << pre_gen << endl;
  string lemmaMorf = fstp_pre_generation.biltrans(pre_gen);
  if (cfg.DoGenTrace) cerr << lemmaMorf << endl;

  if (lemmaMorf[0] == '^' and lemmaMorf[1] == '@') lemmaMorf = lemma + pos + mi;
  else lemmaMorf = lemmaMorf.substr(1, lemmaMorf.size()-2) + mi;

  for (int i=0; i<lemmaMorf.size(); i++) {
    if (lemmaMorf[i] == '\\') {
      lemmaMorf.erase(i,1);
      i--;
    }
  }

  if (is_last)
    lemmaMorf = prefix + lemmaMorf;

  if (cfg.DoGenTrace) cerr << lemmaMorf << endl;


  analisys = "^" + get_generation_order(prefix, lemmaMorf, chunk_mi, cas, head_sem) + "$";

  if (cfg.DoGenTrace) cerr << analisys << endl;

  if (analisys.find("LemaMorf") != string::npos) analisys.replace(analisys.find("LemaMorf"), 8, lemmaMorf);
  if (analisys.find("Lema") != string::npos) analisys.replace(analisys.find("Lema"), 4, lemma);
  if (analisys.find("Num") != string::npos) analisys.replace(analisys.find("Num"), 3, chunk_mi);
  if (analisys.find("Kas") != string::npos) {analisys.replace(analisys.find("Kas"), 3, cas);flexioned=true;}

  for (int i=0; i<analisys.size(); i++) {
    if (analisys[i] == '[') analisys[i]= '<'; 
    if (analisys[i] == ']') analisys[i]= '>'; 
    if (analisys[i] == ' ' ) {
      analisys.erase(i,1); 
      i--;
    }
  }

  if (cfg.DoGenTrace) cerr << "GEN:" << analisys << endl;
  form = fstp_generation.biltrans(analisys);
  if (cfg.DoGenTrace) cerr << "GEN:" << form << endl;

  form = form.substr(1, form.size()-2);

  if (form.size() == 0 or form[0] == '@' or 
      form.find("<") != string::npos or form.find(">") != string::npos) {
    if (cfg.DoGenTrace) cerr << "GEN nolex:" << analisys << endl;
    form = fstp_nolex_generation.biltrans(analisys);
    form = form.substr(1, form.size()-2);
    if (cfg.DoGenTrace) cerr << "GEN nolex:" << form << endl << endl;
    if (form.size() == 0 or form[0] == '@' or 
	form.find("<") != string::npos or form.find(">") != string::npos) {
      form = lemma;
    }
  }

  form = disanbiguate(form);

  form = pre_lemma + form;
  if (flexioned)
    form += postposizio;

  if (cfg.DoGenTrace) cerr << form << endl << endl;
  return form;
}

string number_generation(string lemma, string pos, string suf, string cas, string mi, string head_sem, bool is_last, bool &flexioned, config cfg) {
  string analisys, form, prefix, postposizio, pre_lemma, lemma_osoa;

  if (cfg.DoGenTrace) cerr << lemma << " " << pos << " " << mi << " " << cas << endl;

  if (pos == "[Zu]") {
    string dimension = lemma.substr(0, lemma.find("_"));
    lemma = lemma.substr(lemma.find("_")+1);
    string measure = lemma.substr(0, lemma.find(":"));
    string number = lemma.substr(lemma.find(":")+1);

    //pre_lemma = number;
    lemma = number + measure;
  }

  if (mi == "") mi = "[MG]"; 

  if (cas.find("++") != string::npos) {
    postposizio = " " + cas.substr(cas.find("++") + 2, cas.size());
    cas = cas.substr(0, cas.find("++")); 
  }

  if (is_last) {
    string lemmaMorf = lemma + pos;
    if (cfg.DoGenTrace) cerr << prefix << " " << lemmaMorf << " " << mi << " " << cas << head_sem << endl;

    analisys = "^" + get_generation_order(prefix, lemmaMorf, mi, cas, head_sem) + "$";
  
    if (cfg.DoGenTrace) cerr << analisys << endl;

    if (analisys.find("LemaMorf") != string::npos) analisys.replace(analisys.find("LemaMorf"), 8, lemmaMorf);
    if (analisys.find("Lema") != string::npos) analisys.replace(analisys.find("Lema"), 4, lemma);
    if (analisys.find("Num") != string::npos) analisys.replace(analisys.find("Num"), 3, mi);
    if (analisys.find("Kas") != string::npos) analisys.replace(analisys.find("Kas"), 3, cas);
    
    flexioned = true;
  }
  else {
    analisys = "^" + lemma + pos + "$";
    postposizio = "";
    flexioned = false;
  }

  for (int i=0; i<analisys.size(); i++) {
    if (analisys[i] == '[') analisys[i]= '<'; 
    if (analisys[i] == ']') analisys[i]= '>'; 
    if (analisys[i] == ' ' ) {
      analisys.erase(i,1); 
      i--;
    }
  }

  if (cfg.DoGenTrace) cerr << "GEN:" << analisys << endl;
  form = fstp_measures.biltrans(analisys);
  if (cfg.DoGenTrace) cerr << "GEN:" << form << endl;

  form = form.substr(1, form.size()-2);

  if (form.size() == 0 or form[0] == '@' or 
      form.find("<") != string::npos or form.find(">") != string::npos) {
    form = lemma;
  }

  form = disanbiguate(form);

  form = form + postposizio;

  for (int i=0; i<form.size(); i++) {
    if (form[i] == '_') form[i]= ' '; 
  }

  if (cfg.DoGenTrace) cerr << form << endl << endl;
  return form;
}

string date_generation(string lemma, string pos, string suf, string cas, string mi, string head_sem, bool is_last, bool &flexioned, config cfg) {
  string analisys, form, prefix, postposizio, pre_lemma, lemma_osoa;

  if (cfg.DoGenTrace) cerr << lemma << " " << pos << " " << mi << " " << cas << endl;

  string century, week_day, day, month, year, hour, meridiam;
  string century_cas, week_day_cas, day_cas, month_cas, year_cas, hour_cas;
  bool century_last, week_day_last, day_last, month_last, year_last, hour_last;

  lemma = lemma.substr(1,lemma.size()-2);

  if (lemma.substr(0, 2) == "s:") {
    century = lemma.substr(2);
    week_day = day = month = year = "??";
    hour = "??.??";
  }
  else {
    week_day = lemma.substr(0, lemma.find(":"));
    lemma = lemma.substr(lemma.find(":")+1);
    day = lemma.substr(0, lemma.find("/"));
    lemma = lemma.substr(lemma.find("/")+1);
    month = lemma.substr(0, lemma.find("/"));
    lemma = lemma.substr(lemma.find("/")+1);
    year = lemma.substr(0, lemma.find(":"));
    lemma = lemma.substr(lemma.find(":")+1);
    hour = lemma.substr(0, lemma.find(":"));
    lemma = lemma.substr(lemma.find(":")+1);
    meridiam = lemma;

    century = "??";
  }

  if (century != "??" && cas != "") {century_cas = cas;cas = "";century_last=is_last;is_last=true;}
  else {century_cas="[INE]";century_last=is_last;} 

  if (day != "??" && cas != "") {day_cas = cas;cas = "";day_last=is_last;is_last=true;}
  else {day_cas="[INE]";day_last=is_last;}

  if (month != "??" && cas != "") {month_cas = cas;cas = "";month_last=is_last;is_last=true;}
  else {month_cas = "[GEN]";month_last=is_last;}

  if (year != "??" && cas != "") {year_cas = cas;cas = "";year_last=is_last;is_last=true;}
  else {year_cas="[GEL]";year_last=is_last;}

  if (week_day != "??" && cas != "") {week_day_cas = cas;cas = "";week_day_last=is_last;is_last=true;}
  else {week_day_cas="[ABS]";week_day_last=is_last;}

  if (hour != "??.??" && cas != "") {hour_cas = cas;cas = "";hour_last=is_last;is_last=true;}
  else {hour_cas="[INE]";hour_last=is_last;}



  if (century != "??") 
    form = number_generation(century, "[CN]", suf, century_cas, "[NUMS]", head_sem, century_last, flexioned, cfg);

  if (year != "??") 
    form = number_generation(year, "[Z]", suf, year_cas, "[NUMS]", head_sem, year_last, flexioned, cfg);

  if (month != "??"){
    if (form != "") form += " ";
    form += number_generation(month, "[MM]", suf, month_cas, "[NUMS]", head_sem, month_last, flexioned, cfg);
  }

  if (day != "??") {
    if (form != "") form += " ";
    form += number_generation(day, "[Z]", suf, day_cas, "[NUMS]", head_sem, day_last, flexioned, cfg);
  }

  if (week_day != "??") {
    if (form != "") form += ", ";
    form += number_generation(week_day, "[WD]", suf, week_day_cas, "[NUMS]", head_sem, week_day_last, flexioned, cfg);
  }

  if (hour != "??.??") {
    if (form != "") form += ", ";
    form += number_generation(hour, "[Z]", suf, hour_cas, "[MG]", head_sem, hour_last, flexioned, cfg);
    if (meridiam != "??") form += " (" + meridiam + ")";
  }


  flexioned = true;

  return form;
}

string generation(string lemma, string pos, string suf, string cas, string mi, string head_sem, bool is_last, bool &flexioned, config cfg) {
  string analisys, form, prefix, postposizio, pre_lemma, lemma_osoa;
  lemma_osoa = lemma;

  if (cfg.DoGenTrace) cerr << lemma << " " << pos << " " << suf << " " << mi << " " << cas << " " << head_sem << endl;

  for (int i=0; i<lemma.size(); i++) {
    if (lemma[i] == '_') lemma[i]= ' '; 
  }

  if ((!is_last && suf == "") || cas == "") return lemma;

  if (mi == "") mi = "[NUMS]";
  if (!is_last) cas = "";

  flexioned = true;
  if (cas.find("++") != string::npos) {
    postposizio = " " + cas.substr(cas.find("++") + 2, cas.size());
    cas = cas.substr(0, cas.find("++")); 
  }

  if (cas.find('+') != 0 and cas.find('+') != string::npos) {
    prefix = cas.substr(0,cas.find('+'));
    cas = cas.substr(cas.find('+')+1, cas.size());
  }

  unsigned int position1 = lemma.rfind(' ');
  unsigned int position2 = lemma.rfind('-');
  if (position1 != string::npos or position2 != string::npos) {
    if (position1 == string::npos or (position2 != string::npos and position2 > position1)) {
      position1 = position2;
    }

    pre_lemma = lemma.substr(0, position1+1);
    lemma = lemma.substr(position1+1, lemma.size());
  }

  string pre_gen = "^" + lemma_osoa + pos + "$";
  if (cfg.DoGenTrace) cerr << pre_gen << endl;
  string lemmaMorf = fstp_pre_generation.biltrans(pre_gen);
  if (cfg.DoGenTrace) cerr << lemmaMorf << endl;
  if (lemmaMorf[0] == '^' and lemmaMorf[1] == '@')
    lemmaMorf = lemma + pos;
  else
    lemmaMorf = lemmaMorf.substr(1, lemmaMorf.size()-2);

  if (suf != "") lemmaMorf += suf;
 
  for (int i=0; i<lemmaMorf.size(); i++) {
    if (lemmaMorf[i] == '\\') {
      lemmaMorf.erase(i,1);
      i--;
    }
  }
  if (cfg.DoGenTrace) cerr << lemmaMorf << " " << mi << " " << cas << " " << head_sem << endl;

  analisys = "^" + get_generation_order(prefix, lemmaMorf, mi, cas, head_sem) + "$";

  if (cfg.DoGenTrace) cerr << analisys << endl;

  if (analisys.find("LemaMorf") != string::npos) analisys.replace(analisys.find("LemaMorf"), 8, lemmaMorf);
  if (analisys.find("Lema") != string::npos) analisys.replace(analisys.find("Lema"), 4, lemma);
  if (analisys.find("Num") != string::npos) analisys.replace(analisys.find("Num"), 3, mi);
  if (analisys.find("Kas") != string::npos) analisys.replace(analisys.find("Kas"), 3, cas);

  for (int i=0; i<analisys.size(); i++) {
    if (analisys[i] == '[') analisys[i]= '<'; 
    if (analisys[i] == ']') analisys[i]= '>'; 
    if (analisys[i] == ' ' ) {
      analisys.erase(i,1); 
      i--;
    }
  }

  if (cfg.DoGenTrace) cerr << "GEN:" << analisys << endl;
  form = fstp_generation.biltrans(analisys);
  if (cfg.DoGenTrace) cerr << "GEN:" << form << endl;

  form = form.substr(1, form.size()-2);

  if (form.size() == 0 or form[0] == '@' or 
      form.find("<") != string::npos or form.find(">") != string::npos) {
    if (cfg.DoGenTrace) cerr << "GEN nolex:" << analisys << endl;
    form = fstp_nolex_generation.biltrans(analisys);
    form = form.substr(1, form.size()-2);
    if (cfg.DoGenTrace) cerr << "GEN nolex:" << form << endl;
    if (form.size() == 0 or form[0] == '@' or 
	form.find("<") != string::npos or form.find(">") != string::npos) {
      form = lemma;
    }
  }

  form = disanbiguate(form);

  form = pre_lemma + form + postposizio;

  if (cfg.DoGenTrace) cerr << form << endl << endl;
  return form;
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

string procNODE(xmlTextReaderPtr reader, string chunk_type, string cas, string cas_alloc, string cas_ref, string mi, string head_sem, int chunk_len, config cfg){
  string nodes;
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);

  if (tagName == "NODE" and tagType != XML_READER_TYPE_END_ELEMENT) {
    string lem = attrib(reader, "lem");
    string form;
    bool is_last = (atoi(attrib(reader, "ord").c_str()) == (chunk_len - 1));
    bool flexioned = false;

    if (chunk_type.substr(0,4) == "adi-") {
      form = verb_generation(attrib(reader, "lem"), attrib(reader, "pos"), attrib(reader, "suf"), cas, attrib(reader, "mi"), mi, head_sem, is_last, flexioned, cfg);
    }
    else if (attrib(reader, "pos") == "[W]") {
      form = date_generation(attrib(reader, "lem"), attrib(reader, "pos"), attrib(reader, "suf"), cas, mi, head_sem, is_last, flexioned, cfg);
    }
    else if (attrib(reader, "pos") == "[Z]" || attrib(reader, "pos") == "[Zu]" || attrib(reader, "pos") == "[Zm]" || attrib(reader, "pos") == "[Zp]" || attrib(reader, "pos") == "[Zd]") {
      form = number_generation(attrib(reader, "lem"), attrib(reader, "pos"), attrib(reader, "suf"), cas, mi, head_sem, is_last, flexioned, cfg);
    }
    else {
      form = generation(attrib(reader, "lem"), attrib(reader, "pos"), attrib(reader, "suf"), cas, mi, head_sem, is_last, flexioned, cfg);
    }

    for (int i; i<form.size(); i++) if (form[i] == '_') form[i] = ' '; 

    form = keep_case(form, attrib(reader, "UpCase"));

    nodes += "<NODE " + write_xml("form='" + form+ "'");
    nodes += " ref ='" + write_xml(attrib(reader, "ref"));
    if (flexioned && cas_ref != "") nodes += "," + write_xml(cas_ref);
    nodes += "' alloc ='" + write_xml(attrib(reader, "alloc"));
    if (flexioned && cas_alloc != "") nodes += "," + write_xml(cas_alloc);
    nodes += "'" + write_xml(text_allAttrib_except(allAttrib_except(reader, "alloc"), "ref"));

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
    nodes += procNODE(reader, chunk_type, cas, cas_alloc, cas_ref, mi, head_sem, chunk_len, cfg);

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

string procCHUNK(xmlTextReaderPtr reader, config cfg) {
  string tagName = getTagName(reader);
  int tagType=xmlTextReaderNodeType(reader);
  string tree, det_cas, cas, cas_alloc, cas_ref, mi, type, head_sem;
  int chunk_len;

  if (tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    type = attrib(reader, "type");
    det_cas = attrib(reader, "case");
    cas = attrib(reader, "cas");
    cas_alloc = attrib(reader, "casalloc");
    cas_ref = attrib(reader, "casref");
    mi = attrib(reader, "mi");
    head_sem = attrib(reader, "headsem");
    chunk_len = atoi(attrib(reader, "length").c_str());
    tree = "<CHUNK" + write_xml(allAttrib(reader)) + ">\n";


    if (det_cas != "" && det_cas.rfind("++") != string::npos) {
      bool flexioned;
      string postposizio = det_cas.substr(det_cas.rfind("++")+2);
      if (cfg.DoGenTrace) cerr << postposizio << " " << cas << endl;
      postposizio = generation(postposizio, "", cas, "", mi, head_sem, true, flexioned, cfg);
      if (cfg.DoGenTrace) cerr << postposizio << endl;
      cas = det_cas.substr(0, det_cas.rfind("++")+2) + postposizio;
      if (cfg.DoGenTrace) cerr << cas << endl;
    }


  }
  else {
    cerr << "ERROR: invalid tag: <" << tagName << "> when <CHUNK> was expected..." << endl;
    exit(-1);
  }  

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);

  tree += procNODE(reader, type, cas, cas_alloc, cas_ref, mi, head_sem, chunk_len, cfg);

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType=xmlTextReaderNodeType(reader);
  while (ret == 1 and tagName == "CHUNK" and tagType == XML_READER_TYPE_ELEMENT) {
    tree += procCHUNK(reader, cfg);

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

string procSENTENCE (xmlTextReaderPtr reader, config cfg) {
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
    tree += procCHUNK(reader, cfg);

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

int main(int argc, char *argv[]) {
  config cfg(argv);

  // Sorkuntza morfologikorako transducerraren hasieraketa. Parametro moduan jasotzen den fitxagia erabiltzen da hasieraketarako.
  //cerr << cfg.Morpho_GenFile << " irekitzen..." << endl;
  FILE *transducer = fopen(cfg.Morpho_GenFile, "r");  
  fstp_generation.load(transducer);
  fclose(transducer);
  fstp_generation.initBiltrans();
  
  //cerr << cfg.Measures_GenFile << " irekitzen..." << endl;
  transducer = fopen(cfg.Measures_GenFile, "r");  
  fstp_measures.load(transducer);
  fclose(transducer);
  fstp_measures.initBiltrans();
  
  //cerr << cfg.Tag_ToGenFile << " irekitzen..." << endl;
  transducer = fopen(cfg.Tag_ToGenFile, "r");  
  fstp_pre_generation.load(transducer);
  fclose(transducer);
  fstp_pre_generation.initBiltrans();
  
  //cerr << cfg.NoLex_GenFile << " irekitzen..." << endl;
  transducer = fopen(cfg.NoLex_GenFile, "r");  
  fstp_nolex_generation.load(transducer);
  fclose(transducer);
  fstp_nolex_generation.initBiltrans();
  
  //cerr << cfg.Tag_OrderFile << " irekitzen..." << endl;
  //ordena definituko duen zerbitzaria hasieratu...
  init_generation_order(cfg.Tag_OrderFile);

  //cerr << "Fitxategi guztiak irekita." << endl;

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

  int i = 0;
  // corpus barruan dauden SENTENCE guztietarako 
  while (ret == 1 and tagName == "SENTENCE") {

    //SENTENCE irakurri eta prozesatzen du.
    string tree = procSENTENCE(reader, cfg);
    cout << tree << endl;
    cout.flush();

    if (cfg.DoTrace) {
      ostringstream log_fileName_osoa;
      ofstream log_file;

      log_fileName_osoa << cfg.Trace_File << i++ << ".xml";

      log_file.open(log_fileName_osoa.str().c_str(), ofstream::out | ofstream::app);
      log_file << tree << "</corpus>\n";
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
