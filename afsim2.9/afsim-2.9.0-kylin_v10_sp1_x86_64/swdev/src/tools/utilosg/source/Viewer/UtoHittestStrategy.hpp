// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// UtoHittestStrategy.hpp : header file

#ifndef UTOHITTESTSTRATEGY_HPP
#define UTOHITTESTSTRATEGY_HPP

#include <list>
#include <unordered_map>
#include <unordered_set>

#include "UtoHitInfo.hpp"
#include "UtoTypes.hpp"

class UtoEntity;
// class UtoSymbol;
class UtoViewer;

namespace osgUtil
{
class SceneView;
}

namespace osgViewer
{
class Viewer;
}

class UtoHittestStrategy
{
public:
   // Hittest types
   typedef std::map<UtoShape*, UtoHitInfoList> SymbolHittestList;

   UtoHittestStrategy(UtoViewer* vimp, bool OrthoSelection);
   virtual ~UtoHittestStrategy();

   virtual SymbolHittestList* HitTestSymbol(UtoPoint           pos,
                                            const std::string& symbols,
                                            unsigned int       max,
                                            bool               aScreenScaled,
                                            float              aOrthoScale,
                                            float              aIconScale);

protected:
   // bool ProcessAlphaHit (vtIsectResult &r, vtInt32 &u, vtInt32 &v, vtColor &c);
   class hash_node
   {
   public:
      enum
      {
         bucket_size = 4, // 0 < bucket_size
         min_buckets = 8  // min_buckets = 2 ^^ N, 0 < N
      };
      hash_node() {}
      size_t operator()(const osg::Node* _Keyval) const
      { // just return it
         return (size_t)_Keyval;
      }

      bool operator()(const osg::Node* _Keyval1, const osg::Node* _Keyval2) const
      { // test if _Keyval1 ordered before _Keyval2
         return _Keyval1 < _Keyval2;
      }

      // from include/ResourceDBImp.h
      size_t operator()(const std::string& _Keyval) const
      { // hash _Keyval to size_t value
         size_t nHash = 0;
         auto   size  = _Keyval.size();
         size_t n     = 0;
         while (n < size)
         {
            nHash = (nHash << 5) + nHash + _Keyval[n++];
         }
         return nHash;
      }

      bool operator()(const std::string& _Keyval1, const std::string& _Keyval2) const
      { // test if _Keyval1 ordered before _Keyval2
         return _Keyval1 < _Keyval2;
      }
   };

   using NodeSet       = std::unordered_set<osg::Node*, hash_node>;
   using SymbolNameSet = std::unordered_set<std::string, hash_node, hash_node>;

   void PopulateSymbolName(SymbolNameSet& SymbolName, const std::string& symbols);

protected:
   UtoViewer* m_Viewer;
   /*   osg::ref_ptr<osgUtil::SceneView> m_SceneView;
      osg::ref_ptr<osgViewer::Viewer> m_OSGViewer;*/
   bool m_OrthoSelection;

private:
   UtoHittestStrategy(const UtoHittestStrategy&);
   UtoHittestStrategy& operator=(const UtoHittestStrategy&);
};

#endif // !defined(_UTOHITTESTSTRATEGY_HPP_)
