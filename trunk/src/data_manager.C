#include "data_manager.h"

chunk text2chunk(string text) {
  chunk output;

  unsigned int separator = text.find("/");
  if (separator == string::npos) return output;

  output.condition = text.substr(0,separator);
  output.attrib = text.substr(separator+1);

  return output;
}

bool apply_condition(string attributes, string condition) {
  //cerr << attributes << endl << condition << endl << endl;
  if (condition.find("(") != string::npos) {
    int open_position = condition.find("(");
    int close_position = condition.find(")", open_position+1);

    while (condition.find("(", open_position+1) < close_position) {

      open_position = condition.find("(", open_position+1);
      close_position = condition.find(")", open_position+1);

      if (close_position == string::npos) return false;
    }
    open_position = condition.find("(");

    string condition_2 = condition.substr(open_position+1, close_position-open_position-1);
    bool eval=apply_condition(attributes, condition_2);

    if (open_position != 0) {
      string condition_1 = condition.substr(0, open_position);
      int and_position = condition_1.rfind("&&");
      int or_position = condition_1.rfind("||");

      if (and_position !=string::npos && and_position > or_position) {
	condition_1 = condition.substr(0, and_position);

	eval = eval & apply_condition(attributes, condition_1);
      }
      else if (or_position != string::npos) {
	condition_1 = condition.substr(0, or_position);

	eval = eval | apply_condition(attributes, condition_1);
      }
    }

    if (close_position != condition.size()) {
      string condition_3 = condition.substr(close_position+1);
      int and_position = condition_3.find("&&");
      int or_position = condition_3.find("||");

      if (and_position !=string::npos && and_position < or_position) {
	condition_3 = condition_3.substr(0, and_position);

	eval = eval & apply_condition(attributes, condition_3);
      }
      else if (or_position != string::npos) {
	condition_3 = condition_3.substr(0, or_position);

	eval = eval | apply_condition(attributes, condition_3);
      }
    }

    return eval;
  }
  else if (condition.find("||") != string::npos){
    int operator_position = condition.find("||");

    return (apply_condition(attributes, condition.substr(0, operator_position))
	    || apply_condition(attributes, condition.substr(operator_position+2)));
  }
  else if (condition.find("&&") != string::npos){
    int operator_position = condition.find("&&");

    return (apply_condition(attributes, condition.substr(0, operator_position))
	    && apply_condition(attributes, condition.substr(operator_position+2)));
  }
  else {
    if (condition.find("!=") != string::npos) {
      int operator_position = condition.find("!=");
      int blank_position = condition.rfind(" ", operator_position);
      if (blank_position == string::npos) blank_position = -1;

      string attribute = condition.substr(blank_position+1, operator_position-blank_position-1);

      blank_position = condition.find(" ", operator_position);
      if (blank_position == string::npos) blank_position = condition.size();

      if (condition[operator_position+2]=='\'' and condition[blank_position-1]=='\'') {
	blank_position--;
	operator_position++;
      }
      string value = condition.substr(operator_position+2, blank_position-operator_position-2);

      return (text_attrib(attributes, attribute) != value);
    }
    else if (condition.find("=~") != string::npos) {
      int operator_position = condition.find("=~");
      int blank_position = condition.rfind(" ", operator_position);
      if (blank_position == string::npos) blank_position = -1;

      string attribute = condition.substr(blank_position+1, operator_position-blank_position-1);

      blank_position = condition.find(" ", operator_position);
      if (blank_position == string::npos) blank_position = condition.size();

      if (condition[operator_position+2]=='\'' and condition[blank_position-1]=='\'') {
	blank_position--;
	operator_position++;
      }
      string value = condition.substr(operator_position+2, blank_position-operator_position-2);

      return (text_attrib(attributes, attribute).find(value) != string::npos);
      /*
      Reg_Ex regex = Reg_Ex(value.c_str());
      return (regex.Search(text_attrib(attributes, attribute).c_str()));
      */
    }
    else if (condition.find("=") != string::npos) {
      int operator_position = condition.find("=");
      int blank_position = condition.rfind(" ", operator_position);
      if (blank_position == string::npos) blank_position = -1;

      string attribute = condition.substr(blank_position+1, operator_position-blank_position-1);

      blank_position = condition.find(" ", operator_position);
      if (blank_position == string::npos) blank_position = condition.size();

      if (condition[operator_position+1]=='\'' and condition[blank_position-1]=='\'') {
	blank_position--;
	operator_position++;
      }
      string value = condition.substr(operator_position+1, blank_position-operator_position-1);

      //      if (text_attrib(attributes, attribute) == value) return true;
      //      else return false;
      return (text_attrib(attributes, attribute) == value);
    }
    else if (condition.find(">") != string::npos) {
      int operator_position = condition.find(">");
      int blank_position = condition.rfind(" ", operator_position);
      if (blank_position == string::npos) blank_position = -1;

      string attribute = condition.substr(blank_position+1, operator_position-blank_position-1);

      blank_position = condition.find(" ", operator_position);
      if (blank_position == string::npos) blank_position = condition.size();

      if (condition[operator_position+1]=='\'' and condition[blank_position-1]=='\'') {
	string value = condition.substr(operator_position+2, blank_position-operator_position-3);

	return text_attrib(attributes, attribute) > value;
      }
      else {
	string value = condition.substr(operator_position+2, blank_position-operator_position-3);

	return atoi(text_attrib(attributes, attribute).c_str()) > atoi(value.c_str());
      }
    }
    else if (condition.find("<") != string::npos) {
      int operator_position = condition.find("<");
      int blank_position = condition.rfind(" ", operator_position);
      if (blank_position == string::npos) blank_position = -1;

      string attribute = condition.substr(blank_position+1, operator_position-blank_position-1);

      blank_position = condition.find(" ", operator_position);
      if (blank_position == string::npos) blank_position = condition.size();

      if (condition[operator_position+1]=='\'' and condition[blank_position-1]=='\'') {
	string value = condition.substr(operator_position+2, blank_position-operator_position-3);

	return text_attrib(attributes, attribute) < value;
      }
      else {
	string value = condition.substr(operator_position+2, blank_position-operator_position-3);

	return atoi(text_attrib(attributes, attribute).c_str()) < atoi(value.c_str());
      }
    }
  }

  return true;
}

