
binmode(STDOUT, ":utf8");

$bool=0;
$urratsa = 0;

$proba = ".haber.";
$proba = "_comer_[vmn000]&jan";
$proba = ".tener.[vmip00]+.que.[cs]+_comer_[vmn000]";
$proba = "#.haber.[vaip1s0]+.tener.[vmp000]+.que.[cs]+_comer_[vmn000]&[obj3p][dat00]&[DU]&jan\$";


while (<STDIN>) {
  chomp;
  my $lerroa = $_;                       #    print $lerroa."\n";

  if (  $lerroa =~ m/^define/ && $lerroa !~ m/\#\#\-\-/ ) {

    # Lerro anitzetan dauden define-ak lerro bakar batean jarri
    while ( $lerroa !~ s/;s*/;/ ) {
	$lerroa =~ s/\s*\#\.\.\.\s*//;
	$lerroa =~ s/\s*\#\#\|\s*//;
	$lerroa =~ s/\s*\#\!\!\#\s*//;
	$lerroaSegi = <STDIN>;
	chomp ($lerroaSegi);
	$lerroaSegi =~ s/^\s*//;
	$lerroa = $lerroa." ".$lerroaSegi 	if  ( $lerroaSegi !~ /^\!\#\#/ );
    }

    $lerroa =~ s/\s+/ /g;                                    #print $lerroa."\n";

# Jatorrizko fitxategian KORIREKI eta KORITXI ez dira Kark-en parte.
# Bestealde euskarazko lemetan zuriuneak onartu behar dira.
    $lerroa = "define Karak [ KORIREKI 0-9a-zA-ZñÑ \"_\" \"-\" KORITXI ] ;" if ( $lerroa =~ /define Karak / );

    $lerroa =~ /define (.*?)( \[.*?\])\s*\;/;
    $ler1 = $1;
    $ler2 = $2;

    $ler2 =~ s/\%0/0/g;                                           # %0 > 0
    $ler2 =~ s/\%(.)/\"$1\"/g;                                    # %[ > "["
    $ler2 =~ s/\./\\./g;                                          # .  > \.
    $ler2 =~ s/(\{|\})/ /g;	                                  # {  > \s         #  print $ler2."....................2\n";
    $ler2 =~ s/\"(\[|\]|\+|\_|\&|\/|\=|\>|\))\"/\\$1/g;        # "(" > \(        #    print $ler2."....................33\n";
    $ler2 =~ s/\"//g;                                              # " >             #    print $ler2."....................44\n";
    $ler2 =~ s/\[\[/\[ \[/g;                                       # [[ > [ [        #    print $ler2."....................45\n";
    $ler2 =~ s/\]\]/\] \]/g;                                       # ]] > ] ]        #    print $ler2."....................46\n";
    $ler2 =~ s/([^\\])\[/$1\(/g;                                   # [ > (           #        print $ler2."....................47\n\n";
    $ler2 =~ s/([^\\])\]/$1\)/g;                                   # ] > )           #        print $ler2."....................5\n\n";
    $ler2 =~ s/KORIREKI/\[/g;                                      # KORIREKI > [
    $ler2 =~ s/KORITXI/\]/g;                                       # KORITXI  > ]
    $ler2 =~ s/^ //;                                               #   >             #    print $ler2."\n"; 
    $ler2 =~ s/\?\*/.*?/g;                                         # ?* > .*?        #    print $ler2."\n"; 
    $ler2 =~ s/ _ / __ /g;                                         # ?* > .*?        #    print $ler2."\n"; 

    # Aurretik definitutako sinboloak ordezten dira definizio berrian
    if (defined @defineak) {
      foreach $define ( @defineak ) {
	$define =~ /define (.*?) (.*?)\;$/;
	$def1 = $1;
	$def2 = $2;			#       print "\n$ler2 // $def1 // $def2 \n"  ;
	$ler2 =~ s/$def1(\W)/$def2$1/g;	#	print "\n$ler2 // $def1 // $def2 \n";
      }
    }

    $ler2 =~ s/\s//g;
    $lerroa = "define $ler1 $ler2;"; #    print "\n".$lerroa."\n";


    if ($urratsa == 0) {
      # Definizioak gehitu
      unshift ( @defineak, $lerroa );
    }

    elsif ( $urratsa == 1 ) {
      # Sarrera-irteera patroiak gehitu
      $ler2  =~ /^\(\((\$)\-\>(.*?\$)\|\|(\#.*?)__\)\)$/;
      $atr = "\\".$1;
      $ezk = $3;
      $bal = $2;
      $esk = "";
      $ezk =~ s/\#/\\\#/ ;
      $bal =~ s/([^\\])\)/$1/g ;
      $bal =~ s/([^\\])\(/$1/g ;
      $bal =~ s/^\(//g ;
      $bal =~ s/\\//g ;
#      $bal =~ s/\$/\\\$/ ;
      $ezkAtrEskBal = "$ezk\t$atr\t$esk\t$bal";
      push ( @ordezkapenak, $ezkAtrEskBal );
      print STDOUT $ezkAtrEskBal."\n";
#      $sarIrt = "$sar \$  $irt";
#      push ( @sarIrt, $sarIrt );
#      print STDOUT $sarIrt."\n";
    }

    elsif ( $urratsa == 2 ) {
      if ( $bool == 0) {
#      print STDOUT "\n";

#      ### Proba adibidean aldaketak: irteera gehitu
#	foreach $sarIrt ( @sarIrt ) {
#	  $sarIrt =~ /^(.*?) (.*?)$/;
#	  $sar = $1;
#	  $irt = $2;		                               #  print $proba."\n";  print $sar."\n";  print $irt."\n\n";
#	  if ($proba =~ s/\#($sar)\$/\#$1$irt\$/ ) {
#	    print $proba."\n\n";                               #<STDIN>;
#	    last;
#	  }
	}
	$bool=1;
#      }

    }

    elsif ( $urratsa == 3 ) {
      $ler2 =~ s/^\((.*?)\)$/$1/ ;                              #print $ler2."\n";
      @zatiak = split( /\\.o\\./, $ler2 );
      foreach $zati ( @zatiak ) {                        	#print $zati."\n";
	$zati =~ s/^\((.*?)\)$/($1||.*?__.*?)/ if ( $zati !~ /\|\|/ );
	$zati  =~ /^\((.*?)-\>(.*?)\|\|(.*?)__(.*)\)$/;
	$ezk = $3;
	$esk = $4;
	$atr = $1;
	$bal = $2;		

	$ezk =~ s/\#/\\\#/ ;
	$atr =~ s/\$/\\\$/g;
	$atr =~ s/\#/\\\#/g;
	$bal =~ s/\(/\[/g;
	$bal =~ s/\)/\]/g;                                      #print ">>$ezk\n$atr\n$esk\n    $bal\n\n";

	$ezkAtrEskBal = "$ezk\t$atr\t$esk\t$bal";
	push ( @ordezkapenak, $ezkAtrEskBal );
	print STDOUT $ezkAtrEskBal."\n";

#	### Proba adibidean aldaketak: atributua dagokion balioarekin trukatu
#	if ( $proba =~ /^$ezk$atr$esk$/ ) {
#	  $proba =~ s/$atr/$bal/g ;                             #print "!!!!\n$zati\n\n";#print "$ezk\n$atr\n$esk\n    $bal\n\n";
#	  print "$proba\n\n";	                                #<STDIN>;
#	}
      }
    }

  } #end if define

  $urratsa++ if ($lerroa =~ /echo \>\> \+/ );
  last  if ( $urratsa == 4 );
}





#foreach ( $i=0;$i<10000;$i++){
foreach ( $i=0;$i<1;$i++){
# PROBA
$proba2 = "#.haber.[vaip1s0]+.tener.[vmp000]+.que.[cs]+_comer_[vmn000]&[obj3p][dat00]&[DU]&jan\$";

foreach $sarIrt ( @sarIrt ) {
  $sarIrt =~ /^(.*?) (.*?)$/;
  $sar = $1;
  $irt = $2;		                   	    #  print $proba."\n";  print $sar."\n";  print $irt."\n\n";
  if ($proba2 =~ s/\#($sar)\$/\#$1$irt\$/ ) {
    #print $proba2."\n\n";	                    #<STDIN>;
    last;
  }
}

foreach $ezkAtrEskBal  ( @ordezkapenak  ) {          #print  "$ezkAtrEskBal\n";
  $ezkAtrEskBal =~ /^(.*?)\t(.*?)\t(.*?)\t(.*?)$/;
  $ezk = $1;
  $atr = $2;			
  $esk = $3;
  $bal = $4;			
  if ( $proba2 =~ /^$ezk$atr$esk$/ ) {
    $proba2 =~ s/$atr/$bal/g ;
    #print "$proba2\n\n";
  }

}
#print "$proba2\n\n";

}




