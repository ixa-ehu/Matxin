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
ZCAT=zcat
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
	@echo "Indexing Wordnet files..."
	@echo "   Indexing common Wordnet files..."
	@rm -f $(pkgdatadir)/freeling/common/wn16.db
	@cat $(pkgdatadir)/freeling/common/wn16.src | $(INDEXDICT) $(pkgdatadir)/freeling/common/wn16.db
 ifeq ($(ES),true)
	@echo "   Indexing Spanish Wordnet..."
	@rm -f $(pkgdatadir)/freeling/es/senses16.db
	@cat $(pkgdatadir)/freeling/es/senses16.src | $(INDEXDICT) $(pkgdatadir)/freeling/es/senses16.db
 endif
 ifeq ($(EN),true)
	@echo "   Indexing English Wordnet..."
	@rm -f $(pkgdatadir)/freeling/en/senses16.db
	@cat $(pkgdatadir)/freeling/en/senses16.src | $(INDEXDICT) $(pkgdatadir)/freeling/en/senses16.db
 endif


	@echo
	@echo "Indexing Freeling dictionaries. This will take some minutes."
 ifeq ($(ES),true)
	@echo "   Indexing Spanish dictionary..."
	@rm -f $(pkgdatadir)/freeling/es/maco.db
	@$(ZCAT) $(pkgdatadir)/freeling/es/dicc.src.gz | $(INDEXDICT) $(pkgdatadir)/freeling/es/maco.db
	@chmod +r $(pkgdatadir)/freeling/es/maco.db
 endif
 ifeq ($(EN),true)
	@echo "   Indexing English dictionary..."
	@rm -f $(pkgdatadir)/freeling/en/maco.db
	@$(ZCAT) $(pkgdatadir)/freeling/en/dicc.src.gz | $(INDEXDICT) $(pkgdatadir)/freeling/en/maco.db
	@chmod +r $(pkgdatadir)/freeling/en/maco.db
 endif


	@echo
	@echo "Compiling verb-transfer grammar."
 ifeq ($(ES),true)
	@echo "   Compiling Spanish verb-transfer grammar..."
	@rm -f $(pkgdatadir)/gram/es-eu/eseu_verb_transfer.dat
	@$(bindir)/fst2regex.pl < $(pkgdatadir)/gram/es-eu/eseu_verb_transfer.fst > $(pkgdatadir)/gram/es-eu/eseu_verb_transfer.dat
	@chmod +r $(pkgdatadir)/gram/es-eu/eseu_verb_transfer.dat
 endif
 ifeq ($(EN),true)
	@echo "   Compiling English verb-transfer grammar..."
	@rm -f $(pkgdatadir)/gram/en-eu/eneu_verb_transfer.dat
	@$(bindir)/fst2regex.pl < $(pkgdatadir)/gram/en-eu/eneu_verb_transfer.fst > $(pkgdatadir)/gram/en-eu/eneu_verb_transfer.dat
	@chmod +r $(pkgdatadir)/gram/en-eu/eneu_verb_transfer.dat
 endif

	@echo
	@echo "Compiling dictionaries using lttoolbox."
 ifeq ($(ES),true)
	@echo "   Compiling Spanish dictionary..."
	@rm -f $(pkgdatadir)/dict/es-eu/eseu.bin
	## Change this according to the dictionary we are using
	@$(LT_COMP) lr $(pkgdatadir)/dict/es-eu/eseu_mur.xml $(pkgdatadir)/dict/es-eu/eseu.bin
	@chmod +r $(pkgdatadir)/dict/es-eu/eseu.bin

	@rm -f $(pkgdatadir)/dict/es-eu/eu_changes_morph.bin
	@$(LT_COMP) lr $(pkgdatadir)/dict/es-eu/eu_changes_morph.xml $(pkgdatadir)/dict/es-eu/eu_changes_morph.bin
	@chmod +r $(pkgdatadir)/dict/es-eu/eu_changes_morph.bin

	@rm -f $(pkgdatadir)/dict/es-eu/eu_measures_gen.bin
	@$(LT_COMP) rl $(pkgdatadir)/dict/es-eu/eu_measures_gen.xml $(pkgdatadir)/dict/es-eu/eu_measures_gen.bin
	@chmod +r $(pkgdatadir)/dict/es-eu/eu_measures_gen.bin

	## WARNING: Morphological generation dictionary is too big to compile in a single PC
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
	## Change this according to the dictionary we are using
	@$(LT_COMP) lr $(pkgdatadir)/dict/en-eu/eneu_mur.xml $(pkgdatadir)/dict/en-eu/eneu.bin
	@chmod +r $(pkgdatadir)/dict/en-eu/eneu.bin

	@rm -f $(pkgdatadir)/dict/en-eu/eu_changes_morph.bin
	@$(LT_COMP) lr $(pkgdatadir)/dict/en-eu/eu_changes_morph.xml $(pkgdatadir)/dict/en-eu/eu_changes_morph.bin
	@chmod +r $(pkgdatadir)/dict/en-eu/eu_changes_morph.bin

	@rm -f $(pkgdatadir)/dict/en-eu/eu_measures_gen.bin
	@$(LT_COMP) rl $(pkgdatadir)/dict/en-eu/eu_measures_gen.xml $(pkgdatadir)/dict/en-eu/eu_measures_gen.bin
	@chmod +r $(pkgdatadir)/dict/en-eu/eu_measures_gen.bin

	## WARNING: Morphological generation dictionary is too big to compile in a single PC
	rm -f $(pkgdatadir)/dict/en-eu/eu_morph_gen.bin
	$(LT_COMP) rl $(pkgdatadir)/dict/en-eu/eu_morph_gen_mur.xml $(pkgdatadir)/dict/en-eu/eu_morph_gen.bin
	chmod +r $(pkgdatadir)/dict/en-eu/eu_morph_gen.bin

	@rm -f $(pkgdatadir)/dict/en-eu/eu_morph_nolex.bin
	@$(LT_COMP) rl $(pkgdatadir)/dict/en-eu/eu_morph_nolex.xml $(pkgdatadir)/dict/en-eu/eu_morph_nolex.bin
	@chmod +r $(pkgdatadir)/dict/en-eu/eu_morph_nolex.bin
 endif

