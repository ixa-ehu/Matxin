#include <lttoolbox/fst_processor.h>
#include <lttoolbox/ltstr.h>
#include <locale>

#include <string>
#include <iostream>

#include "matxin_string_utils.h"
#include "string_utils.h"

#include <data_manager.h>
#include <XML_reader.h>

using namespace std;

FSTProcessor fstp_generation;
FSTProcessor fstp_measures;
FSTProcessor fstp_pre_generation;
FSTProcessor fstp_nolex_generation;

bool DoGenTrace = false;

// Hiztegi elebidunaren euskarazko ordainetik lehenengoa lortzen du.
// IN:  Euskarazko ordainak ( ordain1[/ordain2]* )
// OUT: lehenengoa          ( oradin1            )
wstring select_first_translation(wstring &full)
{
  wstring output = full;

  for (size_t i = 0; i < output.size(); i++)
  {
    if (output[i] == L'/')
      output = output.substr(0, i);

    if (output[i] == L'\\')
      output.erase(i, 1);
  }

  if (output[output.size() - 1] == L'-')
    output.erase(output.size() - 1, 1);

  return output;
}


wstring keep_case(wstring form, wstring UpCase)
{
  wstring out_form = form;

  if (UpCase == L"first" || UpCase == L"title" || UpCase == L"all" )
    out_form[0] = toupper(out_form[0]);

  for (size_t i = 1; i<form.size(); i++)
  {
    if (UpCase == L"all" || UpCase == L"title" && form[i - 1] == L' ')
      out_form[i] = toupper(out_form[i]);
  }

  return out_form;
}


