#include <cmath>

#include "gtest/gtest.h"

#include "../src/Vector.h"

TEST(VectorTest, Adding) {
	Vector v1(4,8), v2(-3,5);
	EXPECT_DOUBLE_EQ( 1, (v1+v2).x);
	EXPECT_DOUBLE_EQ(13, (v1+v2).y);
	EXPECT_DOUBLE_EQ( 1, (v2+v1).x);
	EXPECT_DOUBLE_EQ(13, (v2+v1).y);
	EXPECT_DOUBLE_EQ( 7, (v1-v2).x);
	EXPECT_DOUBLE_EQ( 3, (v1-v2).y);
	EXPECT_DOUBLE_EQ(-7, (v2-v1).x);
	EXPECT_DOUBLE_EQ(-3, (v2-v1).y);
	v1 += v2;
	EXPECT_DOUBLE_EQ( 1, v1.x);
	EXPECT_DOUBLE_EQ(13, v1.y);
	v2 -= v1;
	EXPECT_DOUBLE_EQ(-4, v2.x);
	EXPECT_DOUBLE_EQ(-8, v2.y);
	EXPECT_DOUBLE_EQ( 4, -v2.x);
	EXPECT_DOUBLE_EQ( 8, -v2.y);
	EXPECT_DOUBLE_EQ(v1.x, (v1+Vector(0,0)).x);
	EXPECT_DOUBLE_EQ(v1.y, (v1+Vector(0,0)).y);
	EXPECT_DOUBLE_EQ(v1.x, (v1-Vector(0,0)).x);
	EXPECT_DOUBLE_EQ(v1.y, (v1-Vector(0,0)).y);
}

TEST(VectorTest, Multiplying) {
	Vector v(7,8);
	EXPECT_DOUBLE_EQ(  14, (v*2).x);
	EXPECT_DOUBLE_EQ(  16, (v*2).y);
	EXPECT_DOUBLE_EQ(17.5, (v*2.5).x);
	EXPECT_DOUBLE_EQ(  20, (v*2.5).y);
	EXPECT_DOUBLE_EQ(  14, (2*v).x);
	EXPECT_DOUBLE_EQ(  16, (2*v).y);
	EXPECT_DOUBLE_EQ(17.5, (2.5*v).x);
	EXPECT_DOUBLE_EQ(  20, (2.5*v).y);
	EXPECT_DOUBLE_EQ( 3.5, (v/2).x);
	EXPECT_DOUBLE_EQ(   4, (v/2).y);
	EXPECT_DOUBLE_EQ(1.75, (v/4).x);
	EXPECT_DOUBLE_EQ(   2, (v/4).y);
	v *= 2;
	EXPECT_DOUBLE_EQ(  14, v.x);
	EXPECT_DOUBLE_EQ(  16, v.y);
	v *= 1.5;
	EXPECT_DOUBLE_EQ(  21, v.x);
	EXPECT_DOUBLE_EQ(  24, v.y);
	v /= 2;
	EXPECT_DOUBLE_EQ(10.5, v.x);
	EXPECT_DOUBLE_EQ(  12, v.y);
}

TEST(VectorTest, Size) {
	Vector v1(3,4);
	EXPECT_DOUBLE_EQ(  5, v1.size());
	EXPECT_DOUBLE_EQ( 25, v1.sizeSqr());
	v1.x = -3;
	EXPECT_DOUBLE_EQ(  5, v1.size());
	EXPECT_DOUBLE_EQ( 25, v1.sizeSqr());
	v1.y = -4;
	EXPECT_DOUBLE_EQ(  5, v1.size());
	EXPECT_DOUBLE_EQ( 25, v1.sizeSqr());
	v1.x = 3;
	EXPECT_DOUBLE_EQ(  5, v1.size());
	EXPECT_DOUBLE_EQ( 25, v1.sizeSqr());

	Vector v2(7,7);
	EXPECT_DOUBLE_EQ(7*sqrt(2), v2.size());
	EXPECT_DOUBLE_EQ(pow(7*sqrt(2), 2), v2.sizeSqr());

	Vector v3(0,0);
	EXPECT_DOUBLE_EQ(0, v3.size());
	EXPECT_DOUBLE_EQ(0, v3.sizeSqr());
}

