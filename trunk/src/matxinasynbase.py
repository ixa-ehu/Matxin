#!/usr/bin/python

import asyncore
import socket
import re

class MatxinChannelBase(asyncore.dispatcher):
    def __init__(self, *args):
        asyncore.dispatcher.__init__(self, *args)
        self.read_buffer = ''
        self.write_buffer = ''
    def handle_close(self):
        print self.handle_close
        self.close()
    def writable(self):
        return self.write_buffer != ''
    def handle_write(self):
        sent_bytes = self.send(self.write_buffer)
        self.write_buffer = self.write_buffer[sent_bytes:]
    def handle_sentence(self):
        pass
    def handle_read(self):
        # read all available data
        data = self.recv(1024)
        self.read_buffer += data
        # process all available sentences
        sentence = self.read_sentence()
        while sentence != None:
            self.sentence = sentence.decode('utf-8')
            self.handle_sentence()
            sentence = self.read_sentence()
    def find_header(self):
        "Consume characters until finding a possible header"
        m = re.search(r'^\d+$', self.read_buffer, re.MULTILINE)
        if m:
            begin = m.start()
            self.read_buffer = self.read_buffer[begin:]
    def read_sentence(self):
        """read and return a sentence.
        format: header\ndata\n
        header: \d+
        data: .*
        """
        if self.read_buffer == '':
            return None
        self.find_header()
        data_begin = self.read_buffer.find('\n')
        if data_begin != -1:
            header, data = self.read_buffer[:data_begin], self.read_buffer[data_begin+1:]
            data_length = int(header)
            if data_length <= len(data):
                sentence, self.read_buffer = data[:data_length], data[data_length+1:]
                return sentence

