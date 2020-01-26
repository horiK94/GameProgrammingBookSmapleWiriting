// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
// MinDistSq between two line segments:
// Copyright 2001 softSurfer, 2012 Dan Sunday
// This code may be freely used, distributed and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
// ----------------------------------------------------------------

#include "Collision.h"
#include <algorithm>
#include <array>

LineSegment::LineSegment(const Vector3& start, const Vector3& end)
	:mStart(start)
	, mEnd(end)
{
}

Vector3 LineSegment::PointOnSegment(float t) const
{
	return mStart + (mEnd - mStart) * t;
}

float LineSegment::MinDistSq(const Vector3& point) const
{
	//// Construct vectors
	//Vector3 ab = mEnd - mStart;
	//Vector3 ba = -1.0f * ab;
	//Vector3 ac = point - mStart;
	//Vector3 bc = point - mEnd;

	//// Case 1: C projects prior to A
	//if (Vector3::Dot(ab, ac) < 0.0f)
	//{
	//	return ac.LengthSq();
	//}
	//// Case 2: C projects after B
	//else if (Vector3::Dot(ba, bc) < 0.0f)
	//{
	//	return bc.LengthSq();
	//}
	//// Case 3: C projects onto line
	//else
	//{
	//	// Compute p
	//	float scalar = Vector3::Dot(ac, ab)
	//		/ Vector3::Dot(ab, ab);
	//	Vector3 p = scalar * ab;
	//	// Compute length squared of ac - p
	//	return (ac - p).LengthSq();
	//}

	//ベクトルpがある直線上のベクトルabを求める
	Vector3 ab = mEnd - mStart;
	Vector3 ac = point - mStart;
	//点bから伸ばすのが最短のときに使用するba, bcも求めておく
	Vector3 ba = -1 * ab;
	Vector3 bc = point - mEnd;
	//ケース1 cがaの前に突き出ている
	if (Vector3::Dot(ac, ab) < 0)
	{
		return ac.LengthSq();
	}
	//ケース2 cがbのあとに突き出ている
	else if (Vector3::Dot(ba, bc) < 0)
	{
		return bc.LengthSq();
	}
	//ケース3 cをabと垂直になる点を調べて計算する
	else
	{
		float dotAcAb = Vector3::Dot(ac, ab);
		float dotAbAb = Vector3::Dot(ab, ab);
		Vector3 p = (dotAcAb / dotAbAb) * ab;
		return (ac - p).LengthSq();
	}
}

float LineSegment::MinDistSq(const LineSegment& s1, const LineSegment& s2)
{
	Vector3   u = s1.mEnd - s1.mStart;
	Vector3   v = s2.mEnd - s2.mStart;
	Vector3   w = s1.mStart - s2.mStart;
	float    a = Vector3::Dot(u, u);         // always >= 0
	float    b = Vector3::Dot(u, v);
	float    c = Vector3::Dot(v, v);         // always >= 0
	float    d = Vector3::Dot(u, w);
	float    e = Vector3::Dot(v, w);
	float    D = a * c - b * b;        // always >= 0
	float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

								   // compute the line parameters of the two closest points
	if (Math::NearZero(D)) { // the lines are almost parallel
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (Math::NearZero(sN) ? 0.0f : sN / sD);
	tc = (Math::NearZero(tN) ? 0.0f : tN / tD);

	// get the difference of the two closest points
	Vector3   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

	return dP.LengthSq();   // return the closest distance squared
}

Plane::Plane(const Vector3& normal, float d)
	:mNormal(normal)
	, mD(d)
{
}

Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c)
{
	//// Compute vectors from a to b and a to c
	//Vector3 ab = b - a;
	//Vector3 ac = c - a;
	//// Cross product and normalize to get normal
	//mNormal = Vector3::Cross(ab, ac);
	//mNormal.Normalize();
	//// d = -P dot n
	//mD = -Vector3::Dot(a, mNormal);
	Vector3 ab = b - a;
	Vector3 ac = c - a;
	//クロス積と正規化で法線を得る
	mNormal = Vector3::Cross(ab, ac);
	mNormal.Normalize();
	// dは点aと法線ベクトルの内積の負
	mD = -1 * Vector3::Dot(a, mNormal);
}

float Plane::SignedDist(const Vector3& point) const
{
	//return Vector3::Dot(point, mNormal) - mD;
	return Vector3::Dot(point, mNormal) - mD;
}

