SUBDIRS = config dict freeling gram

if USE_ES
ES=true
else
ES=false
endif

if USE_EN
EN=true
else
EN=false
endif

if OS_IS_SOLARIS
ZCAT=gzcat
else
if OS_IS_DARWIN
ZCAT=gzcat 
else
ZCAT=zcat
endif
endif

# IMPORTANT: ifeq expressions need a leading space, automake bug?
install-data-hook:
	@echo
	@echo "Adapting default configuration file"
 ifeq ($(ES),true)
	@echo "   Adapting es-eu.cfg..."
	@cat config/es-eu.cfg | sed 's%MATXINDATADIR%'$(pkgdatadir)'%g' > $(pkgdatadir)/config/es-eu.cfg
 endif
 ifeq ($(EN),true)
	@echo "   Adapting en-eu.cfg..."
	@cat config/en-eu.cfg | sed 's%MATXINDATADIR%'$(pkgdatadir)'%g' > $(pkgdatadir)/config/en-eu.cfg
 endif

	@echo
	@echo "Indexing Freeling dictionaries."
 ifeq ($(ES),true)
	@echo "   Indexing Spanish dictionary..."
	@rm -f $(pkgdatadir)/freeling/es/corrector/corrector.src
	@cat $(pkgdatadir)/freeling/es/dicc.src | $(DICC2PHON) $(pkgdatadir)/freeling/es/corrector/phonetics-corr.dat > $(pkgdatadir)/freeling/es/corrector/corrector.src
	@chmod +r $(pkgdatadir)/freeling/es/corrector/corrector.src
 endif
 # ifeq ($(EN),true)
 # 	@echo "   Indexing English dictionary..."
 # 	@rm -f $(pkgdatadir)/freeling/en/corrector/corrector.src
 # 	@cat $(pkgdatadir)/freeling/en/dicc.src | $(DICC2PHON) $(pkgdatadir)/freeling/en/corrector/phonetics-corr.dat > $(pkgdatadir)/freeling/en/corrector/corrector.src
 # 	@chmod +r $(pkgdatadir)/freeling/en/corrector/corrector.src
 # endif


	@echo
	@echo "Compiling verb-transfer grammar."
 ifeq ($(ES),true)
	@echo "   Compiling Spanish verb-transfer grammar..."
	@rm -f $(pkgdatadir)/gram/es-eu/eseu_verb_transfer.fst
	@cd $(pkgdatadir)/gram/es-eu && $(FOMA) -f $(pkgdatadir)/gram/es-eu/eseu_verb_transfer.foma >& /dev/null
	@chmod +r $(pkgdatadir)/gram/es-eu/eseu_verb_transfer.fst
 endif
 ifeq ($(EN),true)
	@echo "   Compiling English verb-transfer grammar..."
	@rm -f $(pkgdatadir)/gram/en-eu/eneu_verb_transfer.fst
	@cd $(pkgdatadir)/gram/en-eu && $(FOMA) -f $(pkgdatadir)/gram/en-eu/eneu_verb_transfer.foma >& /dev/null
	@chmod +r $(pkgdatadir)/gram/en-eu/eneu_verb_transfer.fst
 endif

	@echo
	@echo "Compiling dictionaries using lttoolbox."
 ifeq ($(ES),true)
	@echo "   Compiling Spanish dictionary..."
	@rm -f $(pkgdatadir)/dict/es-eu/eseu.bin
	@$(LT_COMP) lr $(pkgdatadir)/dict/es-eu/eseu_mur.xml $(pkgdatadir)/dict/es-eu/eseu.bin
	@chmod +r $(pkgdatadir)/dict/es-eu/eseu.bin

	@rm -f $(pkgdatadir)/dict/es-eu/eu_changes_morph.bin
	@$(LT_COMP) lr $(pkgdatadir)/dict/es-eu/eu_changes_morph.xml $(pkgdatadir)/dict/es-eu/eu_changes_morph.bin
	@chmod +r $(pkgdatadir)/dict/es-eu/eu_changes_morph.bin

	@rm -f $(pkgdatadir)/dict/es-eu/eu_measures_gen.bin
	@$(LT_COMP) rl $(pkgdatadir)/dict/es-eu/eu_measures_gen.xml $(pkgdatadir)/dict/es-eu/eu_measures_gen.bin
	@chmod +r $(pkgdatadir)/dict/es-eu/eu_measures_gen.bin

	rm -f $(pkgdatadir)/dict/es-eu/eu_morph_gen.bin
	$(LT_COMP) rl $(pkgdatadir)/dict/es-eu/eu_morph_gen_mur.xml $(pkgdatadir)/dict/es-eu/eu_morph_gen.bin
	chmod +r $(pkgdatadir)/dict/es-eu/eu_morph_gen.bin

	@rm -f $(pkgdatadir)/dict/es-eu/eu_morph_nolex.bin
	@$(LT_COMP) rl $(pkgdatadir)/dict/es-eu/eu_morph_nolex.xml $(pkgdatadir)/dict/es-eu/eu_morph_nolex.bin
	@chmod +r $(pkgdatadir)/dict/es-eu/eu_morph_nolex.bin
 endif

 ifeq ($(EN),true)
	@echo "   Compiling English dictionary..."
	@rm -f $(pkgdatadir)/dict/en-eu/eneu.bin
	@$(LT_COMP) lr $(pkgdatadir)/dict/en-eu/eneu_mur.xml $(pkgdatadir)/dict/en-eu/eneu.bin
	@chmod +r $(pkgdatadir)/dict/en-eu/eneu.bin

	@rm -f $(pkgdatadir)/dict/en-eu/eu_changes_morph.bin
	@$(LT_COMP) lr $(pkgdatadir)/dict/en-eu/eu_changes_morph.xml $(pkgdatadir)/dict/en-eu/eu_changes_morph.bin
	@chmod +r $(pkgdatadir)/dict/en-eu/eu_changes_morph.bin

	@rm -f $(pkgdatadir)/dict/en-eu/eu_measures_gen.bin
	@$(LT_COMP) rl $(pkgdatadir)/dict/en-eu/eu_measures_gen.xml $(pkgdatadir)/dict/en-eu/eu_measures_gen.bin
	@chmod +r $(pkgdatadir)/dict/en-eu/eu_measures_gen.bin

	rm -f $(pkgdatadir)/dict/en-eu/eu_morph_gen.bin
	$(LT_COMP) rl $(pkgdatadir)/dict/en-eu/eu_morph_gen_mur.xml $(pkgdatadir)/dict/en-eu/eu_morph_gen.bin
	chmod +r $(pkgdatadir)/dict/en-eu/eu_morph_gen.bin

	@rm -f $(pkgdatadir)/dict/en-eu/eu_morph_nolex.bin
	@$(LT_COMP) rl $(pkgdatadir)/dict/en-eu/eu_morph_nolex.xml $(pkgdatadir)/dict/en-eu/eu_morph_nolex.bin
	@chmod +r $(pkgdatadir)/dict/en-eu/eu_morph_nolex.bin
 endif

