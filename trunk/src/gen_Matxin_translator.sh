
RE_FORMATER_PARAM=""

usage()
{
  echo "Script options"
  echo "--------------"
  echo "USAGE: $(basename $0) -c /path/to/config/file.cfg [-f format] [-i /path/to/input/file] [-u] [module options]"
  echo " -f format        one of: txt (default), html, rtf"
  echo " -i input file    read input from file"
  echo " -u               display mark '*' for unknown words"
  echo ""
  echo ""
  echo "Module options"
  echo "--------------"
  cd $MATXIN_DIR/bin >& /dev/null
  ./Analyzer --help
  exit 1;
}


ARGS=$(getopt ":uhf:c:i:" -- $*)
for i
do
  case "$i" in
    -c) shift; CONFIG_FILE=$1; shift;;
    -f) shift; FORMAT=$1; shift;;
    -i) shift; FILENAME=$1; shift;;
    -u) RE_FORMATER_PARAM="-u"; shift;;
    -h) usage;;
    --) shift; break;;
  esac
done

if [ "x" == "x$CONFIG_FILE" ]; then
  usage ;
fi

MATXIN_PARAM="-f $CONFIG_FILE $*"

# Determine which formatter to use
case $FORMAT in
txt)
  DE_FORMATER="txt-deformat"
  ;;
wiki)
  DE_FORMATER="wiki-deformat"
  ;;
rtf)
  DE_FORMATER="rtf-deformat"
  ;;
html)
  DE_FORMATER="html-deformat"
  ;;
*)
  DE_FORMATER="txt-deformat"
  ;;
esac

# Get absolute path for the input file
if [ "x" != "x$FILENAME" ]; then
  ABS_FILE=$(readlink -f $(basename $FILENAME))
fi

cd $MATXIN_DIR/bin >& /dev/null

FORMAT_TMP=/tmp/matxin-format.$$.xml
XML_TMP=/tmp/matxin-translation.$$.xml

# The Matxin translation pipe
./$DE_FORMATER $FORMAT_TMP $ABS_FILE | iconv -f utf-8 -t iso-8859-1 - | ./Analyzer $MATXIN_PARAM | iconv -f iso-8859-1 -t utf-8 - | ./LT $MATXIN_PARAM | ./ST_intra $MATXIN_PARAM | ./ST_inter --inter 1 $MATXIN_PARAM  | ./ST_prep $MATXIN_PARAM  | ./ST_inter --inter 2 $MATXIN_PARAM  | ./ST_verb $MATXIN_PARAM | ./ST_inter --inter 3 $MATXIN_PARAM  | ./SG_inter $MATXIN_PARAM | ./SG_intra $MATXIN_PARAM | ./MG $MATXIN_PARAM > $XML_TMP
./reFormat $FORMAT_TMP $RE_FORMATER_PARAM < $XML_TMP

rm $FORMAT_TMP >& /dev/null
rm $XML_TMP >& /dev/null