Sphere::Sphere(const Vector3& center, float radius)
	:mCenter(center)
	, mRadius(radius)
{
}

bool Sphere::Contains(const Vector3& point) const
{
	// Get distance squared between center and point
	//float distSq = (mCenter - point).LengthSq();
	//return distSq <= (mRadius * mRadius);
	float distSq = (point - mCenter).LengthSq();
	return distSq <= mRadius * mRadius;
}

AABB::AABB(const Vector3& min, const Vector3& max)
	: mMin(min)
	, mMax(max)
{
}

void AABB::UpdateMinMax(const Vector3& point)
{
	// Update each component separately
	//すべての点に対して最小点、最大点でないかを確認する
	mMin.x = Math::Min(mMin.x, point.x);
	mMin.y = Math::Min(mMin.y, point.y);
	mMin.z = Math::Min(mMin.z, point.z);

	mMax.x = Math::Max(mMax.x, point.x);
	mMax.y = Math::Max(mMax.y, point.y);
	mMax.z = Math::Max(mMax.z, point.z);
}

void AABB::Rotate(const Quaternion& q)
{
	// Construct the 8 points for the corners of the box
	//std::array<Vector3, 8> points;
	//// Min point is always a corner
	//points[0] = mMin;
	//// Permutations with 2 min and 1 max
	//points[1] = Vector3(mMax.x, mMin.y, mMin.z);
	//points[2] = Vector3(mMin.x, mMax.y, mMin.z);
	//points[3] = Vector3(mMin.x, mMin.y, mMax.z);
	//// Permutations with 2 max and 1 min
	//points[4] = Vector3(mMin.x, mMax.y, mMax.z);
	//points[5] = Vector3(mMax.x, mMin.y, mMax.z);
	//points[6] = Vector3(mMax.x, mMax.y, mMin.z);
	//// Max point corner
	//points[7] = Vector3(mMax);

	//// Rotate first point
	//Vector3 p = Vector3::Transform(points[0], q);
	//// Reset min/max to first point rotated
	//mMin = p;
	//mMax = p;
	//// Update min/max based on remaining points, rotated
	//for (size_t i = 1; i < points.size(); i++)
	//{
	//	p = Vector3::Transform(points[i], q);
	//	UpdateMinMax(p);
	//}

	//回転前のバウンディングボックス
	//ボックスの角の8点を構築
	std::array<Vector3, 8> points;
	//最小の点は常に角にある
	points[0] = mMin;
	points[1] = Vector3(mMax.x, mMax.y, mMin.z);
	points[2] = Vector3(mMax.x, mMin.y, mMax.z);
	points[3] = Vector3(mMin.x, mMax.y, mMax.z);
	points[4] = Vector3(mMax.x, mMin.y, mMin.z);
	points[5] = Vector3(mMin.x, mMax.y, mMin.z);
	points[6] = Vector3(mMin.x, mMin.y, mMax.z);
	points[7] = mMax;

	//以下回転後
	Vector3 p = Vector3::Transform(points[0], q);
	//回転後の点で初期化
	mMin = p;
	mMax = p;

	//残りの点で最大値と最小値を更新
	for (size_t i = 1; i < points.size(); i++)
	{
		UpdateMinMax(Vector3::Transform(points[i], q));
	}
}

bool AABB::Contains(const Vector3& point) const
{
	//bool outside = point.x < mMin.x ||
	//	point.y < mMin.y ||
	//	point.z < mMin.z ||
	//	point.x > mMax.x ||
	//	point.y > mMax.y ||
	//	point.z > mMax.z;
	bool outside = point.x < mMin.x ||
		point.x > mMax.x ||
		point.y < mMin.y ||
		point.y > mMax.y ||
		point.z < mMin.z ||
		point.z > mMax.z;
	// If none of these are true, the point is inside the box
	return !outside;
}

