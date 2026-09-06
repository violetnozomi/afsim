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

#include "UtScriptBasicTypes.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <sstream>

#include "UtCast.hpp"
#include "UtMemory.hpp"
#include "UtScriptException.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtScriptVector.hpp"
#include "UtStringUtil.hpp"

static const UtScriptData cInvalidData{};

UT_MAP_TYPE_TO_SCRIPT_NAME(UtScriptArrayIterator::ArrayIterator, "ArrayIterator");
UT_MAP_TYPE_TO_SCRIPT_NAME(UtScriptMapIterator::MapIterator, "MapIterator");
UT_MAP_TYPE_TO_SCRIPT_NAME(UtScriptSetIterator::SetIterator, "SetIterator");
UT_MAP_TYPE_TO_SCRIPT_NAME(UtScriptSet::Set, "Set<Object>");

//--------------------------------------------------------------
//! UtScriptString Class
//--------------------------------------------------------------

namespace
{
std::string Substring(const std::string& aString, int aStart, int aEnd)
{
   int lastIndex = static_cast<int>(aString.length()) - 1;
   if (aStart < 0)
   {
      aStart = std::max<int>(aStart + lastIndex + 1, 0);
   }
   else
   {
      aStart = std::min<int>(aStart, lastIndex);
   }
   if (aEnd < 0)
   {
      aEnd = std::max<int>(aEnd + lastIndex, 0);
   }
   else
   {
      aEnd = std::min<int>(aEnd, lastIndex);
   }
   if (aEnd >= aStart && aStart >= 0)
   {
      return aString.substr(aStart, (aEnd - aStart) + 1);
   }
   return "";
}
} // namespace

UtScriptString::UtScriptString(UtScriptTypes* aTypesPtr)
   : UtScriptBasicType("string", aTypesPtr)
{
   UT_SCRIPT_WRAP_CLASS(const std::string);

   AddExplicitCastType("double");
   AddExplicitCastType("int");
   AddExplicitCastType("bool");

   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(size_t, Length, ()) { return self.length(); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(bool, Contains, (const std::string& aString)) { return self.find(aString) != std::string::npos; };

   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(bool, StartsWith, (const std::string& aPrefix)) { return self.find(aPrefix) == 0; };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(bool, EndsWith, (const std::string& aSuffix))
   {
      size_t i = self.rfind(aSuffix);
      return (i != std::string::npos) && (i == (self.length() - aSuffix.length()));
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::string, Substring, (int aStart))
   {
      return Substring(self, aStart, static_cast<int>(self.length()) - 1);
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::string, Substring, (int aStart, int aEnd)) { return Substring(self, aStart, aEnd); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::vector<std::string>, Split, (const std::string& aDelim))
   {
      std::vector<std::string> parts;
      UtStringUtil::Parse(self, parts, aDelim);
      return parts;
   };

   // split by whitespace
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::vector<std::string>, Split, ())
   {
      std::vector<std::string> parts;
      UtStringUtil::ParseWhitespace(self, parts);
      return parts;
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::string, Join, (const UtScriptVector<std::string>& aStringList))
   {
      std::string output;
      for (size_t i = 0; i < aStringList.size(); ++i)
      {
         if (i != 0)
         {
            output.append(self);
         }
         output.append(aStringList[i]);
      }
      return output;
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::string, Strip, ())
   {
      std::string strippedString = self;
      UtStringUtil::TrimWhiteSpace(strippedString);
      return strippedString;
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::string, Strip, (const std::string& aToken))
   {
      std::string strippedString = self;
      UtStringUtil::Trim(strippedString, aToken);
      return strippedString;
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::string, Lower, ())
   {
      std::string lowerCase = self;
      UtStringUtil::ToLower(lowerCase);
      return lowerCase;
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::string, Upper, ())
   {
      std::string upperCase = self;
      UtStringUtil::ToUpper(upperCase);
      return upperCase;
   };

   // Return the ascii code for a string with a single character
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(unsigned char, Ord, ())
   {
      // note string[0] is valid even if a string is empty.
      if (self.length() == 1)
      {
         return static_cast<unsigned char>(self[0]);
      }
      // otherwise error
      return '\0';
   };

   // Return the string with a single character given the ascii code
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Chr, (char ascii))
   {
      std::string s;
      s += ascii;
      return s;
   };

   // Return the string with a single character given the ascii code
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::string, Replace, (const std::string& findStr, const std::string& replaceStr))
   {
      std::string        result;
      const std::string& str = self;
      for (size_t i = 0; i < str.size(); ++i)
      {
         bool replaced = false;
         if (str.size() - i >= findStr.size())
         {
            if (0 == str.compare(i, findStr.size(), findStr))
            {
               replaced = true;
               result += replaceStr;
               i += findStr.size();
               --i;
            }
         }
         if (!replaced)
         {
            result += str[i];
         }
      }
      return result;
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::string, Pad, (int aNewLength))
   {
      char        padChar   = ' ';
      std::string result    = self;
      size_t      newLength = static_cast<size_t>(std::abs(aNewLength));
      size_t      curLength = result.length();
      if (newLength > curLength)
      {
         size_t padCount = newLength - curLength;
         if (aNewLength >= 0)
         {
            result.insert(0, padCount, padChar);
         }
         else
         {
            result.append(padCount, padChar);
         }
      }
      return result;
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(std::string, Pad, (int aNewLength, const std::string& padStr))
   {
      char        padChar   = padStr.empty() ? ' ' : padStr.front();
      std::string result    = self;
      size_t      newLength = static_cast<size_t>(std::abs(aNewLength));
      size_t      curLength = result.length();
      if (newLength > curLength)
      {
         size_t padCount = newLength - curLength;
         if (aNewLength >= 0)
         {
            result.insert(0, padCount, padChar);
         }
         else
         {
            result.append(padCount, padChar);
         }
      }
      return result;
   };
}

