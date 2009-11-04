h! ES->EU Aditz Kateen Transferentziarako FST definizioa


echo >> Definizioak ; 

!Karak barruan daudenak, espresio erregular bihurtzerakoan (konpilatzekoan) tratatzen dira, eta hemengoei ez zaie kasurik egiten.

!define Alfabeto      [  a | b | c | d | e | f | g | h | i | j | k | l | m | n |
!                        o | p | q | r | s | t | u | v | w | x | y | z ] ;
!define AlfabetoLarri [  A | B | C | D | E | F | G | H | I | J | K | L | M | N |
!                        O | P | Q | R | S | T | U | V | W | X | Y | Z ] ;
!define Zifrak        [ %0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 ] ;
define Karak         [ Alfabeto | AlfabetoLarri | Zifrak | "_" | "-" ] ;

define KarakPlus     [ Karak | %[ | %] | %+ ] ;

define IM	     [ "[" Karak+ "]" ] ;

define XXX	     [ "=" "=" ">" ];


!define IVM	     [ {VM}|{VS}|{VA} ] ;
define IVM	     [ {VM}|{VS} ] ;
define IVA	     [ {VA} ] ;

define IMInf	     [ "[" [IVM|IVA]{N} Karak* "]" ] ;
define IMPar	     [ "[" [IVM|IVA]{P} Karak* "]" ] ;
define IMGer	     [ "[" [IVM|IVA]{G} Karak* "]" ] ;
define IMAdjPar	     [ "[" {AQ} Karak* {P} "]" ] ;

!define IMInfGer     [ IMInf | IMGer ] ;
!define IMEzJok	     [ IMInf | IMPar | IMGer ] ;

define IMInfGer	     [ "[" IVM[N|G]   Karak* "]"  ] ;
define IMEzJok	     [ "[" IVM[N|G|P] Karak* "]"  ] ;


<<<<<<< def_aditz_transf.aer
define VMIP	     [ "[" VM{ip} Karak* "]" ];
define VMII	     [ "[" VM{ii} Karak* "]" ];
define VMIS	     [ "[" VM{is} Karak* "]" ];
define VMIF	     [ "[" VM{if} Karak* "]" ];
define VMIC	     [ "[" VM{ic} Karak* "]" ];
define VMSP	     [ "[" VM{sp} Karak* "]" ];
define VMSI	     [ "[" VM{si} Karak* "]" ];
define VMM	     [ "[" VM{m} Karak* "]" ];

define VAIP	     [ "[" VA{ip} Karak* "]" ];
define VAII	     [ "[" VA{ii} Karak* "]" ];
define VAIS	     [ "[" VA{is} Karak* "]" ];
define VAIF	     [ "[" VA{if} Karak* "]" ];
define VAIC	     [ "[" VA{ic} Karak* "]" ];
define VASP	     [ "[" VA{sp} Karak* "]" ];
define VASI	     [ "[" VA{si} Karak* "]" ];
=======
define VMIP	     [ "[" IVM{IP} Karak* "]" ];
define VMII	     [ "[" IVM{II} Karak* "]" ];
define VMIS	     [ "[" IVM{IS} Karak* "]" ];
define VMIF	     [ "[" IVM{IF} Karak* "]" ];
define VMIC	     [ "[" IVM{IC} Karak* "]" ];
define VMSP	     [ "[" IVM{SP} Karak* "]" ];
define VMSI	     [ "[" IVM{SI} Karak* "]" ];
define VMM	     [ "[" IVM{M} Karak* "]" ];

define VAIP	     [ "[" IVA{IP} Karak* "]" ];
define VAII	     [ "[" IVA{II} Karak* "]" ];
define VAIS	     [ "[" IVA{IS} Karak* "]" ];
define VAIF	     [ "[" IVA{IF} Karak* "]" ];
define VAIC	     [ "[" IVA{IC} Karak* "]" ];
define VASP	     [ "[" IVA{SP} Karak* "]" ];
define VASI	     [ "[" IVA{SI} Karak* "]" ];
>>>>>>> 1.22


!define IMJok	     [ VMIP | VMII | VMIS | VMIF | VMIC | 
!		       VAIP | VAII | VAIS | VAIF | VAIC  ];
!define IMPI	     [ VMIP | VMII | 
!		       VAIP | VAII  ];


define IMJok	     [ "[" [IVM|IVA][{IP}|{II}|{IS}|{IF}|{IC}|{SP}|{SI}|{M}] Karak* "]" ];
!define IMJok	     [ "[" IVM[{IP}|{II}|{IS}|{IF}|{IC}|{SP}|{SI}] Karak* "]" ];

!define IMPI	     [ "[" [IVM|IVA][{IP}|{II}|{SP}|{SI}] Karak* "]" ];


define AdNES	     [ "_" Karak+  "_" ] ;
define HaberNag	     [ "_" {haber} "_" ] ;
define HaberLag	     [ {.haber.} ] ;
define SerPas	     [ {.ser.} ] ;
define AdNEU	     [ Karak+  ] ;

echo >> Perifrastikoen definizioak;

define AdP1IJok  [   [ {.poder.} | {.deber.} | {.querer.}]			  IMJok
		       | [ {.tener.} | {.haber.} ]				  IMJok %+ {.que.}			IM
		       | [ {.haber.} |{.terminar.} | {.dejar.} ]		  IMJok %+ {.de.}			IM
		       | {.estar.}						  IMJok %+ {.a}"_"{punto}"_"{de.}	IM
		       | [ {.empezar.}|{.comenzar.}|{.echar.}|{.romper.}|{.ir.}]  IMJok %+ {.a.}			IM
		     ];
define AdP1GJok    [   [ {.acabar.} | {.andar.} | {.comenzar.} | {.empezar.}
			 | {.continuar.} | {.continuar.} | {.estar.} | {.ir.}  
			 | {.llevar.} | {.seguir.} | {.terminar.} | {.venir.}]    IMJok
		     ];
define AdP1Jok     [ AdP1IJok | AdP1GJok ];

define AdP2Jok     [   {.soler.}						  IMJok
		       | [ {.deber.} | {.acabar.} ]				  IMJok %+ {.de.}         IM
		       | {.acostumbrar.}					  IMJok %+ {.a.}          IM
		     ];
define AdP3Jok     [   {.volver.}						  IMJok %+ {.a.}          IM
		     ];
define AdP4Jok     [   [ {.llegar.} | {.acertar.} | {.alcanzar.} ]		  IMJok %+ {.a.}          IM
		     ];

define AdP1IPar  [   [ {.poder.} | {.deber.} | {.querer.}]			  IMPar
		       | [ {.tener.}| {.haber.} ]				  IMPar %+ {.que.}			IM
		       | [ {.haber.} |{.terminar.} | {.dejar.} ]		  IMPar %+ {.de.}			IM
		       | {.estar.}						  IMPar %+ {.a}"_"{punto}"_"{de.}	IM
		       | [ {.empezar.}|{.comenzar.}|{.echar.}|{.romper.}|{.ir.}]  IMPar %+ {.a.}			IM
		     ];