float AABB::MinDistSq(const Vector3& point) const
{
	// Compute differences for each axis
	//float dx = Math::Max(mMin.x - point.x, 0.0f);
	//dx = Math::Max(dx, point.x - mMax.x);
	//float dy = Math::Max(mMin.y - point.y, 0.0f);
	//dy = Math::Max(dy, point.y - mMax.y);
	//float dz = Math::Max(mMin.z - point.z, 0.0f);
	//dz = Math::Max(dy, point.z - mMax.z);
	//// Distance squared formula
	//return dx * dx + dy * dy + dz * dz;
	float dx = Math::Max(mMin.x - point.x, 0.0f);		//point.x < mMin.x のときと、 mMin.x <= point.x <= mMax.x のときの比較
	dx = Math::Max(dx, point.x - mMax.x);
	float dy = Math::Max(mMin.y - point.y, 0.0f);
	dy = Math::Max(dy, point.y - mMax.y);
	float dz = Math::Max(mMin.z - point.z, 0.0f);
	dz = Math::Max(dy, point.z - mMax.z);

	return dx * dx + dy * dy + dz * dz;
}

Capsule::Capsule(const Vector3& start, const Vector3& end, float radius)
	:mSegment(start, end)
	, mRadius(radius)
{
}

Vector3 Capsule::PointOnSegment(float t) const
{
	return mSegment.PointOnSegment(t);
}

bool Capsule::Contains(const Vector3& point) const
{
	// Get minimal dist. sq. between point and line segment
	//float distSq = mSegment.MinDistSq(point);
	//return distSq <= (mRadius * mRadius);
	float distSp = mSegment.MinDistSq(point);
	return distSp <= mRadius * mRadius;
}

bool ConvexPolygon::Contains(const Vector2& point) const
{
	//float sum = 0.0f;
	//Vector2 a, b;
	//for (size_t i = 0; i < mVertices.size() - 1; i++)
	//{
	//	// From point to first vertex
	//	a = mVertices[i] - point;
	//	a.Normalize();
	//	// From point to second vertex
	//	b = mVertices[i + 1] - point;
	//	b.Normalize();
	//	// Add angle to sum
	//	sum += Math::Acos(Vector2::Dot(a, b));
	//}
	//// Have to add angle for last vertex and first vertex
	//a = mVertices.back() - point;
	//a.Normalize();
	//b = mVertices.front() - point;
	//b.Normalize();
	//sum += Math::Acos(Vector2::Dot(a, b));
	//// Return true if approximately 2pi
	//return Math::NearZero(sum - Math::TwoPi);

	//平方根とarccosを多用しているため、効率が良くない
	float sum = 0.0f;		//ラジアン合計
	Vector2 a, b;
	for (size_t i = 0; i < mVertices.size() - 1; i++)
	{
		//点から1つ目の頂点へのベクトル
		a = mVertices[i] - point;
		a.Normalize();
		//点から2つ目の頂点へのベクトル
		b = mVertices[i + 1] - point;
		b.Normalize();
		//2つのベクトルが成す角度を合計に加算
		sum += Math::Acos(Vector2::Dot(a, b));
	}
	//最後と最初の成す角度を求める
	a = mVertices[mVertices.size() - 1];
	a.Normalize();
	b = mVertices[0];
	b.Normalize();
	sum += Math::Acos(Vector2::Dot(a, b));

	//合計が約2πならtrueを返す
	return Math::NearZero(sum - Math::TwoPi);
}

bool Intersect(const Sphere& a, const Sphere& b)
{
	//float distSq = (a.mCenter - b.mCenter).LengthSq();
	//float sumRadii = a.mRadius + b.mRadius;
	//return distSq <= (sumRadii * sumRadii);
	float distance = (a.mCenter - b.mCenter).LengthSq();
	float sumRadius = a.mRadius + b.mRadius;
	return distance <= sumRadius * sumRadius;
}

bool Intersect(const AABB& a, const AABB& b)
{
	/*bool no = a.mMax.x < b.mMin.x ||
		a.mMax.y < b.mMin.y ||
		a.mMax.z < b.mMin.z ||
		b.mMax.x < a.mMin.x ||
		b.mMax.y < a.mMin.y ||
		b.mMax.z < a.mMin.z;*/
		// 分離軸定理の応用
		// 凸オブジェクトA, Bが交差しないときは分離する軸が必ず存在する
	bool no = a.mMax.x < b.mMin.x ||
		a.mMax.y < b.mMin.y ||
		a.mMax.z < b.mMin.z ||
		a.mMin.x > b.mMax.x ||
		a.mMin.y > b.mMax.y ||
		a.mMin.z > b.mMax.z;
	// If none of these are true, they must intersect
	return !no;
}

