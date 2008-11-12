bin_PROGRAMS = Analyzer LT ST_intra ST_inter ST_prep ST_verb SG_inter SG_intra \
               MG reFormat txt-deformat html-deformat rtf-deformat

bin_SCRIPTS = Matxin_translator fst2regex.pl

Analyzer_SOURCES = Analyzer.C
Analyzer_LDADD = -lmorfo -lcfg+ -lpcre -ldb_cxx -lfries -lomlet 

LT_SOURCES = LT.C data_manager.C XML_reader.C simpleregex.C
LT_LDADD = -llttoolbox-2.0 -lmorfo -lcfg+ -lpcre -ldb_cxx -lxml2 -lfries -lomlet

ST_intra_SOURCES = ST_intra.C data_manager.C XML_reader.C simpleregex.C
ST_intra_LDADD = -lmorfo -lcfg+ -lpcre -ldb_cxx -lxml2 -lfries -lomlet 

ST_inter_SOURCES = ST_inter.C data_manager.C XML_reader.C simpleregex.C
ST_inter_LDADD = -lmorfo -lcfg+ -lpcre -ldb_cxx -lxml2 -lfries -lomlet 

ST_prep_SOURCES = ST_prep.C data_manager.C XML_reader.C simpleregex.C
ST_prep_LDADD = -lmorfo -lcfg+ -lpcre -ldb_cxx -lxml2 -lfries -lomlet 

ST_verb_SOURCES = ST_verb.C data_manager.C XML_reader.C simpleregex.C
ST_verb_LDADD = -lmorfo -lcfg+ -lpcre -ldb_cxx -lxml2 -lfries -lomlet 

SG_inter_SOURCES = SG_inter.C data_manager.C XML_reader.C simpleregex.C
SG_inter_LDADD = -lmorfo -lcfg+ -lpcre -ldb_cxx -lxml2 -lfries -lomlet 

SG_intra_SOURCES = SG_intra.C data_manager.C XML_reader.C simpleregex.C
SG_intra_LDADD = -lmorfo -lcfg+ -lpcre -ldb_cxx -lxml2 -lfries -lomlet 

MG_SOURCES = MG.C data_manager.C XML_reader.C simpleregex.C
MG_LDADD = -lmorfo -llttoolbox-2.0 -lcfg+ -lpcre -ldb_cxx -lxml2 -lfries -lomlet 

reFormat_SOURCES = reFormat.C data_manager.C XML_reader.C simpleregex.C
reFormat_LDADD = -lpcre -lxml2

txt_deformat_SOURCES = txt-deformat.C
html_deformat_SOURCES = html-deformat.C
rtf_deformat_SOURCES = rtf-deformat.C

txt-deformat.C: txt-format.xml Makefile.am deformat.xsl
	$(XSLTPROC) --stringparam mode matxin deformat.xsl txt-format.xml | $(FLEX) -Cfer -t >$@

html-deformat.C: html-format.xml Makefile.am deformat.xsl
	$(XSLTPROC) --stringparam mode matxin deformat.xsl html-format.xml | $(FLEX) -Cfer -t >$@

rtf-deformat.C: rtf-format.xml Makefile.am deformat.xsl
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