wstring verb_generation(wstring lemma, wstring pos, wstring suf, wstring cas,
                        wstring mi, wstring chunk_mi, wstring head_sem,
                        bool is_last, bool &flexioned)
{
  wstring analysis, form, prefix, postposizio, pre_lemma, lemma_osoa;
  lemma_osoa = lemma;

  if (DoGenTrace) {
    wcerr << lemma << L" " << pos << L" " << mi << L" " << cas << endl;
  }

  for (size_t i = 0; i < lemma.size(); i++)
  {
    if (lemma[i] == L'_')
      lemma[i] = L' ';
  }

  if (cas.find(L"++") != wstring::npos)
  {
    postposizio = L" " + cas.substr(cas.find(L"++") + 2, cas.size());
    cas = cas.substr(0, cas.find(L"++"));
  }

  if (cas.find(L'+') != 0 && cas.find(L'+') != wstring::npos)
  {
    prefix = cas.substr(0, cas.find(L'+'));
    cas = cas.substr(cas.find(L'+') + 1, cas.size());

    if (prefix.find(L"[") != wstring::npos)
      prefix.insert(prefix.find(L"["), L"+");
  }

  size_t position1 = lemma.rfind(L' ');
  size_t position2 = lemma.rfind(L'-');
  if (position1 != wstring::npos or position2 != wstring::npos)
  {
    if (position1 == wstring::npos or
        (position2 != wstring::npos and position2 > position1))
    {
      position1 = position2;
    }

    pre_lemma = lemma.substr(0, position1 + 1);
    lemma = lemma.substr(position1 + 1, lemma.size());
  }
  
  // TODO: LANGUAGE INDEPENDENCE
  if (mi.substr(0, 10) == L"[ADI][SIN]")
  {
    pos = L"[ADI][SIN]";
    mi.erase(0, 10);
  }
  if (mi.substr(0, 10) == L"[ADI][ADP]")
  {
    pos = L"[ADI][ADP]";
    mi.erase(0, 10);
  }
  if (mi.substr(0, 5) == L"[ADL]")
  {
    pos = L"[ADL]";
    mi.erase(0, 5);
  }
  if (mi.substr(0, 5) == L"[ADT]")
  {
    pos = L"[ADT]";
    mi.erase(0, 5);
  }

  wstring pre_gen = L"^" + lemma_osoa + pos + L"$";

  if (DoGenTrace) {
    wcerr << pre_gen << endl;
  }

  wstring lemmaMorf = fstp_pre_generation.biltrans(pre_gen);

  if (DoGenTrace) {
    wcerr << lemmaMorf << endl;
  }

  if (lemmaMorf[0] == L'^' and lemmaMorf[1] == L'@') {
    lemmaMorf = lemma + pos + mi;
  } else {
    lemmaMorf = lemmaMorf.substr(1, lemmaMorf.size() - 2) + mi;
  }

  for (size_t i = 0; i < int(lemmaMorf.size()); i++)
  {
    if (lemmaMorf[i] == L'\\')
    {
      lemmaMorf.erase(i, 1);
      i--;
    }
  }

  // TODO: LANGUAGE INDEPENDENCE
  if (is_last) {
    lemmaMorf = prefix + lemmaMorf;
  }

  if (DoGenTrace) {
    wcerr << lemmaMorf << endl;
  }

  analysis = L"^" + get_generation_order(prefix, lemmaMorf, chunk_mi, cas, head_sem) + L"$";


  if (DoGenTrace) {
    wcerr << analysis << endl;
  }

  if (analysis.find(L"LemaMorf") != wstring::npos)
    analysis.replace(analysis.find(L"LemaMorf"), 8, lemmaMorf);
  if (analysis.find(L"Lema") != wstring::npos)
    analysis.replace(analysis.find(L"Lema"), 4, lemma);
  if (analysis.find(L"Num") != wstring::npos)
    analysis.replace(analysis.find(L"Num"), 3, chunk_mi);
  if (analysis.find(L"Kas") != wstring::npos)
  {
    analysis.replace(analysis.find(L"Kas"), 3, cas);
    flexioned = true;
  }

  for (int i = 0; i < int(analysis.size()); i++)
  {
    if (analysis[i] == L'[')
      analysis[i]= L'<';
    if (analysis[i] == L']')
      analysis[i]= L'>';
    if (analysis[i] == L' ' )
    {
      analysis.erase(i, 1);
      i--;
    }
  }


  if (DoGenTrace)
    wcerr << L"GEN:" << analysis << endl;

  form = fstp_generation.biltrans(analysis);

  if (DoGenTrace)
    wcerr << L"GEN:" << form << endl;

  form = form.substr(1, form.size() - 2);

  if (form.size() == 0 or form[0] == '@' or
      form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
  {

    if (DoGenTrace)
      wcerr << L"GEN nolex:" << analysis << endl;

    form = fstp_nolex_generation.biltrans(analysis);
    form = form.substr(1, form.size() - 2);

    if (DoGenTrace)
      wcerr << L"GEN nolex:" << form << endl << endl;

    if (form.size() == 0 or form[0] == L'@' or
        form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
    {
      form = lemma;
    }
  }

  form = select_first_translation(form);
  form = pre_lemma + form;
  if (flexioned)
    form += postposizio;


  if (DoGenTrace)
    wcerr << form << endl << endl;

  return form;
}


wstring number_generation(wstring lemma, wstring pos, wstring suf, wstring cas,
                          wstring mi, wstring head_sem, bool is_last,
                          bool &flexioned)
{
  wstring analysis, form, prefix, postposizio, pre_lemma, lemma_osoa;


  if (DoGenTrace)
    wcerr << lemma << L" " << pos << L" " << mi << L" " << cas << endl;


  // TODO: LANGUAGE INDEPENDENCE
  if (pos == L"[Zu]" || pos == L"[Zm]")
  {
    wstring dimension = lemma.substr(0, lemma.find(L"_"));
    lemma = lemma.substr(lemma.find(L"_") + 1);
    wstring measure = lemma.substr(0, lemma.find(L":"));
    wstring number = lemma.substr(lemma.find(L":") + 1);
    lemma = number + measure;
  }

  // TODO: LANGUAGE INDEPENDENCE
  if (pos == L"[Zp]")
  {
    wstring number = lemma.substr(0, lemma.find(L"/"));
    wstring ratio = lemma.substr(lemma.find(L"/") + 1);
    lemma = ratio + L"/" + number;
  }

  // TODO: LANGUAGE INDEPENDENCE
  if (mi == L"")
    mi = L"[MG]";

  if (cas.find(L"++") != wstring::npos)
  {
    postposizio = L" " + cas.substr(cas.find(L"++") + 2, cas.size());
    cas = cas.substr(0, cas.find(L"++"));
  }

  if (is_last)
  {
    wstring lemmaMorf = lemma + pos;

    if (DoGenTrace)
      wcerr << prefix << L" " << lemmaMorf << L" " << mi << L" " << cas << head_sem << endl;

    analysis = L"^" + get_generation_order(prefix, lemmaMorf, mi, cas, head_sem) + L"$";


    if (DoGenTrace) wcerr << analysis << endl;

    if (analysis.find(L"LemaMorf") != wstring::npos)
      analysis.replace(analysis.find(L"LemaMorf"), 8, lemmaMorf);
    if (analysis.find(L"Lema") != wstring::npos)
      analysis.replace(analysis.find(L"Lema"), 4, lemma);
    if (analysis.find(L"Num") != wstring::npos)
      analysis.replace(analysis.find(L"Num"), 3, mi);
    if (analysis.find(L"Kas") != wstring::npos)
      analysis.replace(analysis.find(L"Kas"), 3, cas);

    flexioned = true;
  }
  else
  {
    analysis = L"^" + lemma + pos + L"$";
    postposizio = L"";
    flexioned = false;
  }

  for (int i = 0; i < int(analysis.size()); i++)
  {
    if (analysis[i] == L'[')
      analysis[i]= L'<';
    if (analysis[i] == L']')
      analysis[i]= L'>';
    if (analysis[i] == L' ')
    {
      analysis.erase(i, 1);
      i--;
    }
  }

/*
  if (DoGenTrace)
    wcerr << L"GEN:" << analysis << endl;
*/
  form = fstp_measures.biltrans(analysis);
/*
  if (DoGenTrace)
    wcerr << L"GEN:" << form << endl;
*/
  form = form.substr(1, form.size() - 2);

  if (form.size() == 0 or form[0] == L'@' or
      form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
  {
    form = lemma;
  }

  form = select_first_translation(form);

  form = form + postposizio;

  for (size_t i = 0; i < form.size(); i++)
  {
    if (form[i] == L'_')
      form[i]= L' ';
  }

/*
  if (DoGenTrace)
    wcerr << form << endl << endl;
*/
  return form;
}


wstring date_generation(wstring lemma, wstring pos, wstring suf, wstring cas,
                        wstring mi, wstring head_sem, bool is_last,
                        bool &flexioned)
{
  wstring analysis, form, prefix, postposizio, pre_lemma, lemma_osoa;

/*
  if (DoGenTrace)
    wcerr << lemma << L" " << pos << L" " << mi << L" " << cas << endl;
*/
  wstring century, week_day, day, month, year, hour, meridiam;
  wstring century_cas, week_day_cas, day_cas, month_cas, year_cas, hour_cas;
  bool century_last, week_day_last, day_last, month_last, year_last, hour_last;

  lemma = lemma.substr(1, lemma.size() - 2);

  // TODO: LANGUAGE INDEPENDENCE
  if (lemma.substr(0, 2) == L"s:")
  {
    century = lemma.substr(2);
    week_day = day = month = year = L"??";
    hour = L"??.??";
  }
  else
  {
    week_day = lemma.substr(0, lemma.find(L":"));
    lemma = lemma.substr(lemma.find(L":") + 1);
    day = lemma.substr(0, lemma.find(L"/"));
    lemma = lemma.substr(lemma.find(L"/") + 1);
    month = lemma.substr(0, lemma.find(L"/"));
    lemma = lemma.substr(lemma.find(L"/") + 1);
    year = lemma.substr(0, lemma.find(L":"));
    lemma = lemma.substr(lemma.find(L":") + 1);
    hour = lemma.substr(0, lemma.find(L":"));
    lemma = lemma.substr(lemma.find(L":") + 1);
    meridiam = lemma;

    century = L"??";
  }

  if (century != L"??" && cas != L"")
  {
    century_cas = cas;
    cas = L"";
    century_last = is_last;
    is_last = true;
  }
  else
  {
    century_cas = L"[INE]";
    century_last = is_last;
  }

  if (day != L"??" && cas != L"")
  {
    day_cas = cas;
    cas = L"";
    day_last = is_last;
    is_last = true;
  }
  else
  {
    day_cas = L"[INE]";
    day_last = is_last;
  }

  if (month != L"??" && cas != L"")
  {
    month_cas = cas;
    cas = L"";
    month_last = is_last;
    is_last = true;
  }
  else
  {
    month_cas = L"[GEN]";
    month_last = is_last;
  }

  if (year != L"??" && cas != L"")
  {
    year_cas = cas;
    cas = L"";
    year_last = is_last;
    is_last = true;
  }
  else
  {
    year_cas = L"[GEL]";
    year_last = is_last;
  }

  if (week_day != L"??" && cas != L"")
  {
    week_day_cas = cas;
    cas = L"";
    week_day_last = is_last;
    is_last = true;
  }
  else
  {
    week_day_cas = L"[ABS]";
    week_day_last = is_last;
  }

  if (hour != L"??.??" && cas != L"")
  {
    hour_cas = cas;
    cas = L"";
    hour_last = is_last;
    is_last = true;
  }
  else
  {
    hour_cas = L"[INE]";
    hour_last = is_last;
  }


  // TODO: LANGUAGE INDEPENDENCE
  if (century != L"??")
    form = number_generation(century, L"[CN]", suf, century_cas, L"[NUMS]",
                             head_sem, century_last, flexioned);

  if (year != L"??")
    form = number_generation(year, L"[Z]", suf, year_cas, L"[NUMS]", head_sem,
                             year_last, flexioned);

  if (month != L"??")
  {
    if (form != L"")
      form += L" ";
    form += number_generation(month, L"[MM]", suf, month_cas, L"[NUMS]",
                              head_sem, month_last, flexioned);
  }

  if (day != L"??")
  {
    if (form != L"")
      form += L" ";
    form += number_generation(day, L"[Z]", suf, day_cas, L"[NUMS]", head_sem,
                              day_last, flexioned);
  }

  if (week_day != L"??")
  {
    if (form != L"")
      form += L", ";
    form += number_generation(week_day, L"[WD]", suf, week_day_cas, L"[NUMS]",
                              head_sem, week_day_last, flexioned);
  }

  if (hour != L"??.??")
  {
    if (form != L"")
      form += L", ";
    form += number_generation(hour, L"[Z]", suf, hour_cas, L"[MG]", head_sem,
                              hour_last, flexioned);
    if (meridiam != L"??")
      form += L" (" + meridiam + L")";
  }

  flexioned = true;

  return form;
}


/* This method needs completely re-writing. Generation should be performed
   on all nodes in a chunk. The generation should consist of concatenation of
   the lemma, the part of speech and any morphological information. This should
   then be looked up in the transducer.  */ 
wstring generation(wstring lemma, wstring pos, wstring suf, wstring cas,
                   wstring mi, wstring head_sem, bool is_last, bool &flexioned)
{
  wstring analysis, form, prefix, postposizio, pre_lemma, lemma_osoa;
  lemma_osoa = lemma;

  if (DoGenTrace) {
    wcerr << L"lem: " << lemma << L" pos: " << pos << L" suf: " << suf << L" mi: " << mi << L" cas: " << cas << L" head_sem: " << head_sem << endl;
  }

  // TODO: this also happen to form, in procNODE...why? -KBU
  for (size_t i = 0; i < lemma.size(); i++) 
  {
    // Replace all '_' with ' '
    if (lemma[i] == L'_') {
      lemma[i]= L' ';
    }
  }

/*
  if ((!is_last && suf == L"") || cas == L"") {
    // If this is not the last lemma in the phrase and
    // there is no suffix and no case, then don't 
    // perform generation !
    return lemma;
  }

  if (mi == L"") {
    // If there is no morphological information, set
    // the number to singular
    mi = L"[NUMS]";
  }

  if (!is_last) {
    // If this is not the last lemma in the phrase, 
    // remove case information
    cas = L"";
  }

*/

  flexioned = true;  // The word should be inflected

  if (cas.find(L"++") != wstring::npos)
  {
    postposizio = L" " + cas.substr(cas.find(L"++") + 2, cas.size());
    cas = cas.substr(0, cas.find(L"++"));
  }

  if (cas.find(L'+') != 0 and cas.find(L'+') != wstring::npos)
  {
    prefix = cas.substr(0, cas.find(L'+'));
    cas = cas.substr(cas.find(L'+') + 1, cas.size());
  }

  size_t position1 = lemma.rfind(L' ');
  size_t position2 = lemma.rfind(L'-');

  if (position1 != wstring::npos or position2 != wstring::npos)
  {
    if (position1 == wstring::npos or (position2 != wstring::npos
        and position2 > position1))
    {
      position1 = position2;
    }

    pre_lemma = lemma.substr(0, position1 + 1);
    lemma = lemma.substr(position1 + 1, lemma.size());
  }

  //wstring pre_gen = L"^" + lemma_osoa + pos + L"$";
  wstring newpos = StringUtils::substitute(pos, L"[", L"<");
  newpos = StringUtils::substitute(newpos, L"]", L">");

  wstring newmi = StringUtils::substitute(mi, L"[", L"<");
  newmi = StringUtils::substitute(newmi, L"]", L">");

  wstring pre_gen = L"^" + lemma_osoa + newpos + newmi + L"$";

  if (DoGenTrace) {
    wcerr << L"pre_gen: " << pre_gen << endl;
  }

  //wstring lemmaMorf = fstp_pre_generation.biltrans(pre_gen);
  wstring lemmaMorf = fstp_generation.biltrans(pre_gen);

  if (DoGenTrace) {
    wcerr << L"lemmaMorf: " << lemmaMorf << endl;
  }

  if (lemmaMorf[0] == L'^' and lemmaMorf[1] == L'@') {
    lemmaMorf = lemma + pos;
  } else {
    lemmaMorf = lemmaMorf.substr(1, lemmaMorf.size() - 2);
    return lemmaMorf;
  }

  if (suf != L"") {
    // If the suffix is not empty, append it after
    // the lemma and POS
    lemmaMorf += suf;
  }

  for (size_t i = 0; i < lemmaMorf.size(); i++)
  {
    if (lemmaMorf[i] == L'\\')
    {
      lemmaMorf.erase(i, 1);
      i--;
    }
  }


  if (DoGenTrace) {
    wcerr << lemmaMorf << L" " << mi << L" " << cas << L" " << head_sem << endl;
  }

  // Get the generation order for the prefix, lemma+POS, morphological information,
  // case and 'head_sem' (this uses the 'eu_morph_preproc.dat' file)
  analysis = L"^" + get_generation_order(prefix, lemmaMorf, mi, cas, head_sem) + L"$";

  if (DoGenTrace) {
    wcerr << analysis << endl;
  }


  // Here we re-order the parts of a lexical unit depending on the information
  // in the 'eu_morph_preproc.dat' file
  if (analysis.find(L"LemaMorf") != wstring::npos)
    analysis.replace(analysis.find(L"LemaMorf"), 8, lemmaMorf);
  if (analysis.find(L"Lema") != wstring::npos)
    analysis.replace(analysis.find(L"Lema"), 4, lemma);
  if (analysis.find(L"Num") != wstring::npos)
    analysis.replace(analysis.find(L"Num"), 3, mi);
  if (analysis.find(L"Kas") != wstring::npos)
    analysis.replace(analysis.find(L"Kas"), 3, cas);

  // Replace Matxin-style '[' and ']' tags with
  // Apertium-style '<' and '>' tags for lttoolbox

  wstring newanalysis = StringUtils::substitute(analysis, L"[", L"<");
  analysis = StringUtils::substitute(newanalysis, L"]", L">");

/*
  for (int i = 0; i < int(analysis.size()); i++)
  {
    if (analysis[i] == L'[')
      analysis[i]= L'<';
    if (analysis[i] == L']')
      analysis[i]= L'>';
    if (analysis[i] == L' ' )
    {
      analysis.erase(i, 1);
      i--;
    }
  }
*/

  if (DoGenTrace) {
    wcerr << L"GEN:" << analysis << endl;
  }

  // Look up the analysis in the morphological generator
  form = fstp_generation.biltrans(analysis);

  if (DoGenTrace) {
    wcerr << L"GEN:" << form << endl;
  }

  // Remove the '^' and '$' from the beginning and end
  // of the lexical unit
  form = form.substr(1, form.size() - 2);

  // If the word is unknown or there has been an error in generation
  if (form.size() == 0 or form[0] == L'@' or
      form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
  {

    if (DoGenTrace) { 
      wcerr << L"GEN nolex:" << analysis << endl;
    }

    // Do non-lexical generation (guessing), using the 
    // file 'eu_morph_nolex.xml'
    form = fstp_nolex_generation.biltrans(analysis);
    form = form.substr(1, form.size() - 2);

    if (DoGenTrace) {
      wcerr << L"GEN nolex:" << form << endl;
    }

    // If the word is still unknown, set the surface
    // form to be the same as the lemma
    if (form.size() == 0 or form[0] == L'@' or
        form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
    {
      form = lemma;
    }
  }

  // Select the first of multiple translation equivalents
  form = select_first_translation(form);

  // The final form is the pre_lemma, the inflected word form and
  // any postposition
  form = pre_lemma + form + postposizio;

  if (form.size() == 0 or form[0] == L'@' or
       form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
  {
     form = lemma;
  }


  if (DoGenTrace)
  {
    wcerr << L"form: " << form << endl << endl;
  }
  
  return form;
}


wstring procSYN (xmlTextReaderPtr reader)
{
  wstring syns;

  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"SYN" and tagType == XML_READER_TYPE_ELEMENT)
  {
    syns += L"<SYN" + write_xml(allAttrib(reader));
    if (xmlTextReaderIsEmptyElement(reader) == 1)
    {
      syns += L"/>\n";
      return syns;
    }
    else
    {
      syns += L">\n";
    }
  }
  else
  {
    wcerr << L"ERROR: invalid tag: <" << tagName << allAttrib(reader)
          << L"> when <SYN> was expected..." << endl;
    exit(-1);
  }

  if (tagName == L"SYN" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    syns += L"</SYN>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName << allAttrib(reader)
          << L"> when </SYN> was expected..." << endl;
    exit(-1);
  }

  return syns;
}


wstring procNODE(xmlTextReaderPtr reader, wstring chunk_type, wstring cas,
                 wstring cas_alloc, wstring cas_ref, wstring mi,
                 wstring head_sem, int chunk_len)
{
  wstring nodes;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"NODE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    wstring lem = attrib(reader, "lem");
    wstring form;
    bool is_last = (watoi(attrib(reader, "ord").c_str()) == (chunk_len - 1));
    bool flexioned = false;

    // TODO: LANGUAGE INDEPENDENCE
    if (chunk_type.substr(0, 4) == L"adi-")
    {
      form = verb_generation(attrib(reader, "lem"), attrib(reader, "pos"),
                             attrib(reader, "suf"), cas, attrib(reader, "mi"),
                             mi, head_sem, is_last, flexioned);
    }
    else if (attrib(reader, "pos") == L"[W]")
    {
      form = date_generation(attrib(reader, "lem"), attrib(reader, "pos"),
                             attrib(reader, "suf"), cas, mi, head_sem, is_last,
                             flexioned);
    }
    else if (attrib(reader, "pos") == L"[Z]" || attrib(reader, "pos") == L"[Zu]"
             || attrib(reader, "pos") == L"[Zm]" || attrib(reader, "pos") == L"[Zp]"
             || attrib(reader, "pos") == L"[Zd]")
    {
      form = number_generation(attrib(reader, "lem"), attrib(reader, "pos"),
                               attrib(reader, "suf"), cas, mi, head_sem, is_last,
                               flexioned);
    }
    else
    {
//	    wcerr << L"mi:" << mi << L" reader-mi:" << attrib(reader, "mi") << endl;
      form = generation(attrib(reader, "lem"), attrib(reader, "pos"),
                        attrib(reader, "suf"), cas, attrib(reader, "mi"), head_sem, is_last,
                        flexioned);
    }

    // TODO: do we really want this? -KBU
    for (size_t i = 0; i<form.size(); i++) {
      if (form[i] == L'_') {
        form[i] = L' ';
      }
    } 
    
    form = keep_case(form, attrib(reader, "UpCase"));

    nodes += L"<NODE " + write_xml(L"form='" + form + L"'");
    nodes += L" ref ='" + write_xml(attrib(reader, "ref"));
    if (flexioned && cas_ref != L"")
      nodes += L"," + write_xml(cas_ref);
    nodes += L"' alloc ='" + write_xml(attrib(reader, "alloc"));
    if (flexioned && cas_alloc != L"")
      nodes += L"," + write_xml(cas_alloc);
    nodes += L"'" + write_xml(text_allAttrib_except(allAttrib_except(reader, L"alloc"), L"ref"));

    if (xmlTextReaderIsEmptyElement(reader) == 1)
    {
      nodes += L"/>\n";
      return nodes;
    }
    else
    {
      nodes += L">\n";
    }
  }
  else
  {
    wcout << nodes;
    wcerr << L"ERROR: invalid tag: <"<< tagName << allAttrib(reader)
          << L"> when <NODE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  // Process possible synonyms if any
  while (ret == 1 and tagName == L"SYN" and tagType == XML_READER_TYPE_ELEMENT)
  {
    nodes += procSYN(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  while (ret == 1 and tagName == L"NODE" and tagType == XML_READER_TYPE_ELEMENT)
  {
    nodes += procNODE(reader, chunk_type, cas, cas_alloc, cas_ref, mi, head_sem,
                      chunk_len);

    nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  if (tagName == L"NODE" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    nodes += L"</NODE>\n";
  }
  else
  {
    wcout << nodes;
    wcerr << L"ERROR: invalid document: found <" << tagName << allAttrib(reader)
          << L"> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}


wstring procCHUNK(xmlTextReaderPtr reader)
{
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);
  wstring tree, det_cas, cas, cas_alloc, cas_ref, mi, type, head_sem;
  int chunk_len;

  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    type = attrib(reader, "type");
    det_cas = attrib(reader, "case");
    cas = attrib(reader, "cas");
    cas_alloc = attrib(reader, "casalloc");
    cas_ref = attrib(reader, "casref");
    mi = attrib(reader, "mi");
    head_sem = attrib(reader, "headsem");
    chunk_len = watoi(attrib(reader, "length").c_str());
    tree = L"<CHUNK" + write_xml(allAttrib(reader)) + L">\n";


    if (det_cas != L"" && det_cas.rfind(L"++") != wstring::npos)
    {
      bool flexioned;
      wstring postposizio = det_cas.substr(det_cas.rfind(L"++") + 2);

      if (DoGenTrace)
        wcerr << postposizio << L" " << cas << endl;

      postposizio = generation(postposizio, L"", cas, L"", mi, head_sem, true,
                               flexioned);

      if (DoGenTrace)
        wcerr << postposizio << endl;

      cas = det_cas.substr(0, det_cas.rfind(L"++") + 2) + postposizio;

      if (DoGenTrace)
        wcerr << cas << endl;

    }


  }
  else
  {
    wcerr << L"ERROR: invalid tag: <" << tagName << allAttrib(reader)
          << L"> when <CHUNK> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  tree += procNODE(reader, type, cas, cas_alloc, cas_ref, mi, head_sem,
                   chunk_len);

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);
  while (ret == 1 and tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    tree += procCHUNK(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }


  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    tree += L"</CHUNK>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName << allAttrib(reader)
          << L"> when </CHUNK> was expected..." << endl;
    exit(-1);
  }

  return tree;
}


wstring procSENTENCE (xmlTextReaderPtr reader)
{
  wstring tree;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"SENTENCE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    tree = L"<SENTENCE" + write_xml(allAttrib(reader)) + L">\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName << allAttrib(reader)
          << L"> when <SENTENCE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  while (ret == 1 and tagName == L"CHUNK")
  {
    tree += procCHUNK(reader);

    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }

  if (ret == 1 and tagName == L"SENTENCE" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    tree += L"</SENTENCE>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName << allAttrib(reader)
          << L"> when </SENTENCE> was expected..." << endl;
    exit(-1);
  }

  return tree;
}


int main(int argc, char *argv[])
{

  // Set locale information

  // This sets the C++ locale and affects to C and C++ locales.
  // wcout.imbue doesn't have any effect but the in/out streams use the proper encoding.
#ifndef NeXTBSD
#ifdef __APPLE__
  setlocale(LC_ALL, "");
  // locale("") doesn't work on mac, except with C/POSIX
#else
  locale::global(locale(""));
#endif
#else
  setlocale(LC_ALL, "");
#endif


  if(argc < 2) {
    cout << "matxin-gen-morph fst_file [tag order file]" << endl;
    exit(-1);
  }

  ifstream fin;
  fin.open (argv[1]);
  if (fin.fail()) {
    wcerr << "File unreadable `" << argv[1] << "'" << endl;
    exit(-1);
  }
  fin.close();

  // Initialisation of the transducer for morphological generation.
//  FILE *transducer = fopen(cfg.Morpho_GenFile, "r");
   
  FILE *transducer = fopen(argv[1], "r");
  fstp_generation.load(transducer);
  fclose(transducer);
  fstp_generation.initBiltrans();
/*
  transducer = fopen(cfg.Measures_GenFile, "r");
  fstp_measures.load(transducer);
  fclose(transducer);
  fstp_measures.initBiltrans();

  transducer = fopen(cfg.Tag_ToGenFile, "r");
  fstp_pre_generation.load(transducer);
  fclose(transducer);
  fstp_pre_generation.initBiltrans();

  transducer = fopen(cfg.NoLex_GenFile, "r");
  fstp_nolex_generation.load(transducer);
  fclose(transducer);
  fstp_nolex_generation.initBiltrans();
*/

  //ordena definituko duen zerbitzaria hasieratu...
//  init_generation_order(cfg.Tag_OrderFile);
  //init_generation_order(argv[2]);

  //cerr << "Fitxategi guztiak irekita." << endl;

  xmlTextReaderPtr reader;
  reader = xmlReaderForFd(0,"", NULL, 0);

  int ret = nextTag(reader);
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"corpus" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    wcout << L"<?xml version='1.0' encoding='UTF-8'?>" << endl;
    wcout << L"<corpus " << write_xml(allAttrib(reader)) << L">\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName << allAttrib(reader)
          << L"> when <corpus> was expected..." << endl;
    exit(-1);
  }

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  int i = 0;

  // corpus barruan dauden SENTENCE guztietarako
  while (ret == 1 and tagName == L"SENTENCE")
  {
    //SENTENCE irakurri eta prozesatzen du.
    wstring tree = procSENTENCE(reader);
    wcout << tree << endl;
    wcout.flush();

/*
    if (cfg.DoTrace)
    {
      ostringstream log_fileName_osoa;
      wofstream log_file;

      log_fileName_osoa << cfg.Trace_File << i++ << ".xml";

      log_file.open(log_fileName_osoa.str().c_str(), ofstream::out | ofstream::app);
      log_file << tree << L"</corpus>\n";
      log_file.close();
    }
*/
    ret = nextTag(reader);
    tagName = getTagName(reader);
    tagType = xmlTextReaderNodeType(reader);
  }
  xmlFreeTextReader(reader);
  xmlCleanupParser();

  if (ret == 1 and tagName == L"corpus" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    wcout << L"</corpus>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName << allAttrib(reader)
          << L"> when </corpus> was expected..." << endl;
    exit(-1);
  }
}

