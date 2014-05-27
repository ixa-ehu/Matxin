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
//    version 3 of the License, or (at your option) any later version.
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

#include <exception>
#include <fstream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "freeling/morfo/util.h"

#define MOD_TRACENAME L"CONFIG_OPTIONS"
#define MOD_TRACECODE OPTIONS_TRACE

#define DefaultConfigFile "analyzer.cfg" // default ConfigFile

// codes for tagging algorithms
#define HMM   0
#define RELAX 1

// codes for dependency parsers
#define TXALA	0

// codes for ForceSelect
#define FORCE_NONE   0
#define FORCE_TAGGER 1
#define FORCE_RETOK  2

using namespace freeling;

////////////////////////////////////////////////////////////////
///  Class config implements a set of specific options
/// for the NLP analyzer, providing a C++ wrapper to 
/// libcfg+ library.
////////////////////////////////////////////////////////////////

class config {

 public:
    std::string ConfigFile;

    /// Language of text to process
    std::wstring Lang;
    /// Locale of text to process
    std::wstring Locale;
    /// Flush splitter at each line
    bool AlwaysFlush;
    /// produce output in a format suitable to train the tagger.
    bool TrainingOutput;

    /// Tokenizer options
    std::wstring TOK_TokenizerFile;

    /// Splitter options
    std::wstring SPLIT_SplitterFile;

    /// Morphological analyzer options
    bool MACO_UserMap, MACO_AffixAnalysis, MACO_MultiwordsDetection, 
         MACO_NumbersDetection, MACO_PunctuationDetection, 
         MACO_DatesDetection, MACO_QuantitiesDetection, 
         MACO_DictionarySearch, MACO_ProbabilityAssignment, MACO_OrthographicCorrection,
         MACO_NERecognition;
    /// Morphological analyzer options
    std::wstring MACO_Decimal, MACO_Thousand;

    /// Morphological analyzer options
    std::wstring MACO_UserMapFile, MACO_LocutionsFile,   MACO_QuantitiesFile,
            MACO_AffixFile,   MACO_ProbabilityFile, MACO_DictionaryFile, 
            MACO_NPDataFile,  MACO_PunctuationFile, MACO_CorrectorFile; 
  	 
    double MACO_ProbabilityThreshold;
    bool MACO_RetokContractions;

    // NEC options
    bool NEC_NEClassification;
    std::wstring NEC_NECFile;

    /// Tagger options
    std::wstring TAGGER_HMMFile;
    std::wstring TAGGER_RelaxFile;
    int TAGGER_which;
    int TAGGER_RelaxMaxIter;
    double TAGGER_RelaxScaleFactor;
    double TAGGER_RelaxEpsilon;
    bool TAGGER_Retokenize;
    int TAGGER_ForceSelect;

    /// Parser options
    std::wstring PARSER_GrammarFile;

    /// Dependency options
    std::wstring DEP_TxalaFile;    

   ///////////////////////////////OPTIONS OF MATXIN///////////////////////////////////////
    /// Morphological analyzer server options
    bool server;
    std::string InPipe, OutPipe;

    bool UseLexRules;

    bool UsePrepRules;
    bool UseSubcat;
    bool UseTripletes;
    bool first_case;

    bool DoTrace;
    bool DoVerbTrace;
    bool DoGenTrace;
    bool DoPrepTrace;

    bool write_xslt;

    std::string Trace_File;

    /// Lexical transfer options
    std::string DictionaryFile;
    std::string ChunkType_DictFile;
    std::string Noun_SemanticFile;

    // Lexical selection options
    std::string LexSelFile;

    /// Sintactic transfer options
    std::string Intra_MovementsFile;

    int Inter_Phase;
    std::string Inter_Movements1File;
    std::string Inter_Movements2File;
    std::string Inter_Movements3File;
    std::string SubcatFile;
    std::string PrepositionsFile;
    std::string Noun_SubcatFile;

    std::string Verb_TransferFile;

    /// Structural generation Options
    std::string Chunk_OrderFile;

    std::string Node_OrderFile;
    std::string POS_ToOrderFile;

    std::string Generation_MovementsFile;