void* UtScriptString::Create(const UtScriptContext&)
{
   return static_cast<void*>(new std::string());
}

void UtScriptString::Destroy(void* aObjectPtr)
{
   delete static_cast<std::string*>(aObjectPtr);
}

void* UtScriptString::Clone(void* aObjectPtr)
{
   return new std::string(*static_cast<std::string*>(aObjectPtr));
}

void UtScriptString::SetDefaultValue(UtScriptData& aData)
{
   aData.SetString(std::string());
}
//--------------------------------------------------------------
//! UtScriptArray Class
//--------------------------------------------------------------

UtScriptArray::UtScriptArray(const std::string& aClassName, UtScriptTypes* aTypesPtr, const TemplateArgTypes& aTemplateArgTypes)
   : UtScriptClass(aClassName, aTypesPtr, aTemplateArgTypes)
{
   SetClassName("Array");
   UT_SCRIPT_WRAP_CLASS(std::vector<UtScriptData>);
   SetClassName(aClassName);

   mConstructible = true;
   mCloneable     = true;
   mContainer     = true;

   std::unique_ptr<InterfaceMethod> methodPtr;
   ArgTypes                         argTypes;
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(size_t, Size, ()) { return self.size(); };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_METHOD(bool, Empty, ()) { return self.empty(); };

   UT_SCRIPT_METHOD(bool, Erase, (const UtScriptData* aElement))
   {
      for (auto iter = self.begin(); iter != self.end(); ++iter)
      {
         if (*iter == *aElement)
         {
            self.erase(iter);
            return true;
         }
      }
      return false;
   };

   auto& getMethod = UT_SCRIPT_METHOD(UtScriptData&, Get, (size_t index))
   {
      if (self.size() > index)
      {
         return self[index];
      }
      else
      {
         UT_SCRIPT_ABORT("Array index out of bounds");
      }
   };
   if (!aTemplateArgTypes.empty())
   {
      getMethod.SetReturnTypeId(aTemplateArgTypes[1].mTypeId);
   }


   auto& setMethod = UT_SCRIPT_METHOD(void, Set, (unsigned int index, const UtScriptData* aObject))
   {
      // Automatically resize the vector, if an index is larger than its current size.
      if (self.size() <= index)
      {
         self.resize(index + 1);
      }
      self[index] = *aObject;
   };
   // Change Set(int, Object) to Set(T1, T2). In this case T1 will be an integer.
   if (!aTemplateArgTypes.empty())
   {
      argTypes.clear();
      argTypes.push_back(aTemplateArgTypes[0].mTypeId);
      argTypes.push_back(aTemplateArgTypes[1].mTypeId);
      setMethod.SetArgTypes(argTypes);
   }

   auto& pushBackMethod = UT_SCRIPT_METHOD(void, PushBack, (const UtScriptData* aObject)) { self.push_back(*aObject); };
   // Change PushBack(Object) to PushBack(T2).
   if (!aTemplateArgTypes.empty())
   {
      argTypes.clear();
      argTypes.push_back(aTemplateArgTypes[1].mTypeId);
      pushBackMethod.SetArgTypes(argTypes);
   }

   auto& insert_method = UT_SCRIPT_VARIADIC_METHOD(void, Insert, (int index), UtScriptData)
   {
      if (index >= 0 && ut::cast_to_size_t(index) <= self.size())
      {
         auto it = self.begin();
         std::advance(it, index);
         self.insert(it, std::next(aVarArgs.begin()), aVarArgs.end());
      }
      else
      {
         UT_SCRIPT_ABORT("Array index out of bounds");
      }
   };
   // Change Insert(int, Object) to Insert(int, T2).
   if (!aTemplateArgTypes.empty())
   {
      argTypes.clear();
      argTypes.push_back(aTemplateArgTypes[0].mTypeId);
      argTypes.push_back(aTemplateArgTypes[1].mTypeId);
      insert_method.SetArgTypes(argTypes);
   }

   UT_SCRIPT_METHOD(void, PopBack, ())
   {
      if (self.empty())
      {
         UT_SCRIPT_ABORT("Cannot call PopBack(), empty Array");
      }
      else
      {
         self.pop_back();
      }
   };

   UT_SCRIPT_METHOD(void, Clear, ()) { self.clear(); };
   UT_SCRIPT_METHOD(void, Reverse, ()) { std::reverse(self.begin(), self.end()); };
   UT_SCRIPT_METHOD(void, Sort, ()) { std::sort(self.begin(), self.end(), UtScriptData::LessThan()); };
   UT_SCRIPT_METHOD(void, Sort, (bool ascending))
   {
      // Ascending if true
      if (ascending)
      {
         std::sort(self.begin(), self.end(), UtScriptData::LessThan());
      }
      else
      {
         std::sort(self.begin(), self.end(), std::not2(UtScriptData::LessThan()));
      }
   };

   UT_SCRIPT_METHOD(bool, EraseAt, (int index))
   {
      bool canErase = (index >= 0 && index < static_cast<int>(self.size()));
      if (canErase)
      {
         self.erase(self.begin() + index);
      }
      return canErase;
   };

   auto& frontMethod = UT_SCRIPT_METHOD(UtScriptData&, Front, ())
   {
      if (self.empty())
      {
         UT_SCRIPT_ABORT("Cannot return Front(), empty array.");
      }
      return self.front();
   };
   if (!aTemplateArgTypes.empty())
   {
      frontMethod.SetReturnTypeId(aTemplateArgTypes[1].mTypeId);
   }


   auto& backMethod = UT_SCRIPT_METHOD(UtScriptData&, Back, ())
   {
      if (self.empty())
      {
         UT_SCRIPT_ABORT("Cannot return Back(), empty array.");
      }
      return self.back();
   };
   if (!aTemplateArgTypes.empty())
   {
      backMethod.SetReturnTypeId(aTemplateArgTypes[1].mTypeId);
   }

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptArrayIterator::ArrayIterator>, GetIterator, ())
   {
      return ut::make_unique<UtScriptArrayIterator::ArrayIterator>(self);
   };

   if (!aTemplateArgTypes.empty())
   {
      UtStringId     valueType = aTemplateArgTypes[1].mTypeId;
      UtScriptClass* classPtr  = aTypesPtr->GetClass(valueType);
      if (classPtr != nullptr)
      {
         mEqualityComparable = classPtr->IsBasicType() || classPtr->IsEqualityComparable();
         mLessThanComparable = classPtr->IsBasicType() || classPtr->IsLessThanComparable();
      }
   }
}


