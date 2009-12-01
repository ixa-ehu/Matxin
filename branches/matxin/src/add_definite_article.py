#!/opt/local/bin/python

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

for node in doc.xpath('.//NODE[@defnes="[ind]"]'): #{
	hasdet = False;
	for child in node.xpath('.//NODE[@pos="[det]"]'): #{
		hasdet = True;
		break;
	#}
	if hasdet: #{
		continue;
	#}
	article = Ft.Xml.cDomlette.Element(doc, None, 'NODE'); # New node
	article.setAttributeNS(None, 'pos', '[det]');     # Set attributes
	article.setAttributeNS(None, 'lem', 'a');
	article.setAttributeNS(None, 'UpCase', 'none');
	article.setAttributeNS(None, 'mi', '[ind][sg]');
	node.appendChild(article);                           # Add as a child node
	node.removeAttributeNS(None, 'defnes');              # Remove defnes attribute
#}

for node in doc.xpath('.//NODE[@defnes="[def]"]'): #{
	hasdet = False;
	for child in node.xpath('.//NODE[@pos="[det]"]'): #{
		hasdet = True;
		break;
	#}
	if hasdet: #{
		continue;
	#}
	article = Ft.Xml.cDomlette.Element(doc, None, 'NODE'); # New node
	article.setAttributeNS(None, 'pos', '[det]');     # Set attributes
	article.setAttributeNS(None, 'lem', 'the');
	article.setAttributeNS(None, 'UpCase', 'none');
	article.setAttributeNS(None, 'mi', '[def][sg]');
	node.appendChild(article);                           # Add as a child node
	node.removeAttributeNS(None, 'defnes');              # Remove defnes attribute
#}

Ft.Xml.Domlette.PrettyPrint(doc);

