/*
 *  Copyright (C) 2005 IXA Research Group / IXA Ikerkuntza Taldea.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 *  02110-1301  USA
 */

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
#include "stdbool.h"
#include "fomalib.h"

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
vector<movement> get_node_movements_bypair(wstring fromAttributes, wstring toAttributes, wstring direction);

void init_lexInfo(wstring name, string fitxName);
wstring get_lexInfo(wstring name, wstring type_es);

// deprecated?
void init_NODO_order(string fitxName);
string get_NODO_order(string pattern);

void init_verbTrasference(string fileName, bool traza);
wstring apply_verbTransference(wstring AS_source);

void init_chunk_order(string fitxName);
wstring get_chunk_order(wstring parent_attribs, wstring child_attribs, int relative_order);

void init_preposition_transference(string fitxName);
vector<wstring> preposition_transference(wstring parent_attributes, wstring child_attributes, wstring sentenceref, int sentencealloc, config &cfg);

void
  init_lexical_selection(string filename);
vector<wstring>
  lexical_selection(wstring parent_attributes, wstring common_attributes,
                    vector<wstring> child_attributes, config &cfg);

void init_verb_subcategorisation(string fitxName);
wstring verb_subcategorisation(wstring verb_lemma, vector<vector<wstring> > &cases, vector<wstring> &attributes, vector<wstring> &subj_cases, wstring subj_attributes, wstring sentenceref, int sentencealloc, config &cfg);

void init_verb_noun_subcategorisation(string fitxName);
vector<wstring> verb_noun_subcategorisation(wstring verb_lemma, wstring chunk_head, vector<wstring> &cases, wstring &attributes, wstring sentenceref, int sentencealloc, config &cfg);

void init_node_order(string fitxName);
wstring get_node_order(wstring chunk_type, wstring nodes);

void init_generation_order(string fitxName);
wstring get_generation_order(wstring prefix, wstring lemmaMorf, wstring number, wstring kas, wstring head_sem);

