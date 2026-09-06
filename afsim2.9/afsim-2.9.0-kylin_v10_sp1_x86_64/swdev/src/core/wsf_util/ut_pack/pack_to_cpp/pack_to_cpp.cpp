// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

//////////////////////////////////////////////////////////////////////////
// pack_to_cpp
//
// Given a UtPack schema file(s), generates c++ code for handling the
// defined types.
//
// Usage:
//    pack_to_cpp -h path/to/my_schema.utml > my_classes.hpp
//    pack_to_cpp -c path/to/my_schema.utml > my_classes_registration.hpp
//    pack_to_cpp -res cRESOURCE_DATA path/to/resource.dat my_resource_data.hpp

// '-h' and '-c' options:
//    Two header files are required.  the '-h' file defines classes
//       representing the 'struct' types in the schema.  This is used
//       by an application to pack data into messages.
//    The '-c' file must be used to register these struct classes with
//       a UtPackSerializer before messages may be sent or received.
//
// '-res'  <C-Variable-Name> <resource-file> <output-header-file>
//    Given any file, creates a C header file containing a byte array with
//      the contents of the file.
//
// multiple files:
//    Multiple schema files may be specified on the command line.
//       In this case, code is generated for the first file only,
//       but the code will depend on code generated from the other files
//       too.  Use case:
//          common_classes.utml -- no dependencies
//          special_classes.utml -- depends on common_classes.utml
//          pack_to_cpp -h common_classes.utml > common.h
//          pack_to_cpp -h special_classes.utml common_classes.utml > special.h
//          my_code.cpp:
//             #include "common.h"
//             #include "special.h"
//////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>

#include "UtPackSchema.hpp"
#include "UtTextDocument.hpp"
#include "Utml.hpp"

static std::string GetCPP_Name(UtPackType& aType)
{
   std::string cppName   = aType.GetTypePath();
   auto        basicType = dynamic_cast<UtPackBasicType*>(&aType);
   if (basicType != nullptr)
   {
      return basicType->GetCPP_Type();
   }
   for (char& i : cppName)
   {
      if (i == '.')
      {
         i = '_';
      }
   }
   return cppName;
}

static std::string GetMemberTypeName(UtPackType& aType)
{
   if (aType.IsEnum())
   {
      UtPackEnumType& et = (UtPackEnumType&)aType;
      return et.GetCPP_Type();
   }
   else
   {
      return GetCPP_Name(aType);
   }
}

static std::string GetCPP_ParameterType(UtPackType& aType)
{
   std::string cppName = GetMemberTypeName(aType);
   if (aType.IsBasicType())
   {
      UtPackBasicType& bt = (UtPackBasicType&)aType;
      if (bt.GetEncoding() == UtPackBasicType::cENC_LITTLE_ENDIAN && bt.GetSize() <= 8)
      {
         return cppName;
      }
   }
   return "const " + cppName + "&";
}

static std::string GetMemberName(const std::string& aFieldName)
{
   return "m_" + aFieldName;
}

static std::ostream& column1(std::ostream& aOut)
{
   aOut << "      " << std::left << std::setw(20);
   return aOut;
}

using PropertyTest = bool (UtPackField::*)() const;

static std::string OutputPropertyRecursionHelper(const UtPackStruct& aStruct,
                                                 PropertyTest        aTest,
                                                 const std::string&  aPrefix,
                                                 int&                aIndex,
                                                 bool                aSet)
{
   constexpr const char* cINDENT = "         ";
   std::ostringstream    out;

   for (const UtPackField& field : aStruct.GetAllFields())
   {
      if ((field.*aTest)())
      {
         out << cINDENT << "case " << aIndex << ":\n";
         if (aSet)
         {
            if (field.IsOptional())
            {
               out << cINDENT << "   if (" << aPrefix << field.GetFieldName() << "Valid())\n";
               out << cINDENT << "   {\n";
               out << cINDENT << "      " << aPrefix << field.GetFieldName() << "(aValue);\n";
               out << cINDENT << "   }\n";
            }
            else
            {
               out << cINDENT << "   " << aPrefix << field.GetFieldName() << "(aValue);\n";
            }
            out << cINDENT << "   break;\n";
         }
         else
         {
            out << cINDENT << "   return " << aPrefix << field.GetFieldName() << "();\n";
         }

         aIndex++;
      }
      else if (field.GetType().GetType()->IsStruct())
      {
         out << OutputPropertyRecursionHelper(dynamic_cast<const UtPackStruct&>(*field.GetType().GetType()),
                                              aTest,
                                              field.GetFieldName() + "().",
                                              aIndex,
                                              aSet);
      }
   }

   return out.str();
}