bool apply_condition(string parent_attributes, string child_attributes, string condition) {
  //cerr << parent_attributes << "::" << child_attributes << "::" << condition << endl;
  if (condition.find("(") != string::npos) {
    int open_position = condition.find("(");
    int close_position = condition.find(")", open_position+1);

    while (condition.find("(", open_position+1) < close_position) {

      open_position = condition.find("(", open_position+1);
      close_position = condition.find(")", open_position+1);

      if (close_position == string::npos) return false;
    }
    open_position = condition.find("(");

    string condition_2 = condition.substr(open_position+1, close_position-open_position-1);
    bool eval=apply_condition(parent_attributes, child_attributes, condition_2);

    if (open_position != 0) {
      string condition_1 = condition.substr(0, open_position);
      int and_position = condition_1.rfind("&&");
      int or_position = condition_1.rfind("||");

      if (and_position !=string::npos && and_position > or_position) {
	condition_1 = condition.substr(0, and_position);

	eval = eval & apply_condition(parent_attributes, child_attributes, condition_1);
      }
      else if (or_position != string::npos) {
	condition_1 = condition.substr(0, or_position);

	eval = eval | apply_condition(parent_attributes, child_attributes, condition_1);
      }
    }

    if (close_position != condition.size()) {
      string condition_3 = condition.substr(close_position+1);
      int and_position = condition_3.find("&&");
      int or_position = condition_3.find("||");

      if (and_position !=string::npos && and_position < or_position) {
	condition_3 = condition_3.substr(and_position+2);

	eval = eval & apply_condition(parent_attributes, child_attributes, condition_3);
      }
      else if (or_position != string::npos) {
	condition_3 = condition_3.substr(or_position+2);

	eval = eval | apply_condition(parent_attributes, child_attributes, condition_3);
      }
    }

    return eval;
  }
  else if (condition.find("||") != string::npos){
    int operator_position = condition.find("||");

    return (apply_condition(parent_attributes, child_attributes, condition.substr(0, operator_position))
	    || apply_condition(parent_attributes, child_attributes, condition.substr(operator_position+2)));
  }
  else if (condition.find("&&") != string::npos){
    int operator_position = condition.find("&&");

    return (apply_condition(parent_attributes, child_attributes, condition.substr(0, operator_position))
	    && apply_condition(parent_attributes, child_attributes, condition.substr(operator_position+2)));
  }
  else {
    if (condition.find("!=") != string::npos) {
      int operator_position = condition.find("!=");
      int blank_position = condition.rfind(" ", operator_position);
      if (blank_position == string::npos) blank_position = -1;
      int dot_position = condition.rfind(".", operator_position);
      if (dot_position == string::npos) blank_position = -1;

      string attribute = condition.substr(dot_position+1, operator_position-dot_position-1);
      string chunk = condition.substr(blank_position+1, dot_position-blank_position-1);

      blank_position = condition.find(" ", operator_position);
      if (blank_position == string::npos) blank_position = condition.size();

      if (condition[operator_position+2]=='\'' and condition[blank_position-1]=='\'') {
	blank_position--;
	operator_position++;
      }
      string value = condition.substr(operator_position+2, blank_position-operator_position-2);

      if (chunk == "parent") 
	return (text_attrib(parent_attributes, attribute) != value);
      else
	return (text_attrib(child_attributes, attribute) != value);
	
    }
    else if (condition.find("=~") != string::npos) {
      int operator_position = condition.find("=~");
      int blank_position = condition.rfind(" ", operator_position);
      if (blank_position == string::npos) blank_position = -1;
      int dot_position = condition.rfind(".", operator_position);
      if (dot_position == string::npos) blank_position = -1;

      string attribute = condition.substr(dot_position+1, operator_position-dot_position-1);
      string chunk = condition.substr(blank_position+1, dot_position-blank_position-1);

      blank_position = condition.find(" ", operator_position);
      if (blank_position == string::npos) blank_position = condition.size();

      if (condition[operator_position+2]=='\'' and condition[blank_position-1]=='\'') {
	blank_position--;
	operator_position++;
      }
      string value = condition.substr(operator_position+2, blank_position-operator_position-2);

      if (chunk == "parent") 
	return (text_attrib(parent_attributes, attribute).find(value) != string::npos);
      else 
	return (text_attrib(child_attributes, attribute).find(value) != string::npos);
      /*
      Reg_Ex regex = Reg_Ex(value.c_str());
      if (chunk == "parent") {
	cerr << attribute << "::" << value << "\t" << parent_attributes << endl;
	return (regex.Search(text_attrib(parent_attributes, attribute).c_str()));
      }
      else {
	cerr << attribute << "::" << value << "\t" << child_attributes << endl;
	return (regex.Search(text_attrib(child_attributes, attribute).c_str()));
      }
      */
    }
    else if (condition.find("=") != string::npos) {
      int operator_position = condition.find("=");
      int blank_position = condition.rfind(" ", operator_position);
      if (blank_position == string::npos) blank_position = -1;
      int dot_position = condition.rfind(".", operator_position);
      if (dot_position == string::npos) blank_position = -1;

      string attribute = condition.substr(dot_position+1, operator_position-dot_position-1);
      string chunk = condition.substr(blank_position+1, dot_position-blank_position-1);

      blank_position = condition.find(" ", operator_position);
      if (blank_position == string::npos) blank_position = condition.size();

      if (condition[operator_position+1]=='\'' and condition[blank_position-1]=='\'') {
	blank_position--;
	operator_position++;
      }
      string value = condition.substr(operator_position+1, blank_position-operator_position-1);

      if (chunk == "parent") 
	return (text_attrib(parent_attributes, attribute) == value);
      else 
	return (text_attrib(child_attributes, attribute) == value);

    }
    else if (condition.find(">") != string::npos) {
      int operator_position = condition.find(">");
      int blank_position = condition.rfind(" ", operator_position);
      if (blank_position == string::npos) blank_position = -1;
      int dot_position = condition.rfind(".", operator_position);
      if (dot_position == string::npos) blank_position = -1;

      string attribute = condition.substr(dot_position+1, operator_position-dot_position-1);
      string chunk = condition.substr(blank_position+1, dot_position-blank_position-1);

      blank_position = condition.find(" ", operator_position);
      if (blank_position == string::npos) blank_position = condition.size();

      if (condition[operator_position+1]=='\'' and condition[blank_position-1]=='\'') {
	string value = condition.substr(operator_position+2, blank_position-operator_position-3);

	if (chunk == "parent") 
	  return text_attrib(parent_attributes, attribute) > value;
	else 
	  return text_attrib(child_attributes, attribute) > value;
      }
      else {
	string value = condition.substr(operator_position+2, blank_position-operator_position-3);

	if (chunk == "parent") 
	  return atoi(text_attrib(parent_attributes, attribute).c_str()) > atoi(value.c_str());
	else
	  return atoi(text_attrib(child_attributes, attribute).c_str()) > atoi(value.c_str());
      }
    }
    else if (condition.find("<") != string::npos) {
      int operator_position = condition.find("<");
      int blank_position = condition.rfind(" ", operator_position);
      if (blank_position == string::npos) blank_position = -1;
      int dot_position = condition.rfind(".", operator_position);
      if (dot_position == string::npos) blank_position = -1;

      string attribute = condition.substr(dot_position+1, operator_position-dot_position-1);
      string chunk = condition.substr(blank_position+1, dot_position-blank_position-1);

      blank_position = condition.find(" ", operator_position);
      if (blank_position == string::npos) blank_position = condition.size();

      if (condition[operator_position+1]=='\'' and condition[blank_position-1]=='\'') {
	string value = condition.substr(operator_position+2, blank_position-operator_position-3);

	if (chunk == "parent")
	  return text_attrib(parent_attributes, attribute) < value;
	else
	  return text_attrib(child_attributes, attribute) < value;
      }
      else {
	string value = condition.substr(operator_position+2, blank_position-operator_position-3);

	if (chunk == "parent")
	  return atoi(text_attrib(parent_attributes, attribute).c_str()) < atoi(value.c_str());
	else
	  return atoi(text_attrib(child_attributes, attribute).c_str()) < atoi(value.c_str());
      }
    }
  }

  return true;
}