bool Intersect(const OBB& a, const OBB& b)
{
	Vector3 ax = (a.mExtents.x / 2.0f) * Vector3::Normalize(Vector3::Transform(Vector3::UnitX, a.mRotation));
	Vector3 ay = (a.mExtents.y / 2.0f) * Vector3::Normalize(Vector3::Transform(Vector3::UnitY, a.mRotation));
	Vector3 az = (a.mExtents.z / 2.0f) * Vector3::Normalize(Vector3::Transform(Vector3::UnitZ, a.mRotation));
	Vector3 bx = (b.mExtents.x / 2.0f) * Vector3::Normalize(Vector3::Transform(Vector3::UnitX, b.mRotation));
	Vector3 by = (b.mExtents.y / 2.0f) * Vector3::Normalize(Vector3::Transform(Vector3::UnitY, b.mRotation));
	Vector3 bz = (b.mExtents.z / 2.0f) * Vector3::Normalize(Vector3::Transform(Vector3::UnitZ, b.mRotation));

	//分離軸ax
	float ra = ax.Length();
	float rb = Math::Abs(Vector3::Dot(ax, bx)) + Math::Abs(Vector3::Dot(ax, by)) + Math::Abs(Vector3::Dot(ax, bz));
	Vector3 ab = Vector3(b.mCenter - a.mCenter);
	float l = Vector3::Dot(ax, ab);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸ay
	ra = ay.Length();
	rb = Math::Abs(Vector3::Dot(ay, bx)) + Math::Abs(Vector3::Dot(ay, by)) + Math::Abs(Vector3::Dot(ay, bz));
	l = Vector3::Dot(ay, ab);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸az
	ra = az.Length();
	rb = Math::Abs(Vector3::Dot(az, bx)) + Math::Abs(Vector3::Dot(az, by)) + Math::Abs(Vector3::Dot(az, bz));
	l = Vector3::Dot(az, ab);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸bx
	ra = Math::Abs(Vector3::Dot(ax, bx)) + Math::Abs(Vector3::Dot(ay, bx)) + Math::Abs(Vector3::Dot(az, bx));
	rb = bx.Length();
	l = Vector3::Dot(ab, bx);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸by
	ra = Math::Abs(Vector3::Dot(ax, by)) + Math::Abs(Vector3::Dot(ay, by)) + Math::Abs(Vector3::Dot(az, by));
	rb = by.Length();
	l = Vector3::Dot(ab, by);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸bz
	ra = Math::Abs(Vector3::Dot(ax, bz)) + Math::Abs(Vector3::Dot(ay, bz)) + Math::Abs(Vector3::Dot(az, bz));
	rb = bz.Length();
	l = Vector3::Dot(ab, bz);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸 axとbxの垂直ベクトル
	Vector3 c = Vector3::Cross(ax, bx);
	ra = Math::Abs(Vector3::Dot(ay, c)) + Math::Abs(Vector3::Dot(az, c));
	rb = Math::Abs(Vector3::Dot(by, c)) + Math::Abs(Vector3::Dot(bz, c));
	l = Vector3::Dot(ab, c);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸 ayとbxの垂直ベクトル
	c = Vector3::Cross(ay, bx);
	ra = Math::Abs(Vector3::Dot(ax, c)) + Math::Abs(Vector3::Dot(az, c));
	rb = Math::Abs(Vector3::Dot(by, c)) + Math::Abs(Vector3::Dot(bz, c));
	l = Vector3::Dot(ab, c);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸 azとbxの垂直ベクトル
	c = Vector3::Cross(az, bx);
	ra = Math::Abs(Vector3::Dot(ax, c)) + Math::Abs(Vector3::Dot(ay, c));
	rb = Math::Abs(Vector3::Dot(by, c)) + Math::Abs(Vector3::Dot(bz, c));
	l = Vector3::Dot(ab, c);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸 axとbyの垂直ベクトル
	c = Vector3::Cross(ax, by);
	ra = Math::Abs(Vector3::Dot(ay, c)) + Math::Abs(Vector3::Dot(az, c));
	rb = Math::Abs(Vector3::Dot(bx, c)) + Math::Abs(Vector3::Dot(bz, c));
	l = Vector3::Dot(ab, c);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸 ayとbyの垂直ベクトル
	c = Vector3::Cross(ay, by);
	ra = Math::Abs(Vector3::Dot(ax, c)) + Math::Abs(Vector3::Dot(az, c));
	rb = Math::Abs(Vector3::Dot(bx, c)) + Math::Abs(Vector3::Dot(bz, c));
	l = Vector3::Dot(ab, c);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸 azとbyの垂直ベクトル
	c = Vector3::Cross(az, by);
	ra = Math::Abs(Vector3::Dot(ax, c)) + Math::Abs(Vector3::Dot(ay, c));
	rb = Math::Abs(Vector3::Dot(bx, c)) + Math::Abs(Vector3::Dot(bz, c));
	l = Vector3::Dot(ab, c);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸 axとbzの垂直ベクトル
	c = Vector3::Cross(ax, bz);
	ra = Math::Abs(Vector3::Dot(ay, c)) + Math::Abs(Vector3::Dot(az, c));
	rb = Math::Abs(Vector3::Dot(bx, c)) + Math::Abs(Vector3::Dot(by, c));
	l = Vector3::Dot(ab, c);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸 ayとbzの垂直ベクトル
	c = Vector3::Cross(ay, bz);
	ra = Math::Abs(Vector3::Dot(ax, c)) + Math::Abs(Vector3::Dot(az, c));
	rb = Math::Abs(Vector3::Dot(bx, c)) + Math::Abs(Vector3::Dot(by, c));
	l = Vector3::Dot(ab, c);
	if (ra + rb < l)
	{
		return false;
	}

	//分離軸 azとbzの垂直ベクトル
	c = Vector3::Cross(az, bz);
	ra = Math::Abs(Vector3::Dot(ax, c)) + Math::Abs(Vector3::Dot(ay, c));
	rb = Math::Abs(Vector3::Dot(bx, c)) + Math::Abs(Vector3::Dot(by, c));
	l = Vector3::Dot(ab, c);
	if (ra + rb < l)
	{
		return false;
	}

	return true;
}

