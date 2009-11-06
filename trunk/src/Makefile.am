EXTRA_DIST = config.h data_manager.h matxin_string_utils.h simpleregex.h string_utils.h utf_converter.h XML_reader.h gen_fst2regex.sh gen_Matxin_translator.sh deformat.xsl txt-format.xml rtf-format.xml html-format.xml

bin_PROGRAMS = Analyzer LT ST_intra ST_inter ST_prep ST_verb SG_inter SG_intra \
               MG reFormat txt-deformat html-deformat rtf-deformat

bin_SCRIPTS = Matxin_translator fst2regex.pl

Analyzer_SOURCES = Analyzer.cc IORedirectHandler.cc IORedirectHandler.hpp
Analyzer_LDADD = -lmorfo -lcfg+ -ldb_cxx -lfries -lomlet -lboost_filesystem

LT_SOURCES = LT.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h IORedirectHandler.cc IORedirectHandler.hpp
LT_LDADD = -lmorfo -lcfg+ -ldb_cxx -lfries -lomlet -lboost_filesystem

ST_intra_SOURCES = ST_intra.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h IORedirectHandler.cc IORedirectHandler.hpp
ST_intra_LDADD = -lmorfo -lcfg+ -ldb_cxx -lfries -lomlet -lboost_filesystem

ST_inter_SOURCES = ST_inter.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h IORedirectHandler.cc IORedirectHandler.hpp
ST_inter_LDADD = -lmorfo -lcfg+ -ldb_cxx -lfries -lomlet -lboost_filesystem

ST_prep_SOURCES = ST_prep.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h IORedirectHandler.cc IORedirectHandler.hpp
ST_prep_LDADD = -lmorfo -lcfg+ -ldb_cxx -lfries -lomlet -lboost_filesystem

ST_verb_SOURCES = ST_verb.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h IORedirectHandler.cc IORedirectHandler.hpp
ST_verb_LDADD = -lmorfo -lcfg+ -ldb_cxx -lfries -lomlet -lboost_filesystem

SG_inter_SOURCES = SG_inter.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h IORedirectHandler.cc IORedirectHandler.hpp
SG_inter_LDADD = -lmorfo -lcfg+ -ldb_cxx -lfries -lomlet -lboost_filesystem

SG_intra_SOURCES = SG_intra.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h IORedirectHandler.cc IORedirectHandler.hpp
SG_intra_LDADD = -lmorfo -lcfg+ -ldb_cxx -lfries -lomlet -lboost_filesystem

MG_SOURCES = MG.cc data_manager.cc utf_converter.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h IORedirectHandler.cc IORedirectHandler.hpp
MG_LDADD = -lmorfo -lcfg+ -ldb_cxx -lfries -lomlet -lboost_filesystem

reFormat_SOURCES = reFormat.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h IORedirectHandler.cc IORedirectHandler.hpp

txt_deformat_SOURCES = txt-deformat.cc
html_deformat_SOURCES = html-deformat.cc
rtf_deformat_SOURCES = rtf-deformat.cc

txt-deformat.cc: txt-format.xml Makefile.am deformat.xsl
	$(XSLTPROC) --stringparam mode matxin deformat.xsl txt-format.xml | $(FLEX) -Cfer -t >$@

html-deformat.cc: html-format.xml Makefile.am deformat.xsl
	$(XSLTPROC) --stringparam mode matxin deformat.xsl html-format.xml | $(FLEX) -Cfer -t >$@

rtf-deformat.cc: rtf-format.xml Makefile.am deformat.xsl
	$(XSLTPROC) --stringparam mode matxin deformat.xsl rtf-format.xml | $(FLEX) -Cfer -t >$@

Matxin_translator: Makefile.am gen_Matxin_translator.sh
	@echo "Creating $@ script"
	@echo "#!$(BASH)" > $@
	@echo "MATXIN_DIR=\"$(prefix)\"" >> $@
	@cat gen_Matxin_translator.sh >> $@
	@chmod a+x $@

fst2regex.pl: Makefile.am gen_fst2regex.sh
	@echo "Creating $@ script"
	@echo "#!$(PERL)" > $@
	@cat gen_fst2regex.sh >> $@
	@chmod a+x $@
