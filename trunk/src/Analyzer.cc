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
#include "IORedirectHandler.hpp"


void PrintDepTree(dep_tree::iterator n, int depth, const config &cfg)
{
  dep_tree::sibling_iterator d, dm;
  int last, min;
  bool trob;

    wcout << wstring(depth * 2, ' ');

    wcout << n->info.get_link()->info.get_label() << L"/" << n->info.get_label() << L"/";
    word w = n->info.get_word();
    wcout << L"(" << w.get_form() << L" " << w.get_lemma() << L" " << w.get_tag() << L")";

    if (n->num_children() > 0)
    {
       wcout<<L" ["<<endl;

       //Print Nodes
       for (d = n->sibling_begin(); d != n->sibling_end(); ++d)
         if (!d->info.is_chunk())
           PrintDepTree(d, depth + 1, cfg);

       // print CHUNKS (in order)
       last = 0;
       trob = true;
       while (trob)
       {
         // while an unprinted chunk is found look, for the one with lower chunk_ord value
         trob = false;
         min = 9999;
         for (d = n->sibling_begin(); d != n->sibling_end(); ++d)
         {
           if (d->info.is_chunk())
           {
             if (d->info.get_chunk_ord()>last && d->info.get_chunk_ord()<min)
             {
               min = d->info.get_chunk_ord();
               dm = d;
               trob = true;
             }
           }
         }
         if (trob)
           PrintDepTree(dm, depth + 1, cfg);
         last = min;
       }

       wcout << wstring(depth * 2,' ') << L"]";
    }
    wcout << endl;
}


int get_alloc(dep_tree::iterator n, bool subChunks)
{
  int alloc = (n->info.get_word()).get_span_start();

  dep_tree::sibling_iterator d;
  for (d = n->sibling_begin(); d != n->sibling_end(); ++d)
  {
    if (subChunks || !d->info.is_chunk())
    {
      int sub_alloc = get_alloc(d, subChunks);
      if (sub_alloc < alloc)
        alloc = sub_alloc;
    }
  }

  return alloc;
}

wstring PrintCHUNK(sentence &s, parse_tree & fulltree, dep_tree &tr,
                  dep_tree::iterator n, int depth, const bool printHead=false);

wstring PrintNODE(sentence & s, parse_tree & fulltree, dep_tree &tr,
                 dep_tree::iterator n, int depth, const bool printHead=false);

//---------------------------------------------
// print obtained analysis.
//---------------------------------------------
void PrintResults(list<sentence> &ls, const config &cfg, int &nsentence)
{
  word::const_iterator ait;
  sentence::const_iterator w;
  list<sentence>::iterator is;
  parse_tree tr;

  for (is = ls.begin(); is != ls.end(); is++, ++nsentence)
  {
    wostringstream tree;

    dep_tree &dep = is->get_dep_tree();

    tree << L"<SENTENCE ord='" << nsentence << L"' alloc='"
         << get_alloc(dep.begin(), true) << L"'>" << endl;

    tree << PrintCHUNK(*is,tr,dep,dep.begin(), 0);

    tree << L"</SENTENCE>" << endl;
    wcout << tree.str();

    if (cfg.DoTrace)
    {
      ostringstream log_fileName_osoa;
      wofstream log_file;

      log_fileName_osoa.imbue(std::locale("C"));
      log_fileName_osoa << cfg.Trace_File << (nsentence - 1) << ".xml";
      log_file.open(log_fileName_osoa.str().c_str(), wfstream::out);

      log_file << L"<?xml version='1.0' encoding='UTF-8' ?>" << endl;
      log_file << L"<?xml-stylesheet type='text/xsl' href='profit.xsl'?>" << endl;
      log_file << L"<corpus>" << endl;
      log_file << L"<!-- Analysis-->" << endl;
      log_file << tree.str();
      log_file.close();
    }

  }
}


