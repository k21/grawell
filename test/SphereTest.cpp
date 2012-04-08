#include <cmath>

#include "gtest/gtest.h"

#include "../src/Sphere.h"

TEST(SphereTest, Constructor) {
	Sphere s1(Point(1,4), 7);
	EXPECT_DOUBLE_EQ(s1.center.x, 1);
	EXPECT_DOUBLE_EQ(s1.center.y, 4);
	EXPECT_DOUBLE_EQ(s1.radius,   7);
	Sphere s2(Point(-6,15), 9);
	EXPECT_DOUBLE_EQ(s2.center.x, -6);
	EXPECT_DOUBLE_EQ(s2.center.y, 15);
	EXPECT_DOUBLE_EQ(s2.radius,    9);
}

TEST(SphereTest, IntersectsSphere) {
	{
		Sphere s1(Point(0,0),4), s2(Point(10,0),5);
		Sphere s3(Point(10,0),6), s4(Point(10,0),7);
		EXPECT_FALSE(s1.intersects(s2));
		EXPECT_TRUE (s1.intersects(s3));
		EXPECT_TRUE (s1.intersects(s4));
		EXPECT_FALSE(s2.intersects(s1));
		EXPECT_TRUE (s3.intersects(s1));
		EXPECT_TRUE (s4.intersects(s1));
		EXPECT_TRUE (s2.intersects(s4));
		EXPECT_TRUE (s4.intersects(s2));
	}
	{
		Sphere s1(Point(2,2),3), s2(Point(5,-2),1);
		Sphere s3(Point(5,-2),2), s4(Point(5,-2),3);
		EXPECT_FALSE(s1.intersects(s2));
		EXPECT_TRUE (s1.intersects(s3));
		EXPECT_TRUE (s1.intersects(s4));
		EXPECT_FALSE(s2.intersects(s1));
		EXPECT_TRUE (s3.intersects(s1));
		EXPECT_TRUE (s4.intersects(s1));
		EXPECT_TRUE (s2.intersects(s4));
		EXPECT_TRUE (s4.intersects(s2));
	}
}

TEST(SphereTest, IntersectsPoint) {
	Sphere s1(Point(2,2),5);
	EXPECT_TRUE (s1.intersects(Point(2,2)));
	EXPECT_TRUE (s1.intersects(Point(6,-1)));
	EXPECT_TRUE (s1.intersects(Point(2,7)));
	EXPECT_TRUE (s1.intersects(Point(2,-3)));
	EXPECT_TRUE (s1.intersects(Point(4,4)));
	EXPECT_FALSE(s1.intersects(Point(6,-2)));
	EXPECT_FALSE(s1.intersects(Point(12,34)));
	EXPECT_FALSE(s1.intersects(Point(2,7.01)));
}