std::string UtScriptArray::ToStringN(void* aDataPtr, int aMaxStringSize) const
{
   std::stringstream ss;
   auto*             arrayPtr = static_cast<std::vector<UtScriptData>*>(aDataPtr);
   ss << '{';
   if (arrayPtr != nullptr)
   {
      for (size_t i = 0; i < arrayPtr->size(); ++i)
      {
         UtScriptData& data = arrayPtr->at(i);
         if (i != 0)
         {
            ss << ", ";
            if (ss.tellp() > aMaxStringSize)
            {
               ss << "...";
               break;
            }
         }
         ss << data;
      }
   }
   ss << '}';
   return ss.str();
}

std::string UtScriptArray::ToString(void* aDataPtr) const
{
   return ToStringN(aDataPtr, 100000);
}

bool UtScriptArray::LessThan(void* aLHS, void* aRHS)
{
   auto* lhs = static_cast<std::vector<UtScriptData>*>(aLHS);
   auto* rhs = static_cast<std::vector<UtScriptData>*>(aRHS);
   return std::lexicographical_compare(lhs->begin(), lhs->end(), rhs->begin(), rhs->end());
}

bool UtScriptArray::EqualTo(void* aLHS, void* aRHS)
{
   return (*static_cast<std::vector<UtScriptData>*>(aLHS) == *static_cast<std::vector<UtScriptData>*>(aRHS));
}

void* UtScriptArray::Clone(void* aObjectPtr)
{
   return new Array(*static_cast<Array*>(aObjectPtr));
}

void UtScriptArray::Destroy(void* aObjectPtr)
{
   delete static_cast<Array*>(aObjectPtr);
}

void* UtScriptArray::Create(const UtScriptContext&)
{
   return static_cast<void*>(new Array());
}

