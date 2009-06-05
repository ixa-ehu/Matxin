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

using namespace std;

#include <sstream>
#include <iostream>

#include <map>
#include <vector>

#include "config.h"
#include "freeling.h"


void PrintDepTree(dep_tree::iterator n, int depth, const config &cfg) {
  dep_tree::sibling_iterator d, dm;
  int last, min;
  bool trob;
  
    cout<<string(depth*2,' '); 

    cout<<n->info.get_link()->info.get_label()<<"/"<<n->info.get_label()<<"/";
    word w=n->info.get_word();
    cout<<"("<<w.get_form()<<" "<<w.get_lemma()<<" "<<w.get_parole()<<")";  

    if (n->num_children()>0) { 
       cout<<" ["<<endl; 

       //Print Nodes
       for (d=n->sibling_begin(); d!=n->sibling_end(); ++d)
	 if(!d->info.is_chunk())
	   PrintDepTree(d, depth+1, cfg);

       // print CHUNKS (in order)
       last=0; trob=true;
       while (trob) { 
	 // while an unprinted chunk is found look, for the one with lower chunk_ord value
	 trob=false; min=9999;  
	 for (d=n->sibling_begin(); d!=n->sibling_end(); ++d) {
	   if(d->info.is_chunk()) {
	     if (d->info.get_chunk_ord()>last && d->info.get_chunk_ord()<min) {
	       min=d->info.get_chunk_ord();
	       dm=d;
	       trob=true;
	     }
	   }
	 }
	 if (trob) PrintDepTree(dm, depth+1, cfg);
	 last=min;
       }

       cout<<string(depth*2,' ')<<"]"; 
    }
    cout<<endl;
}

int get_alloc(dep_tree::iterator n, bool subChunks) {
  int alloc=(n->info.get_word()).get_span_start();

  dep_tree::sibling_iterator d;
  for (d=n->sibling_begin(); d!=n->sibling_end(); ++d) {
    if (subChunks || !d->info.is_chunk()) {
      int sub_alloc = get_alloc(d, subChunks);
      if (sub_alloc < alloc) alloc = sub_alloc;
    }
  }     
  
  return alloc;
}


string PrintCHUNK(sentence &s, parse_tree & fulltree, dep_tree &tr, dep_tree::iterator n, int depth, const bool printHead=false);

string PrintNODE(sentence & s, parse_tree & fulltree, dep_tree &tr, dep_tree::iterator n, int depth, const bool printHead=false);

//---------------------------------------------
// print obtained analysis.
//---------------------------------------------
void PrintResults(list<sentence> &ls, const config &cfg, int &nsentence) {
  word::const_iterator ait;
  sentence::const_iterator w;
  list<sentence>::iterator is;
  parse_tree tr;   

  for (is=ls.begin(); is!=ls.end(); is++,++nsentence) {
    ostringstream tree; 
   
    dep_tree &dep = is->get_dep_tree();

    //sort(dep.begin(), dep.end());
    //if (dep.num_children() == 0) {nsentence--; continue;}


    tree << "<SENTENCE ord='" << nsentence << "' alloc='" << get_alloc(dep.begin(), true) << "'>" << endl;

    //PrintDepTree(dep.begin(), 0, cfg);
    tree << PrintCHUNK(*is,tr,dep,dep.begin(), 0);

    tree << "</SENTENCE>" << endl;
    cout << tree.str();

    if (cfg.DoTrace) {
      ostringstream log_fileName_osoa;
      ofstream log_file;

      log_fileName_osoa << cfg.Trace_File << (nsentence-1) << ".xml";
      log_file.open(log_fileName_osoa.str().c_str(), ofstream::out);

      log_file << "<?xml version='1.0' encoding='UTF-8' ?>" << endl;
      log_file << "<?xml-stylesheet type='text/xsl' href='profit.xsl'?>" << endl;
      log_file << "<corpus>\n";
      log_file << tree.str();
      log_file.close();
    }

  } 
}