static std::string OutputProperty(const UtPackStruct& aStruct,
                                  PropertyTest        aTest,
                                  const std::string&  aReturnType,
                                  const std::string&  aPropertyName,
                                  bool                aSet)
{
   constexpr const char* cINDENT = "      ";
   std::ostringstream    out;
   out << cINDENT << "virtual ";
   if (aSet)
   {
      out << "void Set";
   }
   else
   {
      out << aReturnType << " Get";
   }
   out << aPropertyName << "(size_t aWhich, ";
   if (aSet)
   {
      out << aReturnType << " aValue)\n";
   }
   else
   {
      out << "bool& aGood) const\n";
   }
   out << cINDENT << "{\n";

   int         i     = 0;
   std::string cases = OutputPropertyRecursionHelper(aStruct, aTest, std::string(), i, aSet);

   if (i > 0)
   {
      if (!aSet)
      {
         out << cINDENT << "   aGood = true;\n";
      }
      out << cINDENT << "   switch(aWhich)\n";
      out << cINDENT << "   {\n";
      out << cases;
      out << cINDENT << "   }\n";
   }

   if (!aSet)
   {
      out << cINDENT << "   aGood = false;\n";
      out << cINDENT
          << "   return 0;\n"; // Should really be returning "aReturnType{}", but ran into issues with that approach.
   }
   out << cINDENT << "}\n";

   return out.str();
}

class StructWriter
{
   std::string              mMessageBase;
   std::string              mNamespace;
   std::vector<std::string> registerFunctions;

public:
   std::string mInputFileBaseName;

   StructWriter(UtPackSchema& aSchema, std::vector<std::string>&& aTypes, std::ostream& aStream)
      : mMessageBase("UtPackMessage")
      , mTypesToOutput(std::move(aTypes))
      , mSchema(&aSchema)
      , mStream(aStream)
   {
   }

   void SetMessageBaseClass(const std::string& aBase) { mMessageBase = aBase; }
   void SetNamespace(const std::string& aNamespace) { mNamespace = aNamespace; }
   void OutputDefinitions(int aMsgIdOrValue, const std::string& aMessageBaseFile, std::string& aMessageIdImplementations)
   {
      std::string namespaceName = mSchema->GetNamespaceName();

      // utilize specified output stream here
      mStream << "#ifndef HEADER_" << mInputFileBaseName << "_GUARD\n";
      mStream << "#define HEADER_" << mInputFileBaseName << "_GUARD\n";

      mStream << "#include \"" << aMessageBaseFile << "\"\n\n";
      mStream << "// This file was generated by pack_to_cpp from \"" << mInputFileBaseName << ".utpack\"\n";
      mStream << "// DO NOT MODIFY: Your changes will be lost.\n\n";

      std::string ns = mNamespace;
      if (ns.empty())
      {
         ns = namespaceName;
      }
      if (!ns.empty())
      {
         mStream << "namespace " << ns << " {\n";
      }
      std::set<std::string> messageSet;
      while (!mTypesToOutput.empty())
      {
         OutputDefinition(*mTypesToOutput.begin(), aMsgIdOrValue, messageSet);
      }
      if (!ns.empty())
      {
         mStream << "\n} // end namespace " << ns << "\n";
      }
      mStream << "#endif\n";

      aMessageIdImplementations += "// The following implementation will be unnecessary as of C++17\n";
      for (const auto& msg : messageSet)
      {
         if (!ns.empty())
         {
            aMessageIdImplementations += "constexpr int " + ns + "::" + msg + "::cMESSAGE_ID;\n";
         }
         else
         {
            aMessageIdImplementations += "constexpr int " + msg + "::cMESSAGE_ID;\n";
         }
      }
   }

