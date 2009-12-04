#!/usr/bin/python
# -*- coding: utf-8 -*-

from subprocess import Popen
import time
import pdb
import threading
import sys
from SimpleXMLRPCServer import SimpleXMLRPCServer
from xml.parsers import expat
import re
from optparse import OptionParser
import os
import os.path as path
import signal

def write_read_fifos(txt, fifo1, fifo2):
    """Write in fifo1 and read from fifo2 avoiding risk of blocking."""
    inpipe = open(fifo1, 'w')
    outpipe = open(fifo2)
    def write():
        inpipe.write(txt)
        inpipe.close()
    thread = threading.Thread(target=write)
    thread.start()
    result = outpipe.read()
    thread.join()
    outpipe.close()
    return result
    
class FifoCommand:
    """Instantiate a matxin process with inpipe/outpipe as stdin/stdout. Ensure this process keeps running in the face of errors"""
    def __init__(self, commandstr, inpipe, outpipe):
        self.inpipe, self.outpipe = inpipe, outpipe
        self.commandstr = commandstr
        self.fifo_command = Popen(self.commandstr.split(' '))
        self.ensure_running()
    def __del__(self):
        if self.fifo_command.poll() == None:
            self.fifo_command.terminate()
        self.fifo_command.wait()
    def ensure_running(self):
        if self.fifo_command.poll() != None:
            self.fifo_command.wait()
            self.fifo_command = Popen(self.commandstr.split(' '))
        assert self.fifo_command.poll() == None
    def process(self, txt):
        self.ensure_running()
        return write_read_fifos(txt, self.inpipe, self.outpipe)

class FifoCommandList:
    def __init__(self, fifo_commands):
        self.commands = fifo_commands
    def process(self, txt):
        for p in self.commands:
            p.ensure_running()
        return write_read_fifos(txt, self.commands[0].inpipe, self.commands[-1].outpipe)        

def make_fifos(*fifos):
    for fifo in fifos:
        try:
            os.mkfifo(fifo)
        except:
            pass
CONFIGFILE = '/opt/matxin/share/matxin/config/es-eu-freeling.cfg'
def make_analyzer(configfile=CONFIGFILE, pipedir='.'):
    """Create FifoCommand instance for Analyzer."""
    inpipe, outpipe = path.join(pipedir, 'analyzer_in.fifo'), path.join(pipedir, 'analyzer_out.fifo')
    make_fifos(inpipe, outpipe)
    commandstr = 'Analyzer -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s'
    return FifoCommand(commandstr%{'configfile':configfile, 'ipipe':inpipe, 'opipe':outpipe}, inpipe, outpipe)

def make_transfer_engine(configfile=CONFIGFILE, pipedir='.'):
    """Create FifoCommandList instance for transfer engine."""
    inpipe, outpipe = path.join(pipedir, 'transfer_engine_in.fifo'), path.join(pipedir, 'transfer_engine_out.fifo')
    commandstr = 'LT -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s|ST_intra -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s|ST_inter --inter 1 -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s|ST_prep -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s|ST_inter --inter 2 -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s|ST_verb -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s|ST_inter --inter 3 -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s|SG_inter -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s|SG_intra -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s|MG -f %(configfile)s --server --ipipe %(ipipe)s --opipe %(opipe)s'
    command_strlist = commandstr.split('|')
    fifos = [inpipe] + [path.join(pipedir, 'fifo%d.fifo'%i) for i in range(1, len(command_strlist))] + [outpipe]
    make_fifos(*fifos)
    assert len(fifos) == len(command_strlist)+1
    fifo_commands = [FifoCommand(command_strlist[i]%{'configfile':configfile, 'ipipe':fifos[i], 'opipe':fifos[i+1]}, fifos[i], fifos[i+1]) for i in range(len(command_strlist))]
    return FifoCommandList(fifo_commands)

class XMLTreeToOrderedForms:
    """Parser transfer engine output and build resulting sentence. The translated sentence is accesible through result()"""
    def __init__(self):
        self._result = []
        self.p = expat.ParserCreate()
        self.p.StartElementHandler = self.start_element
        self.chunk_ord = -1
    def parse(self, uxml):
        self._result = []
        self.p.Parse(uxml, 1)
    def start_element(self, name, attrs):
        if name == 'SENTENCE':
            self._result.append({})
        if name == 'CHUNK':
            self.chunk_ord = int(attrs['ord'])
            if ord not in self._result[-1]:
                self._result[-1][self.chunk_ord] = {}
        elif name == 'NODE':
            node_ord = int(attrs['ord'])
            form = attrs['form']
            self._result[-1][self.chunk_ord][node_ord] = form
    def result(self):
        result = []
        for sentence in self._result:
            for chunk_ord, formdict in sentence.iteritems():
                for form in formdict.itervalues():
                    result.append(form)
        txt = ' '.join(result)
        txt = re.sub(' ([.,!?])', r'\1', txt)
        return txt