//--------------------------------------------------------------
//! UtScriptMap Class
//--------------------------------------------------------------
UtScriptMap::UtScriptMap(const std::string& aClassName, UtScriptTypes* aTypesPtr, const TemplateArgTypes& aTemplateArgTypes)
   : UtScriptClass(aClassName, aTypesPtr, aTemplateArgTypes)
{
   SetClassName("Map");
   UT_SCRIPT_WRAP_CLASS(UtScriptMap::Map);
   SetClassName(aClassName);

   mConstructible = true;
   mCloneable     = true;
   mContainer     = true;

   ArgTypes argTypes;

   UT_SCRIPT_METHOD(size_t, Size, ()) { return self.size(); };
   UT_SCRIPT_METHOD(bool, Empty, ()) { return self.empty(); };

   UT_SCRIPT_METHOD(bool, Erase, (const UtScriptData* aKey))
   {
      auto iter = self.find(*aKey);
      if (iter != self.end())
      {
         self.erase(iter);
         return true;
      }
      return false;
   };

   UT_SCRIPT_METHOD(void, Remove, (const UtScriptData* aKey)) // NO_DOC | DEPRECATED
   {
      // clear the element out, so its not deleted when erased
      self[*aKey] = 0;
      // find the element and erase it; the key is removed here
      auto iter = self.find(*aKey);
      if (iter != self.end())
      {
         self.erase(iter);
      }
   };

   UT_SCRIPT_METHOD(void, Clear, ()) { self.clear(); };

   //! Returns true if the key is present in the map
   UT_SCRIPT_METHOD(bool, Exists, (const UtScriptData* aKey)) { return self.find(*aKey) != self.end(); };

   //! Returns true if the key-value pair is present in the map
   UT_SCRIPT_METHOD(bool, Exists, (const UtScriptData* aKey, const UtScriptData* aVal))
   {
      auto iter = self.find(*aKey);
      return iter != self.end() && *aVal == iter->second;
   };

   auto& setMethod = UT_SCRIPT_METHOD(void, Set, (const UtScriptData* aKey, const UtScriptData* aVal))
   {
      self[*aKey] = *aVal;
   };
   // Change Set(Object, Object) to Set(T1, T2). Since this container
   // is a map the key (T1) can be of any type.
   if (!aTemplateArgTypes.empty())
   {
      argTypes.clear();
      argTypes.push_back(aTemplateArgTypes[0].mTypeId);
      argTypes.push_back(aTemplateArgTypes[1].mTypeId);
      setMethod.SetArgTypes(argTypes);
   }

   auto& eleAtMethod = UT_SCRIPT_METHOD(UtScriptData, ElementKeyAtIndex, (int index))
   {
      if (index >= 0)
      {
         for (auto iter = self.begin(); iter != self.end(); ++iter)
         {
            if (index == 0)
            {
               return iter->first;
            }
            --index;
         }
      }
      // throw error?
      return {};
   };
   if (!aTemplateArgTypes.empty())
   {
      eleAtMethod.SetReturnTypeId(aTemplateArgTypes[0].mTypeId);
   }

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptMapIterator::MapIterator>, GetIterator, ())
   {
      return ut::make_unique<UtScriptMapIterator::MapIterator>(self);
   };

   auto& keyset_method = UT_SCRIPT_METHOD(std::unique_ptr<UtScriptSet::Set>, KeySet, ())
   {
      auto keyset = ut::make_unique<UtScriptSet::Set>();
      std::transform(self.begin(),
                     self.end(),
                     std::inserter(*keyset, keyset->end()),
                     [](const UtScriptMap::Map::value_type& entry) { return entry.first; });
      return keyset;
   };
   if (!aTemplateArgTypes.empty())
   {
      std::string rtype = "Set<" + aTemplateArgTypes[0].mTypeId + ">";
      keyset_method.SetReturnTypeId(rtype);
   }

   auto& insert_method = UT_SCRIPT_VARIADIC_METHOD(void, __Insert, (), UtScriptData)
   {
      for (size_t i = 0; i < aVarArgs.size(); i += 2)
      {
         self[aVarArgs[i]] = aVarArgs[i + 1];
      }
   };
   // Change __Insert(Object, Object) to __Insert(T1, T2). Since this container
   // is a map the key (T1) can be of any type.
   if (!aTemplateArgTypes.empty())
   {
      argTypes.clear();
      argTypes.push_back(aTemplateArgTypes[0].mTypeId);
      argTypes.push_back(aTemplateArgTypes[1].mTypeId);
      insert_method.SetArgTypes(argTypes);
   }

   auto methodPtr = ut::make_unique<Get>();
   if (!aTemplateArgTypes.empty())
   {
      methodPtr->SetReturnTypeId(aTemplateArgTypes[1].mTypeId);
   }
   AddMethod(std::move(methodPtr));

   if (!aTemplateArgTypes.empty())
   {
      UtStringId     keyType       = aTemplateArgTypes[0].mTypeId;
      UtStringId     valueType     = aTemplateArgTypes[1].mTypeId;
      UtScriptClass* keyClassPtr   = aTypesPtr->GetClass(keyType);
      UtScriptClass* valueClassPtr = aTypesPtr->GetClass(valueType);
      if (keyClassPtr != nullptr && valueClassPtr != nullptr)
      {
         mEqualityComparable = (keyClassPtr->IsBasicType() || keyClassPtr->IsEqualityComparable()) &&
                               (valueClassPtr->IsBasicType() || valueClassPtr->IsEqualityComparable());
         mLessThanComparable = (keyClassPtr->IsBasicType() || keyClassPtr->IsLessThanComparable()) &&
                               (valueClassPtr->IsBasicType() || valueClassPtr->IsLessThanComparable());
      }
   }
}

bool UtScriptMap::EqualTo(void* aLHS, void* aRHS)
{
   return *static_cast<Map*>(aLHS) == *static_cast<Map*>(aRHS);
}

namespace
{
bool ScriptDataPairLessThan(const std::pair<UtScriptData, UtScriptData>& aLhs,
                            const std::pair<UtScriptData, UtScriptData>& aRhs)
{
   std::less<ut::script::Data> cmp;
   return cmp(aLhs.first, aRhs.first) || ((aLhs.first == aRhs.first) && cmp(aLhs.second, aRhs.second));
}
} // namespace

bool UtScriptMap::LessThan(void* aLHS, void* aRHS)
{
   Map* lhs = static_cast<Map*>(aLHS);
   Map* rhs = static_cast<Map*>(aRHS);
   return std::lexicographical_compare(lhs->begin(), lhs->end(), rhs->begin(), rhs->end(), &ScriptDataPairLessThan);
}

void* UtScriptMap::Clone(void* aObjectPtr)
{
   return new Map(*static_cast<Map*>(aObjectPtr));
}

void UtScriptMap::Destroy(void* aObjectPtr)
{
   delete static_cast<Map*>(aObjectPtr);
}

void* UtScriptMap::Create(const UtScriptContext&)
{
   return static_cast<void*>(new Map());
}

std::string UtScriptMap::ToStringN(void* aDataPtr, int aMaxStringSize) const
{
   std::stringstream ss;
   Map*              mapPtr = static_cast<Map*>(aDataPtr);
   ss << '{';
   if (mapPtr != nullptr)
   {
      for (auto i = mapPtr->begin(); i != mapPtr->end(); ++i)
      {
         const UtScriptData& key  = i->first;
         const UtScriptData& data = i->second;
         if (i != mapPtr->begin())
         {
            ss << ", ";
            if (ss.tellp() > aMaxStringSize)
            {
               ss << "...";
               break;
            }
         }
         ss << key << " : " << data;
      }
   }
   ss << '}';
   return ss.str();
}

