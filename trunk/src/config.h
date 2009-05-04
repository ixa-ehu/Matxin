//////////////////////////////////////////////////////////////////
//
//    FreeLing - Open Source Language Analyzers
//
//    Copyright (C) 2004   TALP Research Center
//                         Universitat Politecnica de Catalunya
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//    contact: Lluis Padro (padro@lsi.upc.es)
//             TALP Research Center
//             despatx C6.212 - Campus Nord UPC
//             08034 Barcelona.  SPAIN
//
////////////////////////////////////////////////////////////////

#ifndef _CONFIG
#define _CONFIG

#include <cfg+.h>
#include "freeling/traces.h"
#include "freeling/ner.h"

#define MOD_TRACENAME "CONFIG_OPTIONS"
#define MOD_TRACECODE OPTIONS_TRACE

#define DefaultConfigFile "analyzer.cfg" // default ConfigFile

// codes for input-output formats
#define PLAIN    0
#define TOKEN    1
#define SPLITTED 2
#define MORFO    3
#define TAGGED   4
#define SENSES   5
#define PARSED   6
#define DEP      7

// codes for tagging algorithms
#define HMM   0
#define RELAX 1

// codes for dependency parsers
#define TXALA	0
#define MALT	1

// codes for sense annotation
#define NONE  0
#define ALL   1
#define MFS   2

// codes for ForceSelect
#define FORCE_NONE   0
#define FORCE_TAGGER 1
#define FORCE_RETOK  2

using namespace std;

////////////////////////////////////////////////////////////////
///  Class config implements a set of specific options
/// for the NLP analyzer, providing a C++ wrapper to 
/// libcfg+ library.
////////////////////////////////////////////////////////////////

class config {

 public:
    char * ConfigFile;

    /// General options
    char * Lang;
    /// General options
    int InputFormat, OutputFormat;
    /// General options
    int AlwaysFlush;

    /// Tokenizer options
    char * TOK_TokenizerFile;

    /// Splitter options
    char * SPLIT_SplitterFile;

    /// Morphological analyzer options
    int MACO_SuffixAnalysis, MACO_MultiwordsDetection, 
        MACO_NumbersDetection, MACO_PunctuationDetection, 
        MACO_DatesDetection, MACO_QuantitiesDetection, 
        MACO_DictionarySearch, MACO_ProbabilityAssignment; 
    int MACO_NER_which;
    /// Morphological analyzer options
    char *MACO_Decimal, *MACO_Thousand;

    /// Morphological analyzer options
    char *MACO_LocutionsFile, *MACO_QuantitiesFile, *MACO_SuffixFile, 
         *MACO_ProbabilityFile, *MACO_DictionaryFile, 
         *MACO_NPdataFile, *MACO_PunctuationFile;
    double MACO_ProbabilityThreshold;

    // NEC options
    int NEC_NEClassification;
    char *NEC_FilePrefix;

    // Sense annotator options
    int SENSE_SenseAnnotation;
    char *SENSE_SenseFile;
    int SENSE_DuplicateAnalysis;

    /// Tagger options
    char * TAGGER_HMMFile;
    char * TAGGER_RelaxFile;
    int TAGGER_which;
    int TAGGER_RelaxMaxIter;
    double TAGGER_RelaxScaleFactor;
    double TAGGER_RelaxEpsilon;
    int TAGGER_Retokenize;
    int TAGGER_ForceSelect;

    /// Parser options
    char * PARSER_GrammarFile;

    /// Dependency options
    int DEP_which;
    char * DEP_TxalaFile;    
    char * DEP_MaltFile;

    ///////////////////////////////OPTIONS OF MATXIN///////////////////////////////////////
    int write_xslt;
    int DoTrace;
    int DoVerbTrace;
    int DoGenTrace;
    int DoPrepTrace;

    int UseRules;
    int UseSubcat;
    int UseTripletes;
    int first_case;

    char * Trace_File;

    /// Lexical transfer options
    char * DictionaryFile;
    char * ChunkType_DictFile;
    char * Noum_SemanticFile;

    /// Sintactic transfer options
    char * Intra_MovementsFile;

    int Inter_Phase;
    char * Inter_Movements1File;
    char * Inter_Movements2File;
    char * Inter_Movements3File;
    char * SubcatFile;
    char * PrepositionsFile;
    char * Noum_SubcatFile;

    char * Verb_TransferFile;

    /// Structural generation Options
    char * Chunk_OrderFile;

    char * Node_OrderFile;
    char * POS_ToOrderFile;

    /// Morphological Generation
    char * Morpho_GenFile;
    char * Measures_GenFile;
    char * NoLex_GenFile;
    char * Tag_ToGenFile;
    char * Tag_OrderFile;