   void OutputRegistration(const std::string& aClassHeader)
   {
      mStream << "#include \"UtPackReflect.hpp\"\n\n";
      if (!aClassHeader.empty())
      {
         mStream << "#include \"" << aClassHeader << "\"\n\n";
      }
      std::string namespaceName = mSchema->GetNamespaceName();
      if (!mNamespace.empty())
      {
         mStream << "namespace " << mNamespace << " {\n";
      }
      if (!namespaceName.empty())
      {
         mStream << "namespace " << namespaceName << " {\n";
      }
      std::stringstream listRegistration;
      for (const auto& type : mTypesToOutput)
      {
         UtPackType* typePtr = mSchema->FindType(type);
         if (typePtr && typePtr->IsStruct())
         {
            outputStructRegistration(*(UtPackStruct*)typePtr, mStream);
         }
         else if (typePtr && typePtr->IsUnion())
         {
            outputUnionRegistration(*(UtPackUnion*)typePtr, mStream);
         }
         else if (typePtr && typePtr->IsList())
         {
            UtPackListType* listTypePtr = (UtPackListType*)typePtr;
            listRegistration << "   aSerializer.RegisterListType(\"" << listTypePtr->GetTypePath() << "\", \""
                             << listTypePtr->mContainedType.GetType()->GetTypePath() << "\");\n";
         }
      }

      std::cout << '\n'; // Remaining as cout

      mStream << "inline void UtPack_register_all_" << mInputFileBaseName << "_types(UtPackSerializer& aSerializer)\n{ \n";
      mStream << "   UtPackReflector reflector(aSerializer);\n";

      for (const auto& function : registerFunctions)
      {
         mStream << "   " << function << "(reflector);\n";
      }
      mStream << listRegistration.str();
      mStream << "}\n";
      if (!namespaceName.empty())
      {
         mStream << "\n} // end namespace " << namespaceName << "\n";
      }
      if (!mNamespace.empty())
      {
         mStream << "\n} // end namespace " << mNamespace << "\n";
      }
   }

private:
   void outputStructRegistration(UtPackStruct& s, std::ostream& out)
   {
      if (s.IsVirtual())
      {
         return;
      }
      std::string className = GetCPP_Name(s);
      registerFunctions.push_back("Register_" + className);
      out << "inline void "
          << "Register_" << className << "(UtPackReflector& aReflector)\n{ \n";
      out << "   typedef " << className << " STRUCT;\n";
      out << "   STRUCT obj;\n";
      out << "   aReflector.BeginStruct(\"" << s.GetTypePath() << "\", obj);\n";
      if (s.IsMessage() && s.GetMessageId() >= 0)
      {
         out << "   aReflector.RegisterMessage(obj);\n";
      }
      std::vector<UtPackStruct*> allTypes;
      {
         allTypes.push_back(&s);
         UtPackTypeReference base = s.GetBaseType();
         while (!base.IsNull())
         {
            UtPackStruct* baseStruct = (UtPackStruct*)base.GetType();
            allTypes.push_back(baseStruct);
            base = baseStruct->GetBaseType();
         }
         std::reverse(allTypes.begin(), allTypes.end());
      }

      for (size_t classIndex = 0; classIndex < allTypes.size(); ++classIndex)
      {
         UtPackStruct* classPtr = allTypes[classIndex];
         // std::string className = GetCPP_Name(*classPtr);
         if (classPtr->GetBitsetSize())
         {
            if (classIndex != allTypes.size() - 1)
            {
               std::cerr << "Cannot have a base class with optional members.\n";
               exit(1);
            }
            out << "   aReflector.OptionalFlags(obj.mBits);\n";
         }
         const std::vector<UtPackField>& fields = classPtr->GetFields();
         for (const UtPackField& field : fields)
         {
            if (!field.IsObsolete() && field.GetBitIndex() == -1)
            {
               // UtPackType& fieldType = *field.GetType().GetType();
               // std::string fieldTypeName = GetCPP_Name(fieldType);
               if (field.IsOptional())
               {
                  out << "   aReflector.OptionalMember(";
               }
               else
               {
                  out << "   aReflector.RequiredMember(";
               }
               out << '"';
               if (field.GetType().GetType()->IsEnum())
               {
                  UtPackEnumType* enumType = (UtPackEnumType*)field.GetType().GetType();
                  out << enumType->GetUnderlyingType();
               }
               else
               {
                  out << field.GetType().GetType()->GetTypeName();
               }
               out << "\", \"" << field.GetFieldName() << '"' << ", obj." << GetMemberName(field.GetFieldName());
               if (field.IsOptional())
               {
                  out << ", " << field.GetOptionIndex();
               }
               out << ");\n";
            }
         }
      }

      out << "   aReflector.End();\n}\n";
   }

   void outputUnionRegistration(UtPackUnion& s, std::ostream& out)
   {
      std::string className = GetCPP_Name(s);
      registerFunctions.push_back("Register_" + className);
      out << "inline void "
          << "Register_" << className << "(UtPackReflector& aReflector)\n{ \n";

      out << "   typedef " << className << " STRUCT;\n";
      out << "   STRUCT obj;\n";
      out << "   aReflector.BeginUnion(\"" << s.GetTypePath() << "\", obj);\n";

      const std::vector<UtPackField>& fields = s.GetFields();
      for (const UtPackField& field : fields)
      {
         if (!field.IsObsolete())
         {
            UtPackType& fieldType     = *field.GetType().GetType();
            std::string fieldTypeName = GetMemberTypeName(fieldType);
            out << "   aReflector.OptionalMember(";
            out << '"' << field.GetType().GetType()->GetTypeName() << "\", \"" << field.GetFieldName() << '"' << ", ("
                << fieldTypeName << "&)obj." << GetMemberName(field.GetFieldName()) << ", " << field.GetOptionIndex();
            out << ");\n";
         }
      }

      out << "   aReflector.End();\n";

      out << "};\n\n";
   }

