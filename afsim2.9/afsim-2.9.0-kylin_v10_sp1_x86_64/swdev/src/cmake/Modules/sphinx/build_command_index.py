#!/usr/bin/env python3
# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
from __future__ import print_function
from html.parser import HTMLParser

import glob
import sys
import os
import re
import string

class command_data:
   def __init__(self):
      self.anchor = ''
      self.formats = []

class page_data:
   def __init__(self):
      self.title = ''
      self.commands = []
      self.rule_references = []

def record_page_data(txt):
   page = page_data()
   class Parser(HTMLParser):
      def __init__(self):
         HTMLParser.__init__(self)
         self.links = []
         self.cmd = ''
         self.inCmd = False
         self.cf = command_data()
         self.title = ''
         self.inTitle = False

      def get_id(self, tag, attrs):
         if tag == 'dt':
            for attr in attrs:
               if attr[0] == 'id':
                  return attr[1]
         return None

      def handle_starttag(self, tag, attrs):
         if tag == 'h1':
            self.inTitle = True
         if tag == 'dl' and len(attrs) and attrs[0][1] in ['command', 'method']:
            self.inCmd = True
         id = self.get_id(tag, attrs)
         if self.inCmd and id and self.cf.anchor == '':
            self.cf.anchor = id
         if tag=='dd':
            self.wrap_up_command()
         if tag=='pre' and len(attrs) and attrs[0][1] == 'literal-block':
            self.inCmd = False

      def handle_data(self, data):
         if self.inCmd:
            self.cmd += data
         if self.inTitle:
            if len(data)>len(page.title):
               page.title = data

      def handle_entityref(self, name):
         if self.inCmd:
            if name == 'lt':
               self.cmd += '<'
            if name == 'gt':
               self.cmd += '>'
            if name == 'nbsp':
               self.cmd += ' '

      def wrap_up_command(self):
         if self.inCmd:
            self.inCmd = False
            page.commands.append(self.cf)
            self.cf = command_data()

      def handle_endtag(self, tag):
         self.inTitle = False
         if tag == 'dt':
            if self.inCmd:
               self.cf.formats.append(self.cmd)
               self.cmd = ''

   p = Parser()
   p.feed(txt)
   grammarRefs = re.finditer('<div id="GRAMMAR:([^"]*)">', txt)
   for match in grammarRefs:
      ruleName = match.group(1)
      page.rule_references.append(ruleName)
   for cmd in page.commands:
      cmd.formats = [fmt.strip() for fmt in cmd.formats]
   return page

def main():
   if len(sys.argv) < 3:
      print('usage:  build_command_index.py <documentation-directory> <output-directory>')
      return
   docDir = sys.argv[1]
   outDir = sys.argv[2]
   files = glob.glob(docDir + '/docs/*.html')
   if len(files) == 0:
      print('ERROR: found no pages!')
      return
   pageData = {}
   for f in files:
      try:
         with open(f, 'r', encoding='utf-8') as cur_file:
            data = record_page_data(cur_file.read())
            data.page_name = os.path.splitext(os.path.basename(f))[0]
            pageData[os.path.relpath(f, docDir).replace("\\", "/")] = data
      except HTMLParseError:
         pass
   with open(outDir + '/command_index.idx', 'w', encoding='utf-8') as idxf:
      for link, data in pageData.items():
         print(link, file=idxf)
         print(' page_name',data.page_name, file=idxf)
         print(' title', data.title, file=idxf)
         for r in data.rule_references:
            print(' rule_reference',r, file=idxf)
         for c in data.commands:
            print(' cmd_anchor',c.anchor, file=idxf)
            for fmt in c.formats:
               for line in fmt.split('\n'):
                  txt = line.strip().replace("\u00B6", '').replace("\u00C2", '')
                  if len(txt):
                     print(' cmd_format',txt, file=idxf)

if __name__ == "__main__":
   main()
