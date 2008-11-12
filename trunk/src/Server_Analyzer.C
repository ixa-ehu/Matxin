//////////////////////////////////////////////////////////////////
//
//    FreeLing - Open Source Language Analyzers
//
//    Copyright (C) 2004   TALP Research Center
//                         Universitat Politecnica de Catalunya
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//    contact: Lluis Padro (padro@lsi.upc.es)
//             TALP Research Center
//             despatx C6.212 - Campus Nord UPC
//             08034 Barcelona.  SPAIN
//
////////////////////////////////////////////////////////////////


//------------------------------------------------------------------//
//
//                    IMPORTANT NOTICE
//
//  This file contains a simple main program to illustrate 
//  usage of FreeLing analyzers library.
//
//  This sample main program may be used straightforwardly as 
//  a basic front-end for the analyzers (e.g. to analyze corpora)
//
//  Neverthless, if you want embed the FreeLing libraries inside
//  a larger application, or you want to deal with other 
//  input/output formats (e.g. XML), the efficient and elegant 
//  way to do so is consider this file as a mere example, and call 
//  the library from your your own main code.
//
//------------------------------------------------------------------//



using namespace std;

#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#include <map>
#include <vector>

void create_message(char *buff, string message, int size){
  for (int i=0; i<size; i++) {
    if (i<message.size()) buff[i] = message[i];
    else buff[i]=0;
  }
  buff[size]='\0';
}

void error(char *msg) {
  perror(msg);
  exit(0);
}

int main(int argc, char **argv) {
  int sockfd, servlen,n;
  struct sockaddr_un  serv_addr;
  char buff[20480];
  string text;
  
  serv_addr.sun_family = AF_UNIX;
  strcpy(serv_addr.sun_path, "/tmp/matxin_freeling_sock");
  servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

  if ((sockfd = socket(AF_UNIX, SOCK_STREAM,0)) < 0)
    error("Creating socket");

  if (connect(sockfd, (struct sockaddr *) 
	      &serv_addr, servlen) < 0)
    error("Connecting");
  
  //iosockunix iosock (sockbuf::sock_stream);
  //sunb->connect("/tmp/socket+-");
  
  while (getline(cin, text)) {
    //iosock << text;
    text += "\n";
    write(sockfd, text.c_str(), text.size());
  }
  write(sockfd, "/*END OF FILE*/", 15);

  //while (getline(iosock, text);) {
  int len = 0;
  while (len=read(sockfd, buff, 20480)) {
    buff[len] = '\0';
    cout << buff;
  }

}