   void OutputDefinition(std::string aTypeName, int aMsgIdOrValue, std::set<std::string>& aMessageSet)
   {
      // If mTypesToOutput contains aTypeName, remove it and continue.
      // Otherwise, return immediately.
      auto it = std::remove(mTypesToOutput.begin(), mTypesToOutput.end(), aTypeName);
      if (it == mTypesToOutput.end())
      {
         return;
      }
      mTypesToOutput.erase(it, mTypesToOutput.end());

      // Continue...
      UtPackType* typePtr = mSchema->FindType(aTypeName);
      if (typePtr && typePtr->IsStruct())
      {
         // outputStruct() will reenter this function
         // output to stringstream, and then finally dump it to mStreamPtr
         std::stringstream ss;
         UtPackStruct*     structPtr = (UtPackStruct*)typePtr;
         outputStruct(*structPtr, ss, aMsgIdOrValue);
         if (structPtr->GetMessageId() >= 0)
         {
            aMessageSet.insert(GetCPP_Name(*typePtr));
         }
         mStream << ss.str();
      }
      else if (typePtr && typePtr->IsUnion())
      {
         // outputUnion() will reenter this function
         // output to stringstream, and then finally dump it to mStreamPtr
         std::stringstream ss;
         UtPackUnion*      unionPtr = (UtPackUnion*)typePtr;
         outputUnion(*unionPtr, ss);
         mStream << ss.str();
      }
      else if (typePtr && typePtr->IsList())
      {
         std::stringstream ss;
         UtPackListType*   listPtr = (UtPackListType*)typePtr;
         outputList(*listPtr, ss, aMsgIdOrValue);
         mStream << ss.str();
      }
      else if (typePtr && typePtr->IsEnum())
      {
         UtPackEnumType* listPtr = (UtPackEnumType*)typePtr;
         outputEnum(*listPtr, mStream);
      }
   }

   void outputList(UtPackListType& aListType, std::ostream& out, int aMsgIdOrValue)
   {
      std::set<std::string> mset;
      OutputDefinition(aListType.mContainedType.GetType()->GetTypePath(), aMsgIdOrValue, mset);

      std::string className(GetCPP_Name(aListType));
      out << "class " << className << " : public UtPackList<" << GetMemberTypeName(*aListType.mContainedType.GetType())
          << ">\n{";
      out << "   public:\n";
      out << "      virtual ~" << className << "() {}\n";
      out << "};\n\n";
   }

   void outputEnum(UtPackEnumType& e, std::ostream& out)
   {
      out << "class " << GetCPP_Name(e) << '\n';
      out << '{';
      out << "   public:\n";
      out << "      enum Value {\n";
      const std::map<int, std::string>& vals = e.GetValueToName();
      for (std::map<int, std::string>::const_iterator i = vals.begin(); i != vals.end(); ++i)
      {
         if (i != vals.begin())
         {
            out << ",\n";
         }
         out << "         ";
         out << i->second;
         out << " = " << i->first;
      }
      out << "\n      };\n\n";
      out << "      static const char* ToString(int aValue)\n";
      out << "      {\n";
      out << "         switch (aValue) {\n";

      for (const auto& val : vals)
      {
         out << "         case " << val.first << ": return \"" << val.second << "\";\n";
      }
      out << "         default: return 0;\n";
      out << "         }\n";

      out << "      }\n";
      out << "};\n\n";
   }

