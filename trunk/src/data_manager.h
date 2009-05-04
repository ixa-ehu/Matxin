#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

#include "config.h"

#include "simpleregex.h"
#include "XML_reader.h"
#include "utf_converter.h"

using namespace std;

struct chunk {
  wstring condition;
  wstring attrib;
};

struct movement {
  chunk from;
  chunk to;
  wstring direction;
  wstring write_type;
};

void init_chunkMovement(string fitxName);
vector<movement> get_chunk_movements(wstring fromAttributes, wstring toAttributes, wstring direction);

void init_nodeMovement(string fitxName);
vector<movement> get_node_movements_byfrom(wstring attributes);
vector<movement> get_node_movements_byto(wstring attributes);

void init_lexInfo(wstring name, string fitxName);
wstring get_lexInfo(wstring name, wstring type_es);

// deprecated?
void init_NODO_order(string fitxName);
string get_NODO_order(string pattern);

void init_verbTrasference(string fileName, bool traza);
wstring apply_verbTransference(wstring AS_source);

void init_chunk_order(string fitxName);
wstring get_chunk_order(wstring parent_type, wstring child_type, int relative_order);

void init_preposition_transference(string fitxName, config &cfg);
vector<wstring> preposition_transference(wstring parent_attributes, wstring child_attributes, wstring sentenceref, int sentencealloc, config &cfg);

void init_verb_subcategoritation(string fitxName, config &cfg);
wstring verb_subcategoritation(wstring verb_lemma, vector<vector<wstring> > &cases, vector<wstring> &attributes, vector<wstring> &subj_cases, wstring subj_attributes, wstring sentenceref, int sentencealloc, config &cfg);

void init_verb_noum_subcategoritation(string fitxName, config &cfg);
vector<wstring> verb_noum_subcategoritation(wstring verb_lemma, wstring chunk_head, vector<wstring> &cases, wstring &attributes, wstring sentenceref, int sentencealloc, config &cfg);

void init_node_order(string fitxName);
wstring get_node_order(wstring chunk_type, wstring nodes);

void init_generation_order(string fitxName);
wstring get_generation_order(wstring prefix, wstring lemmaMorf, wstring number, wstring kas, wstring head_sem);

