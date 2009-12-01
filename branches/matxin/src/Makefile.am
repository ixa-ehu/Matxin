matxindir = $(prefix)/share/matxin
matxininclude = $(prefix)/include/matxin-$(GENERIC_API_VERSION)
matxinlib = $(prefix)/lib

matxin_DATA = modes.dtd modes2bash.xsl matxin-createmodes.awk


EXTRA_DIST = config.h data_manager.h matxin_string_utils.h simpleregex.h string_utils.h utf_converter.h XML_reader.h gen_fst2regex.sh gen_Matxin_translator.sh deformat.xsl txt-format.xml rtf-format.xml html-format.xml

bin_PROGRAMS = matxin-xfer-lex matxin-xfer-intra matxin-xfer-inter matxin-xfer-prep matxin-gen-inter matxin-gen-intra \
	       matxin-gen-morph matxin-xfer-verb \
               matxin-reformat matxin-destxt matxin-deshtml
#txt-deformat html-deformat rtf-deformat

bin_SCRIPTS = matxin-gen-modes fst2regex.pl matxin add_definite_article.py

matxin_xfer_lex_SOURCES = matxin_xfer_lex.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h string_utils.cc string_utils.h
matxin_xfer_lex_LDADD =  -lboost_filesystem

matxin_xfer_intra_SOURCES = matxin_xfer_intra.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h
matxin_xfer_intra_LDADD =      -lboost_filesystem

matxin_xfer_inter_SOURCES = matxin_xfer_inter.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h
matxin_xfer_inter_LDADD =      -lboost_filesystem

matxin_xfer_prep_SOURCES = matxin_xfer_prep.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h
matxin_xfer_prep_LDADD =      -lboost_filesystem

matxin_xfer_verb_SOURCES = matxin_xfer_verb.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h
matxin_xfer_verb_LDADD =      -lboost_filesystem

matxin_gen_inter_SOURCES = matxin_gen_inter.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h
matxin_gen_inter_LDADD =      -lboost_filesystem

matxin_gen_intra_SOURCES = matxin_gen_intra.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h
matxin_gen_intra_LDADD =      -lboost_filesystem

matxin_gen_morph_SOURCES = matxin_gen_morph.cc data_manager.cc utf_converter.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h string_utils.cc
matxin_gen_morph_LDADD =      -lboost_filesystem

matxin_reformat_SOURCES = matxin_reformat.cc utf_converter.cc data_manager.cc XML_reader.cc simpleregex.cc matxin_string_utils.cc matxin_string_utils.h

matxin_destxt_SOURCES = txt-deformat.cc
matxin_deshtml_SOURCES = html-deformat.cc
#rtf_deformat_SOURCES = rtf-deformat.cc

# I'm Using the files from fran instead -KBU
# txt-deformat.cc: txt-format.xml Makefile.am deformat.xsl
# 	$(XSLTPROC) --stringparam mode matxin deformat.xsl txt-format.xml | $(FLEX) -Cfer -t >$@

# html-deformat.cc: html-format.xml Makefile.am deformat.xsl
# 	$(XSLTPROC) --stringparam mode matxin deformat.xsl html-format.xml | $(FLEX) -Cfer -t >$@

#rtf-deformat.cc: rtf-format.xml Makefile.am deformat.xsl
#	$(XSLTPROC) --stringparam mode matxin deformat.xsl rtf-format.xml | $(FLEX) -Cfer -t >$@

#Matxin_translator: Makefile.am gen_Matxin_translator.sh
#	@echo "Creating $@ script"
#	@echo "#!$(BASH)" > $@
#	@echo "MATXIN_DIR=\"$(prefix)\"" >> $@
#	@cat gen_Matxin_translator.sh >> $@
#	@chmod a+x $@
#
matxin-gen-modes: Makefile.am modes-header.sh
	@echo "Creating matxin-gen-modes script"
	@echo "#!$(BASH)" > $@
	@echo "MATXIN_DIR="$(matxindir) >> $@
	@cat modes-header.sh >> $@
	@echo "$(XMLLINT) --dtdvalid $(matxindir)/modes.dtd --noout \$$FILE1 && \\" >> $@
	@if [ `basename $(XSLTPROC)` == xsltproc ]; \
          then echo "$(XSLTPROC) --stringparam prefix $(prefix)/bin --stringparam dataprefix \$$FULLDIRNAME  $(matxindir)/modes2bash.xsl \$$FILE1 | awk -f $(matxindir)/matxin-createmodes.awk PARAM=\$$FULLDIRNAME"; \
          else echo "$(XSLTPROC) $(matxindir)/modes2bash.xsl \$$FILE1 \\\$$prefix=$(prefix)/bin \\\$$dataprefix=\$$FULLDIRNAME| awk -f $(matxindir)/matxin-createmodes.awk PARAM=\$$FULLDIRNAME"; \
          fi >> $@ 
	@chmod a+x $@

matxin: Makefile.am matxin-header.sh
	@echo "Creating matxin script"
	@echo "#!$(BASH)" > $@
	@echo "MATXIN_PATH=\"$(prefix)/bin\"" >>$@
	@echo "DEFAULT_DIRECTORY=\"$(prefix)/share/matxin\"" >>$@
	@cat matxin-header.sh >>$@
	@chmod a+x $@


fst2regex.pl: Makefile.am gen_fst2regex.sh
	@echo "Creating $@ script"
	@echo "#!$(PERL)" > $@
	@cat gen_fst2regex.sh >> $@
	@chmod a+x $@