define AdP1GPar    [   [ {.acabar.} | {.andar.} | {.comenzar.} | {.empezar.}
			 | {.continuar.} | {.estar.} | {.ir.}  
			 | {.llevar.} | {.seguir.} | {.terminar.} | {.venir.}]    IMPar
		     ];

define AdP1Par     [ AdP1IPar | AdP1GPar ];

define AdP2Par     [   {.soler.}						  IMPar
		       | [ {.deber.} | {.acabar.} ]				  IMPar %+ {.de.}         IM
		       | {.acostumbrar.}					  IMPar %+ {.a.}          IM
		     ];
define AdP3Par     [   {.volver.}						  IMPar %+ {.a.}          IM
		     ];
define AdP4Par     [   [ {.llegar.} | {.acertar.} | {.alcanzar.} ]		  IMPar %+ {.a.}          IM
		     ];
define PerifEdun [  "nahi"|"behar"|"jarraitu"|"eraman"|"amaitu" ] ;
define PerifIzan [  "hasi"|"ari"|"aritu"|"egon"|"joan"|"etorri" ] ;


define Trans         [ "[" [ {DA} | {DU} | {ZAIO} | {DIO}] "]" ];
define Perts	     [ %0 | 1 | 2 | 3 ];
define Num	     [ %0 | s | p ];
define Sub	     [ "[" {sub} Perts Num "]" ];
define Obj	     [ "[" {obj} Perts Num "]" ];
define Dat	     [ "[" {dat} Perts Num "]" ];

define AdTrTran	     [ {edun} | {ukan} | {eduki} | {eraman} | {ekarri} | {jakin} ] ;
define AdTrIntr	     [ {izan} | {egon}  | {etorri}  | {joan}            ] ;
define AdTrinkoa     [ AdTrTran | AdTrIntr ];


