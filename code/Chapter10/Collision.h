// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Math.h"
#include <vector>

struct LineSegment
{
	LineSegment(const Vector3& start, const Vector3& end);
	// Get point along segment where 0 <= t <= 1
	Vector3 PointOnSegment(float t) const;
	// Get minimum distance squared between point and line segment
	float MinDistSq(const Vector3& point) const;
	// Get MinDistSq between two line segments
	static float MinDistSq(const LineSegment& s1, const LineSegment& s2);

	//始点
	Vector3 mStart;
	//終点
	Vector3 mEnd;
};

struct Plane
{
	Plane(const Vector3& normal, float d);
	// Construct plane from three points
	//3つの点から平面を構築(三角形は必ずその三角形が乗っている平面が存在するという定義より)
	Plane(const Vector3& a, const Vector3& b, const Vector3& c);
	// Get the signed distance between the point and the plane
	float SignedDist(const Vector3& point) const;

	//平面の法線ベクトル
	Vector3 mNormal;
	//平面と原点の距離
	float mD;
};

struct Sphere
{
	Sphere(const Vector3& center, float radius);
	bool Contains(const Vector3& point) const;

	//中心
	Vector3 mCenter;
	//半径
	float mRadius;
};

struct AABB
{
	AABB(const Vector3& min, const Vector3& max);
	// Update min and max accounting for this point
	// (used when loading a model)
	void UpdateMinMax(const Vector3& point);
	// Rotated by a quaternion
	void Rotate(const Quaternion& q);
	bool Contains(const Vector3& point) const;
	float MinDistSq(const Vector3& point) const;

	Vector3 mMin;
	Vector3 mMax;
};

struct OBB
{
	//中心
	Vector3 mCenter;
	//回転
	Quaternion mRotation;
	//幅
	Vector3 mExtents;
};

//カプセル
struct Capsule
{
	Capsule(const Vector3& start, const Vector3& end, float radius);
	// Get point along segment where 0 <= t <= 1
	Vector3 PointOnSegment(float t) const;
	bool Contains(const Vector3& point) const;

	//線分
	LineSegment mSegment;
	//円の半径
	float mRadius;
};

struct ConvexPolygon
{
	bool Contains(const Vector2& point) const;
	// Vertices have a clockwise ordering
	//頂点の順序は時計回り
	std::vector<Vector2> mVertices;
};

// Intersection functions
//当たり判定調査関数(globalであるため、他のクラスではIntersect()だけで使用可能)
bool Intersect(const Sphere& a, const Sphere& b);
bool Intersect(const AABB& a, const AABB& b);
bool Intersect(const OBB& a, const OBB& b);
bool Intersect(const Capsule& a, const Capsule& b);
bool Intersect(const Sphere& s, const AABB& box);

bool Intersect(const LineSegment& l, const Sphere& s, float& outT);
bool Intersect(const LineSegment& l, const Plane& p, float& outT);
bool Intersect(const LineSegment& l, const AABB& b, float& outT);
bool Intersect(const LineSegment& l, const AABB& b, float& outT,
	Vector3& outNorm);

bool SweptSphere(const Sphere& P0, const Sphere& P1,
	const Sphere& Q0, const Sphere& Q1, float& t);
