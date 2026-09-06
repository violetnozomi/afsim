# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# update_parser.py calls coco/R to generate the script parser code
# and then does some transformations to the output source

import re
import os

os.system("Coco -trace FGPSX -namespace UtScriptLanguage script.atg")

# NOTE: Update the file 'Copyright.frame' to define file header info.

# COCO/R doesn't have a way to change errors, so we replace them with python
# Errors in the form " '' expected"
operatorNames={ "dot": ".",
   "semicolon": ";",
   "lparen": "(",
   "rparen": ")",
   "assign": "=",
   "eq": "==",
   "ne": "!=",
   "ge": ">=",
   "le": "<=",
   "plus": "+",
   "minus": "-",
   "and": "&&",
   "or": "||",
   "rbrace": "}",
   "comma": ",",
   "plus_assign": "+=",
   "minus_assign": "-=",
   "times_assign": "*=",
   "div_assign": "/=",
   "r_arrow": "->"
   }
   
# Remaining errors
errorReplacement = {  "invalid trailer": "syntax error in trailing expression",
   "invalid static_exp": "syntax error",
   "this symbol not expected in stat": "unexpected symbol",
   "invalid stat": "invalid statement",
   "invalid extern_stat": "syntax error",
   "this symbol not expected in stat2": "unexpected symbol",
   "this symbol not expected in stat2": "unexpected symbol",
   "invalid stat2": "invalid statement",
   "invalid exp5": "invalid expression",
   "invalid atom": "invalid value",
   "ident expected": "identifier expected" 
   }

def fixFile(fn, outFn):
   alllines=[]
   for line in open(fn,'r').readlines():
      #  L"string" -> "string"         (and not replace "NULL" -> "NUL")
      line=re.sub('([^L])L"',r'\1"',line)
      line=line.replace("L'","'")
      line=line.replace("%ls","%s")    #for printf
      line = line.replace('wchar_t', 'cocochar_t')
      line=line.replace('"Scanner.h"', '"UtScriptLanguageScanner.hpp"')
      line=line.replace('"Parser.h"', '"UtScriptLanguageParser.hpp"')
      line=line.replace('_COCO_PARSER_H__', 'PARSER_HPP')
      line=line.replace('_COCO_SCANNER_H__', 'SCANNER_HPP')
      if 'expected' in line:
         for k,v in operatorNames.iteritems():
            line = line.replace('"' + k + ' expected', '"\\"' + v + '\\" expected')
      if 'coco_string_create' in line:
         for k,v in errorReplacement.iteritems():
            line = line.replace('coco_string_create("' + k + '");',
                  'coco_string_create("' + v + '");')
      alllines.append(line)
   text = ''.join(alllines)
   # Remove unused variables to quiet the compiler
   if fn == 'Scanner.cpp':
      text = text.replace('''bool Scanner::Comment0() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;''', '''bool Scanner::Comment0() {
	int level = 1, line0 = line;''')
   ##text = sourceHeader + text
      
   open(outFn, 'w').write(text)
   
fixFile('Parser.cpp', '..\\UtScriptLanguageParser.cpp')
fixFile('Scanner.cpp', '..\\UtScriptLanguageScanner.cpp')
fixFile('Parser.h', '..\\UtScriptLanguageParser.hpp')
fixFile('Scanner.h', '..\\UtScriptLanguageScanner.hpp')