define PaSe	     [ [ {.se.} "[" {P0000000} "]" ] | [ {.él.} "[" {P0300000} "]" ] | [ {.se.} "[" {PP3CN000} "]" ];
define PaMe	     [ [ {.yo.} "[" {PP1CS000} "]" ] | [ {.yo.} "[" {P010S000} "]" ] ];
define PaTe	     [ [ {.tú.} "[" {PP2CS000} "]" ] | [ {.tú.} "[" {P020S000} "]" ] ];
define PaLo	     [ {.él.} "[" {PP3MSA00} "]" ];
define PaLa	     [ {.él.} "[" {PP3FSA00} "]" ];
define PaLe	     [ {.él.} "[" {PP3CSD00} "]" ];
define PaOs	     [ [ {.tú.} "[" {PP2CP000} "]" ] | [ {.tú.} "[" {P020P000} "]" ] ];
define PaNos	     [ [ {.yo.} "[" {PP1CP000} "]" ] | [ {.yo.} "[" {P010P000} "]" ] ];
define PaLos	     [ {.él.} "[" {PP3MPA00} "]" ];
define PaLas	     [ {.él.} "[" {PP3FPA00} "]" ];
define PaLes	     [ {.él.} "[" {PP3CPD00} "]" ];
define PaSePost	     [ {.él.} "[" {PP3CN000} "]" ];
define PaLoPost	     [ {.él.} "[" {PP3CNA00} "]" ];

define Pr	     [ PaSe | PaSePost | PaMe | PaTe | PaLo | PaLoPost | PaLa | PaLe | PaNos | PaOs | PaLos | PaLas | PaLes ];


!define Erl	     [ "[" [ {KAUS} | {KONP} | {ZHG} | {ERLT} | {MEN} ] "]" ];


! Beharrezkoa da loakiluaren gaineko aditzik balego, ber informazio morfologikoa, eta aditz mota (edo ze aditza den), subjuntibozoak tratatu ahal izateko: dudo que haya venido / etorri izana dudan jartzen dut
!define PAN	     [ "<" [ {vmip} | {vmpp} ] ">" ];
!define PANVMIP

!Gorkak aldatua 
!define AdKEJ       [							   AdNES IMEzJok		 		      "&" AdNEU ] ;

define AdKEJ          [							          AdNES IMEzJok       "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKArrSin      [ [Pr %+]*						  AdNES IMJok [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKArrLag      [ [Pr %+]* HaberLag IMJok	[%+Pr]*			       %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer1ISin    [ [Pr %+]*                           AdP1IJok [%+Pr]*    %+ AdNES IMInf [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer1ILag    [ [Pr %+]* HaberLag IMJok [%+Pr]* %+ AdP1IPar	       %+ AdNES IMInf [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer1GSin    [ [Pr %+]*			   AdP1GJok [%+Pr]*    %+ AdNES IMGer [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer1GLag    [ [Pr %+]* HaberLag IMJok [%+Pr]* %+ AdP1GPar	       %+ AdNES IMGer [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer2Sin     [ [Pr %+]*			   AdP2Jok  [%+Pr]*    %+ AdNES IMInf [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer2Lag     [ [Pr %+]* HaberLag IMJok [%+Pr]* %+ AdP2Par	       %+ AdNES IMInf [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer3Sin     [ [Pr %+]*			   AdP3Jok  [%+Pr]*    %+ AdNES IMInf [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer3Lag     [ [Pr %+]* HaberLag IMJok [%+Pr]* %+ AdP3Par	       %+ AdNES IMInf [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer4Sin     [ [Pr %+]*			   AdP4Jok  [%+Pr]*    %+ AdNES IMInf [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer4Lag     [ [Pr %+]* HaberLag IMJok [%+Pr]* %+ AdP4Par	       %+ AdNES IMInf [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;

define AdKEJPas	      [			                      SerPas IMEzJok %+ AdNES IMPar         "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKArrSinPas   [ [Pr%+]*		                      SerPas IMJok   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKArrLagPas   [ [Pr%+]* HaberLag IMJok		   %+ SerPas IMPar   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer1ISinPas [ [Pr%+]*                   AdP1IJok %+ SerPas IMInf   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer1ILagPas [ [Pr%+]* HaberLag IMJok %+ AdP1IPar %+ SerPas IMInf   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer1GSinPas [ [Pr%+]*                   AdP1GJok %+ SerPas IMGer   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer1GLagPas [ [Pr%+]* HaberLag IMJok %+ AdP1GPar %+ SerPas IMGer   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer2SinPas  [ [Pr%+]*                   AdP2Jok  %+ SerPas IMInf   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer2LagPas  [ [Pr%+]* HaberLag IMJok %+ AdP2Par  %+ SerPas IMInf   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer3SinPas  [ [Pr%+]*			  AdP3Jok  %+ SerPas IMInf   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer3LagPas  [ [Pr%+]* HaberLag IMJok %+ AdP3Par  %+ SerPas IMInf   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer4SinPas  [ [Pr%+]*			  AdP4Jok  %+ SerPas IMInf   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;
define AdKPer4LagPas  [ [Pr%+]* HaberLag IMJok %+ AdP4Par  %+ SerPas IMInf   %+ AdNES IMPar [%+Pr]* "&" Sub Obj Dat "&" Trans "&" AdNEU ] ;


define AdKPer1Sin     [ AdKPer1ISin | AdKPer1GSin ] ;
define AdKPer1Lag     [ AdKPer1ILag | AdKPer1GLag ] ;

define AdKPer1SinPas  [ AdKPer1ISinPas | AdKPer1GSinPas ] ;
define AdKPer1LagPas  [ AdKPer1ILagPas | AdKPer1GLagPas ] ;


echo >> Sarrera ;

define AdKArr     [ AdKArrSin  | AdKArrLag  ] ;
define AdKPer1    [ AdKPer1Sin | AdKPer1Lag ] ;
define AdKPer2    [ AdKPer2Sin | AdKPer2Lag ] ;
define AdKPer3    [ AdKPer3Sin | AdKPer3Lag ] ;
define AdKPer4    [ AdKPer4Sin | AdKPer4Lag ] ;

!define AdKArr     [ AdKArrSin  | AdKArrLag  | AdKArrSinPas  | AdKArrLagPas  ] ;
!define AdKPer1    [ AdKPer1Sin | AdKPer1Lag | AdKPer1SinPas | AdKPer1LagPas ] ;
!define AdKPer2    [ AdKPer2Sin | AdKPer2Lag | AdKPer2SinPas | AdKPer2LagPas ] ;
!define AdKPer3    [ AdKPer3Sin | AdKPer3Lag | AdKPer3SinPas | AdKPer3LagPas ] ;
!define AdKPer4    [ AdKPer4Sin | AdKPer4Lag | AdKPer4SinPas | AdKPer4LagPas ] ;

define AdKArrPas     [ AdKArrSinPas  | AdKArrLagPas   ] ;
define AdKPer1Pas    [ AdKPer1SinPas | AdKPer1LagPas  ] ;
define AdKPer2Pas    [ AdKPer2SinPas | AdKPer2LagPas  ] ;
define AdKPer3Pas    [ AdKPer3SinPas | AdKPer3LagPas  ] ;
define AdKPer4Pas    [ AdKPer4SinPas | AdKPer4LagPas  ] ;

define AdKPas	     [ AdKEJPas | AdKArrPas | AdKPer1Pas | AdKPer2Pas | AdKPer3Pas | AdKPer4Pas ];

echo >> Irteera ; 

define IrtEJ [ XXX {EJ}")" "<NAG>" {AspN}									 {Erl} ] ;
define IrtAR [ XXX {AR}")" "<NAG>" {AspN}			 "/" {AdM} {AspM} "/"{AdL} {DenMod} {Nr}{Ni}{Nk} {Erl} ] ;
define IrtP1 [ XXX {P1}")" "<NAG>" {AspN} "/" {Per}"<PER>"{AspP} "/" {AdM} {AspM} "/"{AdL} {DenMod} {Nr}{Ni}{Nk} {Erl} ] ;
define IrtP2 [ XXX {P2}")" "<NAG>" {AspN} "/" {Prt}              "/" {AdM} {AspM} "/"{AdL} {DenMod} {Nr}{Ni}{Nk} {Erl} ] ;
define IrtP3 [ XXX {P3}")" "<NAG>" {AspN} "/" {Adb}              "/" {AdM} {AspM} "/"{AdL} {DenMod} {Nr}{Ni}{Nk} {Erl} ] ;

define IrtEJPas [ XXX {EJPAS}")" "<NAG>" {AspN}									       {Erl} ] ;
define IrtARPas [ XXX {ARPAS}")" "<NAG>" {AspN}			       "/" {AdM} {AspM} "/"{AdL} {DenMod} {Nr}{Ni}{Nk} {Erl} ] ;
define IrtP1Pas [ XXX {P1PAS}")" "<NAG>" {AspN} "/" {Per}"<PER>"{AspP} "/" {AdM} {AspM} "/"{AdL} {DenMod} {Nr}{Ni}{Nk} {Erl} ] ;
define IrtP2Pas [ XXX {P2PAS}")" "<NAG>" {AspN} "/" {Prt}              "/" {AdM} {AspM} "/"{AdL} {DenMod} {Nr}{Ni}{Nk} {Erl} ] ;
define IrtP3Pas [ XXX {P3PAS}")" "<NAG>" {AspN} "/" {Adb}              "/" {AdM} {AspM} "/"{AdL} {DenMod} {Nr}{Ni}{Nk} {Erl} ] ;


echo >> + Sarrera->irteera ;
 
define TransPatrEJ    [ 
			[ "$" -> IrtEJ "$" || "#" AdKEJ   _ ] 
]  ;
define TransPatrArr   [ 
			[ "$" -> IrtAR "$" || "#" AdKArr  _ ] 
]  ;
define TransPatrPer1  [ 	
			[ "$" -> IrtP1 "$" || "#" AdKPer1 _ ] 
]  ;
define TransPatrPer2  [  
			[ "$" -> IrtP2 "$" || "#" AdKPer2 _ ] 
]  ;
define TransPatrPer3  [  
			[ "$" -> IrtP3 "$" || "#" AdKPer3 _ ] 
]  ;
define TransPatrPer4  [  
			[ "$" -> IrtAR "$" || "#" AdKPer4 _ ] 
]  ;


define TransPatrEJPas    [ 
			[ "$" -> IrtEJPas "$" || "#" AdKEJPas   _ ] 
]  ;
define TransPatrArrPas   [ 
			[ "$" -> IrtARPas "$" || "#" AdKArrPas  _ ] 
]  ;
define TransPatrPer1Pas  [ 	
			[ "$" -> IrtP1Pas "$" || "#" AdKPer1Pas _ ] 
]  ;
define TransPatrPer2Pas  [  
			[ "$" -> IrtP2Pas "$" || "#" AdKPer2Pas _ ] 
]  ;
define TransPatrPer3Pas  [  
			[ "$" -> IrtP3Pas "$" || "#" AdKPer3Pas _ ] 
]  ;
define TransPatrPer4Pas  [  
			[ "$" -> IrtARPas "$" || "#" AdKPer4Pas _ ] 
]  ;



echo >> + TransPatr ;

define TransPatr [ 
       TransPatrEJ 
       .o.##|
       TransPatrArr 
       .o.##|
       TransPatrPer1 
       .o.##|
       TransPatrPer2 
       .o.##|
       TransPatrPer3
       .o.##|
       TransPatrPer4
       .o.##|
       TransPatrEJPas
       .o.##|
       TransPatrArrPas
       .o.##|
       TransPatrPer1Pas
       .o.##|
       TransPatrPer2Pas
       .o.##|
       TransPatrPer3Pas
       .o.##|
       TransPatrPer4Pas 
];


echo >> + Ordezkapenak ;


! "hay coches/hay dinero" bezalakoetan analizatzailetik datozen erroreak zuzentzeko
define egonVA [
       [  {VA} -> "VM"    ||   ?* "_egon_" "[" _ ?* ]
        .o.##|
       [ {VA} -> "VM"     ||  ?* {.haber.} "[" _ Karak*  "]" %+ {.que.} ?* ]


];

define egonPertsNum [
       [  {3S} -> "3P"    ||   ?* "_egon_" ?* _ ?* "obj3p" ]
];


!define izanTrans [
!       [  {DU} -> "DA"       ||   ?* "_izan_" ?* _ ?* ]
!       .o.##|
!       [  {DIO} -> "ZAIO"    ||   ?* "_izan_" ?* _ ?* ]
!       .o.##|
!       [  {DA} -> "DU"       ||   ?* "_edun_" ?* _ ?* ]
!       .o.##|
!       [  {ZAIO} -> "DIO"    ||   ?* "_edun_" ?* _ ?* ]
!];


define edunUkan [
       [  {_edun} -> "_ukan"    ||  ?* _ ?* ]
       .o.##|
       [  {_edun} -> "_ukan"    ||  ?* _ ?* ]
];

;

define LoLa [
       [  {obj00} -> "obj3s"    || ?* [ PaLo  | PaLa  | PaLoPost  ] ?*				 _ ?*  ]
       .o.##|
       [  {obj00} -> "obj3p"    || ?* [ PaLos | PaLas ] ?*					 _ ?*  ]
       .o.##|
       [  {dat00} -> "dat3s"    || ?* [ PaLe  ] ?*						 _ ?*  ]
       .o.##|
       [  {dat00} -> "dat3p"    || ?* [ PaLes ] ?*						 _ ?*  ]
];

define meTeSeDuDa [
       [  {DU} -> "DA"    || ?* [ PaMe   ?* "1S" | PaTe  ?* "2S" | PaSe  ?* "3S"  | #...
				  PaNos  ?* "1P" | PaOs  ?* "2P" | PaSe  ?* "3P"  ] ?*	"obj00" ?* "dat00" ?*	_ ?*  ]
       .o.##|
       [  {DU} -> "DA"    || ?* PaSe ?* [ PaMe | PaTe | PaNos | PaOs ]  ?* ["3S"|"3P"] ?*	_ ?*  ]
];
	     
define meTeSe [
       [  {obj00} -> "obj1s"    || ?* ["2"|"3"]["S"|"P"] ?* [ PaMe ]  ?*                        _ ?* ["DU"|"DIO"] ?*  ]
       .o.##|
       [  {dat00} -> "dat1s"    || ?* ["2"|"3"]["S"|"P"] ?* [ PaMe ]  ?* "obj" ["0"|"2"|"3"] ?*  _ ?*  ]
       .o.##|
       [  {obj00} -> "obj2s"    || ?* ["1"|"3"]["S"|"P"] ?* [ PaTe ]  ?* 			 _ ?*  ]
       .o.##|
       [  {dat00} -> "dat2s"    || ?* ["1"|"3"]["S"|"P"] ?* [ PaTe ]  ?* "obj" ["0"|"1"|"3"] ?*  _ ?*  ]
       .o.##|
       [  {obj00} -> "obj1p"    || ?* ["2"|"3"]["S"|"P"] ?* [ PaNos ] ?* 			 _ ?*  ]
       .o.##|
       [  {dat00} -> "dat1p"    || ?* ["2"|"3"]["S"|"P"] ?* [ PaNos ] ?* "obj" ["0"|"2"|"3"] ?*  _ ?*  ]
       .o.##|
       [  {obj00} -> "obj2p"    || ?* ["1"|"3"]["S"|"P"] ?* [ PaOs ]  ?* 			 _ ?*  ]
       .o.##|
       [  {dat00} -> "dat2p"    || ?* ["1"|"3"]["S"|"P"] ?* [ PaOs ]  ?* "obj" ["0"|"1"|"3"] ?*  _ ?*  ]
       .o.##|
       [  {dat00} -> "dat3s"    || ?* [ PaSe | PaSePost ]  ?* [PaLo|PaLos|PaLa|PaLas|PaLoPost] ?* _ ?*  ]
];

define meTeSe2 [
       [  {obj00} -> "obj1s"    || ?* [ PaMe ]  ?* ["2"|"3"]["S"|"P"] ?*                        _ ?* ["DU"|"DIO"] ?*  ]
       .o.##|
       [  {dat00} -> "dat1s"    || ?* [ PaMe ]  ?* ["2"|"3"]["S"|"P"] ?*  "obj" ["0"|"2"|"3"] ?*  _ ?*  ]
       .o.##|
       [  {obj00} -> "obj2s"    || ?* [ PaTe ]  ?* ["1"|"3"]["S"|"P"] ?*  			 _ ?*  ]
       .o.##|
       [  {dat00} -> "dat2s"    || ?* [ PaTe ]  ?* ["1"|"3"]["S"|"P"] ?*  "obj" ["0"|"1"|"3"] ?*  _ ?*  ]
       .o.##|
       [  {obj00} -> "obj1p"    || ?* [ PaNos ] ?* ["2"|"3"]["S"|"P"] ?*  			 _ ?*  ]
       .o.##|
       [  {dat00} -> "dat1p"    || ?* [ PaNos ] ?* ["2"|"3"]["S"|"P"] ?*  "obj" ["0"|"2"|"3"] ?*  _ ?*  ]
       .o.##|
       [  {obj00} -> "obj2p"    || ?* [ PaOs ]  ?* ["1"|"3"]["S"|"P"] ?* 			 _ ?*  ]
       .o.##|
       [  {dat00} -> "dat2p"    || ?* [ PaOs ]  ?* ["1"|"3"]["S"|"P"] ?*  ?* "obj" ["0"|"1"|"3"] ?*  _ ?*  ]
];


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Trinkoetan: "/"{AdM}{AspM}"/"{AdL}  bihurtu [ADT]

define TransTrinkArr [ 
       [  {AspN} ?* "/" {AdL}  -> "[ADT]"	||  ?* [VMIP | VMII] ?* AdTrinkoa ?* XXX {AR} ?*  _   		?*      ]
!##       |
!##       [				       ~ [  ?* [VMIP | VMII] ?* AdTrinkoa ?* XXX {AR} ?* {AspN} ?* "/" {AdL}  ?*   ]  ]
];



!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Aditz nagusiaren aspektua (AspN)

define TransAspNEJ [ 
       [ {AspN} -> "ADIZE"	||  ?* IMInf		?*  XXX {EJ} ?* _ ?* ] 
       .o.##|
       [ {AspN} -> "INE"	||  ?* IMGer		?*  XXX {EJ} ?* _ ?* ] 
       .o.##|
       [ {AspN} -> "MOD"	||  ?* IMPar		?*  XXX {EJ} ?* _ ?* ] 
       .o.##|
       [ {AspN} -> "BURU"	||  ?* IMAdjPar		?*  XXX {EJ} ?* _ ?* ] 
] ;

define TransAspNArrPer3 [ 
       [ {AspN} -> "EZBU" ||  ?* [ VMIP | VMII ]				?*  XXX [{AR}|{P3}] ?* _ ?*  ] 
       .o.##|
       [ {AspN} -> "BURU" ||  ?* [ VMIS | VAIP | VAII | VAIS | VAIF | VASP ]	?*  XXX [{AR}|{P3}] ?* _ ?*  ] 
       .o.##|
       [ {AspN} -> "GERO" ||  ?* [ VMIF | VMIC | VAIC | VASI ]			?*  XXX [{AR}|{P3}] ?* _ ?*  ] 
       .o.##|
       [ {AspN} -> "ADOIN" ||  ?* [ VMSP | VMSI | VMM ]				?*  XXX [{AR}|{P3}] ?* _ ?*  ] 
] ;

define TransAspNPer1 [  
       [ {AspN} -> "EZBU" ||  ?* [ [{.empezar.}|{.comenzar.}|{.echar.}|{.romper.}] IM %+ {.a.}			      #...
				    | {.terminar.}				   IM %+ {.de.} ] ?* XXX {P1} ?*  _ ?*  ] 
       .o.##|
       [ {AspN} -> "BURU" ||  ?* [[{.tener.}|{.haber.}] IM %+ {.que.}
				    |{.haber.} IM %+ {.de.}|{.poder.}|{.deber.}]		 ?* XXX {P1} ?*  _ ?*  ] 
       .o.##|
       [ {AspN} -> "ALA"  ||  ?* {.ir.}	IM %+ {.a.}						 ?* XXX {P1} ?*  _ ?*  ] 
       .o.##|
       [ {AspN} -> "HUR"  ||  ?* {.estar.} IM %+ {.a}"_"{punto}"_"{de.}				 ?* XXX {P1} ?*  _ ?*  ] 
       .o.##|
       [ {AspN} -> "DAT"  ||  ?* {.dejar.} IM %+ {.de.}						 ?* XXX {P1} ?*  _ ?*  ] 
       .o.##|
       [ {AspN} -> "INE"  ||  ?* IMGer								 ?* XXX {P1} ?*  _ ?*  ] 
];

define TransAspNPer2 [  
       [ {AspN} -> "BURU" ||  ?* {.deber.} IM %+ {.de.}  ?* [ VMIP | VMII ]			?* XXX {P2} ?*  _ ?*  ] 
       .o.##|
       [ {AspN} -> "PART" ||									?* XXX {P2} ?*  _ ?*  ] 
];


! TransAspNArr-ren berdina!!!



!##define TransAspTrinkoak [ 
!##       [ ~[ ?* {AspN} ?* ] ]
!##];

!##define TransAspN [ TransAspNEJ | TransAspNArrPer3 | TransAspNPer1 | TransAspNPer2 | TransAspTrinkoak ];
		   



!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Aditz Kate perifrastikoetan ordezkapenak ;


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Aditz perifrastikoetan (P1) perifrastikoen itzulpena; 

define TransPer [
	[ {Per} -> "ahal" "_izan"	||   ?* {.poder.}							?*   _ ?* ]
	.o.##|
	[ {Per} -> "amaitu"		||   ?* [ {.acabar.} | {.terminar.} ]					?*   _ ?* ]
	.o.##|
	[ {Per} -> "egon"		||   ?* {.estar.}IM%+{.a}"_"{punto}"_"{de.}				?*   _ ?* ]
	.o.##|	    
	[ {Per} -> "ari" "_izan"	||   ?* [ {.andar.}  | {.estar.} ]					?*   _ ?* ]
	.o.##|
	[ {Per} -> "behar" "_izan"	||   ?* [ {.deber.}|[{.tener.}|{.haber.}] IM %+ {.que.}|{.haber.} IM %+ {.de.}]	?*   _ ?* ]
	.o.##|
	[ {Per} -> "eraman"		||   ?* [ {.llevar.} ]							?*   _ ?* ]
	.o.##|
	[ {Per} -> "etorri"		||   ?* [ {.venir.} ]							?*   _ ?* ]
	.o.##|
	[ {Per} -> "hasi"		||   ?* [ {.empezar.}|{.comenzar.}|{.echar.}|{.romper.} ]		?*   _ ?* ]
	.o.##|
	[ {Per} -> "jarraitu"		||   ?* [ {.seguir.}| {.continuar.} ]					?*   _ ?* ]
	.o.##|
	[ {Per} -> "joan"		||   ?* {.ir.}								?*   _ ?* ]
	.o.##|
	[ {Per} -> "nahi" "_izan"	||   ?* {.querer.}							?*   _ ?* ]
	.o.##|
	[ {Per} -> "utzi"		||   ?* {.dejar.} IM %+ {.de.}						?*   _ ?* ]
!##    |
!##    [ ~ [ ?* XXX {P1} ?* ] ]
];


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Aditz perifrastiko (P1) trinkoak;

define TransTrinkPer [ 
	[ {AspP} ?* "/"{AdL}  -> "[ADT]" ||   ?* [VMIP | VMII] ?*  XXX {P1} ?* "/" AdTrinkoa "<"{PER}">" ?* _ ?* ] 
!##    |
!##    [ ~ [ ?* [VMIP | VMII] ?*  XXX {P1} ?* "/" AdTrinkoa "<"{PER}">" ?* {AspP} ?* "/"{AdL} ?* ] ]
] ;

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Aditz perifrastikoetan (P1) aspektuaren (AspP) itzulpena;

define TransAspPA [ 
!##	[ 
	  [ {AspP}		-> "ADPGER"	||   ?* [ VMIF|VMIC|VAIC ]	?* XXX {P1} ?*        "_izan"   "<PER>" _ ?*  ] 
	  .o.##|
	  [ {AspP}		-> "ADOIN"	||   				?* XXX {P1} ?*  "ari" "_izan"   "<PER>" _ ?*  ] 
	  .o.##|
	  [ {AspP}		-> "IZE"	||   				?* XXX {P1} ?* ["ahal"|"behar"|"nahi"]	           #...
												      "_izan"   "<PER>" _ ?*  ]  
	  .o.##|
	  [ {AspP}		-> "GERO"	||   ?* [ VMIF|VMIC|VAIC ]	?* XXX {P1} ?*				_ ?*  ] 
	  .o.##|		
	  [ {AspP}		-> "EZBU"	||   ?* [ VMIP|VMII ]		?* XXX {P1} ?* ["hasi"|"amaitu"|"utzi"|"jarraitu"    #...
											       |"joan"|"eraman"|"etorri" ]	_ ?*  ] 
	  .o.##|
	  [ {AspP}		-> "BURU"	|| 				?* XXX {P1} ?*				_ ?*  ] 
!##	]
!##    	 |
!##    	  [ [ ?* [VMIP | VMII] ?*  XXX {P1} ?* "/" AdTrinkoa "<"{PER}">" ?* {[ADT]} ?* ] ]
!##      |
!##       [ ~ [ ?* XXX {P1} ?* ] ]
];

define TransAspPB [ 
!##	[
	  [ "ari"   "_izan"	-> "aritu"	|| 				?* XXX {P1} ?*	     _  "<PER>" "ADOIN" ?* ] #!!#
	  .o.##|			
	  [ "ahal"  "_izan"	-> "ahal"	|| 				?* XXX {P1} ?*	     _  "<PER>" "IZE" ?* ] #!!#
	  .o.##|			
	  [ "behar" "_izan"	-> "behar"	|| 				?* XXX {P1} ?*	     _  "<PER>" "IZE" ?* ] #!!#
	  .o.##|			
	  [ "nahi"  "_izan"	-> "nahi"	|| 				?* XXX {P1} ?*	     _  "<PER>" "IZE" ?* ] #!!#
!##	]
!##    	 |
!##    	  [ ~ [ ?* XXX {P1} ?* [{ahal}|{behar}|{nahi}|{ari}] {_izan} "<"{PER}">" ?* ] ]
] ;

define TransAspP [ TransAspPA .o. TransAspPB ];   ##--


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Aditz perifrastikoetan (P2) partikularen itzulpena;

define TransPrt [
	[ {Prt} -> "ohi<PRT>[BST]"		||  ?* [ {.acostumbrar.} ?* {.a.} | {.soler.} ]		?*  "/" _ ?* ]
	.o.##|
	[ {Prt} -> "omen<PRT>[PRT][ZIU]"	||  ?* {.deber.}  ?* {.de.} 				?*  "/" _ ?* ]
	.o.##|
	[ {Prt} -> "berri<PRT>[ADJ][IZO]"	||  ?* {.acabar.} ?* {.de.} 				?*  "/" _ ?* ]
!##    |
!##    [ ~ [ ?* XXX {P2} ?* ] ]
];


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Aditz perifrastikoetan (P3) adberbioaren itzulpena;

define TransAdb [
	[ {Adb} -> "berriz<ADB>[ADB][ADOARR]" ||   ?* {.volver.} ?* {.a.}				?*  "/" _ ?* ]
!##    |
!##    [ ~ [ ?* XXX {P3} ?* ] ]
];


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Aditz modala eta bere aspektua (AdMod /AspM)


define TransAdMAspMKendu1 [
       [ "/" {AdM} {AspM} 	-> ""		||  [ "#" [ AdKArrSin | AdKPer4Sin | AdKArrSinPas | AdKPer4SinPas ]                  #...
						    | ?* [ VAIP |VAII |VAIC |VMSP |VMSI |VASP |VASI |VMM ]   ] 	?* XXX {AR} ?* _ ?* ] 
       .o.##|
       [ "/" {AdM}  {AspM} 	-> ""		||  ?* [ [ VMIP | VMII | VMIF | VMIC | VAIC ]	    	  	?* XXX {P1}	   #...
						      | [ VMIS | VAIP | VAII ]			    	  	?* XXX {P1} ?*	   #...
							  [ "hasi"|"amaitu"|"utzi"|"jarraitu"|"joan"|"eraman"|"etorri" ]	   #...
						      | VAIS ?* IMInf					?* XXX {P1} ?* "hasi" ]  ?*  _ ?* ] 
];

define TransAdMAspMKendu2 [
       [ "/" {AdM}  {AspM} 	-> ""			||  ?* [ [ VMIP |VMII |VMIS |VMIF				          #...
								 |VMIC |VAIS |VAIF |VAIC ]  ?* XXX {P2}				  #...
							      |[ VAIP | VAII ]		    ?* XXX {P2} ?* ["behar"|"amaitu"] ] ?* _ ?* ] 
       .o.##|
       [ "/" {AdM}  {AspM}   	-> ""			||  ?* [ [ AdKPer3Sin | AdKPer3SinPas ]				           #...
							  | ?* [ VAIP | VAII | VAIC ]	   ?*] XXX {P3} ?*		_ ?* ] 
];


define TransAdM [
    [  {AdM}	-> "izan<ADM>"	]
];

define TransAspM [
       [  {AspM} -> "BURU"	|| ?* [   VAIS				?* XXX [{AR}|{P3}]		 #...
				      | [ VMIS | VAIP | VAII | VAIS ]	?* XXX {P1}			 #... 
				      | [ VAIP | VAII ]			?* XXX {P2}	     ] ?* _ ?* ] 
       .o.##|
       [  {AspM} -> "GERO"	||  ?*   VAIF				?* XXX [{AR}|{P1}|{P3}] ?* _ ?* ] 
] ;


!##define TransAdMAspM [ 
!##	    TransAdMAspMKendu1 
!##	  | TransAdMAspMKendu2
!##	  | [ TransAdM .o. TransAspM ]
!##	  | ~ [ "#" ?* {AdM}{AspM} ?* "$" ]
!##];


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! Aditzaren laguntzailea  (Lag)


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Aditz laguntzailea (perifrastikoak ezartzen duenean laguntzaile mota);


!define TransAdLPer [   
!       [ {AdL} -> "edun<ADL>""[ADL]"  ||  ?* XXX {P} ?* PerifEdun  ?* "/" _ ?* ]
!       .o.##|
!       [ {AdL} -> "izan<ADL>""[ADL]"  ||  ?* XXX {P} ?* PerifIzan  ?* "/" _ ?* ]
!##    |
!##    [ ~ [ ?* XXX {P1} ?* [PerifIzan|PerifEdun]  ?* ] ]
!];

define TransAdLPer [   
       [ {DA}   -> "DU"      ||  ?*  _ ?* XXX {P} ?* PerifEdun ?* ]
       .o.##|
       [ {ZAIO} -> "DIO"     ||  ?*  _ ?* XXX {P} ?* PerifEdun ?* ]
       .o.##|
       [ {DU}   -> "DA"      ||  ?*  _ ?* XXX {P} ?* PerifIzan ?* ]
       .o.##|  
       [ {DIO}  -> "ZAIO"    ||  ?*  _ ?* XXX {P} ?* PerifIzan ?* ]
       .o.##|
       [ {DU}   -> "DA"      ||  ?* {.haber.} IM %+ {.que.} ?* _ ?* ]
       .o.##|  
       [ {DIO}  -> "ZAIO"    ||  ?* {.haber.} IM %+ {.que.} ?* _ ?* ]


];
		           

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Aditz laguntzailea (Bestelako kasuetan) ;

define TransAdL [	
        [ {AdL}    -> "edin<ADL>""[ADL]"	||   ?* [VMSP | VMSI | VMM] ?* [{DA} | {ZAIO}]  ?*  XXX ?* _ ?* ] 
	.o.##|
        [ {AdL}    -> "izan<ADL>""[ADL]"	||   ?* [{DA} | {ZAIO}]				?*  XXX ?* _ ?* ] 
	.o.##|
	[ {AdL}    -> "ezan<ADL>""[ADL]"	||   ?* [VMSP | VMSI | VMM] ?* [{DU} | {DIO}]   ?*  XXX ?* _ ?* ] 
	.o.##|
	[ {AdL}    -> "edun<ADL>""[ADL]"	||   ?* [{DU} | {DIO}]				?*  XXX ?* _ ?* ] 
!##     |
!##	[ ~ [ ?* {AdL} ?* ]  ] 
] ;




!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Denbora Modua (DenMod) ;

define TransDenMod [	
        [ {DenMod}    -> "[A1]"	||   ?* [ VMIP | VMIF | VAIP | VAIF | VASP]	?*  XXX ?*  _ ?* ] 
	.o.##|
	[ {DenMod}    -> "[A3]"	||   ?*   VMSP					?*  XXX ?*  _ ?* ] 
	.o.##|
	[ {DenMod}    -> "[B1]"	||   ?* [ VMII | VMIS | VAII | VAIS ]		?*  XXX ?*  _ ?* ] 
	.o.##| 
	[ {DenMod}    -> "[B2]"	||   ?* [ VMIC | VASI ] 			?*  XXX ?*  _ ?* ] 
	.o.##| 
	[ {DenMod}    -> "[B3]"	||   ?*   VAIC  				?*  XXX ?*  _ ?* ] 
	.o.##| 
	[ {DenMod}    -> "[B5B]"||   ?*   VMSI  				?*  XXX ?*  _ ?* ] 
	.o.##| 
	[ {DenMod}    -> "[A3]"||   ?*  VMM {01}				?*  XXX ?*  _ ?* ]
	.o.##| 
	[ {DenMod}    -> "[MDNC]" ||   ?*   VMM					?*  XXX ?*  _ ?* ] 
!##     |
!##	[ ~ [ ?* {DenMod} ?* ]  ] 
] ;


define ErlMorf [	
        [ {Erl}    -> "+n[ERL][MEN][HELB]"	||   ?*  ["edin<ADL>" | "ezan<ADL>" ] ?* [ "[A1]" | "[A3]" | "[B2]"  | "[B5B]"] ?* _ ?* ] 
	.o.##|
        [ {Erl}    -> ""	                ||   ?* _ ?* ] 
] ;



!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Nor(Nr) Sub;

define TransNrSub [	
        [ {Nr}    -> "[NR_NI]"	|| ?* {1S} Karak	?* [ "izan<ADL>" | AdTrIntr ?* "[ADT]" | "edin<ADL>" ] ?*  _ ?* ] 
	.o.##|
	[ {Nr}    -> "[NR_ZU]"	|| ?* {2S} Karak	?* [ "izan<ADL>" | AdTrIntr ?* "[ADT]" | "edin<ADL>" ] ?*  _ ?* ] 
	.o.##| 
	[ {Nr}    -> "[NR_HU]"	|| ?* {3S} Karak	?* [ "izan<ADL>" | AdTrIntr ?* "[ADT]" | "edin<ADL>" ] ?*  _ ?* ] 
	.o.##| 
	[ {Nr}    -> "[NR_GU]"	|| ?* {1P} Karak	?* [ "izan<ADL>" | AdTrIntr ?* "[ADT]" | "edin<ADL>" ] ?*  _ ?* ] 
	.o.##| 
	[ {Nr}    -> "[NR_ZK]"	|| ?* {2P} Karak	?* [ "izan<ADL>" | AdTrIntr ?* "[ADT]" | "edin<ADL>" ] ?*  _ ?* ] 
	.o.##| 
	[ {Nr}    -> "[NR_HK]"	|| ?* {3P} Karak	?* [ "izan<ADL>" | AdTrIntr ?* "[ADT]" | "edin<ADL>" ] ?*  _ ?* ] 
] ;

echo >> Nor(Nr) Obj;

define TransNrObj [	
        [ {Nr}    -> "[NR_NI]"	|| ?*  {obj1s}		?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
	.o.##|
	[ {Nr}    -> "[NR_ZU]"	|| ?*  {obj2s}		?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
	.o.##| 
	[ {Nr}    -> "[NR_HU]"	|| ?*  {obj}[{3s}|{00}] ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
	.o.##| 
	[ {Nr}    -> "[NR_GU]"	|| ?*  {obj1p}		?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
	.o.##| 
	[ {Nr}    -> "[NR_ZK]"	|| ?*  {obj2p}		?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
	.o.##| 
	[ {Nr}    -> "[NR_HK]"	|| ?*  {obj3p}		?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
] ;

!##define TransNr [  TransNrSub | TransNrObj | ~ [ "#" ?* {Nr} ?* "$" ] ];


echo >> Nori(Ni);

define TransNi [	
        [ {Ni}    -> "[NI_NI]"	||   ?* {dat1s} ?* _ ?* ] 
	.o.##|
	[ {Ni}    -> "[NI_ZU]"	||   ?* {dat2s} ?* _ ?* ] 
	.o.##| 
	[ {Ni}    -> "[NI_HU]"	||   ?* [ {dat3s}| "utzi""<PER>" ] ?* _ ?* ] 
	.o.##| 
	[ {Ni}    -> "[NI_GU]"	||   ?* {dat1p} ?* _ ?* ] 
	.o.##| 
	[ {Ni}    -> "[NI_ZK]"	||   ?* {dat2p} ?* _ ?* ] 
	.o.##| 
	[ {Ni}    -> "[NI_HK]"	||   ?* {dat3p} ?* _ ?* ] 
	.o.##| 
	[ {Ni}    -> ""		||   ?* {dat00} ?* _ ?* ] 

!##     |
!##	[ ~ [ ?* {Ni} ?* ]  ] 
] ;


echo >> Nork(Nk) Subj Pas;

define TransNkPas [	
        [ {Nk}    -> "[NK_NI]"	|| ?*  {sub1s}		?* "PAS" ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
	.o.##|
	[ {Nk}    -> "[NK_ZU]"	|| ?*  {sub2s}		?* "PAS" ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
	.o.##| 
	[ {Nk}    -> "[NK_HU]"	|| ?*  {sub3s}		?* "PAS" ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
	.o.##| 
	[ {Nk}    -> "[NK_GU]"	|| ?*  {sub1p}		?* "PAS" ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
	.o.##| 
	[ {Nk}    -> "[NK_ZK]"	|| ?*  {sub2p}		?* "PAS" ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
	.o.##| 
	[ {Nk}    -> "[NK_HK]"	|| ?*  {sub}[{3p}|{00}]	?* "PAS" ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ]  ?*  _ ?* ] 
] ;

echo >> Nork(Nk);

define TransNk [	
        [ {Nk}    -> "[NK_NI]"	||    ?* {1S} Karak ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ] ?*    _ ?* ] 
	.o.##|
	[ {Nk}    -> "[NK_ZU]"	||    ?* {2S} Karak ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ] ?*    _ ?* ]
	.o.##| 
	[ {Nk}    -> "[NK_HU]"	||    ?* {3S} Karak ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ] ?*    _ ?* ]
	.o.##| 
	[ {Nk}    -> "[NK_GU]"	||    ?* {1P} Karak ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ] ?*    _ ?* ]
	.o.##| 
	[ {Nk}    -> "[NK_ZK]"	||    ?* {2P} Karak ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ] ?*    _ ?* ]
	.o.##| 
	[ {Nk}    -> "[NK_HK]"	||    ?* {3P} Karak ?* [ "edun<ADL>" | AdTrTran ?* "[ADT]" | "ezan<ADL>" ] ?*    _ ?* ]
	.o.##| 
	[ {Nk}    -> ""		||    ?*	       [ "izan<ADL>" | AdTrIntr 	   | "edin<ADL>" ] ?*    _ ?* ] 

!##     |
!##	[ ~ [ ?* {Nk} ?* ]  ] 
] ;

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!echo >> Erlazio morfema (ErlMor) ;

!define TransErlMor [	
!        [ {ErlMor}     -> "lako[ERL][MEN][KAUS]"	||   ?*  "[" {KAUS}		"]" ?* XXX ?* "+"  _ ?* ] 
!	.o.##|
!        [ {ErlMor}     -> "la[ERL][MEN][KONP]"		||   ?*  "[" {KONP}		"]" ?* XXX ?* "+"  _ ?* ] 
!	.o.##|
!        [ {ErlMor}     -> "n[ERL][MEN][ZHG]"		||   ?*  "[" {ZHG}		"]" ?* XXX ?* "+"  _ ?* ] 
!	.o.##|
!        [ {ErlMor}     -> "n[ERL][MEN][ERLT]"		||   ?*  "[" {ERLT}		"]" ?* XXX ?* "+"  _ ?* ] 
!	.o.##|
!        [ {ErlMor}     -> "n[ERL][MEN][MOS]"		||   ?*  "[" {MOS}		"]" ?* XXX ?* "+"  _ ?* ] 
!	.o.##|
!        [ "+" {ErlMor} -> ""				||   ?*				       XXX ?*      _ ?* ] 
!##     |
!##	[ ~ [ ?* {ErlMor} ?* ]  ] 
!];


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >>  Aspektuak osatu ;


define TransAspOsatu [  		     		     		     		     
       [  "PART"   -> "[ADI][SIN]+[AMM][PART]" ]				     #!!#
       .o.								     
       [  "ADOIN"  -> "[ADI][SIN]+[AMM][ADOIN]" ]				     #!!#
       .o.								     
       [  "EZBU"   -> "[ADI][SIN]+[AMM][ADOIN]+[ASP][EZBU]" ] 		     #!!#
       .o.								    
       [  "BURU"   -> "[ADI][SIN]+[AMM][PART]+[ASP][BURU]" ]		     #!!#
       .o.								    
       [  "GERO"   -> "[ADI][SIN]+[AMM][PART]+[ASP][GERO]" ]		     #!!#
       .o.								    
       [  "ADPGER" -> "[ADI][SIN]+[AMM][PART]+[ASP][GERO]" ]		     #!!#
       .o.								    
       [  "MOD"    -> "[ADI][SIN]+[AMM][PART]+[ERL][MEN][MOD]" ]		     #!!#
       .o.								    
       [  "ADIZE"  -> "[ADI][SIN]+[AMM][ADIZE]" ]				     #!!#
       .o.								    
       [  "HUR"    -> "[ADI][SIN]+[AMM][ADOIN]+[ASP][EZBU][HUR]" ]	     #!!#
       .o.								    
       [  "ALA"    -> "[ADI][SIN]+[AMM][ADIZE]+[DEK][ALA]" ]		     #!!#
       .o.								    
       [  "DAT"    -> "[ADI][SIN]+[AMM][ADIZE]+[DEK][DAT][NUMS]" ]	     #!!#
       .o.								    
       [  "INE"    -> "[ADI][SIN]+[AMM][ADIZE]+[DEK][INE]" ]		     #!!#
       .o.								    
       [  "IZE"    -> "[IZE][ARR]"  ||                        "<PER>" _ ?* ]					     #!!#
];

!Gorkak aldatua ([ADIZE]  ->  [AD[IZE][ARR]]) bihurketa egiten zuen
!       [  "IZE"    -> "[IZE][ARR]" ]					     #!!#
!Gorkak aldatua (sorkuntzan ez da ahal_izan[ADI][ADP] erabili behar baizik eta izan[ADI][ZIN]
!       [  "ADPGER" -> "[ADI][SIN]+[AMM][PART]+[ASP][GERO]" ]		     #!!#



define ukanIzan [
		      [ "ukan" -> "izan" || ?* _ XXX ?* "<""NAG"">""[""ADI""]""[""SIN""]" ?* ]
		      .o.
		      [ "ukan" -> "izan" || ?* _ XXX ?* "<""NAG"">""[""ADI""]""[""SIN""]" ?* ]

];
								     
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo >> Irteera garbitzeko;

define TransIrteera1 [
		      [ "#" ?* "&" ?* "&" -> "" ]
		      .o.
		      [ XXX  ?* ")"		 -> "" ]
		    ];	

define TransIrteera2 [
		      [ "$"			-> ""	]
		      .o.
		      [ "#"			-> ""	]
		    ];

!##define TransIrteera [TransIrteera1 .o. TransIrteera2 ];




!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

echo 
echo >> + Automatak gorde

read regex [TransPatr.i];		save stack ../xtr01.fst;	clear stack;
read regex [TransAspNEJ.i];		save stack ../xtr02.fst;	clear stack;		
read regex [TransTrinkArr.i];		save stack ../xtr03.fst;	clear stack;		
read regex [TransAspNArrPer3.i];	save stack ../xtr04.fst;	clear stack;		
read regex [TransAspNPer1.i];		save stack ../xtr05.fst;	clear stack;
read regex [TransAspNPer2.i];		save stack ../xtr06.fst;	clear stack;
read regex [TransPer.i];		save stack ../xtr07.fst;	clear stack;
read regex [TransTrinkPer.i];		save stack ../xtr08.fst;	clear stack;
read regex [TransAspP.i];		save stack ../xtr09.fst;	clear stack;
read regex [TransPrt.i];		save stack ../xtr10.fst;	clear stack;
read regex [TransAdb.i];		save stack ../xtr11.fst;	clear stack;
read regex [TransAdMAspMKendu1.i];	save stack ../xtr12.fst;	clear stack;
read regex [TransAdMAspMKendu2.i];	save stack ../xtr13.fst;	clear stack;
read regex [TransAdM.i];		save stack ../xtr14.fst;	clear stack;	
read regex [TransAspM.i];		save stack ../xtr15.fst;	clear stack;
read regex [TransAdLPer.i];		save stack ../xtr16.fst;	clear stack;
read regex [TransAdL.i];		save stack ../xtr17.fst;	clear stack;
read regex [TransDenMod.i];		save stack ../xtr18.fst;	clear stack;
read regex [TransNrSub.i];		save stack ../xtr19.fst;	clear stack;
read regex [TransNrObj.i];		save stack ../xtr20.fst;	clear stack;
read regex [TransNi.i];		save stack ../xtr21.fst;	clear stack;
read regex [TransNkPas.i];		save stack ../xtr22.fst;	clear stack;
read regex [TransNk.i];		save stack ../xtr22.fst;	clear stack;
!read regex [TransErlMor.i];		save stack ../xtr23.fst;	clear stack;
read regex [TransAspOsatu.i];		save stack ../xtr24.fst;	clear stack;
read regex [TransIrteera1.i];		save stack ../xtr25.fst;	clear stack;
read regex [TransIrteera2.i];		save stack ../xtr26.fst;	clear stack;


echo  >> Bukatu du


!!FSAn egikaritu behar direnak
!TransPatr
!TransTrinkArr
!TransAspN
!TransPerif
!TransAdMAspM
!TransAdLPer
!TransAdL
!TransDenMod
!TransNr
!TransNi
!TransNk
!TransErlMor
!TransAspOsatu
!TransIrteera
