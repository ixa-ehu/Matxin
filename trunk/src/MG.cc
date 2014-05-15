#include <lttoolbox/fst_processor.h>
#include <lttoolbox/ltstr.h>
#include <locale>

#include <string>
#include <iostream>

#include "config.h"
#include "matxin_string_utils.h"
#include "IORedirectHandler.hpp"

#include <data_manager.h>
#include <XML_reader.h>

using namespace std;

FSTProcessor fstp_generation;
FSTProcessor fstp_measures;
FSTProcessor fstp_pre_generation;
FSTProcessor fstp_nolex_generation;


// Hiztegi elebidunaren euskarazko ordainetik lehenengoa lortzen du.
// IN:  Euskarazko ordainak ( ordain1[/ordain2]* )
// OUT: lehenengoa          ( oradin1            )
wstring disanbiguate(wstring &full)
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
//    if (UpCase == L"title" && form[i - 1] == L' ')
    if (UpCase == L"all" || UpCase == L"title" && form[i - 1] == L'_')
      out_form[i] = toupper(out_form[i]);
  }

  return out_form;
}


wstring verb_generation(wstring lemma, wstring pos, wstring suf, wstring cas,
                        wstring mi, wstring head_sem, bool &flexioned, config cfg)
{
  wstring analisys, form, prefix, postposizio, pre_lemma, lemma_osoa;
  lemma_osoa = lemma;

  if (cfg.DoGenTrace)
    wcerr << lemma << L" " << pos << L" " << mi << L" " << cas << endl;

  for (size_t i = 0; i < lemma.size(); i++)
  {
    if (lemma[i] == L'_')
      lemma[i] = L' ';
  }

  if (mi.find(L"++") != wstring::npos)
  {
    postposizio = L" " + mi.substr(mi.find(L"++") + 2, mi.size());
    mi = mi.substr(0, mi.find(L"++"));
    cas = L"";
    flexioned = true;
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

  if (cas.find(L"+") == 0) {
    size_t position = mi.find(L"+n[ERL][MEN]");
    if (position != wstring::npos) {
      mi = mi.substr(0,position);
    }

    mi = mi + cas;
    cas = L"";
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
  if (cfg.DoGenTrace)
    wcerr << pre_gen << endl;
  wstring lemmaMorf = fstp_pre_generation.biltrans(pre_gen);
  if (cfg.DoGenTrace)
    wcerr << lemmaMorf << endl;

  if (lemmaMorf[0] == L'^' and lemmaMorf[1] == L'@')
    lemmaMorf = lemma + pos + mi;
  else
    lemmaMorf = lemmaMorf.substr(1, lemmaMorf.size() - 2) + mi;

  for (size_t i = 0; i < int(lemmaMorf.size()); i++)
  {
    if (lemmaMorf[i] == L'\\')
    {
      lemmaMorf.erase(i, 1);
      i--;
    }
  }

  lemmaMorf = prefix + lemmaMorf;

  if (cfg.DoGenTrace)
    wcerr << lemmaMorf << endl;


  analisys = L"^" + get_generation_order(prefix, lemmaMorf, mi, cas, head_sem) + L"$";

  if (cfg.DoGenTrace)
    wcerr << analisys << endl;

  if (analisys.find(L"LemaMorf") != wstring::npos)
    analisys.replace(analisys.find(L"LemaMorf"), 8, lemmaMorf);
  if (analisys.find(L"Lema") != wstring::npos)
    analisys.replace(analisys.find(L"Lema"), 4, lemma);
  if (analisys.find(L"Num") != wstring::npos)
    analisys.replace(analisys.find(L"Num"), 3, mi);
  if (analisys.find(L"Kas") != wstring::npos)
  {
    analisys.replace(analisys.find(L"Kas"), 3, cas);
    flexioned = true;
  }

  for (int i = 0; i < int(analisys.size()); i++)
  {
    if (analisys[i] == L'[')
      analisys[i]= L'<';
    if (analisys[i] == L']')
      analisys[i]= L'>';
    if (analisys[i] == L' ' )
    {
      analisys.erase(i, 1);
      i--;
    }
  }

  if (cfg.DoGenTrace)
    wcerr << L"GEN:" << analisys << endl;
  form = fstp_generation.biltrans(analisys);
  if (cfg.DoGenTrace)
    wcerr << L"GEN:" << form << endl;

  form = form.substr(1, form.size() - 2);

  if (form.size() == 0 or form[0] == '@' or
      form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
  {
    if (cfg.DoGenTrace)
      wcerr << L"GEN nolex:" << analisys << endl;
    form = fstp_nolex_generation.biltrans(analisys);
    form = form.substr(1, form.size() - 2);
    if (cfg.DoGenTrace)
      wcerr << L"GEN nolex:" << form << endl << endl;
    if (form.size() == 0 or form[0] == L'@' or
        form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
    {
      form = lemma;
    }
  }

  form = disanbiguate(form);
  form = pre_lemma + form;
  form += postposizio;

  if (cfg.DoGenTrace)
    wcerr << form << endl << endl;

  return form;
}


wstring number_generation(wstring lemma, wstring pos, wstring suf, wstring cas,
                          wstring mi, wstring head_sem, bool &flexioned, config cfg)
{
  wstring analisys, form, prefix, postposizio, pre_lemma, lemma_osoa;

  if (cfg.DoGenTrace)
    wcerr << lemma << L" " << pos << L" " << mi << L" " << cas << endl;

  if (pos == L"[Zu]" || pos == L"[Zm]")
  {
    wstring dimension = lemma.substr(0, lemma.find(L"_"));
    lemma = lemma.substr(lemma.find(L"_") + 1);
    wstring measure = lemma.substr(0, lemma.find(L":"));
    wstring number = lemma.substr(lemma.find(L":") + 1);
    lemma = number + measure;
  }

  if (pos == L"[Zp]")
  {
    wstring number = lemma.substr(0, lemma.find(L"/"));
    wstring ratio = lemma.substr(lemma.find(L"/") + 1);
    lemma = ratio + L"/" + number;
  }

  if (mi == L"")
    mi = L"[MG]";

  if (cas.find(L"++") != wstring::npos)
  {
    postposizio = L" " + cas.substr(cas.find(L"++") + 2, cas.size());
    cas = cas.substr(0, cas.find(L"++"));
  }

  if (cas != L"")
  {
    wstring lemmaMorf = lemma + pos;
    if (cfg.DoGenTrace)
      wcerr << prefix << L" " << lemmaMorf << L" " << mi << L" " << cas << head_sem << endl;

    analisys = L"^" + get_generation_order(prefix, lemmaMorf, mi, cas, head_sem) + L"$";

    if (cfg.DoGenTrace) wcerr << analisys << endl;

    if (analisys.find(L"LemaMorf") != wstring::npos)
      analisys.replace(analisys.find(L"LemaMorf"), 8, lemmaMorf);
    if (analisys.find(L"Lema") != wstring::npos)
      analisys.replace(analisys.find(L"Lema"), 4, lemma);
    if (analisys.find(L"Num") != wstring::npos)
      analisys.replace(analisys.find(L"Num"), 3, mi);
    if (analisys.find(L"Kas") != wstring::npos)
      analisys.replace(analisys.find(L"Kas"), 3, cas);

    flexioned = true;
  }
  else
  {
    analisys = L"^" + lemma + pos + L"$";
    postposizio = L"";
    flexioned = false;
  }

  for (int i = 0; i < int(analisys.size()); i++)
  {
    if (analisys[i] == L'[')
      analisys[i]= L'<';
    if (analisys[i] == L']')
      analisys[i]= L'>';
    if (analisys[i] == L' ')
    {
      analisys.erase(i, 1);
      i--;
    }
  }

  if (cfg.DoGenTrace)
    wcerr << L"GEN:" << analisys << endl;
  form = fstp_measures.biltrans(analisys);
  if (cfg.DoGenTrace)
    wcerr << L"GEN:" << form << endl;

  form = form.substr(1, form.size() - 2);

  if (form.size() == 0 or form[0] == L'@' or
      form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
  {
    form = lemma;
  }

  form = disanbiguate(form);

  form = form + postposizio;

  for (size_t i = 0; i < form.size(); i++)
  {
    if (form[i] == L'_')
      form[i]= L' ';
  }

  if (cfg.DoGenTrace)
    wcerr << form << endl << endl;

  return form;
}


wstring date_generation(wstring lemma, wstring pos, wstring suf, wstring cas,
                        wstring mi, wstring head_sem, bool &flexioned, config cfg)
{
  wstring analisys, form, prefix, postposizio, pre_lemma, lemma_osoa;
  bool is_last = true;

  if (cfg.DoGenTrace)
    wcerr << lemma << L" " << pos << L" " << mi << L" " << cas << endl;

  wstring century, week_day, day, month, year, hour, meridiam;
  wstring century_cas, week_day_cas, day_cas, month_cas, year_cas, hour_cas;

  lemma = lemma.substr(1, lemma.size() - 2);

  if (lemma.substr(0, 2) == L"s:" || lemma.substr(0, 2) == L"S:")
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

  if (century != L"??" && is_last)
  {
    century_cas = cas;
    is_last=false;
  }
  else
  {
    century_cas = L"[INE]";
  }

  if (day != L"??" && is_last)
  {
    day_cas = cas;
    is_last=false;
  }
  else
  {
    day_cas = L"[INE]";
  }

  if (month != L"??" && is_last)
  {
    month_cas = cas;
    is_last=false;
  }
  else
  {
    month_cas = L"[GEN]";
  }

  if (year != L"??" && is_last)
  {
    year_cas = cas;
    is_last=false;
  }
  else
  {
    year_cas = L"[GEL]";
  }

  if (week_day != L"??" && is_last)
  {
    week_day_cas = cas;
    is_last=false;
  }
  else
  {
    week_day_cas = L"[ABS]";
  }

  if (hour != L"??.??" && is_last)
  {
    hour_cas = cas;
    is_last=false;
  }
  else
  {
    hour_cas = L"[INE]";
  }



  if (century != L"??")
    form = number_generation(century, L"[CN]", suf, century_cas, L"[NUMS]",
                             head_sem, flexioned, cfg);

  if (year != L"??")
    form = number_generation(year, L"[Z]", suf, year_cas, L"[NUMS]", head_sem,
                             flexioned, cfg);

  if (month != L"??")
  {
    if (form != L"")
      form += L" ";
    form += number_generation(month, L"[MM]", suf, month_cas, L"[NUMS]",
                              head_sem, flexioned, cfg);
  }

  if (day != L"??")
  {
    if (form != L"")
      form += L" ";
    form += number_generation(day, L"[Z]", suf, day_cas, L"[NUMS]", head_sem,
                              flexioned, cfg);
  }

  if (week_day != L"??")
  {
    if (form != L"")
      form += L", ";
    form += number_generation(week_day, L"[WD]", suf, week_day_cas, L"[NUMS]",
                              head_sem, flexioned, cfg);
  }

  if (hour != L"??.??")
  {
    if (form != L"")
      form += L", ";
    form += number_generation(hour, L"[Z]", suf, hour_cas, L"[MG]", head_sem,
                              flexioned, cfg);
    if (meridiam != L"??")
      form += L" (" + meridiam + L")";
  }

  flexioned = true;

  return form;
}


wstring generation(wstring lemma, wstring pos, wstring suf, wstring cas,
                   wstring mi, wstring head_sem, bool &flexioned,
                   config cfg)
{
  wstring analisys, form, prefix, postposizio, pre_lemma, lemma_osoa;
  lemma_osoa = lemma;

  if (cfg.DoGenTrace)
    wcerr << lemma << L" " << pos << L" " << suf << L" " << mi << L" "
          << cas << L" " << head_sem << endl;

  for (size_t i = 0; i < lemma.size(); i++)
  {
    if (lemma[i] == L'_')
      lemma[i]= L' ';
  }

  if (suf == L"" && cas == L"")
    return lemma;

  if (mi == L"")
    mi = L"[NUMS]";

  flexioned = true;
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

  wstring pre_gen = L"^" + lemma_osoa + pos + L"$";
  if (cfg.DoGenTrace)
    wcerr << pre_gen << endl;
  wstring lemmaMorf = fstp_pre_generation.biltrans(pre_gen);
  if (cfg.DoGenTrace)
    wcerr << lemmaMorf << endl;
  if (lemmaMorf[0] == L'^' and lemmaMorf[1] == L'@')
    lemmaMorf = lemma + pos;
  else
    lemmaMorf = lemmaMorf.substr(1, lemmaMorf.size() - 2);

  if (suf != L"")
    lemmaMorf += suf;

  for (size_t i = 0; i < lemmaMorf.size(); i++)
  {
    if (lemmaMorf[i] == L'\\')
    {
      lemmaMorf.erase(i, 1);
      i--;
    }
  }

  if (cfg.DoGenTrace)
    wcerr << lemmaMorf << L" " << mi << L" " << cas << L" " << head_sem << endl;

  analisys = L"^" + get_generation_order(prefix, lemmaMorf, mi, cas, head_sem) + L"$";

  if (cfg.DoGenTrace)
    wcerr << analisys << endl;

  if (analisys.find(L"LemaMorf") != wstring::npos)
    analisys.replace(analisys.find(L"LemaMorf"), 8, lemmaMorf);
  if (analisys.find(L"Lema") != wstring::npos)
    analisys.replace(analisys.find(L"Lema"), 4, lemma);
  if (analisys.find(L"Num") != wstring::npos)
    analisys.replace(analisys.find(L"Num"), 3, mi);
  if (analisys.find(L"Kas") != wstring::npos)
    analisys.replace(analisys.find(L"Kas"), 3, cas);

  for (int i = 0; i < int(analisys.size()); i++)
  {
    if (analisys[i] == L'[')
      analisys[i]= L'<';
    if (analisys[i] == L']')
      analisys[i]= L'>';
    if (analisys[i] == L' ' )
    {
      analisys.erase(i, 1);
      i--;
    }
  }

  if (cfg.DoGenTrace)
    wcerr << L"GEN:" << analisys << endl;
  form = fstp_generation.biltrans(analisys);
  if (cfg.DoGenTrace)
    wcerr << L"GEN:" << form << endl;

  form = form.substr(1, form.size() - 2);

  if (form.size() == 0 or form[0] == L'@' or
      form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
  {
    if (cfg.DoGenTrace)
      wcerr << L"GEN nolex:" << analisys << endl;
    form = fstp_nolex_generation.biltrans(analisys);
    form = form.substr(1, form.size() - 2);
    if (cfg.DoGenTrace)
      wcerr << L"GEN nolex:" << form << endl;
    if (form.size() == 0 or form[0] == L'@' or
        form.find(L"<") != wstring::npos or form.find(L">") != wstring::npos)
    {
      form = lemma;
    }
  }

  form = disanbiguate(form);
  form = pre_lemma + form + postposizio;

  if (cfg.DoGenTrace)
    wcerr << form << endl << endl;

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
    wcerr << L"ERROR: invalid tag: <" << tagName
          << L"> when <SYN> was expected..." << endl;
    exit(-1);
  }

  if (tagName == L"SYN" and tagType == XML_READER_TYPE_END_ELEMENT)
  {
    syns += L"</SYN>\n";
  }
  else
  {
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </SYN> was expected..." << endl;
    exit(-1);
  }

  return syns;
}

wstring procNODE(xmlTextReaderPtr reader, wstring chunk_type, config cfg)
{
  wstring nodes;
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);

  if (tagName == L"NODE" and tagType != XML_READER_TYPE_END_ELEMENT)
  {
    wstring lem = attrib(reader, "lem");
    wstring pos = attrib(reader, "pos");
    wstring suf = attrib(reader, "suf");
    wstring cas = attrib(reader, "cas");
    wstring mi = attrib(reader, "mi");
    wstring head_sem = attrib(reader, "headsem");

    wstring form;
    bool flexioned = false;

    lem = keep_case(lem, attrib(reader, "UpCase"));

    if (chunk_type.substr(0, 4) == L"adi-")
    {
      form = verb_generation(lem, pos, suf, cas, mi, head_sem,
			     flexioned, cfg);
    }
    else if (pos == L"[W]")
    {
      form = date_generation(lem, pos, suf, cas, mi, head_sem, 
			     flexioned, cfg);
    }
    else if (pos == L"[Z]" || pos == L"[Zu]" || pos == L"[Zm]" 
	     || pos == L"[Zp]" || pos == L"[Zd]")
    {
      form = number_generation(lem, pos, suf, cas, mi, head_sem, 
			       flexioned, cfg);
    }
    else
    {
      form = generation(lem, pos, suf, cas, mi, head_sem, 
			flexioned, cfg);
    }

    for (size_t i = 0; i<form.size(); i++)
      if (form[i] == L'_')
        form[i] = L' ';

//    form = keep_case(form, attrib(reader, "UpCase"));

    nodes += L"<NODE form='" + write_xml(form) + L"'" + write_xml(allAttrib(reader));

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
    wcerr << L"ERROR: invalid tag: <"<< tagName
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
    nodes += procNODE(reader, chunk_type, cfg);

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
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </NODE> was expected..." << endl;
    exit(-1);
  }

  return nodes;
}


wstring procCHUNK(xmlTextReaderPtr reader, config cfg)
{
  wstring tagName = getTagName(reader);
  int tagType = xmlTextReaderNodeType(reader);
  wstring tree, type;

  if (tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    type = attrib(reader, "type");
    tree = L"<CHUNK" + write_xml(allAttrib(reader)) + L">\n";
  }
  else
  {
    wcerr << L"ERROR: invalid tag: <" << tagName
          << L"> when <CHUNK> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  tree += procNODE(reader, type, cfg);

  ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);
  while (ret == 1 and tagName == L"CHUNK" and tagType == XML_READER_TYPE_ELEMENT)
  {
    tree += procCHUNK(reader, cfg);

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
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </CHUNK> was expected..." << endl;
    exit(-1);
  }

  return tree;
}


wstring procSENTENCE (xmlTextReaderPtr reader, config cfg)
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
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when <SENTENCE> was expected..." << endl;
    exit(-1);
  }

  int ret = nextTag(reader);
  tagName = getTagName(reader);
  tagType = xmlTextReaderNodeType(reader);

  while (ret == 1 and tagName == L"CHUNK")
  {
    tree += procCHUNK(reader, cfg);

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
    wcerr << L"ERROR: invalid document: found <" << tagName
          << L"> when </SENTENCE> was expected..." << endl;
    exit(-1);
  }

  return tree;
}


int main(int argc, char *argv[])
{
  config cfg(argc, argv);

  // Set locale information
  locale::global(locale(""));
  
  // Initialization of the transducer for morphological generation.
  FILE *transducer = fopen(cfg.Morpho_GenFile.c_str(), "r");
  fstp_generation.load(transducer);
  fclose(transducer);
  fstp_generation.initBiltrans();

  transducer = fopen(cfg.Measures_GenFile.c_str(), "r");
  fstp_measures.load(transducer);
  fclose(transducer);
  fstp_measures.initBiltrans();

  transducer = fopen(cfg.NoLex_GenFile.c_str(), "r");
  fstp_nolex_generation.load(transducer);
  fclose(transducer);
  fstp_nolex_generation.initBiltrans();

  transducer = fopen(cfg.Tag_ToGenFile.c_str(), "r");
  fstp_pre_generation.load(transducer);
  fclose(transducer);
  fstp_pre_generation.initBiltrans();

  //ordena definituko duen zerbitzaria hasieratu...
  init_generation_order(cfg.Tag_OrderFile);

  //cerr << "Fitxategi guztiak irekita." << endl;

  while (true)
  {
    // redirect io
    Fd0WcoutRedirectHandler ioredirect(cfg);
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
      wcerr << L"ERROR: invalid document: found <" << tagName
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
      wstring tree = procSENTENCE(reader, cfg);
      wcout << tree << endl;
      wcout.flush();
  
      if (cfg.DoTrace)
      {
        ostringstream log_fileName_osoa;
        wofstream log_file;
  
	log_fileName_osoa.imbue(std::locale("C"));
        log_fileName_osoa << cfg.Trace_File << i++ << ".xml";
  
        log_file.open(log_fileName_osoa.str().c_str(), ofstream::out | ofstream::app);
	log_file << L"<!-- Morphological generation (final tree) -->" << endl;
        log_file << tree << L"</corpus>\n";
        log_file.close();
      }
  
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
      wcerr << L"ERROR: invalid document: found <" << tagName
            << L"> when </corpus> was expected..." << endl;
      exit(-1);
    }
    if (!ioredirect.serverOK())
      break;
  }
}