std::string UtScriptMap::ToString(void* aDataPtr) const
{
   return ToStringN(aDataPtr, 100000);
}

using Map = UtScriptMap::Map;
UT_DEFINE_SCRIPT_METHOD_NO_ARG_CHECK(UtScriptMap, Map, Get, 1, "Object", "Object")
{
   UtScriptMap::Map::const_iterator iter = (*aObjectPtr).find(aVarArgs[0]);
   if (iter != (*aObjectPtr).end())
   {
      aReturnVal.Set(iter->second);
   }
   else
   {
      UtScriptClass* dataClassPtr = aObjectClassPtr->GetContainerDataType();
      if (dataClassPtr != nullptr)
      {
         dataClassPtr->SetDefaultValue(aReturnVal);
      }
      else
      {
         aReturnVal.Clear();
      }
   }
}


//--------------------------------------------------------------
//! UtScriptSet Class
//--------------------------------------------------------------
UtScriptSet::UtScriptSet(const std::string& aClassName, UtScriptTypes* aTypesPtr, const TemplateArgTypes& aTemplateArgTypes)
   : UtScriptClass(aClassName, aTypesPtr, aTemplateArgTypes)
{
   SetClassName("Set");
   UT_SCRIPT_WRAP_CLASS(UtScriptSet::Set);
   SetClassName(aClassName);
   UtStringId thisTypeId = aClassName;
   mConstructible        = true;
   mCloneable            = true;
   mContainer            = true;

   ArgTypes argTypes;

   UT_SCRIPT_METHOD(size_t, Size, ()) { return self.size(); };
   UT_SCRIPT_METHOD(bool, Empty, ()) { return self.empty(); };

   UT_SCRIPT_METHOD(bool, Erase, (const UtScriptData* aKey))
   {
      auto iter = self.find(*aKey);
      if (iter != self.end())
      {
         self.erase(iter);
         return true;
      }
      return false;
   };

   UT_SCRIPT_METHOD(void, Clear, ()) { return self.clear(); };

   //! Returns true if the key is present in the Set
   UT_SCRIPT_METHOD(bool, Exists, (const UtScriptData* aKey)) { return self.find(*aKey) != self.end(); };

   auto& insertMethod = UT_SCRIPT_METHOD(void, Insert, (const UtScriptData* aKey)) { self.insert(*aKey); };
   // Change Insert(Object) to Insert(T2).
   if (!aTemplateArgTypes.empty())
   {
      argTypes.clear();
      argTypes.push_back(aTemplateArgTypes[1].mTypeId);
      insertMethod.SetArgTypes(argTypes);
   }

   std::unique_ptr<UtScriptClass::InterfaceMethod> methodPtr = ut::make_unique<Union>();
   if (!aTemplateArgTypes.empty())
   {
      argTypes.clear();
      argTypes.push_back(thisTypeId);
      methodPtr->SetArgTypes(argTypes);
      methodPtr->SetReturnTypeId(thisTypeId);
   }
   AddMethod(std::move(methodPtr));

   methodPtr = ut::make_unique<Difference>();
   if (!aTemplateArgTypes.empty())
   {
      argTypes.clear();
      argTypes.push_back(thisTypeId);
      methodPtr->SetArgTypes(argTypes);
      methodPtr->SetReturnTypeId(thisTypeId);
   }
   AddMethod(std::move(methodPtr));

   methodPtr = ut::make_unique<Intersection>();
   if (!aTemplateArgTypes.empty())
   {
      argTypes.clear();
      argTypes.push_back(thisTypeId);
      methodPtr->SetArgTypes(argTypes);
      methodPtr->SetReturnTypeId(thisTypeId);
   }
   AddMethod(std::move(methodPtr));

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptSetIterator::SetIterator>, GetIterator, ())
   {
      return ut::make_unique<UtScriptSetIterator::SetIterator>(self);
   };

   auto& insert_method = UT_SCRIPT_VARIADIC_METHOD(void, __Insert, (), UtScriptData)
   {
      for (const auto& arg : aVarArgs)
      {
         self.insert(arg);
      }
   };
   // Change __Insert(Object) to __Insert(T1). Since this container
   // is a map the key (T1) can be of any type.
   if (!aTemplateArgTypes.empty())
   {
      argTypes.clear();
      argTypes.push_back(aTemplateArgTypes[0].mTypeId);
      insert_method.SetArgTypes(argTypes);
   }

   if (!aTemplateArgTypes.empty())
   {
      UtStringId     valueType = aTemplateArgTypes[1].mTypeId;
      UtScriptClass* classPtr  = aTypesPtr->GetClass(valueType);
      if (classPtr != nullptr)
      {
         mEqualityComparable = classPtr->IsBasicType() || classPtr->IsEqualityComparable();
         mLessThanComparable = classPtr->IsBasicType() || classPtr->IsLessThanComparable();
      }
   }
}

void* UtScriptSet::Clone(void* aObjectPtr)
{
   return new Set(*static_cast<Set*>(aObjectPtr));
}

void UtScriptSet::Destroy(void* aObjectPtr)
{
   delete static_cast<Set*>(aObjectPtr);
}

