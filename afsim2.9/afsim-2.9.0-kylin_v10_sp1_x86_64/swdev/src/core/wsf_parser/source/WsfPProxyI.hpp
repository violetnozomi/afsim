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

#ifndef WSFPPROXYI_HPP
#define WSFPPROXYI_HPP

class WsfParseNode;
class WsfPProxyPath;

class WsfPProxyDeserializeTracerI
{
public:
   virtual ~WsfPProxyDeserializeTracerI() {}

   virtual void BeginNode(WsfParseNode* aNodePtr, const WsfPProxyPath& aCurrentPath) = 0;
   virtual void EndNode(WsfParseNode* aNodePtr)                                      = 0;

   virtual void RollbackComplete(WsfParseNode* aNodePtr) = 0;

   enum DeserializeAction
   {
      cDA_CONTINUE,
      cDA_ROLLBACK
   };

   virtual DeserializeAction CanContinue(WsfParseNode* aParentNodePtr, WsfParseNode* aChildNodePtr) = 0;
};

#endif
