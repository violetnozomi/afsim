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
#include <string>

#include <gtest/gtest.h>

#include "UtMemory.hpp"
#include "UtReflectConstVisitor.hpp"
#include "UtReflectObject.hpp"
#include "UtReflectVisitor.hpp"

namespace
{
struct TestReflectNode : ut::reflect::Node
{
   TestReflectNode(int aDummy){};
};

using TestObjectNode = ut::reflect::ObjectNode<TestReflectNode>;

struct IntNode : ut::reflect::Derive<IntNode, TestReflectNode>
{
   IntNode(int aValue = 0)
      : ReflectBase(4)
      , value(aValue)
   {
   }

   int value;
};

struct CoordinateNode : ut::reflect::Derive<CoordinateNode, TestObjectNode>
{
   //! Ideally, we could just write `using ReflectBase::ReflectBase;`, but there is a bug in GCC that breaks default
   //! member initializers.
   CoordinateNode(int aDummy)
      : ReflectBase(aDummy)
   {
   }

   Field<IntNode> x{this};
   Field<IntNode> y{this, 6};
};
} // namespace

TEST(UtReflect, Primitive)
{
   IntNode node;

   EXPECT_EQ(node.value, 0);
   EXPECT_EQ(node.NodeTypeCount(), 1);
   EXPECT_EQ(node.GetNodeType(0), typeid(IntNode));
   EXPECT_THROW(node.GetNodeType(1), std::bad_typeid);
}

TEST(UtReflect, Coordinate)
{
   CoordinateNode coord{4};

   EXPECT_EQ(coord.NodeTypeCount(), 2);
   EXPECT_EQ(coord.GetNodeType(0), typeid(CoordinateNode));
   EXPECT_EQ(coord.GetNodeType(1), typeid(TestObjectNode));
   EXPECT_THROW(coord.GetNodeType(2), std::bad_typeid);

   EXPECT_EQ(coord.FieldCount(), 2);

   auto& first  = coord.GetField(0);
   auto& second = coord.GetField(1);

   EXPECT_THROW(coord.GetField(2), std::out_of_range);

   EXPECT_EQ(&second, &coord.GetField(-1));
   EXPECT_EQ(&first, &coord.GetField(-2));

   EXPECT_THROW(coord.GetField(-3), std::out_of_range);

   EXPECT_EQ(&first, coord.x.operator->());
   EXPECT_EQ(&second, coord.y.operator->());

   EXPECT_EQ(coord.x->NodeTypeCount(), 1);
   EXPECT_EQ(coord.y->NodeTypeCount(), 1);
   EXPECT_EQ(coord.x->GetNodeType(0), typeid(IntNode));
   EXPECT_EQ(coord.y->GetNodeType(0), typeid(IntNode));

   EXPECT_EQ(coord.x->value, 0);
   EXPECT_EQ(coord.y->value, 6);
}

namespace
{
using ut::reflect::Node;

struct TestVisitor : public ut::reflect::Visitor<TestVisitor, const Node, std::string()>
{
   std::string Fallback(const ut::reflect::Node& aNode) override { return "!"; }

   std::string IntMemberFunction(const IntNode& aNode) { return "M" + std::to_string(aNode.value); }

   std::string ConstIntMemberFunction(const IntNode& aNode) const { return "m" + std::to_string(aNode.value); }

   std::string ObjectMemberFunction(const TestObjectNode& aObject)
   {
      std::string retval = "O";

      for (int i = 0; i < aObject.FieldCount(); i++)
      {
         retval += Visit(aObject.GetField(i));
      }

      return retval;
   }

   std::string CoordinateMemberFunction(const CoordinateNode& aCoordinate)
   {
      return "CX" + std::to_string(aCoordinate.x->value) + "Y" + std::to_string(aCoordinate.y->value);
   }
};

std::string FreeFunction(const IntNode& aNode)
{
   return "F" + std::to_string(aNode.value);
}

std::string PseudoMemberFunction(TestVisitor&, const IntNode& aNode)
{
   return "P" + std::to_string(aNode.value);
}

std::string ConstPseudoMemberFunction(const TestVisitor&, const IntNode& aNode)
{
   return "p" + std::to_string(aNode.value);
}
} // namespace