void* UtScriptSet::Create(const UtScriptContext&)
{
   return static_cast<void*>(new Set());
}

std::string UtScriptSet::ToStringN(void* aDataPtr, int aMaxStringSize) const
{
   std::stringstream ss;
   Set*              setPtr = static_cast<Set*>(aDataPtr);
   ss << '{';
   if (setPtr != nullptr)
   {
      for (auto i = setPtr->begin(); i != setPtr->end(); ++i)
      {
         const UtScriptData& data = *i;
         if (i != setPtr->begin())
         {
            ss << ", ";
            if (ss.tellp() >= aMaxStringSize)
            {
               ss << "...";
               break;
            }
         }
         ss << data;
      }
   }
   ss << '}';
   return ss.str();
}

std::string UtScriptSet::ToString(void* aDataPtr) const
{
   return ToStringN(aDataPtr, 100000);
}

bool UtScriptSet::EqualTo(void* aLHS, void* aRHS)
{
   return *static_cast<Set*>(aLHS) == *static_cast<Set*>(aRHS);
}

bool UtScriptSet::LessThan(void* aLHS, void* aRHS)
{
   Set* lhs = static_cast<Set*>(aLHS);
   Set* rhs = static_cast<Set*>(aRHS);
   return std::lexicographical_compare(lhs->begin(), lhs->end(), rhs->begin(), rhs->end());
}

using Set = UtScriptSet::Set;

//! Returns the union of two sets: (this) U (arg)
UT_DEFINE_SCRIPT_METHOD(UtScriptSet, Set, Union, 1, "Set<Object>", "Set<Object>")
{
   // UtScriptData's operator< depends on its const-ness, so cast to const...
   auto* argPtr = aVarArgs[0].GetPointer()->GetAppObject<const UtScriptSet::Set>();
   auto* rValue = new UtScriptSet::Set;
   std::set_union(aObjectPtr->begin(),
                  aObjectPtr->end(),
                  argPtr->begin(),
                  argPtr->end(),
                  std::inserter(*rValue, rValue->begin()));
   // I don't know why the aReturnClassPtr is null here, but we really want aObjectClassPtr anyway...
   aReturnVal.SetPointer(new UtScriptRef(static_cast<void*>(rValue), aObjectClassPtr, UtScriptRef::cMANAGE));
}

//! Returns the difference of two sets: (this) - (arg).
UT_DEFINE_SCRIPT_METHOD(UtScriptSet, Set, Difference, 1, "Set<Object>", "Set<Object>")
{
   // UtScriptData's operator< depends on its const-ness, so cast to const...
   auto* argPtr = aVarArgs[0].GetPointer()->GetAppObject<const UtScriptSet::Set>();
   auto* rValue = new UtScriptSet::Set;
   std::set_difference(aObjectPtr->begin(),
                       aObjectPtr->end(),
                       argPtr->begin(),
                       argPtr->end(),
                       std::inserter(*rValue, rValue->begin()));
   // I don't know why the aReturnClassPtr is null here, but we really want aObjectClassPtr anyway...
   aReturnVal.SetPointer(new UtScriptRef(static_cast<void*>(rValue), aObjectClassPtr, UtScriptRef::cMANAGE));
}

//! Returns the intersection of two sets: (this) n (arg)
UT_DEFINE_SCRIPT_METHOD(UtScriptSet, Set, Intersection, 1, "Set<Object>", "Set<Object>")
{
   // UtScriptData's operator< depends on its const-ness, so cast to const...
   auto* argPtr = aVarArgs[0].GetPointer()->GetAppObject<const UtScriptSet::Set>();
   auto* rValue = new UtScriptSet::Set;
   std::set_intersection(aObjectPtr->begin(),
                         aObjectPtr->end(),
                         argPtr->begin(),
                         argPtr->end(),
                         std::inserter(*rValue, rValue->begin()));
   // I don't know why the aReturnClassPtr is null here, but we really want aObjectClassPtr anyway...
   aReturnVal.SetPointer(new UtScriptRef(static_cast<void*>(rValue), aObjectClassPtr, UtScriptRef::cMANAGE));
}


//--------------------------------------------------------------
//! UtScriptIterator Class
//--------------------------------------------------------------
UtScriptIterator::UtScriptIterator(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   UtScriptClass& scriptClass = *this;
   using MethodBuilder        = ut::script::UtScriptMethodBuilder<UtScriptIterator::Iterator>;

   UT_SCRIPT_METHOD(bool, HasNext, ()) { return self.HasNext(); };
   UT_SCRIPT_METHOD(const UtScriptData&, Next, ()) { return self.Next(); };
}

void UtScriptIterator::Destroy(void* aObjectPtr)
{
   delete static_cast<Iterator*>(aObjectPtr);
}

