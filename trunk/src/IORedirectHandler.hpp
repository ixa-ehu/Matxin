#ifndef IOREDIRECTHANDLER_HPP
#define IOREDIRECTHANDLER_HPP

#include <iostream>
#include <fstream>
#include "config.h"

/*
  If --server given, then
      Redirect cin and cout to the input and output pipes given as parametres --ipipe --opipe. 
  else
      do nothing

  Redirection is done by the constructor. The destructor undoes the redirection.

  serverOK() == true -> --server was given and pipes where successfuly opened.
*/
class CppIORedirectHandler
{
  std::streambuf *cin_buffer;
  std::streambuf *cout_buffer;
  std::ifstream *ifs;
  std::ofstream *ofs;
  bool ok;
public:
  CppIORedirectHandler(const config &cfg);
  ~CppIORedirectHandler();
  bool serverOK();
};

class CppWIORedirectHandler
{
  std::wstreambuf *cin_buffer;
  std::wstreambuf *cout_buffer;
  std::wifstream *ifs;
  std::wofstream *ofs;
  bool ok;
public:
  CppWIORedirectHandler(const config &cfg);
  ~CppWIORedirectHandler();
  bool serverOK();
};

class Fd0WcoutRedirectHandler
{
  int original_stdin;
  std::wstreambuf *cout_buffer;
  std::wofstream *ofs;
  bool ok;
public:
  Fd0WcoutRedirectHandler(const config &cfg);
  ~Fd0WcoutRedirectHandler();
  bool serverOK();
};

#endif