//---------------------------------------------
// Plain text, start with tokenizer.
//---------------------------------------------
void ProcessPlain(const config &cfg, tokenizer *tk, splitter *sp, maco *morfo,
                  POS_tagger *tagger, nec* neclass, chart_parser *parser,
		  dependency_parser *dep)
{
  wstring text;
  list<word> av;
  list<word>::const_iterator i;
  list<sentence> ls;
  int nsentence = 1;

  long unsigned int offset = 0;
  while (true)
  {
    CppWIORedirectHandler ioredirect(cfg);

    wcout << L"<?xml version='1.0' encoding='UTF-8' ?>" << endl;
    if (cfg.write_xslt)
      wcout << L"<?xml-stylesheet type='text/xsl' href='profit.xsl'?>" << endl;
    wcout << L"<corpus>" << endl;

    while (std::getline(std::wcin, text))
    {
      av = tk->tokenize(text, offset);
      ls = sp->split(av, cfg.AlwaysFlush);
      morfo->analyze(ls);
      tagger->analyze(ls);
      if (cfg.NEC_NEClassification)
        neclass->analyze(ls);
      parser->analyze(ls);
      dep->analyze(ls);
      
      PrintResults(ls, cfg, nsentence);
      
      av.clear(); // clear list of words for next use
      ls.clear(); // clear list of sentences for next use
      text.clear(); // necessary for the case when the input file does not end in '\n'
    }

    // process last sentence in buffer (if any)
    av = tk->tokenize(text, offset);
    ls = sp->split(av, true);  //flush splitter buffer
    morfo->analyze(ls);
    tagger->analyze(ls);
    if (cfg.NEC_NEClassification)
      neclass->analyze(ls);
    parser->analyze(ls);
    dep->analyze(ls);

    PrintResults(ls, cfg, nsentence);
    wcout << L"</corpus>" << endl;

    if (!ioredirect.serverOK())
      break;
  }
}


//---------------------------------------------
// Sample main program
//---------------------------------------------
int main(int argc, char **argv)
{
  // we use pointers to the analyzers, so we
  // can create only those strictly necessary.
  tokenizer *tk = NULL;
  splitter *sp = NULL;
  maco *morfo = NULL;
  nec *neclass = NULL;
  POS_tagger *tagger = NULL;
  chart_parser *parser = NULL;
  dependency_parser *dep = NULL;

  // read configuration file and command-line options
  config cfg(argc, argv);

  /// set the locale to UTF to properly handle special characters.
  util::init_locale(cfg.Locale);

  // create required analyzers
  tk = new tokenizer(cfg.TOK_TokenizerFile);
  sp = new splitter(cfg.SPLIT_SplitterFile);

  // the morfo class requires several options at creation time.
  // they are passed packed in a maco_options object.
  maco_options opt(cfg.Lang);
  // boolean options to activate/desactivate modules
  // default: all modules activated (options set to "false")
  opt.set_active_modules (cfg.MACO_UserMap,
			  cfg.MACO_AffixAnalysis,
			  cfg.MACO_MultiwordsDetection,
			  cfg.MACO_NumbersDetection,
			  cfg.MACO_PunctuationDetection,
			  cfg.MACO_DatesDetection,
			  cfg.MACO_QuantitiesDetection,
			  cfg.MACO_DictionarySearch,
			  cfg.MACO_ProbabilityAssignment,
			  cfg.MACO_NERecognition,
			  cfg.MACO_OrthographicCorrection);
  // decimal/thousand separators used by number detection
  opt.set_nummerical_points(cfg.MACO_Decimal, cfg.MACO_Thousand);
  // Minimum probability for a tag for an unkown word
  opt.set_threshold(cfg.MACO_ProbabilityThreshold);
  // Whether the dictionary offers inverse acces (lemma#pos -> form). 
  // Only needed if your application is going to do such an access.
  opt.set_inverse_dict(false);
  // Whether contractions are splitted by the dictionary right away,
  // or left for later "retok" option to decide.
  opt.set_retok_contractions(cfg.MACO_RetokContractions);
  // Data files for morphological submodules. by default set to ""
  // Only files for active modules have to be specified
  opt.set_data_files (cfg.MACO_UserMapFile,
		      cfg.MACO_LocutionsFile, cfg.MACO_QuantitiesFile,
		      cfg.MACO_AffixFile, cfg.MACO_ProbabilityFile,
		      cfg.MACO_DictionaryFile, cfg.MACO_NPDataFile,
		      cfg.MACO_PunctuationFile,cfg.MACO_CorrectorFile);
  // create analyzer with desired options
  morfo = new maco(opt);

  if (cfg.TAGGER_which == HMM)
    // tagger = new hmm_tagger(cfg.Lang, cfg.TAGGER_HMMFile, cfg.TAGGER_Retokenize,
    //                         cfg.TAGGER_ForceSelect);
    tagger = new hmm_tagger(cfg.TAGGER_HMMFile, cfg.TAGGER_Retokenize,
                            cfg.TAGGER_ForceSelect);
  else if (cfg.TAGGER_which == RELAX)
    tagger = new relax_tagger(cfg.TAGGER_RelaxFile, cfg.TAGGER_RelaxMaxIter,
                              cfg.TAGGER_RelaxScaleFactor, cfg.TAGGER_RelaxEpsilon,
                              cfg.TAGGER_Retokenize, cfg.TAGGER_ForceSelect);

  if (cfg.NEC_NEClassification)
    neclass = new nec (cfg.NEC_NECFile);

  parser = new chart_parser(cfg.PARSER_GrammarFile);
  dep = new dep_txala(cfg.DEP_TxalaFile, parser->get_start_symbol());

  // Input is plain text.
  ProcessPlain(cfg, tk, sp, morfo, tagger, neclass, parser, dep);

  // clean up. Note that deleting a null pointer is a safe (yet useless) operation
  delete tk;
  delete sp;
  delete morfo;
  delete tagger;
  delete neclass;
  delete parser;
  delete dep;
}


