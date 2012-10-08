
RE_FORMATER_PARAM=""
POSTEDIT=/tmp/matxin-$$
REMOVE_TMP="1";

usage()
{
  echo "Script options"
  echo "--------------"
  echo "USAGE: $(basename $0) -c /path/to/config/file.cfg [-f format] [-i /path/to/input/file] [-o /path/to/output/file] [-p /path/to/post-edition/files-roots] [-u] [module options]"
  echo " -f format                       one of: txt (default), html, rtf"
  echo " -i input file                   read input from file"
  echo " -o output file                  write output in file"
  echo " -p root of post-edition files   Save post-edition XML into $root.format.xml and $root.translation.xml"
  echo " -u                              display mark '*' for unknown words"
  echo ""
  echo ""
  echo "Module options"
  echo "--------------"
  cd $MATXIN_DIR/bin >& /dev/null
  ./Analyzer --help
  exit 1;
}


ARGS=$(getopt ":uhf:c:i:o:p:" -- $*)
for i
do
  case "$i" in
    -c) shift; CONFIG_FILE=$1; shift;;
    -f) shift; FORMAT=$1; shift;;
    -i) shift; INPUT=$1; shift;;
    -o) shift; OUTPUT=$1; shift;;
    -p) shift; POSTEDIT=$1; REMOVE_TMP=0; shift;;
    -u) RE_FORMATER_PARAM="-u"; shift;;
    -h) usage;;
    --) shift; break;;
  esac
done

if [ "x" == "x$CONFIG_FILE" ]; then
  usage ;
fi

MATXIN_PARAM="-f $CONFIG_FILE $*"

# Get absolute path for the input file
if [ "x" != "x$INPUT" ]; then
  INPUT_ABS=$(readlink -f $INPUT)
fi

# Get absolute path for the input file
if [ "x" != "x$OUTPUT" ]; then
  OUTPUT_ABS=$(readlink -f $OUTPUT)
fi

FORMAT_TMP=$POSTEDIT.format.xml
XML_TMP=$POSTEDIT.translation.xml
WIKI_TMP=$POSTEDIT.wiki.xml

cd $MATXIN_DIR/bin >& /dev/null

# Determine which formatter to use
case $FORMAT in
txt)
  DE_FORMATER="txt-deformat $FORMAT_TMP $INPUT_ABS"
  ;;
wiki)
  DE_FORMATER="wiki-deformat $FORMAT_TMP $INPUT_ABS"
  ;;
rtf)
  DE_FORMATER="rtf-deformat $FORMAT_TMP $INPUT_ABS"
  ;;
html)
  DE_FORMATER="html-deformat $FORMAT_TMP $INPUT_ABS"
  ;;
*)
  DE_FORMATER="txt-deformat $FORMAT_TMP $INPUT_ABS"
  ;;
esac

# The Matxin translation pipe
./$DE_FORMATER | ./Analyzer $MATXIN_PARAM | ./LT $MATXIN_PARAM | ./ST_intra $MATXIN_PARAM | ./ST_inter --inter 1 $MATXIN_PARAM  | ./ST_prep $MATXIN_PARAM  | ./ST_inter --inter 2 $MATXIN_PARAM  | ./ST_verb $MATXIN_PARAM | ./ST_inter --inter 3 $MATXIN_PARAM  | ./SG_inter $MATXIN_PARAM | ./SG_intra $MATXIN_PARAM | ./SG_move $MATXIN_PARAM | ./MG $MATXIN_PARAM > $XML_TMP

if [ "x" != "x$OUTPUT" ]; then
    ./reFormat $FORMAT_TMP $RE_FORMATER_PARAM < $XML_TMP > $OUTPUT_ABS
else
    ./reFormat $FORMAT_TMP $RE_FORMATER_PARAM < $XML_TMP
fi

if [ "x1" == "x$REMOVE_TMP" ]; then
    rm $FORMAT_TMP >& /dev/null
    rm $XML_TMP >& /dev/null
fi 
rm $WIKI_TMP >& /dev/null

