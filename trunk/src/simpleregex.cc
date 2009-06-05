#include "simpleregex.h"

 
Reg_Ex::Reg_Ex(const char * regex, int options)
{
  const char * error;
  int          erroffset;
  
  re = pcre_compile(regex, options, &error, &erroffset, NULL);
  if (re == NULL)
    throw error;
  pe = pcre_study(re, 0, &error);
  pcre_fullinfo(re, pe, PCRE_INFO_CAPTURECOUNT, &substrcount);
  substrcount++;
  ovector = new int[3*substrcount];
  matchlist = NULL;
}

Reg_Ex::~Reg_Ex()
{
  ClearMatchList();
  delete ovector;
  if (pe)
    pcre_free(pe);
  pcre_free(re);
}

bool Reg_Ex::Search(const char * subject, int len, int options)
{
  ClearMatchList();
  return pcre_exec(re, pe, lastsubject = subject, slen = (len >= 0) ? len : strlen(subject), 0, options, ovector, 3*substrcount) > 0;
}
  
bool Reg_Ex::SearchAgain(int options)
{
  ClearMatchList();
  return pcre_exec(re, pe, lastsubject, slen, ovector[1], options, ovector, 3*substrcount) > 0;
}
 
const char * Reg_Ex::Match(int i)
{
  if (i < 0)
    return lastsubject;
  if (matchlist == NULL)
    pcre_get_substring_list(lastsubject, ovector, substrcount, &matchlist);
  return matchlist[i];
}
                                                                                                            
inline void Reg_Ex::ClearMatchList(void)
{
  if (matchlist)
    pcre_free_substring_list(matchlist),
      matchlist = NULL;
}