   // Output a C++ class containing all of the fields as members
   void outputStruct(UtPackStruct& s, std::ostream& out, int aMsgIdOrValue)
   {
      std::vector<std::string>        initializers;
      const std::vector<UtPackField>& fields = s.GetFields();
      out << "class " << GetCPP_Name(s);
      if (!s.GetBaseType().IsNull())
      {
         out << " : public " << GetCPP_Name(*s.GetBaseType().GetType());
      }
      else if (s.IsMessage())
      {
         out << " : public " << mMessageBase;
      }
      out << "\n{\n   public:\n";

      // constructor
      {
         out << "      " << GetCPP_Name(s) << "() ";
         // bool firstInitializer = true;
         std::string bitInitializers;
         for (const UtPackField& field : fields)
         {
            UtPackType&           fieldType = *field.GetType().GetType();
            std::set<std::string> mset;
            OutputDefinition(fieldType.GetTypePath(), aMsgIdOrValue, mset);
            std::string name       = field.GetFieldName();
            std::string memberName = GetMemberName(name);
            // std::string cppTypeName = GetCPP_Name(fieldType);
            if (fieldType.GetEncoding() == UtPackType::cENC_BIT)
            {
               if (!field.GetDefaultValue().empty() && field.GetBitIndex() != -1)
               {
                  if (field.GetDefaultValue() != "false" && field.GetDefaultValue() != "0")
                  {
                     std::stringstream ss;
                     ss << "         mBits.set(" << field.GetBitIndex() << ", true);\n";
                     bitInitializers += ss.str();
                  }
               }
               // nothing to initialize
            }
            else if (fieldType.IsBasicType())
            {
               std::string defaultValue = field.GetDefaultValue();
               if (defaultValue.empty() && fieldType.GetTypeName() == "string")
               {
                  // no initializer required
               }
               else
               {
                  if (fieldType.GetTypeName() == "string")
                  {
                     defaultValue = '"' + defaultValue + '"';
                  }
                  initializers.push_back(memberName + "(" + defaultValue + ")");
               }
            }
            else if (fieldType.IsEnum())
            {
               std::string defaultValue = field.GetDefaultValue();

               UtPackEnumType* enumPtr = (UtPackEnumType*)&fieldType;
               if (defaultValue.empty())
               {
                  defaultValue = enumPtr->GetEnumValues().begin()->first;
               }

               if (enumPtr->GetEnumValues().find(defaultValue) != enumPtr->GetEnumValues().end())
               {
                  defaultValue = GetCPP_Name(fieldType) + "::" + defaultValue;
               }
               else
               {
                  std::cerr << "Invalid default value: " << s.GetTypePath() << " . " << field.GetFieldName() << '\n';
               }

               initializers.push_back(memberName + "(" + defaultValue + ")");
            }
         }
         for (size_t i = 0; i < initializers.size(); ++i)
         {
            if (!i)
            {
               out << " : ";
            }
            else
            {
               out << ",";
            }
            out << "\n       " << initializers[i];
         }

         out << "\n      {\n";
         out << bitInitializers;
         out << "      }\n";
      }

      if (s.IsMessage())
      {
         if (s.GetMessageId() >= 0)
         {
            int messageId = s.GetMessageId() | aMsgIdOrValue;
            out << "      static constexpr int cMESSAGE_ID = " << messageId << ";\n";
            out << "      virtual int            GetMessageId() const { return " << messageId << "; }\n";
            out << "      virtual const char*    GetMessageName() const { return \"" << s.GetTypePath() << "\"; }\n";
            out << "      virtual " << mMessageBase << "* Clone() const { return new " << GetCPP_Name(s) << "(*this); }\n";
         }

         out << OutputProperty(s, &UtPackField::IsIndex, "unsigned int", "Index", false);
         out << OutputProperty(s, &UtPackField::IsIndex, "unsigned int", "Index", true);

         out << OutputProperty(s, &UtPackField::IsTime, "double", "Time", false);
         out << OutputProperty(s, &UtPackField::IsTime, "double", "Time", true);
      }

      // methods
      for (const UtPackField& field : fields)
      {
         UtPackType& fieldType = *field.GetType().GetType();
         std::string name      = field.GetFieldName();

         std::string memberName  = GetMemberName(name);
         std::string cppTypeName = GetMemberTypeName(fieldType);

         out << "\n      // Field: " << cppTypeName << " " << name << "\n";

         if (field.IsOptional())
         {
            column1(out) << "bool" << name << "Valid() const { return mBits.test(" << field.GetOptionIndex() << "); }\n";
            column1(out) << "void" << name << "Valid(bool aIsValid) { return mBits.set(" << field.GetOptionIndex()
                         << ", aIsValid); }\n";
         }
         if (fieldType.GetEncoding() == UtPackType::cENC_BIT)
         {
            column1(out) << "bool" << name << "() const { return mBits.test(" << field.GetBitIndex() << "); }\n";
            column1(out) << "void" << name << "(bool aValue) { mBits.set(" << field.GetBitIndex() << ", aValue); ";
            if (field.IsOptional())
            {
               out << name << "Valid(true); ";
            }
            out << "}\n";
         }
         else
         {
            std::string parameterType = GetCPP_ParameterType(*field.GetType().GetType());
            column1(out) << "void" << name << "(" << parameterType << " aValue) { " << memberName << " = aValue; ";
            if (field.IsOptional())
            {
               out << name << "Valid(true); ";
            }
            out << "}\n";

            column1(out) << parameterType << name << "() const { return " << memberName << "; }\n";
            if (parameterType.substr(0, 5) == "const")
            {
               column1(out) << parameterType.substr(6) << name << "() { return " << memberName << "; }\n";
            }
         }
      }

      // members
      out << "\n\n   protected:\n\n";
      int bitsetSize = s.GetBitsetSize();
      if (bitsetSize > 0)
      {
         int               bytes = (bitsetSize + 7) / 8;
         std::stringstream ss;
         ss << "typedef UtPackBitset<" << bytes << ">";
         column1(out) << ss.str() << " Bitset;\n";
         column1(out) << "Bitset"
                      << " mBits;\n";
      }

      for (const UtPackField& field : fields)
      {
         UtPackType& fieldType   = *field.GetType().GetType();
         std::string name        = field.GetFieldName();
         std::string memberName  = GetMemberName(name);
         std::string cppTypeName = GetMemberTypeName(fieldType);

         if (fieldType.GetEncoding() != UtPackType::cENC_BIT)
         {
            // std::string cppName = GetMemberTypeName(*fields[i].GetType().GetType());
            column1(out) << cppTypeName << ' ' << memberName << ";\n";
         }
      }
      out << "\n   public:\n";
      out << "      friend void Register_" << GetCPP_Name(s) << "(UtPackReflector& aReflector);\n";
      out << "};\n\n";
   }