//---------------------------------------------
// Plain text, start with tokenizer.
//---------------------------------------------
void ProcessPlain(const config &cfg, tokenizer *tk, splitter *sp, maco *morfo, POS_tagger *tagger, nec* neclass, senses* sens, chart_parser *parser, dependency_parser *dep) {
  string text;
  list<word> av;
  list<word>::const_iterator i;
  list<sentence> ls;
  int nsentence=1;

  long unsigned int offset=0;
  while (std::getline(std::cin,text)) {
      av=tk->tokenize(text, offset);
      ls=sp->split(av, cfg.AlwaysFlush);
      morfo->analyze(ls);
      if (cfg.SENSE_SenseAnnotation!=NONE) sens->analyze(ls);
      tagger->analyze(ls);
      if (cfg.NEC_NEClassification) neclass->analyze(ls);
      parser->analyze(ls);
      dep->analyze(ls);

      PrintResults(ls, cfg, nsentence);

      //offset++; // count \n like a character
      av.clear(); // clear list of words for next use
      ls.clear(); // clear list of sentences for next use
    }

    // process last sentence in buffer (if any)
    av=tk->tokenize(text, offset);
    ls=sp->split(av, true);  //flush splitter buffer
    morfo->analyze(ls);
    if (cfg.SENSE_SenseAnnotation!=NONE) sens->analyze(ls);
    tagger->analyze(ls);
    if (cfg.NEC_NEClassification) neclass->analyze(ls);
    parser->analyze(ls);
    dep->analyze(ls);

    PrintResults(ls, cfg, nsentence);
}


//---------------------------------------------
// Sample main program
//---------------------------------------------
int main(int argc, char **argv) {

  // we use pointers to the analyzers, so we
  // can create only those strictly necessary.
  tokenizer *tk=NULL;
  splitter *sp=NULL;
  maco *morfo=NULL;
  nec *neclass=NULL;
  senses *sens=NULL;
  POS_tagger *tagger=NULL;
  chart_parser *parser=NULL;
  dependency_parser *dep=NULL;

  // read configuration file and command-line options
  config cfg(argv);

  // create required analyzers
  tk = new tokenizer(cfg.TOK_TokenizerFile); 
  sp = new splitter(cfg.SPLIT_SplitterFile);

  // the morfo class requires several options at creation time.
  // they are passed packed in a maco_options object.
  maco_options opt(cfg.Lang);
  // boolean options to activate/desactivate modules
  // default: all modules activated (options set to "false")
  opt.set_active_modules(cfg.MACO_SuffixAnalysis,   cfg.MACO_MultiwordsDetection, 
			 cfg.MACO_NumbersDetection, cfg.MACO_PunctuationDetection, 
			 cfg.MACO_DatesDetection,   cfg.MACO_QuantitiesDetection,
			 cfg.MACO_DictionarySearch, cfg.MACO_ProbabilityAssignment,
			 cfg.MACO_NER_which);
  // decimal/thousand separators used by number detection
  opt.set_nummerical_points(cfg.MACO_Decimal, cfg.MACO_Thousand);
  // Minimum probability for a tag for an unkown word
  opt.set_threshold(cfg.MACO_ProbabilityThreshold);
  // Data files for morphological submodules. by default set to ""
  // Only files for active modules have to be specified 
  opt.set_data_files(cfg.MACO_LocutionsFile,   cfg.MACO_QuantitiesFile, cfg.MACO_SuffixFile, 
		     cfg.MACO_ProbabilityFile, cfg.MACO_DictionaryFile, cfg.MACO_NPdataFile,
		     cfg.MACO_PunctuationFile);
  // create analyzer with desired options
  morfo = new maco(opt);

  if (cfg.TAGGER_which == HMM)
    tagger = new hmm_tagger(cfg.Lang, cfg.TAGGER_HMMFile, cfg.TAGGER_Retokenize, cfg.TAGGER_ForceSelect);
  else if (cfg.TAGGER_which == RELAX)
    tagger = new relax_tagger(cfg.TAGGER_RelaxFile, cfg.TAGGER_RelaxMaxIter, 
			      cfg.TAGGER_RelaxScaleFactor, cfg.TAGGER_RelaxEpsilon,
			      cfg.TAGGER_Retokenize, cfg.TAGGER_ForceSelect); 

  if (cfg.NEC_NEClassification)
    neclass = new nec("NP", cfg.NEC_FilePrefix);

  if (cfg.SENSE_SenseAnnotation!=NONE)
    sens = new senses(cfg.SENSE_SenseFile, cfg.SENSE_DuplicateAnalysis);

  parser = new chart_parser(cfg.PARSER_GrammarFile); 
 
  if (cfg.DEP_which == TXALA)
    dep = new dep_txala(cfg.DEP_TxalaFile, parser->get_start_symbol ());
  else if (cfg.DEP_which == MALT) {
    cerr <<"Error - Malt Parser was requested, but malt-plugin was not built." <<endl;
    cerr <<"        Use option --enable-maltplugin when running ./configure"<<endl;	
    exit (1);
  }

  //PROFIT
  cout << "<?xml version='1.0' encoding='UTF-8' ?>" << endl;
  if (cfg.write_xslt) cout << "<?xml-stylesheet type='text/xsl' href='profit.xsl'?>" << endl;
  cout << "<corpus>" << endl;
    
    
  // Input is plain text.
  ProcessPlain(cfg,tk,sp,morfo,tagger,neclass,sens,parser,dep);
    
  //PROFIT
  cout << "</corpus>"  << endl;

  // clean up. Note that deleting a null pointer is a safe (yet useless) operation
  delete tk;
  delete sp; 
  delete morfo; 
  delete tagger;
  delete neclass; 
  delete sens;
  delete parser;
  delete dep;
}