TEST(VectorTest, Dir) {
	{
		Vector v1(0,1), v2(0,-1), v3(1,0), v4(-1,0), v5(-1,-0.f);
		EXPECT_DOUBLE_EQ( M_PI/2, v1.dir());
		EXPECT_DOUBLE_EQ(-M_PI/2, v2.dir());
		EXPECT_DOUBLE_EQ(      0, v3.dir());
		EXPECT_DOUBLE_EQ(   M_PI, v4.dir());
		EXPECT_DOUBLE_EQ(  -M_PI, v5.dir());
	}
	{
		Vector v1(1,1), v2(-1,1), v3(1,-1), v4(-1,-1);
		EXPECT_DOUBLE_EQ(M_PI/4   , v1.dir());
		EXPECT_DOUBLE_EQ(M_PI*3/4 , v2.dir());
		EXPECT_DOUBLE_EQ(-M_PI/4  , v3.dir());
		EXPECT_DOUBLE_EQ(-M_PI*3/4, v4.dir());
	}
	{
		Vector v(734,195);
		EXPECT_DOUBLE_EQ(atan(195.0/734), v.dir());
	}
}

TEST(VectorTest, Product) {
	Vector v1(7,3), v2(9,4);
	EXPECT_DOUBLE_EQ( 75, v1.dot(v2));
	EXPECT_DOUBLE_EQ( 75, v2.dot(v1));
	EXPECT_DOUBLE_EQ(  1, v1.cross(v2));
	EXPECT_DOUBLE_EQ( -1, v2.cross(v1));
}

static void checkSinCos(Vector v1, Vector v2) {
	double d1 = v1.dir(), d2 = v2.dir();
	double dd = fabs(d1-d2);
	if (dd > M_PI) dd = 2*M_PI-dd;
	EXPECT_NEAR(sin(dd), v1.sin(v2), 1e-10);
	EXPECT_NEAR(cos(dd), v1.cos(v2), 1e-10);
}

TEST(VectorTest, SinCos) {
	Vector v1(3,7), v2(-2,7), v3(-3,8), v4(-12,-9);
	checkSinCos(v1, v2);
	checkSinCos(v1, v3);
	checkSinCos(v1, v4);
	checkSinCos(v2, v3);
	checkSinCos(v2, v4);
	checkSinCos(v3, v4);
}

static void checkUnit(Vector v) {
	ASSERT_DOUBLE_EQ(1, v.unit().size());
	double d = v.dir();
	ASSERT_DOUBLE_EQ(d, v.unit().dir());
	v.normalize();
	ASSERT_DOUBLE_EQ(1, v.size());
	ASSERT_DOUBLE_EQ(d, v.dir());
}

TEST(VectorTest, Unit) {
	Vector v1(1,0), v2(-1,0), v3(6,14), v4(-155,-824), v5(sqrt(2)/2, sqrt(2)/2);
	checkUnit(v1);
	checkUnit(v2);
	checkUnit(v3);
	checkUnit(v4);
	checkUnit(v5);
	EXPECT_DOUBLE_EQ(v1.x, v1.unit().x);
	EXPECT_DOUBLE_EQ(v1.y, v1.unit().y);
	EXPECT_DOUBLE_EQ(v2.x, v2.unit().x);
	EXPECT_DOUBLE_EQ(v2.y, v2.unit().y);
	EXPECT_DOUBLE_EQ(v5.x, v5.unit().x);
	EXPECT_DOUBLE_EQ(v5.y, v5.unit().y);
}

void checkDirection(double d) {
	Vector v = Vector::direction(d);
	ASSERT_DOUBLE_EQ(1, v.size());
	ASSERT_DOUBLE_EQ(d, v.dir());
}

TEST(VectorTest, Direction) {
	double d[] = {0, M_PI/2, -M_PI/2, -M_PI*3/4, 0.59383, -1.39485};
	for (size_t i = 0; i < (sizeof d)/(sizeof d[0]); ++i) checkDirection(d[i]);
}
