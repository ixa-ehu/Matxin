//////////////////////////////////////////////////////////////////
//
//    FreeLing - Open Source Language Analyzers
//
//    Copyright (C) 2004   TALP Research Center
//                         Universitat Politecnica de Catalunya
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//    contact: Lluis Padro (padro@lsi.upc.es)
//             TALP Research Center
//             despatx C6.212 - Campus Nord UPC
//             08034 Barcelona.  SPAIN
//
////////////////////////////////////////////////////////////////


//------------------------------------------------------------------//
//
//                    IMPORTANT NOTICE
//
//  This file contains a simple main program to illustrate 
//  usage of FreeLing analyzers library.
//
//  This sample main program may be used straightforwardly as 
//  a basic front-end for the analyzers (e.g. to analyze corpora)
//
//  Neverthless, if you want embed the FreeLing libraries inside
//  a larger application, or you want to deal with other 
//  input/output formats (e.g. XML), the efficient and elegant 
//  way to do so is consider this file as a mere example, and call 
//  the library from your your own main code.
//
//------------------------------------------------------------------//



using namespace std;

#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#include <map>
#include <vector>

#include "util.h"
#include "tokenizer.h"
#include "splitter.h"
#include "maco.h"
#include "nec.h"
#include "senses.h"
#include "tagger.h"
#include "hmm_tagger.h"
#include "relax_tagger.h"
#include "chart_parser.h"
#include "maco_options.h"
#include "config.h"
#include "dependencies.h"

void error(char *msg) {
  perror(msg);
  exit(0);
}

