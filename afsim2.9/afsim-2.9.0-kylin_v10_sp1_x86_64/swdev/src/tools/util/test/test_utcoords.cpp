// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <gtest/gtest.h>

#include "UtCoords.hpp"


namespace ut_coords_tests
{
using namespace ut::coords;

// Ensure various convertibility conditions hold.
// The following tests should be reenabled when we remove the deprecated implicit conversion.
//static_assert(!std::is_convertible<UtVec3d, WCS>::value,
//              "UtVec3d was able to implicitly convert to WCS");
//static_assert(!std::is_convertible<WCS, UtVec3d>::value,
//              "WCS was able to implicitly convert to UtVec3d");

static_assert(std::is_convertible<WCS, UtVec3d>::value, "WCS was unable to implicitly convert to UtVec3d");

static_assert(std::is_constructible<UtVec3d, WCS>::value, "UtVec3d was unable to explicitly convert to WCS");
static_assert(std::is_constructible<WCS, UtVec3d>::value, "WCS was unable to explicitly convert to UtVec3d");

template<typename T>
class Operations
{
public:
   static void test_constructor()
   {
      {
         UtVec3d vec1(1.0, 3.0, 0.0);
         T       vec2(1.0, 3.0, 0.0);
         EXPECT_EQ(vec1, UtVec3d(vec2));
      }

      {
         UtVec3d vec1(1.0, 3.0, 0.0);
         T       vec2(vec1);
         EXPECT_EQ(vec1, UtVec3d(vec2));
      }

      {
         double  arr[3] = {1.0, 3.0, 0.0};
         UtVec3d vec1(arr);
         T       vec2(arr);
         EXPECT_EQ(vec1, UtVec3d(vec2));
      }

      {
         UtVec3d vec1(1.0);
         T       vec2(1.0);
         EXPECT_EQ(vec1, UtVec3d(vec2));
      }
   }

   static void test_compare()
   {
      T a(1.0, 3.0, 0.0);
      T b(1.0, 3.0, 0.0);
      T c(0.0, 3.0, 0.0);
      EXPECT_EQ(a, b);
      EXPECT_NE(a, c);
   }

   static void test_add()
   {
      UtVec3d av(1.0, 3.0, 0.0);
      UtVec3d bv(3.0, 1.0, 0.0);

      EXPECT_EQ(av + bv, UtVec3d(T(av) + T(bv)));
      EXPECT_EQ(av + 3, UtVec3d(T(av) + 3));
      EXPECT_EQ(T(av) + 3, 3 + T(av));
      {
         T pleq(av);
         pleq += T(bv);
         EXPECT_EQ(av + bv, UtVec3d(pleq));
      }

      {
         T pleq(av);
         pleq += 2.0;
         EXPECT_EQ(av + 2.0, UtVec3d(pleq));
      }

      {
         T pleq(av);
         pleq.Add(T(bv));
         EXPECT_EQ(av + bv, UtVec3d(pleq));
      }

      {
         T pleq(av);
         pleq.Add(2.0);
         EXPECT_EQ(av + 2.0, UtVec3d(pleq));
      }

      {
         T pleq;
         pleq.Add(T(av), T(bv));
         EXPECT_EQ(av + bv, UtVec3d(pleq));
      }

      {
         T pleq;
         pleq.Add(T(av), 2);
         EXPECT_EQ(av + 2, UtVec3d(pleq));
      }
   }

   static void test_sub()
   {
      UtVec3d av(1.0, 3.0, 0.0);
      UtVec3d bv(3.0, 1.0, 0.0);

      EXPECT_EQ(av - bv, UtVec3d(T(av) - T(bv)));
      EXPECT_EQ(av - 3, UtVec3d(T(av) - 3));
      EXPECT_EQ(T(av) - 3, -(3 - T(av)));

      {
         T pleq(av);
         pleq -= T(bv);
         EXPECT_EQ(av - bv, UtVec3d(pleq));
      }

      {
         T pleq(av);
         pleq -= 2.0;
         EXPECT_EQ(av - 2.0, UtVec3d(pleq));
      }

      {
         T pleq(av);
         pleq.Subtract(T(bv));
         EXPECT_EQ(av - bv, UtVec3d(pleq));
      }

      {
         T pleq(av);
         pleq.Subtract(2.0);
         EXPECT_EQ(av - 2.0, UtVec3d(pleq));
      }

      {
         T pleq;
         pleq.Subtract(T(av), T(bv));
         EXPECT_EQ(av - bv, UtVec3d(pleq));
      }

      {
         T pleq;
         pleq.Subtract(T(av), 2);
         EXPECT_EQ(av - 2, UtVec3d(pleq));
      }
   }