static struct chunkMovements{
  bool isInit;
  vector<movement> movements;
}chunkAttribMovement;

void init_chunkMovement(string fitxName) {
  ifstream fitx;
  fitx.open(fitxName.c_str());

  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[256];
    //fitx.getline(buffer,256);
    //string lerro = buffer;

    unsigned int sep1 = lerro.find('\t');
    unsigned int sep2 = lerro.find('\t', sep1+1);
    unsigned int sep3 = lerro.find('\t', sep2+1);
    if (sep1 == string::npos || sep2 == string::npos || sep3 == string::npos) continue;

    movement current;
    string from = lerro.substr(0,sep1); 
    current.from = text2chunk(from);

    string to = lerro.substr(sep1+1,sep2-sep1-1);
    current.to = text2chunk(to);

    current.direction = lerro.substr(sep2+1, sep3-sep2-1);
    current.write_type = lerro.substr(sep3+1);

    chunkAttribMovement.movements.push_back(current);
  }

  fitx.close();
  chunkAttribMovement.isInit=true;
}

vector<movement> get_chunk_movements(string fromAttributes, string toAttributes, string direction) {
  vector<movement> output;

//cerr << fromAttributes << "\t" << toAttributes << "\t" << direction << endl;
  for (int i=0;i<chunkAttribMovement.movements.size();i++) {

    if (apply_condition(fromAttributes, chunkAttribMovement.movements[i].from.condition)
	&& apply_condition(toAttributes, chunkAttribMovement.movements[i].to.condition)
	&& chunkAttribMovement.movements[i].direction == direction) {

      output.push_back(chunkAttribMovement.movements[i]);
//cerr << "\t" << chunkAttribMovement.movements[i].from.attrib << endl;
    }
  }

  return output;
}

static struct nodeMovements{
  bool isInit;
  vector<movement> movements;
}nodeAttribMovement;

void init_nodeMovement(string fitxName){
  ifstream fitx;
  fitx.open(fitxName.c_str());

  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[256];
    //fitx.getline(buffer,256);
    //string lerro = buffer;

    unsigned int sep1 = lerro.find('\t');
    unsigned int sep2 = lerro.find('\t', sep1+1);
    if (sep1 == string::npos || sep2 == string::npos) continue;

    movement current;
    string from = lerro.substr(0,sep1); 
    current.from = text2chunk(from);

    string to = lerro.substr(sep1+1,sep2-sep1-1);
    current.to = text2chunk(to);

    current.write_type = lerro.substr(sep2+1);

    //cerr << current.from.attrib << "::" << current.from.condition << "\t" << current.to.attrib << "::" << current.to.condition << "\t" << current.write_type << endl;
    nodeAttribMovement.movements.push_back(current);
  }

  fitx.close();
  nodeAttribMovement.isInit=true;
}

vector<movement> get_node_movements_byfrom(string attributes){
  vector<movement> output;

  //cerr << "byFrom: " << type << "\t" << si << "\t"<< direction << endl;
  for (int i=0;i<nodeAttribMovement.movements.size();i++) {

    if (apply_condition(attributes, nodeAttribMovement.movements[i].from.condition)) {

      output.push_back(nodeAttribMovement.movements[i]);
      //cerr << "\t" << chunkAttribMovement.movements[i].from.attrib << endl;
    }
  }

  //cerr << endl;
  return output;
}

vector<movement> get_node_movements_byto(string attributes){
  vector<movement> output;

  for (int i=0;i<nodeAttribMovement.movements.size();i++) {
    //cerr << "byTo: '" << attributes << "','" << nodeAttribMovement.movements[i].to.condition << "'";
    if (apply_condition(attributes, nodeAttribMovement.movements[i].to.condition)) {

      output.push_back(nodeAttribMovement.movements[i]);
      //cerr << "\t'" << nodeAttribMovement.movements[i].to.attrib << "'" << endl;
    }
    //else cerr << endl;
  }

  //cerr  << endl;
  return output;
}


struct lexInfo{
  string name;
  map<string,string> info;
};

static vector<lexInfo> lexical_information;