void PrintDepTree(dep_tree::iterator n, int depth, const config &cfg) {
  dep_tree::sibling_iterator d,dm;
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

void PrintCHUNK(sentence &s, parse_tree & fulltree, dep_tree &tr, dep_tree::iterator n, int depth, int newsockfd, const bool printHead=false);

void PrintNODE(sentence & s, parse_tree & fulltree, dep_tree &tr, dep_tree::iterator n, int depth, int newsockfd, const bool printHead=false);

//---------------------------------------------
// print obtained analysis.
//---------------------------------------------
void PrintResults(list<sentence> &ls, const config &cfg, int &nsentence, int newsockfd) {
  word::const_iterator ait;
  sentence::const_iterator w;
  list<sentence>::iterator is;
  parse_tree tr;   
  char buff[20480];
 
  for (is=ls.begin(); is!=ls.end(); is++,++nsentence) {
   
    dep_tree &dep = is->get_dep_tree();

    //sort(dep.begin(), dep.end());
    if (dep.num_children() == 0) {nsentence--; continue;}

    sprintf(buff, "<SENTENCE ord='%d'>\n", nsentence);
    write(newsockfd, buff, strlen(buff));

    //PrintDepTree(dep.begin(), 0, cfg);
    PrintCHUNK(*is,tr,dep,dep.begin(), 0, newsockfd);

    sprintf(buff, "</SENTENCE>\n\n");
    write(newsockfd, buff, strlen(buff));
  } 
}

//---------------------------------------------
// Plain text, start with tokenizer.
//---------------------------------------------
void ProcessPlain(const config &cfg, tokenizer *tk, splitter *sp, maco *morfo, POS_tagger *tagger, nec* neclass, senses* sens, chart_parser *parser, dependencyMaker *dep, int newsockfd) {
  string text;
  list<word> av;
  list<word>::const_iterator i;
  list<sentence> ls;
  int nsentence=1;
  char buf[20480];

  int offset=0;
  int len = read(newsockfd,buf,20480);buf[len]='\0';text=buf;
  while (text.size() < 15 || text.substr(text.size()-15) != "/*END OF FILE*/") {
      av=tk->tokenize(text, offset);
      ls=sp->split(av, cfg.AlwaysFlush);
      morfo->analyze(ls);
      tagger->analyze(ls);
      if (cfg.NEC_NEClassification) neclass->analyze(ls);
      if (cfg.SENSE_SenseAnnotation!=NONE) sens->analyze(ls);
      parser->analyze(ls);
      dep->analyze(ls);

      PrintResults(ls, cfg, nsentence, newsockfd);

      //offset++; // count \n like a character
      av.clear(); // clear list of words for next use
      ls.clear(); // clear list of sentences for next use

      len = read(newsockfd,buf,20480);buf[len]='\0';text=buf;
    }

    // process last sentence in buffer (if any)
    text = text.substr(0, text.size()-15);
    av=tk->tokenize(text, offset);
    ls=sp->split(av, true);  //flush splitter buffer
    morfo->analyze(ls);
    tagger->analyze(ls);
    if (cfg.NEC_NEClassification) neclass->analyze(ls);
    if (cfg.SENSE_SenseAnnotation!=NONE) sens->analyze(ls);
    parser->analyze(ls);
    dep->analyze(ls);

    PrintResults(ls, cfg, nsentence, newsockfd);
}


//---------------------------------------------
// Sample main program
//---------------------------------------------
int main(int argc, char **argv) {

  int sockfd, newsockfd, servlen, clilen, n;
  struct sockaddr_un  cli_addr, serv_addr;

  if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
    error("creating socket");

  serv_addr.sun_family = AF_UNIX;

  remove("/tmp/matxin_freeling_sock");
  strcpy(serv_addr.sun_path, "/tmp/matxin_freeling_sock");
  servlen=strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
  if(bind(sockfd,(struct sockaddr *)&serv_addr,servlen)<0)
    error("binding socket"); 
  listen(sockfd,20);


  // we use pointers to the analyzers, so we
  // can create only those strictly necessary.
  tokenizer *tk=NULL;
  splitter *sp=NULL;
  maco *morfo=NULL;
  nec *neclass=NULL;
  senses *sens=NULL;
  POS_tagger *tagger=NULL;
  chart_parser *parser=NULL;
  dependencyMaker *dep=NULL;

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
			 cfg.MACO_NERecognition);
  // decimal/thousand separators used by number detection
  opt.set_nummerical_points(cfg.MACO_Decimal, cfg.MACO_Thousand);
  // Minimum probability for a tag for an unkown word
  opt.set_threshold(cfg.MACO_ProbabilityThreshold);
  // Data files for morphological submodules. by default set to ""
  // Only files for active modules have to be specified 
  opt.set_data_files(cfg.MACO_LocutionsFile,   cfg.MACO_CurrencyFile,   cfg.MACO_SuffixFile, 
		     cfg.MACO_ProbabilityFile, cfg.MACO_DictionaryFile, cfg.MACO_NPdataFile,
		     cfg.MACO_PunctuationFile);
  // create analyzer with desired options
  morfo = new maco(opt);

  if (cfg.TAGGER_which == HMM)
    tagger = new hmm_tagger(cfg.Lang, cfg.TAGGER_HMMFile, cfg.TAGGER_Retokenize);
  else if (cfg.TAGGER_which == RELAX)
    tagger = new relax_tagger(cfg.TAGGER_RelaxFile, cfg.TAGGER_RelaxMaxIter, 
			      cfg.TAGGER_RelaxScaleFactor, cfg.TAGGER_RelaxEpsilon, cfg.TAGGER_Retokenize); 

  if (cfg.NEC_NEClassification)
    neclass = new nec("NP", cfg.NEC_FilePrefix);

  if (cfg.SENSE_SenseAnnotation!=NONE)
    sens = new senses(cfg.SENSE_SenseFile);

  parser = new chart_parser(cfg.PARSER_GrammarFile); 
 
  dep = new dependencyMaker(cfg.DEP_GrammarFile, parser->get_start_symbol());


  while (true) {
    //iosockunix is = sunb.accept ();
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *)&cli_addr,(socklen_t *) &clilen);
    if (newsockfd < 0) 
      error("accepting");
    
    //PROFIT
    write(newsockfd, "<?xml version='1.0' encoding='iso-8859-1'?>\n", 43);
    write(newsockfd, "<corpus>\n", 9);
    
    
    // Input is plain text.
    ProcessPlain(cfg,tk,sp,morfo,tagger,neclass,sens,parser,dep, newsockfd);
    
    //PROFIT
    write(newsockfd, "</corpus>\n", 10);
    close(newsockfd);
  }
  
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
  unsigned int pos;
  if((pos=s.find("&"))!=string::npos) {
     s.replace(pos,1,"&amp;");
  }

  if((pos=s.find("'"))!=string::npos) {
    s.replace(pos,1,"&apos;");
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

void PrintCHUNK(sentence & s, parse_tree & fulltree, dep_tree &tr, dep_tree::iterator n, int depth, int newsockfd, const bool printHead) {
  dep_tree::iterator dm;
//  dep_tree::sibling_iterator d,dm;
  int last, min;
  bool trob;
  char buff[20480];


  if(n->info.is_chunk()) {
    //cerr << "Freeling server: <CHUNK ord=" << n->info.get_chunk_ord();
    //cerr << "' type='" << xmlencode(n->info.get_link()->info.get_label());
    //cerr << "' si='" << xmlencode(n->info.get_label()) << "'>" << endl;

    sprintf(buff, "<CHUNK ord='%d' type='%s' si='%s'>\n", n->info.get_chunk_ord(), xmlencode(n->info.get_link()->info.get_label()).c_str(), xmlencode(n->info.get_label()).c_str()); 
    write(newsockfd, buff, strlen(buff));
  
    PrintNODE(s,fulltree,tr, n, depth, newsockfd, printHead);
  }

  // Print CHUNKS
  /*
  last=0; trob=true;
  while (trob) { 

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


    if (trob) PrintCHUNK(s,fulltree,tr, dm, depth, newsockfd,printHead);
    last=min;
  }
  */

  map<int, dep_tree::iterator> chunks;
  map<int, dep_tree::iterator>::iterator c;
  
  get_chunks(n, chunks);
  
  for (c = chunks.begin(); c != chunks.end(); c++) {
    dm = c->second;
    PrintCHUNK(s,fulltree,tr, dm, depth, newsockfd,printHead);
  }
  
  if(n->info.is_chunk()) write(newsockfd, "</CHUNK>\n", 9);
  
}

void PrintNODE(sentence & s, parse_tree & fulltree, dep_tree &tr, dep_tree::iterator n, int depth, int newsockfd, const bool printHead) {
  dep_tree::sibling_iterator d;
  char buff[20480];
  
  word w=n->info.get_word();

  //cerr << "Freeling server: <NODE ord='" << wordPosition(s,w) << "'";
  //cerr << " alloc='" << w.get_span_start() << "'";
  //cerr << " form='" << xmlencode(w.get_form()) << "'";
  //cerr << " lem='" << xmlencode(w.get_lemma()) << "'";
  //cerr << " mi='" << xmlencode(w.get_parole()) << "'>" << endl;

  sprintf(buff, "<NODE ord='%d'", wordPosition(s,w));
  sprintf(buff, "%s alloc='%d'", buff, w.get_span_start());
  sprintf(buff, "%s form='%s'", buff, xmlencode(w.get_form()).c_str());
  sprintf(buff, "%s lem='%s'", buff, xmlencode(w.get_lemma()).c_str());
  sprintf(buff, "%s mi='%s'>\n", buff, xmlencode(w.get_parole()).c_str());
  write(newsockfd, buff, strlen(buff));    
    
  //Print Nodes
  for (d=n->sibling_begin(); d!=n->sibling_end(); ++d) {
    if(!d->info.is_chunk()) {
	
      PrintNODE(s,fulltree,tr, d, depth+1, newsockfd,printHead);
      
    }
  }

  write(newsockfd, "</NODE>\n", 8);
}