   // Output a C++ class containing all of the fields as members
   //
   // class MyUnion
   // {
   //    public:
   //       MyUnion()
   //        : mFieldType(cFT_NONE)
   //       {
   //       }
   //       ~MyUnion()
   //       {
   //          DestroyTypeP(mFieldType);
   //       }
   //
   //       enum FieldType {
   //          cFT_fieldOne = 0,
   //          cFT_myVec = 1,
   //          cFT_NONE = 255
   //       };
   //
   //       // Field: double fieldOne
   //       double&        fieldOne() { assert(mFieldType == cFT_fieldOne); return (double&)m_fieldOne; }
   //       const double&  fieldOne() const { assert(mFieldType == cFT_fieldOne); return (double&)m_fieldOne; }
   //       void           fieldOne(const double& aValue) { SetFieldType(cFT_fieldOne); fieldOne() = aValue; }
   //
   //       // Field: Vec3 myVec
   //       Vec3&        myVec() { assert(mFieldType == cFT_myVec); return (Vec3&)m_myVec; }
   //       const Vec3&  myVec() const { assert(mFieldType == cFT_myVec); return (Vec3&)m_myVec; }
   //       void         myVec(const Vec3& aValue) { SetFieldType(cFT_myVec); myVec() = aValue; }
   //
   //       FieldType  GetFieldType() const { return mFieldType; }
   //       void       SetFieldType(FieldType aFieldType) const {
   //          if (mFieldType != aFieldType) {
   //             DestroyTypeP(mFieldType);
   //             CreateFieldP(aFieldType);
   //             mFieldType = aFieldType;
   //          }
   //       }
   //
   //    protected:
   //       template <typename T> static void CreateP(void* aAddr)  { new (aAddr) T; }
   //       template <typename T> static void DestroyP(void* aAddr)  { ((T*)aAddr)->~T(); }
   //       void        DestroyTypeP(FieldType aType)
   //       {
   //          switch (aType) {
   //             case cFT_fieldOne: DestroyP(&m_fieldOne); break;
   //             case cFT_myVec: DestroyP(&m_myVec); break;
   //             default: break;
   //          }
   //       }
   //       void        CreateFieldP(FieldType aType)
   //       {
   //          switch (aType) {
   //             case cFT_fieldOne: CreateP(&m_fieldOne); break;
   //             case cFT_myVec: CreateP(&m_myVec); break;
   //             default: break;
   //          }
   //       }
   //       FieldType      mFieldType;
   //       union {
   //          int64_t     m_fieldOne[(sizeof(double)+7)/8];
   //          int64_t     m_myVec[(sizeof(Vec3)+7)/8];
   //       };
   // };
   void outputUnion(UtPackUnion& s, std::ostream& out)
   {
      // std::vector<std::string>   initializers;
      const std::vector<UtPackField>& fields = s.GetFields();
      out << "class " << GetCPP_Name(s);
      out << "\n{\n   public:\n";

      // constructor
      {
         out << "      " << GetCPP_Name(s) << "() ";
         out << "\n       : mFieldType(cFT_NONE)";
         out << "\n      {\n";
         out << "      }\n";
      }

      // destructor
      {
         out << '\n';
         out << "      ~" << GetCPP_Name(s) << "()\n";
         out << "      {\n";
         out << "         DestroyTypeP(mFieldType);\n";
         out << "      }\n";
      }

      // enum
      {
         out << "\n      enum FieldType {\n   ";

         for (size_t i = 0; i < fields.size(); ++i)
         {
            const UtPackField& field = fields[i];
            // UtPackType& fieldType = *field.GetType().GetType();
            std::string name = field.GetFieldName();
            if (i != 0)
            {
               out << ",\n   ";
            }
            out << "      cFT_" << name << " = " << field.GetOptionIndex();
         }
         out << ",\n         cFT_NONE = 255\n";
         out << "      };\n";
      }

      // operator=
      {
         out << '\n';
         out << "      " << GetCPP_Name(s) << "& operator=(const " << GetCPP_Name(s) << "& aRHS)\n";
         out << "      {\n";
         out << "         switch (aRHS.mFieldType)\n";
         out << "         {\n";
         for (size_t i = 0; i < fields.size(); ++i)
         {
            const UtPackField& field = fields[i];
            std::string        name  = field.GetFieldName();
            out << "            case cFT_" << name << ": " << name << "(aRHS." << name << "()); break;\n";
         }
         out << "            case cFT_NONE: default: break;\n";
         out << "         }\n";
         out << "         return *this;\n";
         out << "      }\n";
      }


      // accessors
      {
         for (const UtPackField& field : fields)
         {
            UtPackType& fieldType = *field.GetType().GetType();
            std::string name      = field.GetFieldName();

            std::string memberName  = GetMemberName(name);
            std::string cppTypeName = GetMemberTypeName(fieldType);

            out << "\n      // Field: " << cppTypeName << " " << name << "\n";

            std::string parameterType = GetCPP_ParameterType(*field.GetType().GetType());
            column1(out) << "void" << name << "(" << parameterType << " aValue) { "
                         << "SetFieldType(cFT_" << name << "); "
                         << "(" << cppTypeName << "&)" << memberName << " = aValue; }\n";

            column1(out) << parameterType << name << "() const { "
                         << "assert(mFieldType == cFT_" << name << "); "
                         << "return (" << cppTypeName << "&)" << memberName << "; }\n";
            if (parameterType.substr(0, 5) == "const")
            {
               column1(out) << parameterType.substr(6) << name << "() { "
                            << "assert(mFieldType == cFT_" << name << "); "
                            << "return (" << cppTypeName << "&)" << memberName << "; }\n";
            }
         }

         out << "      FieldType  GetFieldType() const { return (FieldType)mFieldType; }\n";
         out << "      void       SetFieldType(FieldType aFieldType) {\n";
         out << "         if (mFieldType != aFieldType) {\n";
         out << "            DestroyTypeP(mFieldType);\n";
         out << "            CreateTypeP(aFieldType);\n";
         out << "            mFieldType = aFieldType;\n";
         out << "         }\n";
         out << "      };\n\n";
      }

      out << "\n\n   protected:\n\n";
      // Field Create/Destroy
      {
         out << "      template <typename T> static void CreateP(void* aAddr)  { new (aAddr) T; }\n";
         out << "      template <typename T> static void DestroyP(void* aAddr) { ((T*)aAddr)->~T(); }\n";
         out << "      void DestroyTypeP(FieldType aType)\n";
         out << "      {\n";
         out << "         switch (aType) {\n";

         for (const UtPackField& field : fields)
         {
            UtPackType& fieldType = *field.GetType().GetType();
            std::string name      = field.GetFieldName();

            std::string memberName  = GetMemberName(name);
            std::string cppTypeName = GetMemberTypeName(fieldType);
            out << "            case cFT_" << name << ": DestroyP<" << cppTypeName << ">(&" << memberName << "); break; \n";
         }
         out << "            case cFT_NONE: default: break; \n";
         out << "         }\n";
         out << "      }\n";
         out << "      void CreateTypeP(FieldType aType)\n";
         out << "      {\n";
         out << "         switch (aType) {\n";
         for (const UtPackField& field : fields)
         {
            UtPackType& fieldType = *field.GetType().GetType();
            std::string name      = field.GetFieldName();

            std::string memberName  = GetMemberName(name);
            std::string cppTypeName = GetMemberTypeName(fieldType);
            out << "            case cFT_" << name << ": CreateP<" << cppTypeName << ">(&" << memberName << "); break; \n";
         }
         out << "            case cFT_NONE: default: break; \n";
         out << "         }\n";
         out << "      }\n";
      }

      // members
      {
         out << "      FieldType    mFieldType;\n";
         out << "      union {\n";

         for (const UtPackField& field : fields)
         {
            UtPackType& fieldType   = *field.GetType().GetType();
            std::string name        = field.GetFieldName();
            std::string memberName  = GetMemberName(name);
            std::string cppTypeName = GetMemberTypeName(fieldType);
            out << "         "
                << "int64_t      " << memberName << "[(sizeof(" << cppTypeName << ") + 7)/8];\n";
         }
         out << "   };";
      }

      out << "\n   public: friend void Register_" << GetCPP_Name(s) << "(UtPackReflector& aReflector);\n";
      out << "};\n\n";
   }

