#include "IORedirectHandler.hpp"

CppIORedirectHandler::CppIORedirectHandler(const config &cfg):cin_buffer(NULL), cout_buffer(NULL), ifs(NULL), ofs(NULL), ok(cfg.server)
{
  if (ok)
  {
    ifs = new std::ifstream(cfg.InPipe.c_str());
    if (ifs->fail())
    {
      std::cerr << "Cannot open in pipe (" << cfg.InPipe << "). Reverting to Stdio." << std::endl;
      ok = false;
    }
    ofs = new std::ofstream(cfg.OutPipe.c_str());
    if (ofs->fail())
    {
      std::cerr << "Cannot open out pipe (" << cfg.OutPipe << "). Reverting to Stdio." << std::endl;
      ok = false;
    }

    if (ok)
    {
      cin_buffer = std::cin.rdbuf(ifs->rdbuf());
      cout_buffer = std::cout.rdbuf(ofs->rdbuf());
    }
  }
}
CppIORedirectHandler::~CppIORedirectHandler()
{
  if (cin_buffer)
    std::cin.rdbuf(cin_buffer);      
  if (cout_buffer)
    std::cout.rdbuf(cout_buffer);      
  delete ifs;
  delete ofs;
}
bool CppIORedirectHandler::serverOK()
{
  return ok;
}

CppWIORedirectHandler::CppWIORedirectHandler(const config &cfg):cin_buffer(NULL), cout_buffer(NULL), ifs(NULL), ofs(NULL), ok(cfg.server)
{
  if (ok)
  {
    ifs = new std::wifstream(cfg.InPipe.c_str());
    if (ifs->fail())
    {
      std::cerr << "Cannot open in pipe (" << cfg.InPipe << "). Reverting to Stdio." << std::endl;
      ok = false;
    }
    ofs = new std::wofstream(cfg.OutPipe.c_str());
    if (ofs->fail())
    {
      std::cerr << "Cannot open out pipe (" << cfg.OutPipe << "). Reverting to Stdio." << std::endl;
      ok = false;
    }

    if (ok)
    {
      cin_buffer = std::wcin.rdbuf(ifs->rdbuf());
      cout_buffer = std::wcout.rdbuf(ofs->rdbuf());
    }
  }
}
CppWIORedirectHandler::~CppWIORedirectHandler()
{
  if (cin_buffer)
    std::wcin.rdbuf(cin_buffer);      
  if (cout_buffer)
    std::wcout.rdbuf(cout_buffer);      
  delete ifs;
  delete ofs;
}
bool CppWIORedirectHandler::serverOK()
{
  return ok;
}

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

Fd0WcoutRedirectHandler::Fd0WcoutRedirectHandler(const config &cfg):original_stdin(-1), cout_buffer(NULL), ofs(NULL), ok(cfg.server)
{
  int pipe_fd(0);
  if (ok)
  {
    // open input and output pipes
    // open pipe in stdin file descriptor
    pipe_fd = open(cfg.InPipe.c_str(), 0);
    if (-1 == pipe_fd)
    {
      std::cerr << "Cannot open in pipe (" << cfg.InPipe << "). Reverting to Stdio." << std::endl;
      ok = false;
    }
    ofs = new std::wofstream(cfg.OutPipe.c_str());
    if (ofs->fail())
    {
      std::cerr << "Cannot open out pipe (" << cfg.OutPipe << "). Reverting to Stdio." << std::endl;
      ok = false;
    }

    if (ok)
    {
      // duplicate stdin
      original_stdin = dup(0);
      if (original_stdin == -1)
      {
        perror("dup(0)");
        exit(1);
      }
      // close original stdin fd
      if (close(0))
      {
        perror("close(0)");
        exit(1);
      }
      // dup in pipe as stdin
      if (0 != dup(pipe_fd))
      {
        perror("dup(pipe_fd)");
        exit(1);
      }
      // close pipe's original fd
      if (0 != close(pipe_fd))
      {
        perror("close(pipe_fd)");
        exit(1);
      }
      // redirect wcout
      cout_buffer = std::wcout.rdbuf(ofs->rdbuf());
    }
  }
}
Fd0WcoutRedirectHandler::~Fd0WcoutRedirectHandler()
{
  // restore wcout
  if (cout_buffer)
    std::wcout.rdbuf(cout_buffer);
  delete ofs;
  if (original_stdin != -1)
  {
    // close pipe
    if (-1 == close(0))
    {
      perror("~close(0)");
      exit(1);
    }
    // restore stdin
    if (0 != dup(original_stdin))
    {
      perror("~dup(original_stdin)");
      exit(1);
    }
    if (-1 == close(original_stdin))
    {
      perror("~close(original_stdin)");
      exit(1);
    }
  }
}
bool Fd0WcoutRedirectHandler::serverOK()
{
  return ok;
}
