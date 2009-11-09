VERSION=1.5.0
LANG1=es
LANG2=eu

## This is for directory names
PREFIX1=$(LANG1)-$(LANG2)
## This is for filenames 
PREFIX=$(LANG1)$(LANG2)
BASENAME=matxin-$(PREFIX1)

TARGETS_COMMON = $(PREFIX).bin $(LANG2)_changes_morph.bin $(LANG2)_measures_gen.bin \
		 $(LANG2)_morph_gen.bin $(LANG2)_morph_nolex.bin $(PREFIX1).cfg

TARGETS = $(TARGETS_COMMON)

#TransDictFile=MATXINDATADIR/dict/es-eu/eseu.bin
$(PREFIX).bin: $(PREFIX)_mur.xml
	lt-comp lr $(PREFIX)_mur.xml $@	

#GenerationTagsFile=MATXINDATADIR/dict/es-eu/eu_changes_morph.bin
$(LANG2)_changes_morph.bin: $(LANG2)_changes_morph.xml
	lt-comp lr $(LANG2)_changes_morph.xml $@

#MeasuresGenFile=MATXINDATADIR/dict/es-eu/eu_measures_gen.bin
$(LANG2)_measures_gen.bin: $(LANG2)_measures_gen.xml
	lt-comp lr $(LANG2)_measures_gen.xml $@

#MorphGenFile=MATXINDATADIR/dict/es-eu/eu_morph_gen.bin
$(LANG2)_morph_gen.bin: $(LANG2)_morph_gen_mur.xml
	lt-comp lr $(LANG2)_morph_gen_mur.xml $@

#NoLexGenFile=MATXINDATADIR/dict/es-eu/eu_morph_nolex.bin
$(LANG2)_morph_nolex.bin: $(LANG2)_morph_nolex.xml
	lt-comp lr $(LANG2)_morph_nolex.xml $@

EXTRA_DIST=$(PREFIX)_mur.xml $(LANG2)_changes_morph.xml $(LANG2)_measures_gen.xml \
		$(LANG2)_morph_gen_mur.xml $(LANG2)_morph_nolex.xml

matxin_es_eudir = $(datadir)/matxin/matxin-es-eu
matxin_es_eu_DATA=$(TARGETS)

#dictdataeseu_DATA = eseu_chunk_type.txt \
#                    eseu_mur.xml \
#                    eseu_lex.txt \
#                    eseu_prep.txt \
#                    eu_changes_morph.xml \
#                    eu_changes_sint.txt \
#                    eu_measures_gen.xml \
#                    eu_morph_gen_mur.xml \
#                    eu_morph_nolex.xml \
#                    eu_sem.txt \
#                    eu_verb_noun_subcat.txt \
#                    eu_verb_subcat.txt
#

