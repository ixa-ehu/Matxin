source ../environment.sh

INPUT=input.xml
OUTPUT=output.xml
EXPECTED=expected_output.xml

cat $INPUT | $BINROOT/matxin-xfer-intra $DATAROOT/intrachunk_move.dat > $OUTPUT

diff -Naur $EXPECTED $OUTPUT