    /// constructor
    config(char **argv) {
      CFG_CONTEXT con;
      register int ret;
      int help;
      // Auxiliary for string translation
      char *InputF, *OutputF, *Ner, *Tagger, *SenseAnot, *Force, *DepParser;
      // Auxiliary for boolean handling
      int flush,noflush, sufx,nosufx,   loc,noloc,   numb,nonumb,
          punt,nopunt,   date,nodate,   quant,noquant, dict,nodict, prob,noprob,
	  nec,nonec,     dup,nodup,      retok,noretok,
	  trace,notrace, vtrace,novtrace, gtrace,nogtrace, ptrace,noptrace,
	Usubcat,noUsubcat, Urules,noUrules, Utripletes,noUtripletes, fcase, nofcase; 

      int write_xslt_lag;

      char *cf_flush, *cf_sufx, *cf_loc,   *cf_numb,
           *cf_punt,  *cf_date, *cf_quant, *cf_dict, *cf_prob,
	   *cf_nec,  *cf_dup,   *cf_retok,
	   *cf_Usubcat,  *cf_Urules, *cf_Utripletes, *cf_first_case,
	   *cf_trace,    *cf_vtrace, *cf_gtrace, *cf_ptrace;
 
      // Options structure
      struct cfg_option OptionList[] = {  // initialization
	{"help",    'h',  NULL,                      CFG_BOOL, (void *) &help, 0},
	{NULL,      'f',  NULL,                      CFG_STR,  (void *) &ConfigFile, 0},
	// general options
	{"lang",    '\0', "Lang",                    CFG_STR,  (void *) &Lang, 0},
	{"tlevel",   'l', "TraceLevel",              CFG_INT,  (void *) &traces::TraceLevel, 0},
	{"tmod",     'm', "TraceModule",             CFG_INT,  (void *) &traces::TraceModule, 0},
	{"inpf",    '\0', "InputFormat",             CFG_STR,  (void *) &InputF, 0},
	{"outf",    '\0', "OutputFormat",            CFG_STR,  (void *) &OutputF, 0},
	{"flush",   '\0', NULL,                      CFG_BOOL, (void *) &flush, 0},
	{"noflush", '\0', NULL,                      CFG_BOOL, (void *) &noflush, 0},
	{NULL,      '\0', "AlwaysFlush",             CFG_STR,  (void *) &cf_flush, 0},
	// tokenizer options
	{"ftok",    '\0', "TokenizerFile",           CFG_STR, (void *) &TOK_TokenizerFile, 0},
	// splitter options
	{"fsplit",  '\0', "SplitterFile",            CFG_STR,  (void *) &SPLIT_SplitterFile, 0},
	// morfo options
	{"sufx",    '\0', NULL,                      CFG_BOOL, (void *) &sufx, 0},
	{"nosufx",  '\0', NULL,                      CFG_BOOL, (void *) &nosufx, 0},
	{NULL,      '\0', "SuffixAnalysis",          CFG_STR,  (void *) &cf_sufx, 0},
	{"loc",     '\0', NULL,                      CFG_BOOL, (void *) &loc, 0},
	{"noloc",   '\0', NULL,                      CFG_BOOL, (void *) &noloc, 0},
	{NULL,      '\0', "MultiwordsDetection",     CFG_STR,  (void *) &cf_loc, 0},
	{"numb",    '\0', NULL,                      CFG_BOOL, (void *) &numb, 0},
	{"nonumb",  '\0', NULL,                      CFG_BOOL, (void *) &nonumb, 0},
	{NULL,      '\0', "NumbersDetection",        CFG_STR,  (void *) &cf_numb, 0},
	{"punt",    '\0', NULL,                      CFG_BOOL, (void *) &punt, 0},
	{"nopunt",  '\0', NULL,                      CFG_BOOL, (void *) &nopunt, 0},
	{NULL,      '\0', "PunctuationDetection",    CFG_STR,  (void *) &cf_punt, 0},
	{"date",    '\0', NULL,                      CFG_BOOL, (void *) &date, 0},
	{"nodate",  '\0', NULL,                      CFG_BOOL, (void *) &nodate, 0},
	{NULL,      '\0', "DatesDetection",          CFG_STR,  (void *) &cf_date, 0},
	{"quant",   '\0', NULL,                      CFG_BOOL, (void *) &quant, 0},
	{"noquant", '\0', NULL,                      CFG_BOOL, (void *) &noquant, 0},
	{NULL,      '\0', "QuantitiesDetection",     CFG_STR,  (void *) &cf_quant, 0},
	{"dict",    '\0', NULL,                      CFG_BOOL, (void *) &dict, 0},
	{"nodict",  '\0', NULL,                      CFG_BOOL, (void *) &nodict, 0},
	{NULL,      '\0', "DictionarySearch",        CFG_STR,  (void *) &cf_dict, 0},
	{"prob",    '\0', NULL,                      CFG_BOOL, (void *) &prob, 0},
	{"noprob",  '\0', NULL,                      CFG_BOOL, (void *) &noprob, 0},
	{NULL,      '\0', "ProbabilityAssignment",   CFG_STR,  (void *) &cf_prob, 0},
	{"ner",     '\0', "NERecognition",           CFG_STR,  (void *) &Ner, 0},
	{"dec",     '\0', "DecimalPoint",            CFG_STR,  (void *) &MACO_Decimal, 0},
	{"thou",    '\0', "ThousandPoint",           CFG_STR,  (void *) &MACO_Thousand, 0},
	{"floc",    'L',  "LocutionsFile",           CFG_STR,  (void *) &MACO_LocutionsFile, 0},
	{"fqty",    'Q',  "QuantitiesFile",          CFG_STR,  (void *) &MACO_QuantitiesFile, 0},
	{"fsuf",    'S',  "SuffixFile",              CFG_STR,  (void *) &MACO_SuffixFile, 0},
	{"fprob",   'P',  "ProbabilityFile",         CFG_STR,  (void *) &MACO_ProbabilityFile, 0},
	{"thres",   'e', "ProbabilityThreshold",    CFG_DOUBLE, (void *) &MACO_ProbabilityThreshold, 0},
	{"fdict",   'D',  "DictionaryFile",          CFG_STR,  (void *) &MACO_DictionaryFile, 0},
	{"fnp",     'N',  "NPDataFile",              CFG_STR,  (void *) &MACO_NPdataFile, 0},
	{"fpunct",  'F',  "PunctuationFile",         CFG_STR,  (void *) &MACO_PunctuationFile, 0},
	// NEC options
	{"nec",     '\0', NULL,                      CFG_BOOL, (void *) &nec, 0},
	{"nonec",   '\0', NULL,                      CFG_BOOL, (void *) &nonec, 0},
	{NULL,      '\0', "NEClassification",        CFG_STR,  (void *) &cf_nec, 0},
	{"fnec",    '\0', "NECFilePrefix",           CFG_STR,  (void *) &NEC_FilePrefix, 0},
	// Sense options
	{"sense",   's', "SenseAnnotation",         CFG_STR,  (void *) &SenseAnot, 0},
	{"fsense",  'W',  "SenseFile",               CFG_STR,  (void *) &SENSE_SenseFile, 0},
	{"dup",     '\0', NULL,                      CFG_BOOL, (void *) &dup, 0},
	{"nodup",   '\0', NULL,                      CFG_BOOL, (void *) &nodup, 0},
	{NULL,      '\0', "DuplicateAnalysis",       CFG_STR,  (void *) &cf_dup, 0},
	// tagger options
	{"hmm",  'H',  "TaggerHMMFile",              CFG_STR,  (void *) &TAGGER_HMMFile, 0},
	{"rlx",  'R',  "TaggerRelaxFile",            CFG_STR,  (void *) &TAGGER_RelaxFile, 0},
	{"tag",  't',  "Tagger",                     CFG_STR,  (void *) &Tagger, 0},
	{"iter", 'i', "TaggerRelaxMaxIter",         CFG_INT,  (void *) &TAGGER_RelaxMaxIter, 0},
	{"sf",   'r', "TaggerRelaxScaleFactor",     CFG_DOUBLE, (void *) &TAGGER_RelaxScaleFactor, 0},
	{"eps",  '\0', "TaggerRelaxEpsilon",         CFG_DOUBLE, (void *) &TAGGER_RelaxEpsilon, 0},
	{"rtk",   '\0', NULL,                        CFG_BOOL, (void *) &retok, 0},
	{"nortk", '\0', NULL,                        CFG_BOOL, (void *) &noretok, 0},
	{NULL,      '\0', "TaggerRetokenize",        CFG_STR,  (void *) &cf_retok, 0},
	{"force", '\0', "TaggerForceSelect",         CFG_STR,  (void *) &Force, 0},
	// parser options
	{"grammar", 'G',  "GrammarFile",             CFG_STR,  (void *) &PARSER_GrammarFile, 0},
        // dep options
	{"txala", 'T', "DepTxalaFile",               CFG_STR, (void *) &DEP_TxalaFile, 0},
	{"malt",  'M', "DepMaltFile",		     CFG_STR, (void *) &DEP_MaltFile,0},
	{"dep",   'd', "DepParser",		     CFG_STR, (void *) &DepParser, 0},
	// Matxin options
	{"xsl",      '\0', NULL,                     CFG_BOOL, (void *) &write_xslt_lag, 0},
	{"trace",    '\0', NULL,                     CFG_BOOL, (void *) &trace, 0},
	{"notrace",  '\0', NULL,                     CFG_BOOL, (void *) &notrace, 0},
	{NULL,       '\0', "DoTrace",                CFG_STR,  (void *) &cf_trace, 0},
	{"vtrace",   '\0', NULL,                     CFG_BOOL, (void *) &vtrace, 0},
	{"novtrace", '\0', NULL,                     CFG_BOOL, (void *) &novtrace, 0},
	{NULL,       '\0', "DoVerbTrace",            CFG_STR,  (void *) &cf_vtrace, 0},
	{"gtrace",   '\0', NULL,                     CFG_BOOL, (void *) &gtrace, 0},
	{"nogtrace", '\0', NULL,                     CFG_BOOL, (void *) &nogtrace, 0},
	{NULL,       '\0', "DoGenTrace",             CFG_STR,  (void *) &cf_gtrace, 0},
	{"ptrace",   '\0', NULL,                     CFG_BOOL, (void *) &ptrace, 0},
	{"noptrace", '\0', NULL,                     CFG_BOOL, (void *) &noptrace, 0},
	{NULL,       '\0', "DoPrepTrace",            CFG_STR,  (void *) &cf_ptrace, 0},
	{"TraceFile", '\0', "TraceFile",             CFG_STR,  (void *) &Trace_File, 0},
	// lexTrans options
	{"tDict",    '\0', "TransDictFile",          CFG_STR,  (void *) &DictionaryFile, 0},
	{"chunkT",   '\0', "ChunkTypeDict",          CFG_STR,  (void *) &ChunkType_DictFile, 0},
	{"noumSem",  '\0', "NoumSemFile",            CFG_STR,  (void *) &Noum_SemanticFile, 0},
	// sintTrans options
	{"intraMov", '\0', "IntraMoveFile",          CFG_STR,  (void *) &Intra_MovementsFile, 0},
	{"inter",    '\0', NULL,                     CFG_INT,  (void *) &Inter_Phase, 0},
	{"interMov1", '\0', "InterMoveFile1",        CFG_STR,  (void *) &Inter_Movements1File, 0},
	{"interMov2", '\0', "InterMoveFile2",        CFG_STR,  (void *) &Inter_Movements2File, 0},
	{"interMov3", '\0', "InterMoveFile3",        CFG_STR,  (void *) &Inter_Movements3File, 0},
	{"subcatF",  '\0', "SubCatFile",             CFG_STR,  (void *) &SubcatFile, 0},
	{"prep",     '\0', "PrepFile",               CFG_STR,  (void *) &PrepositionsFile, 0},
	{"tripletF", '\0', "TripletsFile",           CFG_STR,  (void *) &Noum_SubcatFile, 0},
	{"verbtrans", '\0', "VerbTransferFile",      CFG_STR,  (void *) &Verb_TransferFile, 0},
	{"chunkord", '\0', "ChunkOrderFile",         CFG_STR,  (void *) &Chunk_OrderFile, 0},
	{"nodeord",  '\0', "NodeOrderFile",          CFG_STR,  (void *) &Node_OrderFile, 0},
	{"POStrans", '\0', "POSTransFile",           CFG_STR,  (void *) &POS_ToOrderFile, 0},
	{"morphgen", '\0', "MorphGenFile",           CFG_STR,  (void *) &Morpho_GenFile, 0},
	{"measugen", '\0', "MeasuresGenFile",        CFG_STR,  (void *) &Measures_GenFile, 0},
	{"nolexgen", '\0', "NoLexGenFile",           CFG_STR,  (void *) &NoLex_GenFile, 0},
	{"gentags",  '\0', "GenerationTagsFile",     CFG_STR,  (void *) &Tag_ToGenFile, 0},
	{"tagorder", '\0', "TagOrderFile",           CFG_STR,  (void *) &Tag_OrderFile, 0},
	// Preposizioen artikuluan erabiltzeko
	{"fcase",    '\0', NULL,                     CFG_BOOL, (void *) &fcase, 0},
	{"nofcase",  '\0', NULL,                     CFG_BOOL, (void *) &nofcase, 0},
	{NULL,       '\0', "FirstCase",              CFG_STR,  (void *) &cf_first_case, 0},
	{"rules",    '\0', NULL,                     CFG_BOOL, (void *) &Urules, 0},
	{"norules",  '\0', NULL,                     CFG_BOOL, (void *) &noUrules, 0},
	{NULL,       '\0', "UseRules",               CFG_STR,  (void *) &cf_Urules, 0},
	{"subcat",   '\0', NULL,                     CFG_BOOL, (void *) &Usubcat, 0},
	{"nosubcat", '\0', NULL,                     CFG_BOOL, (void *) &noUsubcat, 0},
	{NULL,       '\0', "UseSubcat",              CFG_STR,  (void *) &cf_Usubcat, 0},
	{"tripl",    '\0', NULL,                     CFG_BOOL, (void *) &Utripletes, 0},
	{"notripl",  '\0', NULL,                     CFG_BOOL, (void *) &noUtripletes, 0},
	{NULL,       '\0', "UseTripletes",           CFG_STR,  (void *) &cf_Utripletes, 0},
	CFG_END_OF_LIST
      };
      
      // Creating context 
      con = cfg_get_context(OptionList);
      if (con == NULL) {
	ERROR_CRASH("Error creating context. Not enough memory?");
      }
      
      // init auxiliary variables
      InputF=NULL; OutputF=NULL;  Ner=NULL; Tagger=NULL; SenseAnot=NULL; Force=NULL; DepParser=NULL;
      flush=false; noflush=false; sufx=false;   nosufx=false; 
      loc=false;   noloc=false;   numb=false;   nonumb=false;   punt=false; nopunt=false;
      date=false;  nodate=false;  quant=false;  noquant=false;  dict=false; nodict=false; 
      prob=false;  noprob=false;  nec=false;  nonec=false; 
      dup=false;   nodup=false;   retok=false; noretok=false;
      trace=false; notrace=false;vtrace=false;novtrace=false;  gtrace=false;nogtrace=false;  ptrace=false;noptrace=false;
      Usubcat=false; noUsubcat=false;Urules=false;noUrules=false;  Utripletes=false;noUtripletes=false; fcase=false; nofcase=false;
      cf_flush=NULL; cf_sufx=NULL;  cf_loc=NULL;  cf_numb=NULL; 
      cf_punt=NULL;  cf_date=NULL;  cf_quant=NULL; cf_dict=NULL; cf_prob=NULL;
      cf_nec=NULL;   cf_dup=NULL;   cf_retok=NULL;
      cf_trace=NULL; cf_vtrace=NULL;cf_gtrace=NULL;cf_ptrace=NULL;
      cf_Usubcat=NULL; cf_Urules=NULL;cf_Utripletes=NULL;cf_first_case=NULL;

      write_xslt=write_xslt_lag=false;

      // Set built-in default values.
      ConfigFile=NULL; help=false;
      Lang=NULL; traces::TraceLevel=0; traces::TraceModule=0;
      AlwaysFlush=false;
      TOK_TokenizerFile=NULL;
      SPLIT_SplitterFile=NULL;
      MACO_SuffixAnalysis=false;   MACO_MultiwordsDetection=false; 
      MACO_NumbersDetection=false; MACO_PunctuationDetection=false; 
      MACO_DatesDetection=false;   MACO_QuantitiesDetection=false; 
      MACO_DictionarySearch=false; MACO_ProbabilityAssignment=false; 
      MACO_Decimal=NULL; MACO_Thousand=NULL;
      MACO_LocutionsFile=NULL; MACO_QuantitiesFile=NULL; MACO_SuffixFile=NULL; 
      MACO_ProbabilityFile=NULL; MACO_DictionaryFile=NULL; 
      MACO_NPdataFile=NULL; MACO_PunctuationFile=NULL;
      MACO_ProbabilityThreshold=0.0;
      MACO_NER_which=0; 
      NEC_NEClassification=false; NEC_FilePrefix=NULL; 
      SENSE_SenseAnnotation=NONE; SENSE_SenseFile=NULL; 
      SENSE_DuplicateAnalysis=false; 
      TAGGER_which=0; TAGGER_HMMFile=NULL; TAGGER_RelaxFile=NULL; 
      TAGGER_RelaxMaxIter=0; TAGGER_RelaxScaleFactor=0.0; TAGGER_RelaxEpsilon=0.0;
      TAGGER_Retokenize=0; TAGGER_ForceSelect=0;
      PARSER_GrammarFile=NULL;
      DEP_which=0; DEP_TxalaFile=NULL; DEP_MaltFile=NULL;

      Inter_Phase=1;
      DoTrace=false;DoVerbTrace=false;DoGenTrace=false;DoPrepTrace=false;
      UseRules=false;UseSubcat=false;UseTripletes=false;first_case=false;
      Trace_File=NULL;
      DictionaryFile=NULL; ChunkType_DictFile=NULL;
      Noum_SemanticFile=NULL; Intra_MovementsFile=NULL;
      Inter_Movements1File=NULL; Inter_Movements2File=NULL; 
      Inter_Movements2File=NULL; SubcatFile=NULL;
      PrepositionsFile=NULL; Noum_SubcatFile=NULL;
      Verb_TransferFile=NULL; Chunk_OrderFile=NULL;
      Node_OrderFile=NULL; POS_ToOrderFile=NULL;
      Morpho_GenFile=NULL; Measures_GenFile=NULL;
      NoLex_GenFile=NULL; Tag_ToGenFile=NULL; Tag_OrderFile=NULL;

       // parse comand line
      cfg_set_cmdline_context(con, 1, -1, argv);
      ret = cfg_parse(con);
      if (ret != CFG_OK) {
	ERROR_CRASH("Error "+util::int2string(ret)+" parsing command line. "+string(cfg_get_error_str(con)));
      }
      
      // Check options
      
      // Help screen required
      if (help) {
	PrintHelpScreen();
	exit(0); // return to system
      }
      
      // if no config file given, use default
      if (ConfigFile == NULL) {
	WARNING("No config file specified (option -f). Trying to open default file '"+string(DefaultConfigFile)+"'");
	ConfigFile = (char *)malloc(sizeof(char)*(1+strlen(DefaultConfigFile)));
 	strcpy(ConfigFile,DefaultConfigFile);
      }
      
      // parse and load options from ConfigFile
      cfg_set_cfgfile_context(con, 0, -1, ConfigFile);
      ret = cfg_parse(con);
      if (ret != CFG_OK) {
	ERROR_CRASH("Error "+util::int2string(ret)+" parsing config file '"+string(ConfigFile)+"'. "+string(cfg_get_error_str(con)));
      }

      // Handle boolean options expressed as strings in config file
      SetBooleanOptionCF(string(cf_flush),AlwaysFlush,"AlwaysFlush");
      SetBooleanOptionCF(string(cf_sufx),MACO_SuffixAnalysis,"SuffixAnalysis");
      SetBooleanOptionCF(string(cf_loc), MACO_MultiwordsDetection,"MultiwordsDetection");
      SetBooleanOptionCF(string(cf_numb),MACO_NumbersDetection,"NumbersDetection");
      SetBooleanOptionCF(string(cf_punt),MACO_PunctuationDetection,"PunctuationDetection");
      SetBooleanOptionCF(string(cf_date),MACO_DatesDetection,"DatesDetection");
      SetBooleanOptionCF(string(cf_quant),MACO_QuantitiesDetection,"QuantitiesDetection");
      SetBooleanOptionCF(string(cf_dict),MACO_DictionarySearch,"DictionarySearch");
      SetBooleanOptionCF(string(cf_prob),MACO_ProbabilityAssignment,"ProbabilityAssignment");
      SetBooleanOptionCF(string(cf_nec),NEC_NEClassification,"NEClassification");
      SetBooleanOptionCF(string(cf_dup),SENSE_DuplicateAnalysis,"DuplicateAnalysis");
      SetBooleanOptionCF(string(cf_retok),TAGGER_Retokenize,"TaggerRetokenize");

      SetBooleanOptionCF(string(cf_trace),DoTrace,"DoTrace");
      SetBooleanOptionCF(string(cf_vtrace),DoVerbTrace,"DoVerbTrace");
      SetBooleanOptionCF(string(cf_gtrace),DoGenTrace,"DoGenTrace");
      SetBooleanOptionCF(string(cf_ptrace),DoGenTrace,"DoPrepTrace");
      
      SetBooleanOptionCF(string(cf_first_case),first_case,"FirstCase");
      SetBooleanOptionCF(string(cf_Urules),UseRules,"UseRules");
      SetBooleanOptionCF(string(cf_Usubcat),UseSubcat,"UseSubcat");
      SetBooleanOptionCF(string(cf_Utripletes),UseTripletes,"UseTripletes");
      
      // Reload command line options to override ConfigFile options
      cfg_set_cmdline_context(con, 1, -1, argv);
      ret = cfg_parse(con);
      if (ret != CFG_OK) {
	ERROR_CRASH("Error "+util::int2string(ret)+" parsing command line. "+string(cfg_get_error_str(con)));
      }

      // check options involving Filenames for environment vars expansion.
      ExpandFileName(TOK_TokenizerFile);
      ExpandFileName(SPLIT_SplitterFile);
      ExpandFileName(MACO_LocutionsFile);
      ExpandFileName(MACO_QuantitiesFile);
      ExpandFileName(MACO_SuffixFile);
      ExpandFileName(MACO_ProbabilityFile);
      ExpandFileName(MACO_DictionaryFile); 
      ExpandFileName(MACO_NPdataFile);
      ExpandFileName(MACO_PunctuationFile);
      ExpandFileName(NEC_FilePrefix); 
      ExpandFileName(SENSE_SenseFile); 
      ExpandFileName(TAGGER_HMMFile);
      ExpandFileName(TAGGER_RelaxFile); 
      ExpandFileName(PARSER_GrammarFile); 
      ExpandFileName(DEP_TxalaFile);
      ExpandFileName(DEP_MaltFile);
	     
      // Handle boolean options expressed with --myopt or --nomyopt in command line
      SetBooleanOptionCL(flush,noflush,AlwaysFlush,"flush");
      SetBooleanOptionCL(sufx,nosufx,MACO_SuffixAnalysis,"sufx");
      SetBooleanOptionCL(loc,noloc, MACO_MultiwordsDetection,"loc");
      SetBooleanOptionCL(numb,nonumb,MACO_NumbersDetection,"numb");
      SetBooleanOptionCL(punt,nopunt,MACO_PunctuationDetection,"punt");
      SetBooleanOptionCL(date,nodate,MACO_DatesDetection,"date");
      SetBooleanOptionCL(quant,noquant,MACO_QuantitiesDetection,"quant");
      SetBooleanOptionCL(dict,nodict,MACO_DictionarySearch,"dict");
      SetBooleanOptionCL(prob,noprob,MACO_ProbabilityAssignment,"prob");
      SetBooleanOptionCL(nec,nonec,NEC_NEClassification,"nec");
      SetBooleanOptionCL(dup,nodup,SENSE_DuplicateAnalysis,"dup");
      SetBooleanOptionCL(retok,noretok,TAGGER_Retokenize,"retk");

      SetBooleanOptionCL(trace,notrace,DoTrace,"trace");
      SetBooleanOptionCL(vtrace,novtrace,DoVerbTrace,"vtrace");
      SetBooleanOptionCL(gtrace,nogtrace,DoGenTrace,"gtrace");
      SetBooleanOptionCL(ptrace,noptrace,DoPrepTrace,"ptrace");

      SetBooleanOptionCL(fcase,nofcase,first_case,"fcase");
      SetBooleanOptionCL(Urules,noUrules,UseRules,"rules");
      SetBooleanOptionCL(Usubcat,noUsubcat,UseSubcat,"subcat");
      SetBooleanOptionCL(Utripletes,noUtripletes,UseTripletes,"tripl");

      SetBooleanOptionCL(write_xslt_lag,false,write_xslt,"xsl");


      if (DoTrace && Trace_File==NULL) {
	WARNING("No trace file specified (option --TraceFile). No trace will be done.");
	DoTrace=false;
      }

      // translate InputF and OutputF strings to more useful integer values.
      if (string(InputF)=="plain") InputFormat = PLAIN;
      else if (string(InputF)=="token") InputFormat = TOKEN;
      else if (string(InputF)=="splitted") InputFormat = SPLITTED;
      else if (string(InputF)=="morfo") InputFormat = MORFO;
      else if (string(InputF)=="tagged") InputFormat = TAGGED;
      else if (string(InputF)=="sense") InputFormat = SENSES;
      else if (string(InputF)=="parsed") InputFormat = PARSED;
      else if (string(InputF)=="dep") InputFormat = DEP;
      else { ERROR_CRASH("UNKNOWN Input format: "+string(InputF));}
      
      if (string(OutputF)=="plain") OutputFormat = PLAIN;
      else if (string(OutputF)=="token") OutputFormat = TOKEN;
      else if (string(OutputF)=="splitted") OutputFormat = SPLITTED;
      else if (string(OutputF)=="morfo") OutputFormat = MORFO;
      else if (string(OutputF)=="tagged") OutputFormat = TAGGED;
      else if (string(OutputF)=="parsed") OutputFormat = PARSED;
      else if (string(OutputF)=="dep") OutputFormat = DEP;
      else { ERROR_CRASH("UNKNOWN Output format: "+string(OutputF));}

      // translate Ner string to more useful integer values.
      if (string(Ner)=="basic") MACO_NER_which = NER_BASIC;
      else if (string(Ner)=="bio") MACO_NER_which = NER_BIO;
      else if (string(Ner)=="none" || string(Ner)=="no") MACO_NER_which = NER_NONE;
      else WARNING("Invalid NER algorithm '"+string(Ner)+"'. Using default.");

      // translate Tagger string to more useful integer values.
      if (string(Tagger)=="hmm") TAGGER_which = HMM;
      else if (string(Tagger)=="relax") TAGGER_which = RELAX;
      else WARNING("Invalid tagger algorithm '"+string(Tagger)+"'. Using default.");

      if (string(DepParser) == "malt") DEP_which = MALT;
      else if (string(DepParser) == "txala") DEP_which = TXALA;
      else WARNING("Invalid DEP parser '"+string(DepParser)+"'. Using default.");

      // Translate ForceSelect string to more useful integer values.
      if (string(Force)=="none" || string(Force)=="no") TAGGER_ForceSelect = FORCE_NONE;
      else if (string(Force)=="tagger") TAGGER_ForceSelect = FORCE_TAGGER;
      else if (string(Force)=="retok") TAGGER_ForceSelect = FORCE_RETOK;
      else WARNING("Invalid ForceSelect value '"+string(Force)+"'. Using default.");

      // translate SenseAnot string to more useful integer values.
      if (string(SenseAnot)=="none" || string(SenseAnot)=="no") SENSE_SenseAnnotation = NONE;
      else if (string(SenseAnot)=="all") SENSE_SenseAnnotation = ALL;
      else if (string(SenseAnot)=="mfs") SENSE_SenseAnnotation = MFS;
      else WARNING("Invalid sense annotation option '"+string(SenseAnot)+"'. Using default.");
    }