//
// CODI PROFIT
//

string xmlencode(string s) {
  size_t pos=0;
  while ((pos=s.find("&", pos))!=string::npos) {
     s.replace(pos,1,"&amp;");
     pos += 4;
  }

  while ((pos=s.find('"'))!=string::npos) {
    s.replace(pos,1,"&quot;");
  }

  while ((pos=s.find("'"))!=string::npos) {
    s.replace(pos,1,"&apos;");
  }

  while ((pos=s.find("<"))!=string::npos) {
    s.replace(pos,1,"&lt;");
  }

  while ((pos=s.find(">"))!=string::npos) {
    s.replace(pos,1,"&gt;");
  }

  return s;
}

int wordPosition(const sentence & s,  const word & w) {
  sentence::const_iterator i=s.begin();
  int position=0;
  bool found=false;
  while(i!=s.end() && !found) { 
       found=( w.get_span_start()==i->get_span_start() && w.get_span_finish()==i->get_span_finish());
       ++position;
       ++i;
   } 
  if(!found) { cerr<<"INTERNAL ERROR in determinig Word Position"<<endl;}
  return position;
}

void get_chunks(dep_tree::iterator n, map<int, dep_tree::iterator> &chunks) {
  dep_tree::sibling_iterator d;

  for (d=n->sibling_begin(); d!=n->sibling_end(); ++d){
    if (d->info.is_chunk()) {
      int pos = d->info.get_chunk_ord();
      chunks[pos] = d;
    }
    else 
      get_chunks(d, chunks);
  }
}

string PrintCHUNK(sentence & s, parse_tree & fulltree, dep_tree &tr, dep_tree::iterator n, int depth, const bool printHead) {
  dep_tree::iterator dm;
//  dep_tree::sibling_iterator d,dm;
  ostringstream  tree;

  //if(n->info.is_chunk()) {
    tree << string(depth*2,' '); 

    tree << "<CHUNK ord='" << n->info.get_chunk_ord() << "' alloc='"<< get_alloc(n, false) << "'";
    tree << " type='" << xmlencode(n->info.get_link()->info.get_label()).c_str() << "'";
    tree << " si='" << xmlencode(n->info.get_label()).c_str() << "'>" << endl;
  
    tree << PrintNODE(s,fulltree,tr, n, depth+1, printHead);
  //}

  map<int, dep_tree::iterator> chunks;
  map<int, dep_tree::iterator>::iterator c;
  
  get_chunks(n, chunks);
  
  for (c = chunks.begin(); c != chunks.end(); c++) {
    dm = c->second;
    tree << PrintCHUNK(s,fulltree,tr, dm, depth+1, printHead);
  }
  
  //if(n->info.is_chunk()) tree << string(depth*2,' ') << "</CHUNK>" << endl;
  tree << string(depth*2,' ') << "</CHUNK>" << endl;
  
  return tree.str();
}

string PrintNODE(sentence & s, parse_tree & fulltree, dep_tree &tr, dep_tree::iterator n, int depth, const bool printHead) {
  ostringstream tree;
  dep_tree::sibling_iterator d;
  
  word w=n->info.get_word();

  tree << string(depth*2,' '); 

  tree << "<NODE ord='" << wordPosition(s,w) << "'";
  tree << " alloc='" << w.get_span_start() << "'";
  tree << " form='" << xmlencode(w.get_form()).c_str() << "'";
  tree << " lem='" << xmlencode(w.get_lemma()).c_str() << "'";
  tree << " mi='" << xmlencode(w.get_parole()).c_str() << "'>" << endl;
    
  //Print Nodes
  for (d=n->sibling_begin(); d!=n->sibling_end(); ++d) {
    if(!d->info.is_chunk()) {
	
      tree << PrintNODE(s,fulltree,tr, d, depth+1, printHead);
      
    }
  }

  tree << string(depth*2,' ') << "</NODE>" << endl;

  return tree.str();
}