TEST(UtReflection, VisitorFunctionTypes)
{
   TestVisitor vis;
   IntNode     node;

   EXPECT_EQ(vis.Visit(node), "!");

   vis.Register<IntNode, &FreeFunction>();
   EXPECT_EQ(vis.Visit(node), "F0");

   vis.Register<IntNode, &PseudoMemberFunction>();
   EXPECT_EQ(vis.Visit(node), "P0");

   vis.Register<IntNode, &TestVisitor::IntMemberFunction>();
   EXPECT_EQ(vis.Visit(node), "M0");

   vis.Register<IntNode, &ConstPseudoMemberFunction>();
   EXPECT_EQ(vis.Visit(node), "p0");

   vis.Register<IntNode, &TestVisitor::ConstIntMemberFunction>();
   EXPECT_EQ(vis.Visit(node), "m0");

   vis.Unregister<IntNode>();
   EXPECT_EQ(vis.Visit(node), "!");
}

TEST(UtReflection, VisitorHierarchy)
{
   TestVisitor    vis;
   IntNode        node{3};
   CoordinateNode coord{4};

   vis.Register<TestObjectNode, &TestVisitor::ObjectMemberFunction>();

   EXPECT_EQ(vis.Visit(node), "!");
   EXPECT_EQ(vis.Visit(coord), "O!!");

   vis.Register<IntNode, &TestVisitor::IntMemberFunction>();

   EXPECT_EQ(vis.Visit(coord), "OM0M6");

   vis.Register<CoordinateNode, &TestVisitor::CoordinateMemberFunction>();

   EXPECT_EQ(vis.Visit(coord), "CX0Y6");

   EXPECT_EQ(vis.VisitAs<IntNode>(coord), "!");
   EXPECT_EQ(vis.VisitAs<TestObjectNode>(coord), "OM0M6");
}


namespace
{
using ut::reflect::Node;

struct TestConstVisitor : public ut::reflect::ConstVisitor<TestConstVisitor, const Node, std::string()>
{
   std::string Fallback(const ut::reflect::Node& aNode) const override { return "!"; }

   std::string IntMemberFunction(const IntNode& aNode) const { return "m" + std::to_string(aNode.value); }

   std::string ObjectMemberFunction(const TestObjectNode& aObject) const
   {
      std::string retval = "O";

      for (int i = 0; i < aObject.FieldCount(); i++)
      {
         retval += Visit(aObject.GetField(i));
      }

      return retval;
   }

   std::string CoordinateMemberFunction(const CoordinateNode& aCoordinate) const
   {
      return "CX" + std::to_string(aCoordinate.x->value) + "Y" + std::to_string(aCoordinate.y->value);
   }
};

std::string FreeFunctionC(const IntNode& aNode)
{
   return "F" + std::to_string(aNode.value);
}

std::string ConstPseudoMemberFunctionC(const TestConstVisitor&, const IntNode& aNode)
{
   return "p" + std::to_string(aNode.value);
}
} // namespace

TEST(UtReflection, ConstVisitorFunctionTypes)
{
   TestConstVisitor vis;
   IntNode          node;

   EXPECT_EQ(vis.Visit(node), "!");

   vis.Register<IntNode, &FreeFunctionC>();
   EXPECT_EQ(vis.Visit(node), "F0");

   vis.Register<IntNode, &ConstPseudoMemberFunctionC>();
   EXPECT_EQ(vis.Visit(node), "p0");

   vis.Register<IntNode, &TestConstVisitor::IntMemberFunction>();
   EXPECT_EQ(vis.Visit(node), "m0");

   vis.Unregister<IntNode>();
   EXPECT_EQ(vis.Visit(node), "!");
}

TEST(UtReflection, ConstVisitorHierarchy)
{
   TestConstVisitor vis;
   IntNode          node{3};
   CoordinateNode   coord{4};

   vis.Register<TestObjectNode, &TestConstVisitor::ObjectMemberFunction>();

   EXPECT_EQ(vis.Visit(node), "!");
   EXPECT_EQ(vis.Visit(coord), "O!!");

   vis.Register<IntNode, &TestConstVisitor::IntMemberFunction>();

   EXPECT_EQ(vis.Visit(coord), "Om0m6");

   vis.Register<CoordinateNode, &TestConstVisitor::CoordinateMemberFunction>();

   EXPECT_EQ(vis.Visit(coord), "CX0Y6");

   EXPECT_EQ(vis.VisitAs<IntNode>(coord), "!");
   EXPECT_EQ(vis.VisitAs<TestObjectNode>(coord), "Om0m6");
}