 private:

    void ExpandFileName(char* &s) {
       string name(s);
       size_t n=name.find_first_of("$"); 
       if (n!=string::npos) {
         size_t i=name.find_first_of("/\\",n+1);
         if (i==string::npos) i=name.size();
         char* exp=getenv(name.substr(n+1,i-n-1).c_str());
	 if (exp==NULL){
	   WARNING("Undefined variable "+name.substr(n+1,i-n-1)+" in configuration file "+string(ConfigFile)+" expanded as empty string.");
	   name = name.substr(0,n) + name.substr(i);
	 }
	 else {
	   name = name.substr(0,n) + string(exp) + name.substr(i);
	 }
 
	 free(s);
	 s = (char *) calloc(name.size()+1,sizeof(char));
	 strcpy (s,name.c_str());
       }
    }

    void SetBooleanOptionCF (const string &s, int &opt, const string &name) {
      if (s=="yes" || s=="y" || s=="on" || s=="true")
        opt=true;
      else if (s=="no" || s=="n" || s=="off" || s=="false")
        opt=false;
      else if (!s.empty())
	WARNING("Invalid boolean value '"+s+"' for option "+name+" in configuration file. Using default value.");
      //else: s is empty, so the option hasn't been specified. Use default.
    } 

    void SetBooleanOptionCL (const int pos, const int neg, int &opt, const string &name) {
      if (pos && neg)  
	WARNING("Ambiguous specification for option --"+name+" in command line. Using default value.");
      else if (pos)
	opt=true;
      else if (neg)
        opt=false;
      //else: nothing specified, leave things as they are.
    }