   std::vector<std::string> mTypesToOutput;
   UtPackSchema*            mSchema;
   std::ostream&            mStream;
};

int WriteOutput(std::vector<std::string> aInputFiles,
                bool                     aOutputDefinitions,
                const std::string&       aMessageBaseClassName,
                const std::string&       aMessageBaseFileName,
                const std::string&       aNamespace,
                const std::string&       aClassHeader,
                std::ofstream&           aOutputFile,
                const std::string&       aFilename)
{
   UtPackSchema schema;
   try
   {
      UtmlObject               schemaMl = UtmlObject::makeContainer("Schema");
      std::vector<std::string> typesToImplement;
      for (size_t i = 0; i < aInputFiles.size(); ++i)
      {
         UtTextDocument doc;
         doc.ReadFile(UtPath(aInputFiles[i]));
         UtmlParser parser(doc);
         parser.Parse(schemaMl);
         schema.Read(schemaMl);
         if (i == 0)
         {
            typesToImplement = schema.GetTypeNames();
         }
      }
      // UtmlWriter tmp;
      // tmp.WriteRoot(schemaMl, std::cerr);
      //
      schema.Resolve();

      int message_id_and = 0;
      for (int i = 0; i < schemaMl.GetObjectCount(); ++i)
      {
         UtmlObject& obj = schemaMl.GetObject(i);
         if (obj.GetType() == "Settings")
         {
            std::stringstream ss(obj.PropertyValue("message_id_or", "0"));
            ss >> message_id_and;
            break; // only need the first one
         }
      }

      StructWriter writer(schema, std::move(typesToImplement), aOutputFile);
      writer.mInputFileBaseName = UtPath(aInputFiles[0]).GetFileName(false);
      writer.SetMessageBaseClass(aMessageBaseClassName);
      writer.SetNamespace(aNamespace);
      if (aOutputDefinitions)
      {
         std::string impStr;
         writer.OutputDefinitions(message_id_and << 24, aMessageBaseFileName, impStr);

         std::string base    = aFilename.substr(0, aFilename.find_last_of("."));
         std::string srcFile = base + ".cpp";

         std::ofstream srcstr(srcFile);
         srcstr << "#include \"" << base.substr(base.find_last_of("/\\") + 1) << ".hpp\"" << std::endl;

         srcstr << "\n\n// This file was generated by pack_to_cpp from \"" << writer.mInputFileBaseName << ".utpack\"\n";
         srcstr << "// DO NOT MODIFY: Your changes will be lost.\n\n";

         srcstr << impStr;
         srcstr.close();
      }
      else
      {
         writer.OutputRegistration(aClassHeader);
      }
   }
   catch (std::exception& e)
   {
      aOutputFile.close();
      std::cerr << "***** ERROR: " << e.what() << '\n';
      return 1;
   }
   aOutputFile.close();
   return 0;
}