   static void test_neg()
   {
      UtVec3d av(1.0, 3.0, 0.0);

      EXPECT_EQ(-av, UtVec3d(-T(av)));
      //EXPECT_EQ(3 - av, UtVec3d(3 - T(av)));
   }

   static void test_mul()
   {
      UtVec3d av(1.0, 3.0, 0.0);

      EXPECT_EQ(2.0 * av, UtVec3d(2.0 * T(av)));
      EXPECT_EQ(av * 2.0, UtVec3d(T(av) * 2.0));

      {
         T pleq(av);
         pleq *= 2.0;
         EXPECT_EQ(av * 2.0, UtVec3d(pleq));
      }

      {
         T pleq(av);
         pleq.Multiply(2.0);
         EXPECT_EQ(av * 2.0, UtVec3d(pleq));
      }

      {
         T pleq;
         pleq.Multiply(T(av), 2);
         EXPECT_EQ(av * 2, UtVec3d(pleq));
      }
   }

   static void test_div()
   {
      UtVec3d av(1.0, 3.0, 0.0);

      EXPECT_EQ(av / 2.0, UtVec3d(T(av) / 2.0));
      {
         T pleq(av);
         pleq /= 2.0;
         EXPECT_EQ(av / 2.0, UtVec3d(pleq));
      }

      {
         T pleq(av);
         pleq.Divide(2.0);
         EXPECT_EQ(av / 2.0, UtVec3d(pleq));
      }
   }

   static void test_dot()
   {
      UtVec3d av(1.0, 0.0, 0.0);
      UtVec3d bv(0.0, 1.0, 0.0);

      EXPECT_EQ(av.DotProduct(bv), T(av).DotProduct(T(bv)));
   }

   static void test_angle()
   {
      UtVec3d av(1.0, 0.0, 0.0);
      UtVec3d bv(0.0, 1.0, 0.0);

      EXPECT_EQ(av.AngleWith(bv), T(av).AngleWith(T(bv)));
   }

   static void test_cross()
   {
      UtVec3d av(1.0, 0.0, 0.0);
      UtVec3d bv(0.0, 1.0, 0.0);

      EXPECT_EQ(UtVec3d().CrossProduct(av, bv), UtVec3d(T().CrossProduct(T(av), T(bv))));
   }

   static void test_mag()
   {
      UtVec3d av(1.0, 3.0, 0.0);

      EXPECT_EQ(av.Magnitude(), T(av).Magnitude());
      EXPECT_EQ(av.MagnitudeSquared(), T(av).MagnitudeSquared());
   }

