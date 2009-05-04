
RE_FORMATER_PARAM=""

if [ "$#" = "0" ]; then
  echo "usage: $0 -f /path/to/config/file.cfg"
  exit -1
elif [ "$#" = "1" ]; then
  pushd $MATXIN_DIR >& /dev/null
  DE_FORMATER="txt-deformat"
  ABS_FILE=""
else
  i=0

  case $1 in
	txt)
    DE_FORMATER="txt-deformat"
    let i++
    ;;
	rtf)
    DE_FORMATER="rtf-deformat"
    let i++
    ;;
	html)
    DE_FORMATER="html-deformat"
    let i++
    ;;
	txtu)
    DE_FORMATER="txt-deformat"
    RE_FORMATER_PARAM="-u"
    let i++
    ;;
	rtfu)
    DE_FORMATER="rtf-deformat"
    RE_FORMATER_PARAM="-u"
    let i++
    ;;
	htmlu)
    DE_FORMATER="html-deformat"
    RE_FORMATER_PARAM="-u"
    let i++
    ;;
	*)
    DE_FORMATER="txt-deformat"
    ;;
  esac

  if  [ $i = "0" ] && [ -e $1 ]; then
    pushd $(dirname $1) >& /dev/null
    ABS_FILE=$PWD/$(basename $1)
    let i++
  fi

  if  [ $i = "1" ] && [ -e $2 ]; then
    pushd $(dirname $2) >& /dev/null
    ABS_FILE=$PWD/$(basename $2)
    let i++
  fi

  j=0
  dago="false";
  for param in $*; do
    if [ $param = "-h" ] || [ $param = "--help" ]; then
      cd $MATXIN_DIR/bin >& /dev/null
      ./Analyzer --help
      popd >& /dev/null
      exit
    fi

    if [ $j -ge $i ]; then
      MATXIN_PARAM="$MATXIN_PARAM $param";
    fi

    if [ $param = "-f" ]; then
      dago="true";
    fi
    let j++;
  done;

  if [ $dago = "false" ]; then
    echo "usage: $0 -f /path/to/config/file.cfg"
    exit -1
  fi

fi

cd $MATXIN_DIR/bin >& /dev/null

FORMAT_TMP=/tmp/matxin-format.$$.xml
XML_TMP=/tmp/matxin-translation.$$.xml


./$DE_FORMATER $FORMAT_TMP $ABS_FILE | iconv -f utf-8 -t iso-8859-1 - | ./Analyzer $MATXIN_PARAM | iconv -f iso-8859-1 -t utf-8 - | ./LT $MATXIN_PARAM | ./ST_intra $MATXIN_PARAM | ./ST_inter --inter 1 $MATXIN_PARAM  | ./ST_prep $MATXIN_PARAM  | ./ST_inter --inter 2 $MATXIN_PARAM  | ./ST_verb $MATXIN_PARAM | ./ST_inter --inter 3 $MATXIN_PARAM  | ./SG_inter $MATXIN_PARAM | ./SG_intra $MATXIN_PARAM | ./MG $MATXIN_PARAM > $XML_TMP
./reFormat $FORMAT_TMP $RE_FORMATER_PARAM  < $XML_TMP

rm $FORMAT_TMP >& /dev/null
rm $XML_TMP >& /dev/null

popd >& /dev/null

