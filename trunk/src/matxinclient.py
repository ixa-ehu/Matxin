#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import xmlrpclib

def main():
    server = xmlrpclib.ServerProxy('http://localhost:8001')
    for line in sys.stdin:
        utxt = line.strip().decode('utf-8')
        print server.translate(utxt)
if __name__ == '__main__':
    main()