int compile_resource_to_c(const std::string& aResourceVariableName,
                          const std::string& aResourceFilePath,
                          const std::string& aSourceFilePath)
{
   std::ifstream              f(aResourceFilePath, std::ios::binary);
   std::vector<unsigned char> resourceData;
   char                       c;
   while (f.get(c))
   {
      resourceData.push_back(c);
   }
   std::vector<std::string> charRep(256);
   for (int i = 0; i < 256; ++i)
   {
      char c = (char)i;
      if (i >= 127 || i <= 7)
      {
         std::stringstream ss;
         ss << "\\" << (i >> 6) << ((i >> 3) & 7) << (i & 7);
         charRep[i] = ss.str();
      }
      else
      {
         switch (c)
         {
         case '"':
            charRep[i] = "\\\"";
            break;
         case '\\':
            charRep[i] = "\\\\";
            break;
         case '\f':
            charRep[i] = "\\f";
            break;
         case '\b':
            charRep[i] = "\\b";
            break;
         case '\n':
            charRep[i] = "\\n";
            break;
         case '\r':
            charRep[i] = "\\r";
            break;
         default:
            charRep[i] = c;
         }
      }
   }

   std::ofstream out(aSourceFilePath);
   out << "// Generated from file: " << aResourceFilePath << '\n';
   out << "// DO NOT EDIT\n";
   out << "#ifndef HEADER_" << aResourceVariableName << "\n";
   out << "#define HEADER_" << aResourceVariableName << "\n";
   out << "const char " << aResourceVariableName << "[] = \n";
   std::string line;
   for (size_t i = 0; i < resourceData.size(); ++i)
   {
      unsigned char c = resourceData[i];
      line += charRep[(int)c];
      if ((i + 1 == resourceData.size()) || c == '\n' || line.size() > 150)
      {
         out << '"' << line << '"' << '\n';
         line.clear();
      }
   }
   out << ";\n#endif\n";
   return 0;
}

int main(int argc, char* argv[])
{
   std::string msgBase     = "UtPackMessage";
   std::string msgBaseFile = "UtPackMessage.hpp";
   std::string namespaceName;
   std::string namespaceResultFileName = "";
   std::string classHeader             = "";
   bool        quietMode               = false;
   while (argc > 1)
   {
      std::string cmd(argv[1]);
      if (cmd == "-msg-base")
      {
         msgBase = std::string(argv[2]);
         argv += 2;
         argc -= 2;
      }
      else if (cmd == "-msg-base-file")
      {
         msgBaseFile = std::string(argv[2]);
         argv += 2;
         argc -= 2;
      }
      else if (cmd == "-quiet")
      {
         quietMode = true;
         argv += 1;
         argc -= 1;
      }
      else if (cmd == "-class-header")
      {
         classHeader = std::string(argv[2]);
         argv += 2;
         argc -= 2;
      }
      else if (cmd == "-namespace")
      {
         if (!quietMode)
         {
            // Write to cerr to enable documented use case of output redirection.
            std::cerr << "Generating namespace file.\n"; // Remaining as cerr
         }
         namespaceName = argv[2];
         // The file name is the last argument
         namespaceResultFileName = std::string(argv[argc - 1]);
         argv += 2;
         argc -= 2;
      }
      else
      {
         break;
      }
   }
   bool doResourceToC = (argv[1] == std::string("-res"));

   if (doResourceToC)
   {
      return compile_resource_to_c(argv[2], argv[3], argv[4]);
   }

   bool                     doHeader = (argv[1] == std::string("-h"));
   bool                     doSrc    = (argv[1] == std::string("-c"));
   std::vector<std::string> inputFiles;
   for (int i = 2; i < argc; ++i)
   {
      inputFiles.emplace_back(argv[i]);
      if (!quietMode)
      {
         // Write to cerr to enable documented use case of output redirection.
         std::cerr << "file: " << argv[i] << '\n';
      }
   }

   if (!namespaceResultFileName.empty())
   {
      std::ofstream resultFile(namespaceResultFileName);
      if (!resultFile.is_open())
      {
         std::cerr << "Could not create result file: " << namespaceResultFileName << std::endl;
      }
      return WriteOutput(inputFiles, doHeader, msgBase, msgBaseFile, namespaceName, classHeader, resultFile, namespaceResultFileName);
   }
   else
   {
      exit(1);
   }
}
