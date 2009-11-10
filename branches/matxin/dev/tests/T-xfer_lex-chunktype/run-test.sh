DATAROOT=/home/fran/source/matxin/branches/matxin-es-eu/
BINROOT=/home/fran/local/bin/
INPUT=input.xml
OUTPUT=output.xml
EXPECTED=expected_output.xml

cat $INPUT | $BINROOT/matxin-xfer-lex -s $DATAROOT/eu_sem.bin -c $DATAROOT/eseu_chunktype.txt $DATAROOT/eseu.bin > $OUTPUT

diff -Naur $EXPECTED $OUTPUT
