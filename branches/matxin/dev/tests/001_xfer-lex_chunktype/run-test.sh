source ../environment.sh

INPUT=input.xml
OUTPUT=output.xml
EXPECTED=expected_output.xml

cat $INPUT | $BINROOT/matxin-xfer-lex -s $DATAROOT/eu_sem.bin -c $DATAROOT/eseu_chunk_type.txt $DATAROOT/eseu.bin > $OUTPUT

diff -Naur $EXPECTED $OUTPUT