void init_lexInfo(string name, string fitxName) {
  ifstream fitx;
  fitx.open(fitxName.c_str());

  lexInfo lex;
  lex.name=name;
  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[256];
    //fitx.getline(buffer,256);
    //string lerro = buffer;

    //komentarioak kendu.
    if (lerro.find('#') != string::npos)
      lerro = lerro.substr(0, lerro.find('#'));

    //zuriuneak eta kendu...
    for (int i=0; i<lerro.size(); i++) {
      if (lerro[i] == ' ' and (lerro[i+1] == ' ' or lerro[i+1] == '\t'))
	lerro[i] = '\t';
      if ((lerro[i] == ' ' or lerro[i] == '\t') and 
	  (i == 0 or lerro[i-1] == '\t')) {
	lerro.erase(i,1);
	i--;
      }
    }
    if (lerro[lerro.size()-1] == ' ' or lerro[lerro.size()-1] == '\t') lerro.erase(lerro.size()-1,1);

    unsigned int pos = lerro.find("\t");
    if (pos == string::npos) continue;

    string key = lerro.substr(0,pos); 
    string value = lerro.substr(pos+1);

    lex.info[key] = value;
  }

  fitx.close();
  lexical_information.push_back(lex);
}

string get_lexInfo(string name, string key){
  for (int i=0; i<lexical_information.size();i++) {
    if (lexical_information[i].name == name)
      return lexical_information[i].info[key];
  }
  return "";
}

struct pattern{
  string condition;
  string patroi;    // izena aldatu...
};

struct replacement {
  string left_context;
  string right_context;
  string attrib;
  string value;
};

static struct {
//  vector<pattern> patterns;
  bool traza;
  vector<replacement> replacements;
} verbTransference;

void init_verbTrasference(string fileName, bool traza) {
  ifstream fitx;
  fitx.open(fileName.c_str());

  verbTransference.traza = traza;
  /*
  //  cerr << "Patroiak esleitzeko erregelak..." << endl;
  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[3000];
    //fitx.getline(buffer,3000);
    //string lerro = buffer;

    unsigned int pos = lerro.find(" ");

    if (pos == string::npos) break;

    pattern patroia;
    patroia.condition = lerro.substr(0,pos); 
    patroia.patroi = lerro.substr(pos+1);

    //    cerr << patroia.condition << " '" << patroia.patroi << "' " << endl;
    verbTransference.patterns.push_back(patroia);
  }
  */

  //  cerr << endl << "Atributuen balioak esleitzeko erregelak..." << endl;
  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[5000];
    //fitx.getline(buffer,5000);
    //string lerro = buffer;

    unsigned int pos1 = lerro.find("\t");
    if (pos1 == string::npos) continue;
    unsigned int pos2 = lerro.find("\t", pos1+1);
    if (pos2 == string::npos) continue;
    unsigned int pos3 = lerro.find("\t", pos2+1);
    if (pos3 == string::npos) continue;

    replacement lag;
    lag.left_context = lerro.substr(0,pos1); 
    lag.attrib = lerro.substr(pos1+1, pos2-pos1-1);
    lag.right_context = lerro.substr(pos2+1, pos3-pos2-1);
    lag.value = lerro.substr(pos3+1);

    //    cerr << "\t'" << lag.left_context << "' '";
    //    cerr << lag.attrib << "' '";
    //    cerr << lag.right_context << "' '";
    //    cerr << lag.value << "'" << endl;
    verbTransference.replacements.push_back(lag);
  }

  //  cerr << "init_verbTransference funtzioa bukatu da." << endl << endl;
  fitx.close();
}

string apply_verbTransference(string AS_source) {
  string output = AS_source;

  /*
  for (int i=0; i<verbTransference.patterns.size(); i++) {
    //cerr << "\t" << verbTransference.patterns[i].condition << " " << verbTransference.patterns[i].patroi << endl;
    Reg_Ex regex = Reg_Ex(verbTransference.patterns[i].condition.c_str());
    if (regex.Search(output.c_str())) {
      output += verbTransference.patterns[i].patroi;
      break;
    }
  }
  */

  for (int i=0; i<verbTransference.replacements.size(); i++) {
    string pattern = "(" + verbTransference.replacements[i].left_context + ")" + verbTransference.replacements[i].attrib + verbTransference.replacements[i].right_context;
    Reg_Ex regex = Reg_Ex(pattern.c_str());

    pattern = "(" + verbTransference.replacements[i].left_context + verbTransference.replacements[i].attrib + ")" + verbTransference.replacements[i].right_context;
    Reg_Ex regex_2 = Reg_Ex(pattern.c_str());

    if (regex.Search(output.c_str()) && regex_2.Search(output.c_str())) {
      string left_context = regex.Match(1);
      string attrib = regex_2.Match(1);
      if (verbTransference.traza)
	cerr << output << endl << endl << pattern << endl;

      unsigned int attrib_pos = output.find(attrib);
      //ez da egiaztatzen zerbait aurkitu duela expresio erregularraren emaitza denez segurutzat jotzen da.
      string value = left_context + verbTransference.replacements[i].value;
      output.replace(attrib_pos, attrib.size(), value);
    }
  }

  if (verbTransference.traza)  
    cerr << output << endl << endl << endl;
  return output;
}

struct chunk_order{
  string parent_type;
  string child_type;
  string relative_order;
  string order;
};

static vector<chunk_order> order_inter;

void init_chunk_order(string fitxName) {
  ifstream fitx;
  fitx.open(fitxName.c_str());

  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[256];
    //fitx.getline(buffer,256);
    //string lerro = buffer;

    //cerr << lerro << endl;
    //komentarioak kendu.
    if (lerro.find('#') != string::npos)
      lerro = lerro.substr(0, lerro.find('#'));

    //zuriuneak eta kendu...
    for (int i=0; i<lerro.size(); i++) {
      if (lerro[i] == ' ' and (lerro[i+1] == ' ' or lerro[i+1] == '\t'))
	lerro[i] = '\t';
      if ((lerro[i] == ' ' or lerro[i] == '\t') and 
	  (i == 0 or lerro[i-1] == '\t')) {
	lerro.erase(i,1);
	i--;
      }
    }
    if (lerro[lerro.size()-1] == ' ' or lerro[lerro.size()-1] == '\t') lerro.erase(lerro.size()-1,1);

    //cerr << "'" << lerro << "'" << endl;
    unsigned int sep1 = lerro.find("\t");
    unsigned int sep2 = lerro.find("\t", sep1+1);
    unsigned int sep3 = lerro.find("\t", sep2+1);
    if (sep1 == string::npos || sep2 == string::npos || sep3 == string::npos) continue;

    chunk_order current;
    current.parent_type = lerro.substr(0, sep1);
    current.child_type = lerro.substr(sep1+1, sep2-sep1-1);
    current.relative_order = lerro.substr(sep2+1, sep3-sep2-1);
    current.order = lerro.substr(sep3+1, lerro.size()-sep3-1);

    //cerr << "'" << current.parent_type << "' '" << current.child_type << "' '" << current.relative_order << "' '" << current.order << "'" << endl;
    order_inter.push_back(current);
  }

  fitx.close();
}