//
// CODI PROFIT
//
wstring xmlencode(wstring s)
{
  size_t pos = 0;
  while ((pos = s.find(L"&", pos))!=wstring::npos)
  {
     s.replace(pos, 1, L"&amp;");
     pos += 4;
  }

  while ((pos = s.find('"')) != wstring::npos)
  {
    s.replace(pos,1,L"&quot;");
  }

  while ((pos = s.find(L"'")) != wstring::npos)
  {
    s.replace(pos, 1, L"&apos;");
  }

  while ((pos = s.find(L"<")) != wstring::npos)
  {
    s.replace(pos, 1, L"&lt;");
  }

  while ((pos = s.find(L">")) != wstring::npos)
  {
    s.replace(pos, 1, L"&gt;");
  }

  return s;
}


int wordPosition(const sentence &s, const word &w)
{
  sentence::const_iterator i = s.begin();
  int position = 0;
  bool found = false;

  while (i != s.end() && !found)
  {
    found = (w.get_span_start() == i->get_span_start() &&
             w.get_span_finish()==i->get_span_finish());
    ++position;
    ++i;
  }
  if (!found)
    wcerr << L"INTERNAL ERROR in determinig Word Position" << endl;

  return position;
}


void get_chunks(dep_tree::iterator n, map<int, dep_tree::iterator> &chunks)
{
  dep_tree::sibling_iterator d;

  for (d = n->sibling_begin(); d != n->sibling_end(); ++d)
  {
    if (d->info.is_chunk())
    {
      int pos = d->info.get_chunk_ord();
      chunks[pos] = d;
    }
    else
      get_chunks(d, chunks);
  }
}


wstring PrintCHUNK(sentence & s, parse_tree & fulltree, dep_tree &tr,
                  dep_tree::iterator n, int depth, const bool printHead)
{
  dep_tree::iterator dm;
  wostringstream tree;

  tree << wstring(depth * 2, ' ');

  tree << L"<CHUNK ord='" << n->info.get_chunk_ord() << L"' alloc='"<< get_alloc(n, false) << L"'";
  tree << L" type='" << xmlencode(n->info.get_link()->info.get_label()).c_str() << L"'";
  tree << L" si='" << xmlencode(n->info.get_label()).c_str() << L"'>" << endl;

  tree << PrintNODE(s, fulltree, tr, n, depth + 1, printHead);

  map<int, dep_tree::iterator> chunks;
  map<int, dep_tree::iterator>::iterator c;

  get_chunks(n, chunks);

  for (c = chunks.begin(); c != chunks.end(); c++)
  {
    dm = c->second;
    tree << PrintCHUNK(s, fulltree, tr, dm, depth + 1, printHead);
  }

  tree << wstring(depth * 2, ' ') << L"</CHUNK>" << endl;

  return tree.str();
}


wstring PrintNODE(sentence & s, parse_tree & fulltree, dep_tree &tr,
                 dep_tree::iterator n, int depth, const bool printHead)
{
  wostringstream tree;
  dep_tree::sibling_iterator d;

  word w = n->info.get_word();

  tree << wstring(depth * 2, ' ');

  tree << L"<NODE ord='" << wordPosition(s,w) << L"'";
  tree << L" alloc='" << w.get_span_start() << L"'";
  tree << L" form='" << xmlencode(w.get_form()).c_str() << L"'";
  tree << L" lem='" << xmlencode(w.get_lemma()).c_str() << L"'";
  tree << L" mi='" << xmlencode(w.get_tag()).c_str() << L"'>" << endl;

  //Print Nodes
  for (d = n->sibling_begin(); d != n->sibling_end(); ++d)
  {
    if (!d->info.is_chunk())
    {
      tree << PrintNODE(s,fulltree,tr, d, depth + 1, printHead);
    }
  }

  tree << wstring(depth * 2, ' ') << L"</NODE>" << endl;

  return tree.str();
}

