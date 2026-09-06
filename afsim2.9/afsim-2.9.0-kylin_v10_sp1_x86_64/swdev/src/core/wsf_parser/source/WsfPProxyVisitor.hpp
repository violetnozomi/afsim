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

#ifndef WSFPPROXYVISITOR_HPP
#define WSFPPROXYVISITOR_HPP

#include <vector>

#include "WsfPProxyList.hpp"
#include "WsfPProxyNode.hpp"
#include "WsfPProxyObjectMap.hpp"
#include "WsfPProxyStructType.hpp"
#include "WsfPProxyValue.hpp"

class WsfPProxyVisitor
{
public:
   //! Walk the proxy value tree visiting each value.
   //! aFunc should be callable with this signature:
   //!   bool  aFunc(WsfPProxyValue aValue, WsfPProxyValue aParent);
   //!   'false' is returned to prevent descending to children of aValue.
   template<typename FUNC>
   static void Visit(FUNC& aFunc, WsfPProxyValue aValue, WsfPProxyValue aParent = WsfPProxyValue())
   {
      std::vector<std::pair<WsfPProxyValue, WsfPProxyValue>> stack;
      stack.push_back(std::make_pair(aParent, aValue));
      while (!stack.empty())
      {
         auto top = stack.back();
         stack.pop_back();
         WsfPProxyValue& val = top.second;
         if (aFunc(val, top.first))
         {
            const WsfPProxyType* typePtr = top.second.GetType();
            switch (typePtr->mTypeKind)
            {
            case WsfProxy::cLIST:
            {
               WsfPProxyList* listPtr = top.second.GetList();
               if (listPtr)
               {
                  for (size_t i = 0; i < listPtr->Size(); ++i)
                  {
                     stack.push_back(std::make_pair(val, listPtr->Get(i)));
                  }
               }
            }
            break;
            case WsfProxy::cSTRUCT:
            {
               if (!val.IsUnset())
               {
                  const WsfPProxyStructType* structPtr   = (const WsfPProxyStructType*)typePtr;
                  size_t                     memberCount = structPtr->GetMemberCount();
                  for (size_t i = 0; i < memberCount; ++i)
                  {
                     stack.push_back(std::make_pair(val, structPtr->GetAtIndex(val.GetDataPtr(), i)));
                  }
               }
            }
            break;
            case WsfProxy::cOBJECT_MAP:
            {
               WsfPProxyObjectMap* mapPtr = val.GetObjectMap();
               if (mapPtr)
               {
                  auto& vals = mapPtr->GetValues();
                  for (auto i = vals.begin(); i != vals.end(); ++i)
                  {
                     stack.push_back(std::make_pair(val, i->second));
                  }
               }
            }
            break;
            }
         }
      }
   }

   //! Walk the proxy value tree visiting each node.
   //! aFunc should be callable with this signature:
   //!   bool aFunc(WsfPProxyNode& aNode);
   //!   'false' is returned to prevent descending to children of aValue.
   template<typename FUNC>
   static void VisitNodes(FUNC& aFunc, WsfPProxyNode aNode)
   {
      if (aFunc(aNode))
      {
         const WsfPProxyValue nodeValue = aNode.GetValue();
         const WsfPProxyType* typePtr   = nodeValue.GetType();

         switch (typePtr->mTypeKind)
         {
         case WsfProxy::cLIST:
         {
            WsfPProxyList* listPtr = nodeValue.GetList();

            if (listPtr)
            {
               for (size_t i = 0; i < listPtr->Size(); ++i)
               {
                  aNode += i;
                  VisitNodes(aFunc, aNode);
                  aNode.ToParent();
               }
            }

            break;
         }

         case WsfProxy::cSTRUCT:
         {
            if (!nodeValue.IsUnset())
            {
               const WsfPProxyStructType* structPtr   = (const WsfPProxyStructType*)typePtr;
               size_t                     memberCount = structPtr->GetMemberCount();

               for (size_t i = 0; i < memberCount; ++i)
               {
                  aNode += i;
                  VisitNodes(aFunc, aNode);
                  aNode.ToParent();
               }
            }

            break;
         }

         case WsfProxy::cOBJECT_MAP:
         {
            WsfPProxyObjectMap* mapPtr = nodeValue.GetObjectMap();

            if (mapPtr)
            {
               auto& vals = mapPtr->GetValues();

               for (auto i = vals.begin(); i != vals.end(); ++i)
               {
                  const std::string& valName = i->first;
                  aNode += valName;
                  VisitNodes(aFunc, aNode);
                  aNode.ToParent();
               }
            }

            break;
         }
         }
      }
   }
};

#endif