string get_chunk_order(string parent_type, string child_type, int relative_order) {
  ostringstream relative;

  for (int i=0; i<order_inter.size(); i++) {
    Reg_Ex regex_parent = Reg_Ex(order_inter[i].parent_type.c_str());
    Reg_Ex regex_child = Reg_Ex(order_inter[i].child_type.c_str());

    //cerr << order_inter[i].parent_type << "(" << parent_type << ")" << endl;
    //cerr << order_inter[i].child_type << "(" << child_type << ")" << endl;
    //cerr << order_inter[i].relative_order << "(" << relative_order << ")" << endl << endl;

    if (regex_parent.Search(parent_type.c_str()) and regex_child.Search(child_type.c_str()) and 
	(order_inter[i].relative_order == ".*?" or
	 order_inter[i].relative_order[0] == '=' and relative_order == atoi(order_inter[i].relative_order.substr(1, order_inter[i].relative_order.size()).c_str()) or
	 order_inter[i].relative_order[0] == '>' and relative_order > atoi(order_inter[i].relative_order.substr(1, order_inter[i].relative_order.size()).c_str()) or
	 order_inter[i].relative_order[0] == '<' and relative_order < atoi(order_inter[i].relative_order.substr(1, order_inter[i].relative_order.size()).c_str())
	)) {
      return order_inter[i].order;
    }
  }
}

struct preposition {
  string cas;
  string condition;
  string ambigous;
};

map<string, vector<preposition> > prepositions;

void init_preposition_transference(string fitxName, config &cfg){
  ifstream fitx;
  fitx.open(fitxName.c_str());

  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[2560];
    //fitx.getline(buffer,2560);
    //string lerro = buffer;

    //cerr << lerro << endl;
    //komentarioak kendu.
    if (lerro.find('#') != string::npos)
      lerro = lerro.substr(0, lerro.find('#'));

    //zuriuneak eta kendu...
    for (int i=0; i<lerro.size(); i++) {
      if (lerro[i] == ' ' and (lerro[i+1] == ' ' or lerro[i+1] == '\t'))
	lerro[i] = '\t';
      if ((lerro[i] == ' ' or lerro[i] == '\t') and 
	  (i == 0 or lerro[i-1] == '\t')) {
	lerro.erase(i,1);
	i--;
      }
    }
    if (lerro[lerro.size()-1] == ' ' or lerro[lerro.size()-1] == '\t') lerro.erase(lerro.size()-1,1);

    //cerr << "'" << lerro << "'" << endl;
    unsigned int sep1 = lerro.find("\t");
    unsigned int sep2 = lerro.find("\t", sep1+1);
    unsigned int sep3 = lerro.find("\t", sep2+1);
    if (sep1 == string::npos || sep2 == string::npos || sep3 == string::npos) continue;

    preposition current;
    string prep = lerro.substr(0, sep1);

    current.cas = lerro.substr(sep1+1, sep2-sep1-1);
    current.condition = lerro.substr(sep2+1, sep3-sep2-1);
    current.ambigous = lerro.substr(sep3+1, lerro.size()-sep3-1);

    //cerr << "'" << prep << "' '" << current.cas << "' '" << current.condition << "' '" << current.ambigous << "'" << endl;
    prepositions[prep].push_back(current);
  }

  fitx.close();

}

vector<string> preposition_transference(string parent_attributes, string child_attributes, string sentenceref, int sentencealloc, config &cfg){
  vector<string> ambigous_cases;

  string prep = text_attrib(child_attributes, "prep");
  int alloc = atoi(text_attrib(child_attributes, "alloc").c_str()) - sentencealloc;

  if (prep == "")
    prep = "-";

  if (prepositions[prep].size() == 0) {
    ambigous_cases.push_back("[ZERO]");
    cerr << "WARNING: unknown preposition " << prep << endl;
    return ambigous_cases;
  }

  vector<preposition> current_prep = prepositions[prep]; 

  if (cfg.DoPrepTrace) cerr << sentenceref << ":" << alloc << ":" << prep; 
  for (int i=0; i < current_prep.size(); i++) {
    if (cfg.DoPrepTrace) cerr << "\t" << current_prep[i].cas << "::" << current_prep[i].condition << "::" << current_prep[i].ambigous << endl;
    if (current_prep[i].ambigous == "+")
      ambigous_cases.push_back(current_prep[i].cas);

    if (cfg.UseRules && current_prep[i].condition != "-" && apply_condition(parent_attributes, child_attributes, current_prep[i].condition)) {
      vector<string> translation_case;

      translation_case.push_back(current_prep[i].cas);

      if (cfg.DoPrepTrace) cerr << endl << "ERREGELEN BIDEZ ITZULI DA (" << sentenceref << ":" << alloc << ":" << prep << "): " << current_prep[i].cas << endl << endl;
      return translation_case;
    }
    //else cerr << "\tfalse" << endl;
  }

  if (ambigous_cases.size() == 0) {
    ambigous_cases.push_back("[ZERO]");
    cerr << "WARNING: unknown preposition " << prep << endl;
  }

  if (cfg.DoPrepTrace) {
    cerr << endl << "HIZTEGIKO ERANTZUNA(" << sentenceref << ":" << alloc << ":" << prep << "): ";
    for (int j=0; j<ambigous_cases.size(); j++)
      cerr << ambigous_cases[j] << " ";
    cerr << endl << endl;
  }

  return ambigous_cases;
}


struct subcategoritation {
  string trans;
  string subj_case;
  string cases;
};

map<string, vector<subcategoritation> > subcategoritations;

