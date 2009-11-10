PAIR=""
INPUT_FILE="/dev/stdin"
OUTPUT_FILE="/dev/stdout"

message ()
{
  echo "USAGE: $(basename $0) [-d datadir] [-f format] <direction> [in [out]]"
  echo " -d datadir       directory of linguistic data"
  echo " -f format        one of: txt (default), html, rtf, odt, docx, wxml, xlsx, pptx,"
  echo "                  xpresstag";
  echo " -l               lists the available translation directions and exits"
  echo " direction        typically, LANG1-LANG2, but see modes.xml in language data"
  echo " in               input file (stdin by default)"
  echo " out              output file (stdout by default)"
  exit 1;
}

list_directions ()
{
         for i in $DATOS/modes/*.mode;
         do echo "  " $(basename $i) |awk '{gsub(".mode", ""); print;}'
         done;
}

locale_utf8 ()
{
  export LC_CTYPE=$(locale -a|grep -i "utf[.]*8"|head -1);
  if [[ LC_CTYPE = "" ]]
  then echo "Error: Install an UTF-8 locale in your system";
       exit 1;
  fi
}

ARGS=$(getopt "uahlf:d:m:o:" $*)
set -- $ARGS
for i
do
  case "$i" in 
    -f) shift; FORMAT=$1; shift;;
    -d) shift; DIRECTORY=$1; shift;;
    -u) UWORDS="no"; shift;;
    -a) OPTION_TAGGER="-m"; shift;;
    -l) DATOS=$DEFAULT_DIRECTORY; list_directions; exit 0;;
    -h) message;;
    --) shift; break;;
  esac
done

case "$#" in 
     3)
       OUTPUT_FILE=$3; 
       INPUT_FILE=$2;
       PAIR=$1;
       if [ ! -e $INPUT_FILE ];
       then echo "Error: file '$INPUT_FILE' not found."
            message;
       fi
       ;;
     2)
       INPUT_FILE=$2;
       PAIR=$1;
       if [ ! -e $INPUT_FILE ];
       then echo "Error: file '$INPUT_FILE' not found."
            message;
       fi
       ;;
     1)
       PAIR=$1
       ;;
     *)
       message 
       ;;
esac    

if [ x$FORMAT = x ]; then FORMAT="txt"; fi
if [ x$DIRECTORY = x ]; then DIRECTORY=$DEFAULT_DIRECTORY; fi

PREFIJO=$PAIR;
FORMATADOR=$FORMAT;
DATOS=$DIRECTORY;
FICHERO=$INPUT_FILE;
SALIDA=$OUTPUT_FILE;

if [ ! -d $DATOS/modes ];
then echo "Error: Directory '$DATOS/modes' does not exist."
     message
fi

if [ ! -e $DATOS/modes/$PREFIJO.mode ];
  then echo -n "Error: Mode $PREFIJO does not exist";
       c=$(find $DATOS/modes|wc -l)
       if((c <= 1));
       then echo ".";
       else echo ". Try one of:";
         list_directions;
#         for i in $DATOS/modes/*.mode;
#         do echo "  " $(basename $i) |awk '{gsub(".mode", ""); print;}'
#         done;
       fi
       exit 1;
fi

#Parametro opcional, de no estar, lee de la entrada estandar (stdin)

case "$FORMATADOR" in 
        none)
        	if [ "$UWORDS" = "no" ]
        	then OPTION="-n";
        	else OPTION="-g";
        	fi
	        ;;
	txt|rtf|html)
		if [[ $UWORDS = "no" ]]; then OPTION="-n"; 
		else OPTION="-g";
		fi;
		;;

	*) # Por defecto asumimos txt
		FORMATADOR="txt"
		OPTION="-g"
		;;	
esac

if [ -z $REF ]
then 
        REF=$FORMATADOR
fi

if [ "$FORMATADOR" = "none" ]; then 
	cat $FICHERO;
else 
	$MATXIN_PATH/matxin-des$FORMATADOR $FICHERO; fi| \
if [ "$TRANSLATION_MEMORY_FILE" = "" ]; 
then cat;  
else $MATXIN_PATH/lt-tmxproc $TMCOMPFILE;
fi | if [ ! -x $DATOS/modes/$PREFIJO.mode ]
then sh $DATOS/modes/$PREFIJO.mode $OPTION $OPTION_TAGGER
else $DATOS/modes/$PREFIJO.mode $OPTION $OPTION_TAGGER
fi | if [ "$FORMATADOR" = "none" ]
then cat >$SALIDA;
else $MATXIN_PATH/matxin-re$FORMATADOR >$SALIDA;
fi

rm -Rf $TMCOMPFILE