   static void test_norm()
   {
      {
         UtVec3d av(1.0, 3.0, 0.0);
         T       out = T(av);
         EXPECT_EQ(av.Normalize(), out.Normalize());
         EXPECT_EQ(av, UtVec3d(out));
      }

      {
         UtVec3d av(1.0, 3.0, 0.0);
         UtVec3d bv;
         T       out;
         EXPECT_EQ(av.Normalize(bv), T(av).Normalize(out));
         EXPECT_EQ(bv, UtVec3d(out));
      }

      {
         UtVec3d av(1.0, 3.0, 0.0);
         EXPECT_EQ(av.GetNormal(), UtVec3d(T(av).GetNormal()));
      }
   }
};

TEST(COORDINATE_WRAPPER_TESTS, WCS_CONSTRUCTOR)
{
   Operations<WCS>::test_constructor();
}

TEST(COORDINATE_WRAPPER_TESTS, WCS_OPS)
{
   Operations<WCS>::test_compare();
   Operations<WCS>::test_add();
   Operations<WCS>::test_neg();
   Operations<WCS>::test_sub();
   Operations<WCS>::test_mul();
   Operations<WCS>::test_div();
}

TEST(COORDINATE_WRAPPER_TESTS, WCS_VEC_OPS)
{
   Operations<WCS>::test_dot();
   Operations<WCS>::test_angle();
   Operations<WCS>::test_cross();
   Operations<WCS>::test_mag();
   Operations<WCS>::test_norm();
}

TEST(COORDINATE_WRAPPER_TESTS, NED_CONSTRUCTOR)
{
   Operations<NED>::test_constructor();
}

TEST(COORDINATE_WRAPPER_TESTS, NED_OPS)
{
   Operations<NED>::test_compare();
   Operations<NED>::test_add();
   Operations<NED>::test_neg();
   Operations<NED>::test_sub();
   Operations<NED>::test_mul();
   Operations<NED>::test_div();
}

TEST(COORDINATE_WRAPPER_TESTS, NED_VEC_OPS)
{
   Operations<NED>::test_dot();
   Operations<NED>::test_angle();
   Operations<NED>::test_cross();
   Operations<NED>::test_mag();
   Operations<NED>::test_norm();
}

TEST(COORDINATE_WRAPPER_TESTS, ECI_CONSTRUCTOR)
{
   Operations<ECI>::test_constructor();
}

TEST(COORDINATE_WRAPPER_TESTS, ECI_OPS)
{
   Operations<ECI>::test_compare();
   Operations<ECI>::test_add();
   Operations<ECI>::test_neg();
   Operations<ECI>::test_sub();
   Operations<ECI>::test_mul();
   Operations<ECI>::test_div();
}

TEST(COORDINATE_WRAPPER_TESTS, ECI_VEC_OPS)
{
   Operations<ECI>::test_dot();
   Operations<ECI>::test_angle();
   Operations<ECI>::test_cross();
   Operations<ECI>::test_mag();
   Operations<ECI>::test_norm();
}

TEST(COORDINATE_WRAPPER_TESTS, ECS_CONSTRUCTOR)
{
   Operations<ECS>::test_constructor();
}

TEST(COORDINATE_WRAPPER_TESTS, ECS_OPS)
{
   Operations<ECS>::test_compare();
   Operations<ECS>::test_add();
   Operations<ECS>::test_neg();
   Operations<ECS>::test_sub();
   Operations<ECS>::test_mul();
   Operations<ECS>::test_div();
}

TEST(COORDINATE_WRAPPER_TESTS, ECS_VEC_OPS)
{
   Operations<ECS>::test_dot();
   Operations<ECS>::test_angle();
   Operations<ECS>::test_cross();
   Operations<ECS>::test_mag();
   Operations<ECS>::test_norm();
}

TEST(COORDINATE_WRAPPER_TESTS, ENU_CONSTRUCTOR)
{
   Operations<ENU>::test_constructor();
}

TEST(COORDINATE_WRAPPER_TESTS, ENU_OPS)
{
   Operations<ENU>::test_compare();
   Operations<ENU>::test_add();
   Operations<ENU>::test_neg();
   Operations<ENU>::test_sub();
   Operations<ENU>::test_mul();
   Operations<ENU>::test_div();
}

TEST(COORDINATE_WRAPPER_TESTS, ENU_VEC_OPS)
{
   Operations<ENU>::test_dot();
   Operations<ENU>::test_angle();
   Operations<ENU>::test_cross();
   Operations<ENU>::test_mag();
   Operations<ENU>::test_norm();
}

TEST(COORDINATE_WRAPPER_TESTS, PCS_CONSTRUCTOR)
{
   Operations<PCS>::test_constructor();
}

TEST(COORDINATE_WRAPPER_TESTS, PCS_OPS)
{
   Operations<PCS>::test_compare();
   Operations<PCS>::test_add();
   Operations<PCS>::test_neg();
   Operations<PCS>::test_sub();
   Operations<PCS>::test_mul();
   Operations<PCS>::test_div();
}

TEST(COORDINATE_WRAPPER_TESTS, PCS_VEC_OPS)
{
   Operations<PCS>::test_dot();
   Operations<PCS>::test_angle();
   Operations<PCS>::test_cross();
   Operations<PCS>::test_mag();
   Operations<PCS>::test_norm();
}

} // namespace ut_coords_tests