void init_verb_subcategoritation(string fitxName, config &cfg){
  ifstream fitx;
  fitx.open(fitxName.c_str());

  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[163840];
    //fitx.getline(buffer,163840);
    //string lerro = buffer;

    //cerr << lerro << endl;
    //komentarioak kendu.
    //if (lerro.find('#') != string::npos)
    //  lerro = lerro.substr(0, lerro.find('#'));

    //zuriuneak eta kendu...
    for (int i=0; i<lerro.size(); i++) {
      if (lerro[i] == ' ' and (lerro[i+1] == ' ' or lerro[i+1] == '\t'))
	lerro[i] = '\t';
      if ((lerro[i] == ' ' or lerro[i-1] == '\t') and 
	  (i == 0 or lerro[i] == '\t')) {
	lerro.erase(i,1);
	i--;
      }
    }
    if (lerro[lerro.size()-1] == ' ' or lerro[lerro.size()-1] == '\t') lerro.erase(lerro.size()-1,1);

    unsigned int sep1 = lerro.find("\t");
    if (sep1 == string::npos) continue;

    string verb_lem = lerro.substr(0, sep1);
    lerro =lerro.substr(sep1+1);
    while (lerro.find("#") != string::npos) {
      string subcat = lerro.substr(0, lerro.find("#"));
      lerro = lerro.substr(lerro.find("#")+1);

      unsigned int sep2 = subcat.find('/');
      unsigned int sep3 = subcat.find('/', sep2+1);
      if (sep2 == string::npos || sep3 == string::npos) continue;

      subcategoritation current;
      current.trans = subcat.substr(0, sep2);
      current.subj_case = subcat.substr(sep2+1, sep3-sep2-1);
      current.cases = subcat.substr(sep3+1);
      
      //cerr << "'" << verb_lem << "' '" << current.trans << "' '" << current.cases << "' '" << current.subj_case << "'" << endl;
      subcategoritations[verb_lem].push_back(current);
    }
  }
  
  fitx.close();
}

string get_case (string input) {
  if (input == "[ZERO]") return "";

  if (input.rfind("/") != string::npos) 
    return input.substr(input.rfind("/")+1);

  if (input.rfind("++") != string::npos) 
    return input.substr(input.rfind("++")+2);

  if (input[0] == '[' && input[input.size()-1]==']') 
    return input.substr(1, input.size()-2);

  return input;
}

int match_subcategoritation(vector<vector<string> > &cases, string subcat_pattern){
  if (cases.size() == 0)
    return 0;

  string current_case = "";
  vector<string> current_component = cases[0];

  vector<vector<string> > best_cases=cases;
  int best_fixed=0;
  bool current_fixed;

  for (int i=0; i<current_component.size(); i++) {
    if ((current_case = get_case(current_component[i])) == "") continue;
    
    vector<vector<string> > extra_components = cases;
    extra_components.erase(extra_components.begin());

    unsigned int position = subcat_pattern.find(current_case);
    int fixed_cases = 0;
    current_fixed = false;

    if (position != string::npos &&
	(position == 0 || subcat_pattern[position-1] == '-') &&
	(position+current_case.size() == subcat_pattern.size() || subcat_pattern[position+current_case.size()] == '-')) {
      current_fixed = true;
      fixed_cases++;
      subcat_pattern.erase(position, current_case.size());

      if (subcat_pattern.find("--") != string::npos) subcat_pattern.erase(subcat_pattern.find("--"), 1);
      if (subcat_pattern[0] == '-') subcat_pattern.erase(0, 1);
      if (subcat_pattern[subcat_pattern.size()-1] == '-') subcat_pattern.erase(subcat_pattern.size()-1, 1);
    }
      
    if (subcat_pattern.size() > 0)
      fixed_cases += match_subcategoritation(extra_components, subcat_pattern);

    if (fixed_cases > best_fixed){
      best_fixed = fixed_cases;
      best_cases.clear();
      if (current_fixed) {
	vector<string> current_case;
	current_case.push_back(current_component[i]);
	best_cases.push_back(current_case);
      }
      else {
	best_cases.push_back(current_component);
      }

      for (int j=0; j < extra_components.size(); j++)
	best_cases.push_back(extra_components[j]);
    }
  }

  cases = best_cases;
  return best_fixed;
}