    /// Morphological Generation
    std::string Morpho_GenFile;
    std::string Measures_GenFile;
    std::string NoLex_GenFile;
    std::string Tag_ToGenFile;
    std::string Tag_OrderFile;

    /// constructor
    config(int ac, char **av) {

      // Auxiliary variables to store options read as strings before they are converted
      // to their final enumerate/integer values 
      std::string Tagger, Force;
      //std::string tracemod;
      std::string language, locale, tokFile, splitFile, macoDecimal, macoThousand,
 	   usermapFile, locutionsFile, quantitiesFile, affixFile, probabilityFile,
	   dictionaryFile, npDataFile, punctuationFile, correctorFile;
      std::string necFile, hmmFile,relaxFile,grammarFile,txalaFile;
 
      po::options_description vis_cl("Available command-line options");
      vis_cl.add_options()
	("help,h", "Help about command-line options.")
	("help-cf", "Help about configuration file options.")
        #ifndef WIN32
	  ("version,v", "Print installed FreeLing version.")
        #endif
	("fcfg,f", po::value<std::string>(&ConfigFile)->default_value(DefaultConfigFile), "Configuration file to use")
	("lang",po::value<std::string>(&language),"language of the input text")
	("locale",po::value<std::string>(&locale),"locale encoding of input text (\"default\"=en_US.UTF-8, \"system\"=current system locale, [other]=any valid locale string installed in the system (e.g. ca_ES.UTF-8,it_IT.UTF-8,...)")
	("flush","Consider each newline as a sentence end")
	("noflush","Do not consider each newline as a sentence end")
	("train","Produce output format suitable for train scripts")
	("ftok",po::value<std::string>(&tokFile),"Tokenizer rules file")
	("fsplit",po::value<std::string>(&splitFile),"Splitter option file")
	("afx","Perform affix analysis")
	("noafx","Do not perform affix analysis")
	("usr","Apply user mapping file")
	("nousr","Do not apply user mapping file")
	("loc","Perform multiword detection")
	("noloc","Do not perform multiword detection")
	("numb","Perform number detection")
	("nonumb","Do not perform number detection")
	("punt","Perform punctuation detection")
	("nopunt","Do not perform punctuation detection")
	("date","Perform date/time expression detection")
	("nodate","Do not perform date/time expression detection")
	("quant","Perform magnitude/ratio detection")
	("noquant","Do not perform magnitude/ratio detection")
	("dict","Perform dictionary search")
	("nodict","Do not perform dictionary search")
	("prob","Perform probability assignment")
	("noprob","Do not perform probability assignment")
	("rtkcon","Dictionary retokenizes contractions regardless of --nortk option")
	("nortkcon","Dictionary leaves contraction retokenization to --rtk/nortk option")
	("orto","Perform ortographic correction")
	("noorto","Do not perform ortographic correction")
	("ner","Perform NE recognition")
	("noner","Do not perform NE recognition")
	("dec",po::value<std::string>(&macoDecimal),"Decimal point character")
	("thou",po::value<std::string>(&macoThousand),"Thousand point character")
	("fmap,M",po::value<std::string>(&usermapFile),"User-map file")
	("floc,L",po::value<std::string>(&locutionsFile),"Multiwords file")
	("fqty,Q",po::value<std::string>(&quantitiesFile),"Quantities file")
	("fafx,S",po::value<std::string>(&affixFile),"Affix rules file")
	("fprob,P",po::value<std::string>(&probabilityFile),"Probabilities file")
	("thres,e",po::value<double>(&MACO_ProbabilityThreshold),"Probability threshold for unknown word tags")
	("fdict,D",po::value<std::string>(&dictionaryFile),"Form dictionary")
	("fnp,N",po::value<std::string>(&npDataFile),"NE recognizer data file")
	("fcorr,K",po::value<std::string>(&correctorFile),"Ortographic corrector configuration file")
	("fpunct,F",po::value<std::string>(&punctuationFile),"Punctuation symbol file")
	("nec","Perform NE classification")
	("nonec","Do not perform NE classification")
	("fnec",po::value<std::string>(&necFile),"NEC configuration file")
	("hmm,H",po::value<std::string>(&hmmFile),"Data file for HMM tagger")
	("rlx,R",po::value<std::string>(&relaxFile),"Data file for RELAX tagger")
	("tag,t",po::value<std::string>(&Tagger),"Tagging alogrithm to use (hmm, relax)")
	("iter,i",po::value<int>(&TAGGER_RelaxMaxIter),"Maximum number of iterations allowed for RELAX tagger")
	("sf,r",po::value<double>(&TAGGER_RelaxScaleFactor),"Support scale factor for RELAX tagger (affects step size)")
	("eps",po::value<double>(&TAGGER_RelaxEpsilon),"Convergence epsilon value for RELAX tagger")
	("rtk","Perform retokenization after PoS tagging")
	("nortk","Do not perform retokenization after PoS tagging")
	("force",po::value<std::string>(&Force),"When the tagger must be forced to select only one tag per word (no|none,tagger,retok)")
	("grammar,G",po::value<std::string>(&grammarFile),"Grammar file for chart parser")
	("txala,T",po::value<std::string>(&txalaFile),"Rule file for Txala dependency parser")

	//Matxin-en aukerak
	("TraceFile",po::value<std::string>(&Trace_File),"")
	("tDict",po::value<std::string>(&DictionaryFile),"")
	("chunkT",po::value<std::string>(&ChunkType_DictFile),"")
	("nounSem",po::value<std::string>(&Noun_SemanticFile),"")
	("disF",po::value<std::string>(&LexSelFile),"")
	("intraMov",po::value<std::string>(&Intra_MovementsFile),"")
	("interMov1",po::value<std::string>(&Inter_Movements1File),"")
	("interMov2",po::value<std::string>(&Inter_Movements2File),"")
	("interMov3",po::value<std::string>(&Inter_Movements3File),"")
	("genMov",po::value<std::string>(&Generation_MovementsFile),"")
	("subcatF",po::value<std::string>(&SubcatFile),"")
	("prep",po::value<std::string>(&PrepositionsFile),"")
	("tripletF",po::value<std::string>(&Noun_SubcatFile),"")
	("verbtrans",po::value<std::string>(&Verb_TransferFile),"")
	("chunkord",po::value<std::string>(&Chunk_OrderFile),"")
	("nodeord",po::value<std::string>(&Node_OrderFile),"")
	("POStrans",po::value<std::string>(&POS_ToOrderFile),"")
	("morphgen",po::value<std::string>(&Morpho_GenFile),"")
	("measugen",po::value<std::string>(&Measures_GenFile),"")
	("nolexgen",po::value<std::string>(&NoLex_GenFile),"")
	("gentags",po::value<std::string>(&Tag_ToGenFile),"")
	("tagorder",po::value<std::string>(&Tag_OrderFile),"")

	("ipipe",po::value<std::string>(&InPipe),"")
	("opipe",po::value<std::string>(&OutPipe),"")

	("inter",po::value<int>(&Inter_Phase),"")

	("lexrules","")
	("nolexrules","")

	("preprules","")
	("nopreprules","")
	("subcat","")
	("nosubcat","")
	("tripl","")
	("notripl","")
	("fcase","Choose first case (dictionary order) as last disambiguation step")
	("nofcase","Do not choose first case (dictionary order) as last disambiguation step")

	("xsl","")
	("server","")

	("trace","")
	("notrace","")
	("vtrace","")
	("novtrace","")
	("gtrace","")
	("nogtrace","")
	("ptrace","")
	("noptrace","")
     ;
 
      po::options_description vis_cf("Available configuration file options");
      vis_cf.add_options()
	("Lang",po::value<std::string>(&language),"Language of the input text")
	("Locale",po::value<std::string>(&locale)->default_value("default"),"locale encoding of input text (\"default\"=en_US.UTF-8, \"system\"=current system locale, [other]=any valid locale string installed in the system (e.g. ca_ES.UTF-8,it_IT.UTF-8,...)")
	("AlwaysFlush",po::value<bool>(&AlwaysFlush)->default_value(false),"Consider each newline as a sentence end")
	("TokenizerFile",po::value<std::string>(&tokFile),"Tokenizer rules file")
	("SplitterFile",po::value<std::string>(&splitFile),"Splitter option file")

	("AffixAnalysis",po::value<bool>(&MACO_AffixAnalysis)->default_value(false),"Perform affix analysis")
	("UserMap",po::value<bool>(&MACO_UserMap)->default_value(false),"Apply user mapping file")
	("MultiwordsDetection",po::value<bool>(&MACO_MultiwordsDetection)->default_value(false),"Perform multiword detection")
	("NumbersDetection",po::value<bool>(&MACO_NumbersDetection)->default_value(false),"Perform number detection")
	("PunctuationDetection",po::value<bool>(&MACO_PunctuationDetection)->default_value(false),"Perform punctuation detection")
	("DatesDetection",po::value<bool>(&MACO_DatesDetection)->default_value(false),"Perform date/time expression detection")
	("QuantitiesDetection",po::value<bool>(&MACO_QuantitiesDetection)->default_value(false),"Perform magnitude/ratio detection")
	("DictionarySearch",po::value<bool>(&MACO_DictionarySearch)->default_value(false),"Perform dictionary search")
	("RetokContractions",po::value<bool>(&MACO_RetokContractions)->default_value(true),"Dictionary retokenizes contractions regardless of --nortk option")
	("ProbabilityAssignment",po::value<bool>(&MACO_ProbabilityAssignment)->default_value(false),"Perform probability assignment")
	("OrthographicCorrection",po::value<bool>(&MACO_OrthographicCorrection)->default_value(false),"Perform ortographic correction")
	("NERecognition",po::value<bool>(&MACO_NERecognition)->default_value(false),"Perform NE recognition")
	("DecimalPoint",po::value<std::string>(&macoDecimal),"Decimal point character")
	("ThousandPoint",po::value<std::string>(&macoThousand),"Thousand point character")
	("UserMapFile",po::value<std::string>(&usermapFile),"User mapping file")
	("LocutionsFile",po::value<std::string>(&locutionsFile),"Multiwords file")
	("QuantitiesFile",po::value<std::string>(&quantitiesFile),"Quantities file")
	("AffixFile",po::value<std::string>(&affixFile),"Affix rules file")

	("ProbabilityFile",po::value<std::string>(&probabilityFile),"Probabilities file")
	("ProbabilityThreshold",po::value<double>(&MACO_ProbabilityThreshold),"Probability threshold for unknown word tags")
	("DictionaryFile",po::value<std::string>(&dictionaryFile),"Form dictionary")
	("NPDataFile",po::value<std::string>(&npDataFile),"NP recognizer data file")
	("CorrectorFile",po::value<std::string>(&correctorFile),"Ortographic corrector configuration file")
	("PunctuationFile",po::value<std::string>(&punctuationFile),"Punctuation symbol file")
	("NEClassification",po::value<bool>(&NEC_NEClassification)->default_value(false),"Perform NE classification")

	("NECFile",po::value<std::string>(&necFile),"NEC configuration file")
	("TaggerHMMFile",po::value<std::string>(&hmmFile),"Data file for HMM tagger")
	("TaggerRelaxFile",po::value<std::string>(&relaxFile),"Data file for RELAX tagger")
	("Tagger",po::value<std::string>(&Tagger)->default_value("hmm"),"Tagging alogrithm to use (hmm, relax)")
	("TaggerRelaxMaxIter",po::value<int>(&TAGGER_RelaxMaxIter),"Maximum number of iterations allowed for RELAX tagger")
	("TaggerRelaxScaleFactor",po::value<double>(&TAGGER_RelaxScaleFactor),"Support scale factor for RELAX tagger (affects step size)")
	("TaggerRelaxEpsilon",po::value<double>(&TAGGER_RelaxEpsilon),"Convergence epsilon value for RELAX tagger")
	("TaggerRetokenize",po::value<bool>(&TAGGER_Retokenize)->default_value(false),"Perform retokenization after PoS tagging")
	("TaggerForceSelect",po::value<std::string>(&Force)->default_value("retok"),"When the tagger must be forced to select only one tag per word (no|none,tagger,retok)")
	("GrammarFile",po::value<std::string>(&grammarFile),"Grammar file for chart parser")
	("DepTxalaFile",po::value<std::string>(&txalaFile),"Rule file for Txala dependency parser")

	//Matxin-en aukerak
	("TraceFile",po::value<std::string>(&Trace_File),"")
	("TransDictFile",po::value<std::string>(&DictionaryFile),"")
	("ChunkTypeDict",po::value<std::string>(&ChunkType_DictFile),"")
	("NounSemFile",po::value<std::string>(&Noun_SemanticFile),"")
	("LexSelFile",po::value<std::string>(&LexSelFile),"")
	("IntraMoveFile",po::value<std::string>(&Intra_MovementsFile),"")
	("InterMoveFile1",po::value<std::string>(&Inter_Movements1File),"")
	("InterMoveFile2",po::value<std::string>(&Inter_Movements2File),"")
	("InterMoveFile3",po::value<std::string>(&Inter_Movements3File),"")
	("GenerationMoveFile",po::value<std::string>(&Generation_MovementsFile),"")
	("SubCatFile",po::value<std::string>(&SubcatFile),"")
	("PrepFile",po::value<std::string>(&PrepositionsFile),"")
	("TripletsFile",po::value<std::string>(&Noun_SubcatFile),"")
	("VerbTransferFile",po::value<std::string>(&Verb_TransferFile),"")
	("ChunkOrderFile",po::value<std::string>(&Chunk_OrderFile),"")
	("NodeOrderFile",po::value<std::string>(&Node_OrderFile),"")
	("POSTransFile",po::value<std::string>(&POS_ToOrderFile),"")
	("MorphGenFile",po::value<std::string>(&Morpho_GenFile),"")
	("MeasuresGenFile",po::value<std::string>(&Measures_GenFile),"")
	("NoLexGenFile",po::value<std::string>(&NoLex_GenFile),"")
	("GenerationTagsFile",po::value<std::string>(&Tag_ToGenFile),"")
	("TagOrderFile",po::value<std::string>(&Tag_OrderFile),"")

	("UseLexRules",po::value<bool>(&UseLexRules)->default_value(false),"")

	("UsePrepRules",po::value<bool>(&UsePrepRules)->default_value(false),"")
	("UseSubcat",po::value<bool>(&UseSubcat)->default_value(false),"")
	("UseTripletes",po::value<bool>(&UseTripletes)->default_value(false),"")
	("FirstCase",po::value<bool>(&first_case)->default_value(false),"Choose first case (dictionary order) as last disambiguation step")

	("DoTrace",po::value<bool>(&DoTrace)->default_value(false),"")
	("DoVerbTrace",po::value<bool>(&DoVerbTrace)->default_value(false),"")
	("DoGenTrace",po::value<bool>(&DoGenTrace)->default_value(false),"")
	("DoPrepTrace",po::value<bool>(&DoPrepTrace)->default_value(false),"")
      ;

      po::options_description cl_op("All command line options");
      //cl_op.add(vis_cl).add(hid_cl);
      cl_op.add(vis_cl);

      po::options_description cf_op("All configuration file options");
      //cf_op.add(vis_cf).add(hid_cf);
      cf_op.add(vis_cf);

      po::variables_map vm;
      try {
        po::store(po::parse_command_line(ac, av, cl_op), vm);
        po::notify(vm);    
      } 
      catch (std::exception& e) {
	std::cerr<<"Exception ocurred while parsing command line (" << e.what()<< ")" <<std::endl;
	exit(1);
      }

      // Version required
      if (vm.count("version")) {
        #ifdef WIN32
          std::cerr<<"Option '--version' not available under Windows"<<std::endl;
        #else
          std::cerr<<"Option '--version' not available"<<std::endl;
          //std::cerr<<PACKAGE_STRING<<std::endl;
        #endif
        exit(0); // return to system
      }

      // Help screen required
      if (vm.count("help")) {
	std::cerr<<vis_cl<<std::endl;
	exit(0); // return to system
      }

      // Help screen required
      if (vm.count("help-cf")) {
	std::cerr<<vis_cf<<std::endl;
	exit(0); // return to system
      }

	// normal process requested. Load config file.
	std::wifstream fcfg;
	util::open_utf8_file(fcfg,util::string2wstring(ConfigFile));
	if (fcfg.fail()) {
	  std::cerr<<"Can not open config file '"+ConfigFile+"'."<<std::endl;
	  exit(1);
	}
	try {
  	  po::store(po::parse_config_file(fcfg, cf_op), vm);
	  po::notify(vm);    
        } 
        catch (std::exception& e) {
	  std::cerr<<"Exception ocurred while parsing configuration file (" << e.what()<< ")"<<std::endl;
	  exit(1);
	}
	
	// check options involving Filenames for environment vars expansion.
	ExpandFileName(tokFile);
	ExpandFileName(splitFile);
	ExpandFileName(usermapFile);
	ExpandFileName(locutionsFile);
	ExpandFileName(quantitiesFile);
	ExpandFileName(affixFile);
	ExpandFileName(probabilityFile);
	ExpandFileName(dictionaryFile); 
	ExpandFileName(npDataFile);
	ExpandFileName(punctuationFile);
	ExpandFileName(correctorFile);
	ExpandFileName(necFile); 
	ExpandFileName(hmmFile);
	ExpandFileName(relaxFile); 
	ExpandFileName(grammarFile); 
	ExpandFileName(txalaFile);
	
	// translate string options (including expanded filenames) to wstrings
	Lang = util::string2wstring(language);
	Locale = util::string2wstring(locale);
	TOK_TokenizerFile = util::string2wstring(tokFile);
	SPLIT_SplitterFile = util::string2wstring(splitFile);
	MACO_Decimal = util::string2wstring(macoDecimal);
	MACO_Thousand = util::string2wstring(macoThousand);
	MACO_UserMapFile = util::string2wstring(usermapFile);
	MACO_LocutionsFile = util::string2wstring(locutionsFile);
	MACO_QuantitiesFile = util::string2wstring(quantitiesFile);
	MACO_AffixFile = util::string2wstring(affixFile);
	MACO_ProbabilityFile = util::string2wstring(probabilityFile);
	MACO_DictionaryFile = util::string2wstring(dictionaryFile);
	MACO_NPDataFile = util::string2wstring(npDataFile);
	MACO_PunctuationFile = util::string2wstring(punctuationFile);
	MACO_CorrectorFile = util::string2wstring(correctorFile);
	NEC_NECFile = util::string2wstring(necFile);
	TAGGER_HMMFile = util::string2wstring(hmmFile);
	TAGGER_RelaxFile = util::string2wstring(relaxFile);
	PARSER_GrammarFile = util::string2wstring(grammarFile);
	DEP_TxalaFile = util::string2wstring(txalaFile);
	
	// Handle boolean options expressed with --myopt or --nomyopt in command line
	SetBooleanOptionCL(vm.count("train"),!vm.count("train"),TrainingOutput,"train");
	SetBooleanOptionCL(vm.count("flush"),vm.count("noflush"),AlwaysFlush,"flush");
	SetBooleanOptionCL(vm.count("afx"),vm.count("noafx"),MACO_AffixAnalysis,"afx");
	SetBooleanOptionCL(vm.count("usr"),vm.count("nousr"), MACO_UserMap,"usr");
	SetBooleanOptionCL(vm.count("loc"),vm.count("noloc"), MACO_MultiwordsDetection,"loc");
	SetBooleanOptionCL(vm.count("numb"),vm.count("nonumb"),MACO_NumbersDetection,"numb");
	SetBooleanOptionCL(vm.count("punt"),vm.count("nopunt"),MACO_PunctuationDetection,"punt");
	SetBooleanOptionCL(vm.count("date"),vm.count("nodate"),MACO_DatesDetection,"date");
	SetBooleanOptionCL(vm.count("ner"),vm.count("noner"),MACO_NERecognition,"ner");
	SetBooleanOptionCL(vm.count("quant"),vm.count("noquant"),MACO_QuantitiesDetection,"quant");
	SetBooleanOptionCL(vm.count("dict"),vm.count("nodict"),MACO_DictionarySearch,"dict");
	SetBooleanOptionCL(vm.count("rtkcon"),vm.count("nortkcon"),MACO_RetokContractions,"rtkcon");
	SetBooleanOptionCL(vm.count("prob"),vm.count("noprob"),MACO_ProbabilityAssignment,"prob");
	SetBooleanOptionCL(vm.count("orto"),vm.count("noorto"),MACO_OrthographicCorrection,"orto");
	SetBooleanOptionCL(vm.count("nec"),vm.count("nonec"),NEC_NEClassification,"nec");
	SetBooleanOptionCL(vm.count("rtk"),vm.count("nortk"),TAGGER_Retokenize,"rtk");

	//Matxin-en aukerak
	SetBooleanOptionCL(vm.count("lexrules"),vm.count("nolexrules"),UseLexRules,"lexrules");

	SetBooleanOptionCL(vm.count("preprules"),vm.count("nopreprules"),UsePrepRules,"preprules");
	SetBooleanOptionCL(vm.count("subcat"),vm.count("nosubcat"),UseSubcat,"subcat");
	SetBooleanOptionCL(vm.count("tripl"),vm.count("notripl"),UseTripletes,"tripl");
	SetBooleanOptionCL(vm.count("fcase"),vm.count("nofcase"),first_case,"fcase");

	SetBooleanOptionCL(vm.count("trace"),vm.count("notrace"),DoTrace,"trace");
	SetBooleanOptionCL(vm.count("vtrace"),vm.count("novtrace"),DoVerbTrace,"vtrace");
	SetBooleanOptionCL(vm.count("gtrace"),vm.count("nogtrace"),DoGenTrace,"gtrace");
	SetBooleanOptionCL(vm.count("ptrace"),vm.count("noptrace"),DoPrepTrace,"ptrace");


	write_xslt = false;
	if (vm.count("xsl")) write_xslt = true;

	server = false;
	if (vm.count("server")) server = true;
	
	// translate Tagger string to more useful integer values.
	if (Tagger=="hmm") TAGGER_which = HMM;
	else if (Tagger=="relax") TAGGER_which = RELAX;
	else {
	  TAGGER_which = HMM;
	  std::wcerr << L"WARNING: Invalid tagger algorithm '"+util::string2wstring(Tagger)+L"'. Using default." << std::endl;
	}
	
	// Translate ForceSelect string to more useful integer values.
	if (Force=="none" || Force=="no") TAGGER_ForceSelect = FORCE_NONE;
	else if (Force=="tagger") TAGGER_ForceSelect = FORCE_TAGGER;
	else if (Force=="retok") TAGGER_ForceSelect = FORCE_RETOK;
	else {
	  TAGGER_ForceSelect = FORCE_RETOK;
	  std::wcerr << L"WARNING: Invalid ForceSelect value '"+util::string2wstring(Force)+L"'. Using default." << std::endl;
	}
	
	/* // translate tracmod string (hex) into traces::TraceModule unsinged long */
	/* std::stringstream sin; */
	/* sin << std::hex << tracemod; */
	/* sin >> traces::TraceModule; */
    }

 private:

    void ExpandFileName(std::string &s) {
      if (s.empty()) return;
      
      std::string name = s;
      size_t n=name.find_first_of("$"); 
      if (n!=std::string::npos) {
	size_t i=name.find_first_of("/\\",n+1);
	if (i==std::string::npos) i=name.size();
	char* exp=getenv(name.substr(n+1,i-n-1).c_str());
	if (exp==NULL){
	  std::wcerr << L"WARNING: Undefined variable "+util::string2wstring(name.substr(n+1,i-n-1))+L" in configuration file "+util::string2wstring(ConfigFile)+L" expanded as empty string." << std::endl;
	  name = name.substr(0,n) + name.substr(i);
	}
	else {
	  name = name.substr(0,n) + std::string(exp) + name.substr(i);
	}
	
	s=name;
      }
    }
    
    void SetBooleanOptionCL (const int pos, const int neg, bool &opt, const std::string &name) {
      if (pos && neg)  
	std::wcerr << L"WARNING: Ambiguous specification for option --"+util::string2wstring(name)+L" in command line. Using default value." << std::endl;
      else if (pos)
	opt=true;
      else if (neg)
        opt=false;
      //else: nothing specified, leave things as they are.
    }

};


#endif
