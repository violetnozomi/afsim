# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# This script generates MessageDefines_Details.hpp
maxField=32
for index in range(1,maxField):
   argList=','.join(['C'+str(i) for i in range(1,index+1)])
   nameList=','.join(['#C'+str(i) for i in range(1,index+1)])
   ind=str(index)
   print '#define DEFINE_DEBUG_MEMBERS'+ind+'('+argList+') virtual const char* GetFieldName(int i) { const char* names[] = {'+nameList+'}; return names[i]; }' 

for index in range(1,maxField):
   vars= '; '.join(['FieldType'+str(i)+' C'+str(i) for i in range(1,index+1)])
   types= ','.join(['FieldType'+str(i) for i in range(1,index+1)])
   transfer = '<<'.join(['C'+str(i) for i in range(1,index+1)])
   args= ','.join(['C'+str(i) for i in range(1,index+1)])
   ind = str(index)
   print '#define DEFINE_MEMBERS'+ind+'('+args+') \\'
   print '   ' + vars + '; \\'
   print '   static const unsigned int FieldCount = ' + ind + '; \\'
   print '   MessageSizeCheck<___RequiredMessageSize,' +types+'> __MessageSizeChecker__; \\'
   print '   template <class T> void Transfer(T& ar) {ar<<'+transfer+'; } \\'
   print '   DEFINE_DEBUG_MEMBERS'+ind+'('+args+')'
   print