    void PrintHelpScreen(){
      cout<<"Available options: "<<endl<<endl;
      cout<<"-h, --help             This screen "<<endl;
      cout<<"-f filename            Use alternative configuration file (default: analyzer.cfg)"<<endl;
      cout<<"--lang language        Language (sp: Spanish, ca: Catalan, en: English)"<<endl;
      cout<<"--flush                Consider each newline as a sentence end"<<endl;
      cout<<"--inpf string          Input format (plain,token,splitted,morfo,sense,tagged)"<< endl;
      cout<<"--outf string          Output format (plain,token,splitted,morfo,tagged,parsed,dep)"<< endl;
      cout<<"--ftok filename        Tokenizer rules file "<<endl;
      cout<<"--fsplit filename      Splitter options file "<<endl;
      cout<<"--sufx, --nosufx       Whether to perform suffix analysis"<<endl;
      cout<<"--loc, --noloc         Whether to perform multiword detection"<<endl;
      cout<<"--numb, --nonumb       Whether to perform number detection"<<endl;
      cout<<"--punt, --nopunt       Whether to perform punctuation detection"<<endl;
      cout<<"--date, --nodate       Whether to perform date and time expressions detection"<<endl;
      cout<<"--quant, --noquant     Whether to perform quantities detection"<<endl;
      cout<<"--dict, --nodict       Whether to perform dictionary search"<<endl;
      cout<<"--prob, --noprob       Whether to perform probability assignment"<<endl;
      cout<<"--ner string           Which kind of NE recognition is to be performed (basic, bio, none)"<<endl;
      cout<<"--dec string           Decimal point character"<<endl;
      cout<<"--thou string          Thousand point character"<<endl;
      cout<<"--floc,-L filename     Multiwords file"<<endl;
      cout<<"--fqty,-Q filename     Quantities file"<<endl;
      cout<<"--fsuf,-S filename     Suffix rules file"<<endl;
      cout<<"--fprob,-P filename    Probabilities file"<<endl;
      cout<<"--thres,-e float       Probability threshold for unknown word tags"<<endl;
      cout<<"--fdict,-D filename    Dictionary database"<<endl;
      cout<<"--fnp,-N filename      NP recognizer data file"<<endl;
      cout<<"--nec, --nonec         Whether to perform NE classification"<<endl;
      cout<<"--fnec filename        Filename prefix for NEC data XX.rgf, XX.lex, XX.abm"<<endl;
      cout<<"--sense,-s string      Type of sense annotation (no|none,all,mfs)"<<endl;
      cout<<"--fsense,-W filename   Sense dictionary file"<<endl;
      cout<<"--dup, --nodup         Whether to duplicate analysis for each different sense"<<endl;
      cout<<"--fpunct,-F filename   Punctuation symbols file"<<endl;
      cout<<"--tag,-t string        Tagging alogrithm to use (hmm, relax)"<<endl;
      cout<<"--hmm,-H filename      Data file for HMM tagger"<<endl;
      cout<<"--rlx,-R filename      Data file for RELAX tagger"<<endl;
      cout<<"--rtk, --nortk         Whether to perform retokenization after PoS tagging"<<endl;
      cout<<"--force string         Whether/when the tagger must be forced to select only one tag per word (none,tagger,retok)"<<endl;
      cout<<"--iter,-i int             Maximum number of iterations allowed for RELAX tagger."<<endl;
      cout<<"--sf,-r float             Support scale factor for RELAX tagger (affects step size)"<<endl;
      cout<<"--eps float            Epsilon value to decide when RELAX tagger achieves no more changes"<<endl;
      cout<<"--grammar,-G filename  Grammar file for chart parser"<<endl;
      cout<<"--dep,-d string        Dependency parser to use (txala, malt)"<<endl;
      cout<<"--txala,-T filename    Rule file for Txala dependency parser"<<endl;
      cout<<"--malt,-M filename     Data file for dependency parser"<<endl;
      cout << endl;
      cout<<"--trace, --notrace     Egindako itzulpenaren traza (XML zuhaitzak) egin ala ez"<<endl;
      cout<<"--vtrace, --novtrace   Aditz transferentziaren traza egin ala ez"<<endl;
      cout<<"--gtrace, --nogtrace   Sorkuntza morfologikoaren traza egin ala ez"<<endl;
      cout<<"--ptrace, --noptrace   Preposizioen itzulpenaren traza egin ala ez"<<endl;
      cout<<"--TraceFile filename   XML zuhaitzen traza botatzeako fitxategia"<<endl;
      cout<<"--tDict filename       Hiztegi elebiduna"<<endl;
      cout<<"--chunkT filename      Chunk moten hiztegia"<<endl;
      cout<<"--noumSem filename     Izenen semantikaren hiztegia"<<endl;
      cout<<"--intraMov filename    Chunk barruko mugimenduen gramatika"<<endl;
      cout<<"--interMov1 filename   Chunk arteko mugimenduen 1. gramatika"<<endl;
      cout<<"--interMov2 filename   Chunk arteko mugimenduen 2. gramatika"<<endl;
      cout<<"--interMov3 filename   Chunk arteko mugimenduen 3. gramatika"<<endl;
      cout<<"--subcat filename      Aditz azpikategorizazioaren hiztegia"<<endl;
      cout<<"--prep filename        Preposizioen hiztegia"<<endl;
      cout<<"--triplets filename    Tripleten fitxategia"<<endl;
      cout<<"--verbtrans filename   Aditz tranferentzia "<<endl;
      cout<<"--chunkord filename    Chunken ordena definitzeko gramatika"<<endl;
      cout<<"--nodeord filename     Nodoen ordena definitzeko fitxategia"<<endl;
      cout<<"--POStrans filename    Ordenatzeko erabili behar diren POS lortzeko hiztegia"<<endl;
      cout<<"--morphgen filename    Sorkuntza hiztegia"<<endl;
      cout<<"--measugen filename    Zenbaki, data, etab. sorkuntza hiztegia"<<endl;
      cout<<"--nolexgen filename    Lexiko gabeko sorkuntzarako hiztegia"<<endl;
      cout<<"--gentags filename     Sorkuntzarako beharrezko etiketak lortzeko hiztegia"<<endl;
      cout<<"--tagorder filename    Sorkuntzarako etiketen ordena definitzeko gramatika."<<endl;
      cout<<endl;
    }

};

#endif

