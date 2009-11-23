#!/usr/bin/python

## 
##  Suggestion: we have two new programs, called "matxin-gen-chunks" and 
##    "matxin-gen-nodes". These will do chunk/node addition. 
##  Rationale: it might be better to think of adding e.g. the definite
##    article in English as a generation step, e.g. "generating the article
##    based on a feature of the head node", or generating a preposition as 
##    equally. The pipeline would thus be:
##     
##    ... |  matxin_xfer_inter | matxin_gen_chunks | matxin_gen_interchunk | matxin_gen_nodes | matxin_gen_interchunk | ...
##
##    

import sys, codecs, copy, Ft;
from Ft.Xml.Domlette import NonvalidatingReader;
from Ft.Xml.XPath import Evaluate;

input = '';
c = sys.stdin.read(1);
while c: #{
	input = input + c;
	c = sys.stdin.read();
#}

doc = NonvalidatingReader.parseString(input);

for node in doc.xpath('.//NODE[@defnes="[def]"]'): #{
	article = Ft.Xml.cDomlette.Element(doc, '', 'NODE'); # New node
	article.setAttributeNS('', 'pos', '[det][def]');     # Set attributes
	article.setAttributeNS('', 'lem', 'the');
	article.setAttributeNS('', 'UpCase', 'none');
	article.setAttributeNS('', 'mi', '[sg]');
	node.appendChild(article);                           # Add as a child node
	node.removeAttributeNS(None, 'defnes');              # Remove defnes attribute
#}

Ft.Xml.Domlette.PrettyPrint(doc);
