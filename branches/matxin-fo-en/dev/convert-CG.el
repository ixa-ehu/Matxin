(setq fo-cg-repls
  '(
    ("Neu" . "nt")
    ("Msc" . "m")
    ("Fem" . "f")
    ("Num" . "num")
    ("Nom" . "nom")
    ("N" . "n")
    ("V" . "vblex")
    ("PrfPrc" . "pp")
    ("PrsPrc" . "ppres")
    ("Prop" . "np")
    ("Adv" . "adv")
    ("CC" . "cnjcoo")
    ("CS" . "cnjsub")
    ("Interj" . "ij")
    ("ACR" . "acr")
    ("Pron" . "prn")
    ("Pers" . "p1 p2 p3")
    ("Det" . "det")
    ("Dem" . "dem")
    ("Poss" . "pos")
    ("Interr" . "itg")
    ("Acc" . "acc")
    ("Gen" . "gen")
    ("Dat" . "dat")
    ("Def" . "def")
    ("Indef" . "ind")
    ("Cmp" . "comp")
    ("Superl" . "sup")
    ("Prs" . "pres")
    ("Prt" . "past")
    ("1Sg" . "p1 sg")
    ("2Sg" . "p2 sg")
    ("3Sg" . "p3 sg")
    ("Sg" . "sg")
    ("Pl" . "pl")
    ("Inf" . "inf")
    ("Ind" . "pri")
    ("Imp" . "imp")
    ("Pr" . "pr")
    ("Sup" . "supn")
    ("CLB" . "clb")
    ("PUNCT" . "sent cm")
    ("A" . "adj")
    ))

(defun fo-cg-replace (from to)
  (goto-char (point-min))
  (while (re-search-forward from nil t)
    (replace-match (concat "\\1" to "\\2") 'fixedcase nil)))

(defun fo-cg-replace-helper (repls)
  (save-excursion
    (while repls
      
      (let* ((from (caar repls))
	     (to   (cdar repls))
	     (LPAR "\\(?:(\\|([^(;]* \\)")
	     (RPAR "\\(?:)\\| [^);]*)\\)")
	     (LEFT  (concat "[^;]*" LPAR "\\)"))
	     (RIGHT (concat "\\(" RPAR "[^;]*")))
	
	(fo-cg-replace
	 (concat "\\(SELECT" LEFT from RIGHT "IF\\)") to)
	(fo-cg-replace
	 (concat "\\(REMOVE" LEFT from RIGHT "IF\\)") to)
	(fo-cg-replace
	 (concat "\\(TARGET" LEFT from RIGHT "IF\\)") to)
	(fo-cg-replace
	 (concat "\\(SUBSTITUTE" LEFT from RIGHT "TARGET\\)") to)
	(fo-cg-replace
	 (concat "\\(ADD" LEFT from RIGHT "TARGET\\)") to)
	(fo-cg-replace
	 (concat "\\(MAP" LEFT from RIGHT "TARGET\\)") to)
      
	(let ((LEFT2  (concat "[^;]*([^()]*" LPAR "\\)"))
	      (RIGHT2 (concat "\\(" RPAR "[^)]*)[^;]*")))
	  (fo-cg-replace
	   (concat "\\(IF" LEFT2 from RIGHT2 ";\\)") to)))
      
      (setf repls (cdr repls)))))

(defun fo-cg-replace-arrows ()
  (save-excursion
    (goto-char (point-min))
    (while (re-search-forward "\\(@[^ );]*\\)<" nil t)
      (replace-match "\\1←" 'fixedcase nil))
    (goto-char (point-min))
    (while (re-search-forward "\\(@[^ );]*\\)>" nil t)
      (replace-match "\\1→" 'fixedcase nil))))


(defun fo-cg-replace-all ()
  "Replaces tags between SELECT/REMOVE/TARGET and
IF, and between ADD/MAP/SUBSTITUTE and TARGET, and between IF
and \";\"

You have to manually search for instances of \"p1 p2 p3\" and
change them into disjunctions where needed, and change instances
of the \"n\" tag into \"n OR np\" in the appropriate manner...

The same goes for \"vblex\" and \"vbser\"! (still TODO in
matxin-fo-en.fo-en.rlx)

Also note that if you have several instances of the same tag in
one match, only the last will be replaced, so try running this
several times and checking the diff.

A final problem: rules like 

SELECT Adv IF  (0 (\"tá\"))(1 ((\"ið\") OR V OR Det OR Pron OR N);

have an extra parenthesis because of the OR, but get converted by
this function, so these OR rules you'll have to revert back
manually."
  (interactive)
  (let ((old-case-fold-search case-fold-search))
    (setq case-fold-search nil)
    (fo-cg-replace-helper fo-cg-repls)
    (fo-cg-replace-arrows)
    (setq case-fold-search old-case-fold-search)))