def xmltotxt(uxml):
    parser = XMLTreeToOrderedForms()
    parser.parse(uxml.encode('utf-8'))
    return parser.result()

translate_unicode_mutex = threading.Lock()
def translate_unicode(utxt, analyzer, translator):
    try:
        # mutex needed for the case of both servers running
        translate_unicode_mutex.acquire()
        # signal.alarm(1)
        """Helper function to translating unicode string to unicode string"""
        # analyze
        axml = analyzer.process(utxt.encode('latin1', 'ignore')).decode('latin1', 'ignore')
        # translate
        txml = translator.process(axml.encode('utf-8')).decode('utf-8', 'ignore')
        # signal.alarm(0)
        # remove xml marks
        result = xmltotxt(txml)
        return result
    except Exception, e:
        print 'Exception:', e
        return utxt
    finally:
        translate_unicode_mutex.release()
    
def run_xmlrpc_server(host, port):
    def translate(utxt):
        return translate_unicode(utxt, analyzer, transfer_engine)
    server = SimpleXMLRPCServer((host, port))
    server.register_function(translate)
    server.serve_forever()

def run_fifo_server(inpipename, outpipename):
    make_fifos(inpipename, outpipename)
    while True:
        inpipe = open(inpipename)
        outpipe = open(outpipename, 'w')
        utxt = inpipe.read().decode('utf-8')
        print utxt.encode('utf-8')
        result = translate_unicode(utxt, analyzer, transfer_engine)
        outpipe.write(result.encode('utf-8'))
        inpipe.close()
        outpipe.close()

import asyncore
import socket
from matxinasynbase import MatxinChannelBase
class MatxinServer(asyncore.dispatcher):
    def __init__(self, host, port):
        asyncore.dispatcher.__init__(self)
        self.host = host
        self.port = port
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bind((host, port))
        self.listen(10)
    def handle_accept(self):
        channel, addr = self.accept()
        MatxinChannel(channel, addr)
class MatxinChannel(MatxinChannelBase):
    def __init__(self, channel, addr):
        print MatxinChannel.__init__
        MatxinChannelBase.__init__(self, channel)
    def handle_sentence(self):
        print >>sys.stderr, 'SERVER PORT:', self.port, 'SENTENCE:', self.sentence
        print >>sys.stderr
        usentence = self.sentence
        translated = translate_unicode(usentence, analyzer, transfer_engine).encode('utf-8')
        self.write_buffer += '%d\n%s\n'%(len(translated), translated)

def run_async_server(host, port):
    print 'Asyncserver'
    server = MatxinServer(host, port)
    asyncore.loop()

def main():
    global analyzer, transfer_engine
    # parse options
    inpipe = 'in.fifo'
    outpipe = 'out.fifo'
    parser = OptionParser()
    parser.add_option('-d', '--pipedir', dest='pipedir', default='/var/run/matxinserver', help='Where to find the FIFOs')
    parser.add_option('-s', '--serverhost', dest='serverhost', default='0.0.0.0')
    parser.add_option('-p', '--port', dest='port', default=8001, type='int', help='Port for the XMLRPC server to listen.')
    parser.add_option('-a', '--aport', dest='aport', default=8002, type='int', help='Port for the async socket server to listen.')
    parser.add_option('--runxmlrpcserver', dest='runxmlrpcserver', action='store_true', default=False, help='Run XMLRPC server.')
    parser.add_option('--runfifoserver', dest='runfifoserver', action='store_true', default=False, help='Run FIFO server.')
    parser.add_option('--runasyncserver', dest='runasyncserver', action='store_true', default=False, help='Run Async socket server.')
    parser.add_option('-c', '--config', dest='config', default='/usr/local/matxin/config/es-eu.cfg', help='Matxin configuration file')
    options, args = parser.parse_args()

    if options.runfifoserver or options.runxmlrpcserver or options.runasyncserver:
        # set alarm handler
        def alarm_handler(signum, frame):
            print 'alarm'
        signal.signal(signal.SIGALRM, alarm_handler)
        # init engine
        analyzer = make_analyzer(options.config, options.pipedir)
        transfer_engine = make_transfer_engine(options.config, options.pipedir)
        servers = []
        # run requested servers
        if options.runfifoserver:
            thread = threading.Thread(target=run_fifo_server, args=(path.join(options.pipedir, inpipe), path.join(options.pipedir, outpipe)))
            thread.setDaemon(True)
            thread.start()
            servers.append(thread)
        if options.runxmlrpcserver:
            thread = threading.Thread(target=run_xmlrpc_server, args=(options.serverhost, options.port))
            thread.setDaemon(True)
            thread.start()
            servers.append(thread)
        if options.runasyncserver:
            thread = threading.Thread(target=run_async_server, args=(options.serverhost, options.aport))
            thread.setDaemon(True)
            thread.start()
            servers.append(thread)
        # wait in main thread
        try:
            while True:
                time.sleep(60)
        except:
            a = None
            t = None
if __name__ == '__main__':
    main()
