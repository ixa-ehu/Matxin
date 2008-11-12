#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

#include "config.h"

#include "simpleregex.h"
#include "XML_reader.h"

using namespace std;

struct chunk{
  string condition;
  string attrib;
};

struct movement{
  chunk from;
  chunk to;
  string direction;
  string write_type;
};

void init_chunkMovement(string fitxName);
vector<movement> get_chunk_movements(string fromAttributes, string toAttributes, string direction);

void init_nodeMovement(string fitxName);
vector<movement> get_node_movements_byfrom(string attributes);
vector<movement> get_node_movements_byto(string attributes);

void init_lexInfo(string name,string fitxName);
string get_lexInfo(string name, string type_es);

void init_NODO_order(string fitxName);
string get_NODO_order(string pattern);

void init_verbTrasference(string fileName, bool traza);
string apply_verbTransference(string AS_source);

void init_chunk_order(string fitxName);
string get_chunk_order(string parent_type, string child_type, int relative_order);

void init_preposition_transference(string fitxName, config &cfg);
vector<string> preposition_transference(string parent_attributes, string child_attributes, string sentenceref, int sentencealloc, config &cfg);

void init_verb_subcategoritation(string fitxName, config &cfg);
string verb_subcategoritation(string verb_lemma, vector<vector<string> > &cases, vector<string> &attributes, vector<string> &subj_cases, string subj_attributes, string sentenceref, int sentencealloc, config &cfg);

void init_verb_noum_subcategoritation(string fitxName, config &cfg);
vector<string> verb_noum_subcategoritation(string verb_lemma, string chunk_head, vector<string> &cases, string &attributes, string sentenceref, int sentencealloc, config &cfg);

void init_node_order(string fitxName);
string get_node_order(string chunk_type, string nodes);

void init_generation_order(string fitxName);
string get_generation_order(string prefix, string lemmaMorf, string number, string kas, string head_sem);