string verb_subcategoritation(string verb_lemma, vector<vector<string> > &cases, vector<string> &attributes, vector<string> &subj_cases, string subj_attributes, string sentenceref, int sentencealloc, config &cfg){
  vector<subcategoritation> subcat = subcategoritations[verb_lemma];

  if (cfg.DoPrepTrace) {
    cerr << "AZPIKATEGORIZAZIOA APLIKATZEN: " << verb_lemma << endl;
    string prep = text_attrib(subj_attributes, "prep"); if (prep == "") prep = "-";
    int alloc = atoi(text_attrib(subj_attributes, "alloc").c_str()) - sentencealloc;
    cerr << sentenceref << ":" << alloc << ":" << prep << "\t";
    for (int j=0; j<subj_cases.size(); j++)
      cerr << subj_cases[j] << " ";
    cerr << endl << endl;

    for (int i=0; i<cases.size(); i++) {
      prep = text_attrib(attributes[i], "prep"); if (prep == "") prep = "-";
      alloc = atoi(text_attrib(attributes[i], "alloc").c_str()) - sentencealloc;
      cerr << sentenceref << ":" << alloc << ":" << prep << "\t";
      for (int j=0; j<cases[i].size(); j++)
	cerr << cases[i][j] << " ";
      cerr << endl;
    }
    cerr << endl;
  }

  bool matches_subj, best_matches_subj;
  int best_fixed = -1;
  int best_length = 0;
  int cases_size = 0;;
  string best_trans = "DU";
  string best_subj_case;
  vector<vector<string> > best_cases = cases;
  matches_subj = best_matches_subj = false;

  for (int i=0; i<cases.size(); i++) {
    for (int j=0; j<cases[i].size(); j++) {
      if (get_case(cases[i][j]) != "") {
	cases_size++;
	break;
      }
    }
  }

  for (int i = 0; i < subcat.size(); i++) {
    vector<vector<string> > final_cases = cases;
    string subcat_pattern = subcat[i].cases;

    matches_subj = (subj_cases.size() == 0);
    for (int j=0; j<subj_cases.size(); j++) 
      if (get_case(subj_cases[j]) == subcat[i].subj_case)
	matches_subj = true;

    int fixed_cases = match_subcategoritation(final_cases, subcat_pattern);

    int subcat_length = 0;
    if (subcat_pattern.size() != 0) subcat_length++;
    for (int j=0;j<subcat_pattern.size();j++) {
      if (subcat_pattern[j] == '-') subcat_length++;
    }

    if (cfg.DoPrepTrace) cerr << subcat_pattern << "/" << subcat[i].subj_case << "/" << subcat[i].trans << " || " << fixed_cases  << "/" << cases_size << "/" << subcat_length<< endl;

    if (fixed_cases == cases_size && (matches_subj)) {
      cases = final_cases;

      if (cfg.DoPrepTrace) {
	string prep = text_attrib(subj_attributes, "prep"); if (prep == "") prep = "-";
	int alloc = atoi(text_attrib(subj_attributes, "alloc").c_str()) - sentencealloc;
	cerr << endl << "AZPIKATEGORIZAZIOAREN EMAITZA: '" << verb_lemma << "' aditzaren transitibitatea: " << subcat[i].trans << endl;
	cerr << sentenceref << ":" << alloc << ":" << prep << "\t[" << subcat[i].subj_case << "]" << endl << endl;
	for (int k=0; k<cases.size(); k++) {
	  prep = text_attrib(attributes[k], "prep"); if (prep == "") prep = "-";
	  alloc = atoi(text_attrib(attributes[k], "alloc").c_str()) - sentencealloc;
	  cerr << sentenceref << ":" << alloc << ":" << prep << "\t";
	  for (int j=0; j<cases[k].size(); j++)
	    cerr << cases[k][j] << " ";
	  cerr << endl;
	}
	cerr << endl;
      }
      

      subj_cases.clear();
      subj_cases.push_back("["+subcat[i].subj_case+"]");
      return subcat[i].trans;
    }
    else if ((matches_subj || !best_matches_subj) && (fixed_cases > best_fixed || (fixed_cases == best_fixed && best_length > subcat_length))) {
      if (matches_subj) best_subj_case = subcat[i].subj_case;
      best_fixed = fixed_cases;
      best_length = subcat_length;
      best_cases = final_cases;
      best_trans = subcat[i].trans;
    }
  }

  cases = best_cases;
  if (best_subj_case != "") {
    subj_cases.clear();
    subj_cases.push_back("["+best_subj_case+"]");
  }

  if (cfg.DoPrepTrace) {
    string prep = text_attrib(subj_attributes, "prep"); if (prep == "") prep = "-";
    int alloc = atoi(text_attrib(subj_attributes, "alloc").c_str()) - sentencealloc;
    cerr << endl << "AZPIKATEGORIZAZIOAREN EMAITZA: '" << verb_lemma << "' aditzaren transitibitatea: " << best_trans << endl;
    cerr << sentenceref << ":" << alloc << ":" << prep << "\t" << best_subj_case << endl << endl;
    for (int i=0; i<cases.size(); i++) {
      prep = text_attrib(attributes[i], "prep"); if (prep == "") prep = "-";
      alloc = atoi(text_attrib(attributes[i], "alloc").c_str()) - sentencealloc;
      cerr << sentenceref << ":" << alloc << ":" << prep << "\t";
      for (int j=0; j<cases[i].size(); j++)
	cerr << cases[i][j] << " ";
      cerr << endl;
    }
    cerr << endl;
  }
  

  return best_trans;
}

map <string, map<string, vector<string> > > verb_noum_subcat;

void init_verb_noum_subcategoritation(string fitxName, config &cfg) {
  ifstream fitx;
  fitx.open(fitxName.c_str());

  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[1638400];
    //fitx.getline(buffer,1638400);
    //string lerro = buffer;

    /*
    //cerr << lerro << endl;
    //komentarioak kendu.
    if (lerro.find('#') != string::npos)
      lerro = lerro.substr(0, lerro.find('#'));
    */

    //zuriuneak eta kendu...
    for (int i=0; i<lerro.size(); i++) {
      if (lerro[i] == ' ' and (lerro[i+1] == ' ' or lerro[i+1] == '\t'))
	lerro[i] = '\t';
      if ((lerro[i] == ' ' or lerro[i-1] == '\t') and 
	  (i == 0 or lerro[i] == '\t')) {
	lerro.erase(i,1);
	i--;
      }
    }
    if (lerro[lerro.size()-1] == ' ' or lerro[lerro.size()-1] == '\t') lerro.erase(lerro.size()-1,1);

    unsigned int sep1 = lerro.find("\t");
    if (sep1 == string::npos) continue;

    string verb_lem = lerro.substr(0, sep1);

    lerro = lerro.substr(sep1+1);
    while (lerro.find("#") != string::npos) {
      string current = lerro.substr(0, lerro.find("#"));
      lerro = lerro.substr(lerro.find("#")+1);

      unsigned int sep2 = current.find("/");
      if (sep2 == string::npos) continue;

      string chunk_head = current.substr(0, sep2);

      //cerr << verb_lem << "," << chunk_head << endl << "\t";
      
      vector<string> cases;
      current =current.substr(sep2+1) + "/";
      while (current.find("/") != string::npos) {
	string cas = current.substr(0, current.find("/"));
	cases.push_back(cas);
	
	//cerr << cas << " ";
	current = current.substr(current.find("/")+1);
      }
      //cerr << endl;
      
      verb_noum_subcat[verb_lem][chunk_head] = cases;
    }
  }

  fitx.close();
}

string to_upper(string input) {
  string output;
  
  for (int i = 0; i < input.size(); i++) {
    output += toupper(input[i]);
  }

  return output;
}