bool Intersect(const Capsule& a, const Capsule& b)
{
	float distSq = LineSegment::MinDistSq(a.mSegment,
		b.mSegment);
	float sumRadii = a.mRadius + b.mRadius;
	return distSq <= (sumRadii * sumRadii);
}

bool Intersect(const Sphere& s, const AABB& box)
{
	//	float distSq = box.MinDistSq(s.mCenter);		// 球とAABBの距離が半径以下なら範囲内
	//	return distSq <= (s.mRadius * s.mRadius);
	float distSq = box.MinDistSq(s.mCenter);
	return distSq <= s.mRadius * s.mRadius;
}

bool Intersect(const LineSegment& l, const Sphere& s, float& outT)
{
	//// Compute X, Y, a, b, c as per equations
	//Vector3 X = l.mStart - s.mCenter;
	//Vector3 Y = l.mEnd - l.mStart;
	//float a = Vector3::Dot(Y, Y);
	//float b = 2.0f * Vector3::Dot(X, Y);
	//float c = Vector3::Dot(X, X) - s.mRadius * s.mRadius;
	//// Compute discriminant
	//float disc = b * b - 4.0f * a * c;
	//if (disc < 0.0f)
	//{
	//	return false;
	//}
	//else
	//{
	//	disc = Math::Sqrt(disc);
	//	// Compute min and max solutions of t
	//	float tMin = (-b - disc) / (2.0f * a);
	//	float tMax = (-b + disc) / (2.0f * a);
	//	// Check whether either t is within bounds of segment
	//	if (tMin >= 0.0f && tMin <= 1.0f)
	//	{
	//		outT = tMin;
	//		return true;
	//	}
	//	else if (tMax >= 0.0f && tMax <= 1.0f)
	//	{
	//		outT = tMax;
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	// 参考書に沿って、X, Y, a, b, cの値を決定していく
	Vector3 X = l.mStart - s.mCenter;
	Vector3 Y = l.mEnd - l.mStart;

	float a = Vector3::Dot(Y, Y);
	float b = 2 * Vector3::Dot(X, Y);
	float c = Vector3::Dot(X, X); -s.mRadius * s.mRadius;
	//判別式の計算
	float disc = b * b - 4 * a * c;
	if (disc < 0)
	{
		//交点なし
		return false;
	}
	else
	{
		//交点あり
		disc = Math::Sqrt(disc);
		//tの解を求める
		float tMin = (-b - disc) / (2 * a);
		float tMax = (-b + disc) / (2 * a);
		if (tMin >= 0 && tMin <= 1)
		{
			outT = tMin;
			return true;
		}
		else if (tMax >= 0 && tMax <= 1)
		{
			outT = tMax;
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool Intersect(const LineSegment& l, const Plane& p, float& outT)
{
	// First test if there's a solution for t
	//float denom = Vector3::Dot(l.mEnd - l.mStart,
	//	p.mNormal);
	//if (Math::NearZero(denom))
	//{
	//	// The only way they intersect is if start
	//	// is a point on the plane (P dot N) == d
	//	if (Math::NearZero(Vector3::Dot(l.mStart, p.mNormal)
	//		- p.mD))
	//	{
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}
	//else
	//{
	//	float numer = -Vector3::Dot(l.mStart, p.mNormal) -
	//		p.mD;
	//	outT = numer / denom;
	//	// Validate t is within bounds of the line segment
	//	if (outT >= 0.0f && outT <= 1.0f)
	//	{
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	//直線と平面の垂直ベクトルが直行 <=> 直線と平面が平行のときは例外的に求める必要がある
	float denom = Vector3::Dot(l.mEnd - l.mStart, p.mNormal);
	if (Math::NearZero(denom))
	{
		//0ならば、平面上に直線があるかを求める
		//すなわち、l.mStart dot p.mNormal == 0のとき 
		if (Math::NearZero(Vector3::Dot(l.mStart, p.mNormal) + p.mD))
		{
			outT = 0.0f;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		float number = -Vector3::Dot(l.mStart, p.mNormal) - p.mD;
		outT = number / denom;
		return outT >= 0 && outT <= 1;
	}
}

//start, endは線分の始点と終点
//negdは平面の-dを求めるのに使用
//normは平面の法線
bool TestSidePlane(float start, float end, float negd, const Vector3& norm,
	std::vector<std::pair<float, Vector3>>& out)
{
	float denom = end - start;
	if (Math::NearZero(denom))
	{
		return false;
	}
	else
	{
		float numer = -start + negd;
		float t = numer / denom;
		// Test that t is within bounds
		if (t >= 0.0f && t <= 1.0f)
		{
			out.emplace_back(t, norm);
			return true;
		}
		else
		{
			return false;
		}
	}
}

//start, endは線分の始点と終点
//negdは平面の-dを求めるのに使用
bool TestSidePlane(float start, float end, float negd, std::vector<float>& out)
{
	//float denom = end - start;
	//if (Math::NearZero(denom))
	//{
	//	return false;
	//}
	//else
	//{
	//	float numer = -start + negd;
	//	float t = numer / denom;
	//	// Test that t is within bounds
	//	if (t >= 0.0f && t <= 1.0f)
	//	{
	//		out.emplace_back(t, norm);
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	//線分とAABBの交差判定に使用するヘルパー関数
	float demon = end - start;
	if (Math::NearZero(demon))
	{
		//tの値の分母が0になるので、このときは別処理
		//startとendの距離が0ということは線上であることを指す。
		//交差してないからfalseを返してるものと思われる
		return false;
	}
	else
	{
		float number = -start + negd;		//tの値を求める分子
		float t = number / demon;
		//tが範囲内にあるかチェック
		if (t >= 0 && t <= 1)
		{
			out.emplace_back(t);
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool Intersect(const LineSegment& l, const AABB& b, float& outT,
	Vector3& outNorm)
{
	// Vector to save all possible t values, and normals for those sides
	std::vector<std::pair<float, Vector3>> tValues;
	// Test the x planes
	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMin.x, Vector3::NegUnitX,
		tValues);
	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMax.x, Vector3::UnitX,
		tValues);
	// Test the y planes
	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMin.y, Vector3::NegUnitY,
		tValues);
	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMax.y, Vector3::UnitY,
		tValues);
	// Test the z planes
	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMin.z, Vector3::NegUnitZ,
		tValues);
	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMax.z, Vector3::UnitZ,
		tValues);

	// Sort the t values in ascending order
	std::sort(tValues.begin(), tValues.end(), [](
		const std::pair<float, Vector3>& a,
		const std::pair<float, Vector3>& b) {
			return a.first < b.first;
		});
	// Test if the box contains any of these points of intersection
	Vector3 point;
	for (auto& t : tValues)
	{
		point = l.PointOnSegment(t.first);
		if (b.Contains(point))
		{
			outT = t.first;
			outNorm = t.second;
			return true;
		}
	}

	//None of the intersections are within bounds of box
	return false;
}

