// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOVERAGEGRIDACTION_HPP
#define WSFCOVERAGEGRIDACTION_HPP

#include <memory>
#include <utility>

#include "UtMemory.hpp"

namespace wsf
{
namespace coverage
{

class Grid;
class Point;

//! An object that can be invoked for each point in a grid.
class GridAction
{
public:
   virtual ~GridAction() = default;

   virtual void Call(const Grid& aGrid, const Point& aPoint) = 0;
};

//! A convenience class that wraps any callable entity in the GridAction interface.
//!
//! The template parameter can be any callable entity invokable with the
//! signature void(const Grid&, const Point&).
template<typename Callable>
class GridActionT : public GridAction
{
public:
   explicit GridActionT(Callable&& aCallable)
      : mCallable{aCallable}
   {
   }
   ~GridActionT() override = default;

   void Call(const Grid& aGrid, const Point& aPoint) override { mCallable(aGrid, aPoint); }

private:
   Callable mCallable;
};

//! A convenience method for creating GridActions.
//!
//! This is useful when the type of the wrapped object is unknown, which
//! is common for lambdas.
template<typename Callable>
std::unique_ptr<GridAction> CreateGridAction(Callable&& aCallable)
{
   return ut::make_unique<GridActionT<Callable>>(std::forward<Callable>(aCallable));
}

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEGRIDACTION_HPP