//--------------------------------------------------------------
//! UtScriptArrayIterator
//--------------------------------------------------------------
UtScriptArrayIterator::UtScriptArrayIterator(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptIterator(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   UtScriptClass& scriptClass = *this;
   using MethodBuilder        = ut::script::UtScriptMethodBuilder<UtScriptArrayIterator::ArrayIterator>;

   AddImplicitCastType("Iterator");

   UT_SCRIPT_METHOD(bool, HasNext, ()) { return self.HasNext(); };
   UT_SCRIPT_METHOD(const UtScriptData&, Next, ()) { return self.Next(); };
   UT_SCRIPT_WRAP_MEMBER("HasPrev", HasPrev);
   UT_SCRIPT_WRAP_MEMBER("Prev", Prev);
   UT_SCRIPT_WRAP_MEMBER("Key", Key);
   UT_SCRIPT_WRAP_MEMBER("Data", Data);
}

void UtScriptArrayIterator::Destroy(void* aObjectPtr)
{
   delete static_cast<ArrayIterator*>(aObjectPtr);
}

UtScriptArrayIterator::ArrayIterator::ArrayIterator(const UtScriptArray::Array& aArray)
   : mArrayRef(aArray)
{
   mCurrentKey.SetInt(-1);
}

bool UtScriptArrayIterator::ArrayIterator::HasNext()
{
   return (mCurrentKey.GetInt() < (static_cast<int>(mArrayRef.size()) - 1));
}

const UtScriptData& UtScriptArrayIterator::ArrayIterator::Next()
{
   if (mCurrentKey.GetInt() < static_cast<int>(mArrayRef.size()))
   {
      mCurrentKey += 1;
      mCurrentData.Set(mArrayRef[mCurrentKey.GetInt()]);
      return mCurrentData;
   }
   return cInvalidData;
}

bool UtScriptArrayIterator::ArrayIterator::HasPrev()
{
   return (mCurrentKey.GetInt() > 0);
}

const UtScriptData& UtScriptArrayIterator::ArrayIterator::Prev()
{
   if (mCurrentKey.GetInt() > 0)
   {
      mCurrentKey -= 1;
      mCurrentData.Set(mArrayRef[mCurrentKey.GetInt()]);
      return mCurrentData;
   }
   return cInvalidData;
}

const UtScriptData& UtScriptArrayIterator::ArrayIterator::Key() const
{
   return mCurrentKey;
}

const UtScriptData& UtScriptArrayIterator::ArrayIterator::Data() const
{
   return mCurrentData;
}

//--------------------------------------------------------------
//! UtScriptMapIterator
//--------------------------------------------------------------
UtScriptMapIterator::UtScriptMapIterator(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptIterator(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   UtScriptClass& scriptClass = *this;
   using MethodBuilder        = ut::script::UtScriptMethodBuilder<UtScriptMapIterator::MapIterator>;

   AddImplicitCastType("Iterator");

   UT_SCRIPT_METHOD(bool, HasNext, ()) { return self.HasNext(); };
   UT_SCRIPT_METHOD(const UtScriptData&, Next, ()) { return self.Next(); };
   UT_SCRIPT_WRAP_MEMBER("HasPrev", HasPrev);
   UT_SCRIPT_WRAP_MEMBER("Prev", Prev);
   UT_SCRIPT_WRAP_MEMBER("Key", Key);
   UT_SCRIPT_WRAP_MEMBER("Data", Data);
}

void UtScriptMapIterator::Destroy(void* aObjectPtr)
{
   delete static_cast<MapIterator*>(aObjectPtr);
}

UtScriptMapIterator::MapIterator::MapIterator(const UtScriptMap::Map& aMap)
   : mMapRef(aMap)
   , mMapIter(aMap.begin())
{
}

bool UtScriptMapIterator::MapIterator::HasNext()
{
   return (mMapIter != mMapRef.end());
}

const UtScriptData& UtScriptMapIterator::MapIterator::Next()
{
   if (mMapIter != mMapRef.end())
   {
      mCurrentKey  = mMapIter->first;
      mCurrentData = mMapIter->second;
      ++mMapIter;
      return mCurrentData;
   }
   return cInvalidData;
}

bool UtScriptMapIterator::MapIterator::HasPrev()
{
   return (mMapIter != mMapRef.begin());
}

const UtScriptData& UtScriptMapIterator::MapIterator::Prev()
{
   if (mMapIter != mMapRef.begin())
   {
      --mMapIter;
      mCurrentKey  = mMapIter->first;
      mCurrentData = mMapIter->second;
      return mCurrentData;
   }
   return cInvalidData;
}

const UtScriptData& UtScriptMapIterator::MapIterator::Key() const
{
   return mCurrentKey;
}

const UtScriptData& UtScriptMapIterator::MapIterator::Data() const
{
   return mCurrentData;
}

//--------------------------------------------------------------
//! UtScriptSetIterator class
//--------------------------------------------------------------
UtScriptSetIterator::UtScriptSetIterator(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptIterator(aClassName, aTypesPtr)
{
   SetClassName(aClassName);

   UtScriptClass& scriptClass = *this;
   using MethodBuilder        = ut::script::UtScriptMethodBuilder<UtScriptSetIterator::SetIterator>;

   AddImplicitCastType("Iterator");

   UT_SCRIPT_METHOD(bool, HasNext, ()) { return self.HasNext(); };
   UT_SCRIPT_METHOD(const UtScriptData&, Next, ()) { return self.Next(); };
   UT_SCRIPT_WRAP_MEMBER("HasPrev", HasPrev);
   UT_SCRIPT_WRAP_MEMBER("Prev", Prev);
   UT_SCRIPT_WRAP_MEMBER("Data", Data);
}

void UtScriptSetIterator::Destroy(void* aObjectPtr)
{
   delete static_cast<SetIterator*>(aObjectPtr);
}

UtScriptSetIterator::SetIterator::SetIterator(const UtScriptSet::Set& aSet)
   : mSetRef(aSet)
   , mSetIter(aSet.begin())
{
}

bool UtScriptSetIterator::SetIterator::HasNext()
{
   return (mSetIter != mSetRef.end());
}

const UtScriptData& UtScriptSetIterator::SetIterator::Next()
{
   if (mSetIter != mSetRef.end())
   {
      mCurrentData = *mSetIter;
      ++mSetIter;
      return mCurrentData;
   }
   return cInvalidData;
}

bool UtScriptSetIterator::SetIterator::HasPrev()
{
   return (mSetIter != mSetRef.begin());
}

const UtScriptData& UtScriptSetIterator::SetIterator::Prev()
{
   if (mSetIter != mSetRef.begin())
   {
      --mSetIter;
      mCurrentData = *mSetIter;
      return mCurrentData;
   }
   return cInvalidData;
}

const UtScriptData& UtScriptSetIterator::SetIterator::Data() const
{
   return mCurrentData;
}

//--------------------------------------------------------------
//! UtScriptVariant Class
//--------------------------------------------------------------
UtScriptVariant::UtScriptVariant(UtScriptTypes* aTypesPtr)
   : UtScriptClass("var", aTypesPtr)
{
   SetClassName("var");
   mBasicType          = false;
   mEqualityComparable = mLessThanComparable = true;
}

bool UtScriptVariant::CanImplicitCast(UtStringId aClassNameId) const
{
   return true;
}

//--------------------------------------------------------------
//! UtScriptBasicType Class
//--------------------------------------------------------------
UtScriptBasicType::UtScriptBasicType(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr, {}, true)
{
   AddImplicitCastType("Object");
}

//--------------------------------------------------------------
//! UtScriptAppPointer
//--------------------------------------------------------------
UtScriptAppPointerClass::UtScriptAppPointerClass(UtScriptTypes* aTypesPtr)
   : UtScriptClass("_AppPointer", aTypesPtr)
{
   SetClassName("_AppPointer");
   mConstructible = false;
}

//--------------------------------------------------------------
//! UtScriptNull
//--------------------------------------------------------------
UtScriptNull::UtScriptNull(UtScriptTypes* aTypesPtr)
   : UtScriptClass("null", aTypesPtr)
{
   SetClassName("null");
   SetClassName("NULL");
   mBasicType          = false;
   mEqualityComparable = mLessThanComparable = true;
}

//--------------------------------------------------------------
//! UtScriptVoid
//--------------------------------------------------------------
UtScriptVoid::UtScriptVoid(UtScriptTypes* aTypesPtr)
   : UtScriptClass("void", aTypesPtr)
{
   SetClassName("void");
   mBasicType = true;
}

//--------------------------------------------------------------
//! UtScriptInt
//--------------------------------------------------------------
UtScriptInt::UtScriptInt(UtScriptTypes* aTypesPtr)
   : UtScriptBasicType("int", aTypesPtr)
{
   SetClassName("int");

   AddImplicitCastType("double");
   AddImplicitCastType("bool");

   AddExplicitCastType("string");
}

void* UtScriptInt::Clone(void* aObjectPtr)
{
   return new int(*static_cast<int*>(aObjectPtr));
}

void UtScriptInt::Destroy(void* aObjectPtr)
{
   delete static_cast<int*>(aObjectPtr);
}

void* UtScriptInt::Create(const UtScriptContext&)
{
   return static_cast<void*>(new int());
}

bool UtScriptInt::IsInt() const
{
   return true;
}

void UtScriptInt::SetDefaultValue(UtScriptData& aData)
{
   aData.SetInt(0);
}

//--------------------------------------------------------------
//! UtScriptDouble
//--------------------------------------------------------------
UtScriptDouble::UtScriptDouble(UtScriptTypes* aTypesPtr)
   : UtScriptBasicType("double", aTypesPtr)
{
   SetClassName("double");

   AddImplicitCastType("int");
   AddImplicitCastType("bool");

   AddExplicitCastType("string");
}

void* UtScriptDouble::Clone(void* aObjectPtr)
{
   return new double(*static_cast<double*>(aObjectPtr));
}

void UtScriptDouble::Destroy(void* aObjectPtr)
{
   delete static_cast<double*>(aObjectPtr);
}

void* UtScriptDouble::Create(const UtScriptContext&)
{
   return static_cast<void*>(new double());
}

bool UtScriptDouble::IsDouble() const
{
   return true;
}

void UtScriptDouble::SetDefaultValue(UtScriptData& aData)
{
   aData.SetDouble(0.0);
}

//--------------------------------------------------------------
//! UtScriptBool
//--------------------------------------------------------------
UtScriptBool::UtScriptBool(UtScriptTypes* aTypesPtr)
   : UtScriptBasicType("bool", aTypesPtr)
{
   SetClassName("bool");

   AddImplicitCastType("double");
   AddImplicitCastType("int");

   AddExplicitCastType("string");
}

void* UtScriptBool::Clone(void* aObjectPtr)
{
   return new bool(*static_cast<bool*>(aObjectPtr));
}

void UtScriptBool::Destroy(void* aObjectPtr)
{
   delete static_cast<bool*>(aObjectPtr);
}

void* UtScriptBool::Create(const UtScriptContext&)
{
   return static_cast<void*>(new bool());
}

bool UtScriptBool::IsBool() const
{
   return true;
}

void UtScriptBool::SetDefaultValue(UtScriptData& aData)
{
   aData.SetBool(false);
}