bool Intersect(const LineSegment& l, const AABB& b, float& outT/*,
	Vector3& outNorm*/)
{
	//	// Vector to save all possible t values, and normals for those sides
	//	std::vector<std::pair<float, Vector3>> tValues;
	//	// Test the x planes
	//	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMin.x, Vector3::NegUnitX,
	//		tValues);
	//	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMax.x, Vector3::UnitX,
	//		tValues);
	//	// Test the y planes
	//	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMin.y, Vector3::NegUnitY,
	//		tValues);
	//	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMax.y, Vector3::UnitY,
	//		tValues);
	//	// Test the z planes
	//	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMin.z, Vector3::NegUnitZ,
	//		tValues);
	//	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMax.z, Vector3::UnitZ,
	//		tValues);
	//
	//	// Sort the t values in ascending order
	//	std::sort(tValues.begin(), tValues.end(), [](
	//		const std::pair<float, Vector3>& a,
	//		const std::pair<float, Vector3>& b) {
	//			return a.first < b.first;
	//		});
	//	// Test if the box contains any of these points of intersection
	//	Vector3 point;
	//	for (auto& t : tValues)
	//	{
	//		point = l.PointOnSegment(t.first);
	//		if (b.Contains(point))
	//		{
	//			outT = t.first;
	//			outNorm = t.second;
	//			return true;
	//		}
	//	}
	//
	//	//None of the intersections are within bounds of box
	//	return false;

		//可能性のあるtを保存する配列
	std::vector<float> tValue;
	//x平面のテスト
	//x平面左側のテスト(線分は交点を求めるためにstartとendのx座標が、dは-min dot <1, 0>から、minXが入る)
	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMin.x, tValue);
	//右側のテスト
	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMax.x, tValue);

	//y平面のテスト
	//下側
	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMin.y, tValue);
	//上側
	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMax.y, tValue);

	//z平面のテスト
	//手前側
	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMin.z, tValue);
	//奥側
	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMax.z, tValue);
	//tの値を小さい順にソート
	std::sort(tValue.begin(), tValue.end());
	//ボックスに交点が含まれるかテスト
	Vector3 point;
	for (float t : tValue)
	{
		//このtは線分上の値である。側面はAABBのサイズではなく、平面としてずっと伸びていくものとして上では求めていたため、側面と線分がぶつかるかを下でチェックする
		point = l.PointOnSegment(t);
		if (b.Contains(point))		//点がAABBに含まれているかのチェック(今回は交差する点を求めようとしているので、AABBの側面図上に位置するかのチェック)
		{
			outT = t;
			return true;
		}
	}
	return false;
}