vector<string> verb_noum_subcategoritation(string verb_lemma, string chunk_head, vector<string> &cases, string &attributes, string sentenceref, int sentencealloc, config &cfg) {
  vector<string> subcat = verb_noum_subcat[verb_lemma][chunk_head];

  int alloc = atoi(text_attrib(attributes, "alloc").c_str()) - sentencealloc;
  string prep = text_attrib(attributes, "prep"); if (prep == "") prep = "-";
  if (cfg.DoPrepTrace) {
    cerr << sentenceref << ":" << alloc << ":" << prep << "\tTRIPLETEN APLIKAZIOA: " << verb_lemma << "," << chunk_head << "(";
    for (int i=0; i< subcat.size(); i++) {
      cerr << " " << subcat[i];
    }
    cerr << ") [";
    for (int i=0; i< cases.size(); i++) {
      cerr << " " << cases[i];
    }
    cerr << "]" << endl;
  }

  for (int i = 0; i < subcat.size(); i++) {
    vector<string> output;
    string subcat_cases = subcat[i];

    while (subcat_cases.size() > 0) {
      string cas;
      if (subcat_cases.find("=") != string::npos) {
	cas = subcat_cases.substr(0, subcat_cases.find("="));
	subcat_cases = subcat_cases.substr(subcat_cases.find("=")+1);
      }
      else {
	cas = subcat_cases;
	subcat_cases = "";
      }

      for (int j = 0; j < cases.size(); j++) {
	if (to_upper(cas) == get_case(cases[j])) {
	  output.push_back(cases[j]);
	  //return output;
	}
      }
    } 

    if (output.size() > 0) {
      if (cfg.DoPrepTrace) {
	cerr << sentenceref << ":" << alloc << ":" << prep << "\tTRIPLETEKIN ONDOREN CASU POSIBLEAK: ";
	for (int i=0; i<output.size(); i++) {
	  cerr << output[i] << " ";
	}
	cerr << endl << endl;
      }

      return output;
    }
  }

  /*
  vector<string> lehenetsitako_output;
  lehenetsitako_output.push_back(cases[0]);
  return lehenetsitako_output;
  */

  if (cfg.DoPrepTrace) {
    cerr << sentenceref << ":" << alloc << ":" << prep << "\tTRIPLETEKIN ONDOREN CASU POSIBLEAK: ";
    for (int i=0; i<cases.size(); i++) {
      cerr << cases[i] << " ";
    }
    cerr << endl << endl;
  }
    
  return cases;
}


struct node_order{
  string chunk_type;
  string order;
};

static vector<node_order> order_intra;

void init_node_order(string fitxName) {
  ifstream fitx;
  fitx.open(fitxName.c_str());

  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[2560];
    //fitx.getline(buffer,2560);
    //string lerro = buffer;

    unsigned int sep1 = lerro.find("\t");
    if (sep1 == string::npos) continue;

    node_order current;
    current.chunk_type = lerro.substr(0, sep1);
    current.order = lerro.substr(sep1+1, lerro.size()-sep1);

    order_intra.push_back(current);
  }

  fitx.close();
}

string get_node_order(string chunk_type, string nodes) {
  string node_order;
  vector<int> final_order;

  for (int i=0; i<order_intra.size(); i++) {
    if (order_intra[i].chunk_type == chunk_type) {
      node_order = order_intra[i].order;
    }
  }

  string current_pos;
  int current_order=0;
  unsigned int delimiter=nodes.find(')');
  while (delimiter != string::npos) {
    current_pos = nodes.substr(0,delimiter+1);
    nodes = nodes.substr(delimiter+1);
    final_order.push_back(current_order);
    //cerr << node_order << " , " << current_pos << endl;

    ostringstream order_string;
    order_string << "<" << current_order << ">";
    //if (node_order.find(current_pos) != string::npos) node_order.insert(node_order.find(current_pos), order_string.str());
    if (node_order.rfind(current_pos) != string::npos) node_order.replace(node_order.rfind(current_pos), current_pos.size(), order_string.str());
    else node_order += order_string.str();

    current_order++;
    delimiter=nodes.find(')');
  }


  delimiter = node_order.find('(');
  while (delimiter != string::npos) {
    unsigned int size = node_order.find(')') - delimiter;

    node_order.erase(delimiter, size+1);

    delimiter = node_order.find('(');
  }


  unsigned int start=node_order.find('<');
  unsigned int end=node_order.find('>');
  int current_position=0;
  while (start != string::npos && end != string::npos) {
    int old_position =atoi(node_order.substr(start+1, end-start-1).c_str());
    final_order[old_position]=current_position;
    start=node_order.find('<',end+1);
    end=node_order.find('>',end+1);
    current_position++;
  }

  ostringstream order;
  for (int i=0; i<final_order.size(); i++) {
    if (i == 0)
      order << final_order[i];
    else 
      order << "-" << final_order[i];
  }

  return order.str();
}


struct generation_order{
  string regex;
  string order;
};

static vector<generation_order> orders;

void init_generation_order(string fitxName) {
  //cerr << fitxName << endl;
  ifstream fitx;
  fitx.open(fitxName.c_str());

  generation_order gen_order;
  string lerro;
  while (getline(fitx, lerro)) {
  //while (fitx.good()) {
    //char buffer[256];
    //fitx.getline(buffer,256);
    //string lerro = buffer;

    unsigned int pos_comment = lerro.find("#");
    if (pos_comment != string::npos) continue;

    unsigned int pos_separator = lerro.rfind("\t");
    if (pos_separator == string::npos) continue;

    gen_order.regex = lerro.substr(0,pos_separator); 

    if (gen_order.regex.find("\t") != string::npos) gen_order.regex = gen_order.regex.substr(0, gen_order.regex.find("\t"));
    for (int i=0; i<gen_order.regex.size(); i++) {
      if (gen_order.regex[i] == '[' or gen_order.regex[i] == ']') {
	gen_order.regex.insert(i,"\\");
	i++;
      }
    }

    gen_order.order = lerro.substr(pos_separator+1);

    //cerr << "'" << gen_order.regex << "'" << endl;
    //cerr << "'" << gen_order.order << "'" << endl << endl;
    orders.push_back(gen_order);
  }

  fitx.close();
}

string get_generation_order(string prefix, string lemmaMorf, string number, string cas, string head_sem){
  string generation = prefix + lemmaMorf + ">" + number + ">" + cas + ">" + head_sem;

  //cerr << generation << endl;
  for (int i=0; i<orders.size(); i++) {
    //cerr << orders[i].regex << endl;
    Reg_Ex regex = Reg_Ex(orders[i].regex.c_str());
    if (regex.Search(generation.c_str())) {
      //cerr << orders[i].order << endl << endl;
      return orders[i].order;
    } 
  }
}
