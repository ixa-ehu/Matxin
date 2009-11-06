VERSION=0.1.1
LANG1=fo
LANG2=en
PREFIX1=$(LANG1)-$(LANG2)
PREFIX2=$(LANG2)-$(LANG1)

BASENAME=matxin-$(PREFIX1)

TARGETS_COMMON = $(PREFIX1).automorf.bin $(PREFIX1).autobil.bin $(PREFIX1).autogen.bin \
     $(PREFIX2).automorf.bin $(PREFIX2).autobil.bin \
     $(PREFIX2).autogen.bin \
     $(PREFIX1).rlx.bin $(PREFIX1).dep.rlx.bin 

$(PREFIX1).automorf.bin: $(BASENAME).$(LANG1).dix
	apertium-validate-dictionary $(BASENAME).$(LANG1).dix
	lt-comp lr $(BASENAME).$(LANG1).dix $@

$(PREFIX1).autobil.bin: $(BASENAME).$(PREFIX1).dix
	apertium-validate-dictionary $(BASENAME).$(PREFIX1).dix
	lt-comp lr $(BASENAME).$(PREFIX1).dix $@

$(PREFIX1).autogen.bin: $(BASENAME).$(LANG2).dix
	apertium-validate-dictionary $(BASENAME).$(LANG2).dix
	lt-comp rl $(BASENAME).$(LANG2).dix $@


$(PREFIX1).rlx.bin: $(BASENAME).$(PREFIX1).rlx
	cg-comp $(BASENAME).$(PREFIX1).rlx $@

$(PREFIX1).dep.rlx.bin: $(BASENAME).$(PREFIX1).dep.rlx
	cg-comp $(BASENAME).$(PREFIX1).dep.rlx $@


$(PREFIX2).automorf.bin: $(BASENAME).$(LANG2).dix
	apertium-validate-dictionary $(BASENAME).$(LANG2).dix
	lt-comp lr $(BASENAME).$(LANG2).dix $@

$(PREFIX2).autobil.bin: $(BASENAME).$(PREFIX1).dix
	apertium-validate-dictionary $(BASENAME).$(PREFIX1).dix
	lt-comp rl $(BASENAME).$(PREFIX1).dix $@

$(PREFIX2).autogen.bin: $(BASENAME).$(LANG1).dix
	apertium-validate-dictionary $(BASENAME).$(LANG1).dix
	lt-comp rl $(BASENAME).$(LANG1).dix $@

EXTRA_DIST=$(BASENAME).$(LANG1).dix $(BASENAME).$(PREFIX1).dix \
	   $(BASENAME).$(LANG2).dix \
           $(BASENAME).$(PREFIX1).rlx \
           $(BASENAME).$(PREFIX1).dep.rlx \
           $(PREFIX1).prob $(PREFIX2).prob \
	   $(BASENAME).$(LANG1).tsx \
	   $(BASENAME).$(LANG2).tsx modes.xml

clean-dicts:
	touch $(BASENAME).$(LANG1).dix $(BASENAME).$(LANG2).dix $(BASENAME).$(PREFIX1).dix
	rm $(BASENAME).$(LANG1).dix
	rm $(BASENAME).$(LANG2).dix
	rm $(BASENAME).$(PREFIX1).dix

# since we use apertium to call the pipeline, install in apertium:
matxin_fo_endir=$(prefix)/share/apertium/matxin-$(PREFIX1)/
matxin_fo_modesdir=$(prefix)/share/apertium/modes/

modes/$(PREFIX1).mode: modes.xml
	apertium-gen-modes modes.xml
	cp *.mode modes/

modes/$(PREFIX2).mode: modes.xml 
	apertium-gen-modes modes.xml
	cp *.mode modes/

matxin_fo_en_DATA=$(PREFIX1).automorf.bin $(PREFIX2).automorf.bin \
                    $(PREFIX1).autobil.bin $(PREFIX2).autobil.bin \
                    $(PREFIX1).autogen.bin $(PREFIX2).autogen.bin \
                    $(PREFIX1).prob $(PREFIX2).prob \
		    $(PREFIX1).rlx.bin $(PREFIX1).dep.rlx.bin \
	            modes/$(PREFIX1).mode modes/$(PREFIX2).mode modes.xml

install-data-local:
	mv modes modes.bak
	apertium-gen-modes modes.xml matxin-$(PREFIX1)
	rm -rf modes
	mv modes.bak modes
	test -d $(matxin_fo_modesdir) || mkdir $(matxin_fo_modesdir)
	$(INSTALL_DATA) $(PREFIX1).mode $(matxin_fo_modesdir)
	$(INSTALL_DATA) $(PREFIX2).mode $(matxin_fo_modesdir)
	rm $(PREFIX1).mode $(PREFIX2).mode


CLEANFILES = -rf $(TARGETS_COMMON) modes