bool SweptSphere(const Sphere& P0, const Sphere& P1,
	const Sphere& Q0, const Sphere& Q1, float& outT)
{
	//// Compute X, Y, a, b, and c
	//Vector3 X = P0.mCenter - Q0.mCenter;
	//Vector3 Y = P1.mCenter - P0.mCenter -
	//	(Q1.mCenter - Q0.mCenter);
	//float a = Vector3::Dot(Y, Y);
	//float b = 2.0f * Vector3::Dot(X, Y);
	//float sumRadii = P0.mRadius + Q0.mRadius;
	//float c = Vector3::Dot(X, X) - sumRadii * sumRadii;
	//// Solve discriminant
	//float disc = b * b - 4.0f * a * c;
	//if (disc < 0.0f)
	//{
	//	return false;
	//}
	//else
	//{
	//	disc = Math::Sqrt(disc);
	//	// We only care about the smaller solution
	//	outT = (-b - disc) / (2.0f * a);
	//	if (outT >= 0.0f && outT <= 0.0f)
	//	{
	//		return true;
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}
	Vector3 X = P0.mCenter - Q0.mCenter;
	Vector3 Y = (P1.mCenter - P0.mCenter) - (Q1.mCenter - Q0.mCenter);
	float a = Vector3::Dot(Y, Y);
	float b = 2 * Vector3::Dot(X, Y);
	float c = Vector3::Dot(Y, Y) - (P0.mRadius + Q0.mRadius) * (P0.mRadius + Q0.mRadius);
	//判別式
	float disc = b * b - 4 * a * c;
	if (disc < 0)
	{
		//交点なし
		return false;
	}
	else
	{
		//小さい値が交点になる
		disc = Math::Sqrt(disc);
		outT = (-b - disc) / 2 * a;
		return outT >= 0 && outT <= 1;
	}
}
