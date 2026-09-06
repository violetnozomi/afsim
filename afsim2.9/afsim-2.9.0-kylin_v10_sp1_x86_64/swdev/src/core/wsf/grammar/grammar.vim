" ****************************************************************************
" CUI
"
" The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
"
" The use, dissemination or disclosure of data in this file is subject to
" limitation or restriction. See accompanying README and LICENSE for details.
" ****************************************************************************

" 
" VIM syntax file for WSF Grammar (.ag files)
"   To activate, put this file in vimfiles/syntax, and add this to vimrc:
"        au BufRead,BufNewFile *.ag se ft=grammar

se isk=@,45,48-57,_,192-255,-
syntax clear
syn sync fromstart
syntax region grComment  start=+#+  end="$" 

syn region expr matchgroup=Tag start="(\(rule_ref\|nocase\|values\|struct\|value\|constructor\|function\|rule\|load\|new_replace\|new\|delete\|subtype\|error\|symbol\|var\|basic\|delimited\|output-file-reference\|file-reference\|string-except\|typename\|typeref\|type\|name\|enumeration\|enum-rule\|script-var\|initially\|aux-[^ ]*\)" end=")" transparent contains=sequence,expr,grComment,label,bad_expr,rule_ref,quotedString
syn region bad_expr matchgroup=Error start="(\(rule_ref\|nocase\|values\|struct\|value\|constructor\|function\|rule\|load\|new_replace\|new\|delete\|subtype\|error\|symbol\|var\|basic\|delimited\|output-file-reference\|file-reference\|string-except\|typename\|typeref\|type\|name\|enumeration\|enum-rule\|script-var\|initially\|aux-[^ ]*\)\@!" end=")"

syn region sequence matchgroup=Operator start="{" end="}" transparent contains=expr,rule_attr_ref,rule_ref,ruleOp,grComment,sequence,proxyAction,quotedString,nonQuotedString contained
syn match rule_ref /<[^>:]*:\?[^>]*>/ contained
syn match ruleOp /[*?+\|]/ contained
syn match label /:[^ ]*/ contained
syn match proxyAction /\[[^<>]*\]/ contained
syn match quotedString /"[^"]*"/ contained
syn match nonQuotedString /[a-zA-Z_]\+/ contained

syn match rule_ref /[<][^>]+[>]/

hi link grComment Comment
hi link seqOp Comment
hi link ruleOp Keyword
hi link exprOp Keyword
hi link rule_ref Label
hi link rule_attr_ref Macro
hi link proxyAction Macro
hi link quotedString String
hi link nonQuotedString String
let b:current_syntax = "grammar"
