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
	     (LPAR "\\(?:(\\|([^();]* \\)")
	     (RPAR "\\(?:)\\| [^();]*)\\)")
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
      
	(let ((LEFT2  (concat "[^;]*([^();]*" LPAR "\\)"))
	      (RIGHT2 (concat "\\(" RPAR "[^);]*)[^;]*")))
	  ;; only the first within an OR/BARRIER/LINK
	  (fo-cg-replace 
	   (concat "\\(IF" LEFT2 from RIGHT2 ";\\)") to))
      
	(let ((LEFT2 (concat "[^();]*" LPAR "\\)"))
	      (RIGHT2 (concat "\\(" RPAR "[ )]")))
	  ;; subsequent ones within an OR/BARRIER/LINK
	  (fo-cg-replace
	   (concat "\\(IF[^;]*OR" LEFT2 from RIGHT2 "\\)") to)
	  (fo-cg-replace
	   (concat "\\(IF[^;]*BARRIER" LEFT2 from RIGHT2 "\\)") to)
	  (fo-cg-replace
	   (concat "\\(IF[^;]*LINK" LEFT2 from RIGHT2 "\\)") to)
	  ))

      (setf repls (cdr repls)))))

(defun fo-cg-replace-arrows ()
  (save-excursion
    (goto-char (point-min))
    (while (re-search-forward "\\(@[^ );]*\\)<" nil t)
      (replace-match "\\1←" 'fixedcase nil))
    (goto-char (point-min))
    (while (re-search-forward "\\(@[^ );]*\\)>" nil t)
      (replace-match "\\1→" 'fixedcase nil))))

(defun fo-cg-replace-n-np ()
  (save-excursion
    (goto-char (point-min))
    (while (re-search-forward "(n\\( [^);]*)\\)" nil t)
      (replace-match "\\& OR (np\\1" 'fixedcase nil))))


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
manually (grep a diff for OR, there's likely to be fairly few
hits).

Another problem: SUBSTITUTE rules with no IF after the
TARGET (only one occurrence so far).

Maybe I should just write a parser instead."
  (interactive)
  (let ((old-case-fold-search case-fold-search))
    (setq case-fold-search nil)
    (fo-cg-replace-helper fo-cg-repls)
    (fo-cg-replace-arrows)
    (fo-cg-replace-n-np)
    (setq case-fold-search old-case-fold-search))
;;   (occur "IF[^;]*\\()[^();]*OR\\|OR[^();]*(\\)")
  )



;;; 



(require 'cl)

(defun fo-cg-scan-sexps (from bound)
  (let ((to (scan-sexps from 1)))
    (if (< to bound)
	to
      (error (concat "Parens mismatch, expected end before char " (number-to-string bound))))))

(defun fo-cg-replace-all2 ()
  "Does not work with OR on ad-hoc sets (is that legal anyway?), and SUBSTITUTE/MAP/ADD rules without an IF."
  (interactive)
  (save-excursion
    (let ((old-case-fold-search case-fold-search))
      (setq case-fold-search nil)
      (fo-cg-replace-rules)
      (fo-cg-replace-arrows)
      (fo-cg-replace-n-np)
      (setq case-fold-search old-case-fold-search)))
;;   (occur "IF[^;]*\\()[^();]*OR\\|OR[^();]*(\\)")
  )

(defun fo-cg-replace-rules ()
  (goto-char (point-min))
  (while (re-search-forward "SELECT\\|REMOVE\\|ADD\\|MAP\\|SUBSTITUTE" (point-max) t)
    (fo-cg-replace-before-if)
    (fo-cg-replace-if)))

(defun fo-cg-replace-before-if ()
  (let* (;	 (semicolon (save-excursion (re-search-forward "[^\"\\];")))
	 (if-point (save-excursion (re-search-forward "[^\"\\];"))))
    ;\\(\\s \\|)\\)IF\\(\\s \\|(\\)
    (while (search-forward "(" if-point t)
      (let* ((s-open (match-beginning 0))
	     (s-close (fo-cg-scan-sexps s-open if-point)))
	(incf if-point (- (fo-cg-replace-ad-hoc-set s-open s-close)
			  s-close))))))

(defun fo-cg-replace-if ()
  "Replace all matches between IF and ;
Assumes all rules have an ; at the end!"
  (let ((end (save-excursion (re-search-forward "[^\"\\];"))))
    (while (search-forward "(" end t)
      (let* ((c-open (match-beginning 0))
	     (c-close (fo-cg-scan-sexps c-open end)))
	(incf end (- (fo-cg-replace-context-condition c-open c-close)
		     c-close))))))

(defun fo-cg-replace-context-condition (c-beg c-end)
  "A context condition is a parenthesis consisting of named SETs
or ad-hoc SETs. We leave the named SETs untouched, but ad-hoc
SETs, which are also in parentheses, we send to
`fo-cg-replace-ad-hoc-set'. However, if this contains an OR in
it, we have to recurse into it since then the parentheses are for
grouping.

The argument `c-beg' should be before the set parenthesis, `c-end'
should be after the end parenthesis."
  (goto-char (incf c-beg))
  (while (search-forward "(" c-end t)
    (let* ((s-open (match-beginning 0))
	   (s-close (fo-cg-scan-sexps s-open c-end))
	   (s-close2
	    (if (re-search-forward "\\(\\s \\|)\\)OR\\(\\s \\|(\\)" s-close t)
		(fo-cg-replace-context-condition s-open s-close)
	      (fo-cg-replace-ad-hoc-set s-open s-close))))
      (incf c-end (- s-close2 s-close))
      (goto-char s-close2)))
  c-end)


(defun fo-cg-replace-ad-hoc-set (beg end)
  "Replace all matches within an ad-hoc set. The argument `beg'
should be before the set parenthesis, `end' should be after the
end parenthesis."
  (goto-char (incf beg))
  (while (re-search-forward "[ )]" end t)
    (let* ((tag (buffer-substring-no-properties beg (match-beginning 0)))
	   (repl (assoc tag fo-cg-repls)))
      (when repl
	(delete-region beg (match-beginning 0))
	(goto-char beg)
	(insert (cdr repl))
	(incf end (- (length (cdr repl))
		     (length (car repl))))))
    (setq beg (point)))
  end)
