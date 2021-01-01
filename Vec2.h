#pragma once
#include <cmath>
#include <iostream>

/***********************************************************
* Vec2 Class
***********************************************************/
template <class T>
class Vec2
{

public:
  T e[2];

  // Construct
  Vec2() : e{0,0} {}
  Vec2(T e0, T e1) : e{e0,e1} {}
  // Copy 
  Vec2(const Vec2<T>& v) : e{v[0],v[1]} {}
  Vec2<T>& operator=(const Vec2<T>& v) { e[0]=v[0]; e[1]=v[1]; }
  // Move
  Vec2(Vec2<T>&& v) : e{v[0],v[1]} {}
  Vec2<T>& operator=(Vec2<T>&& v) { e[0]=v[0]; e[1]=v[1]; }

  // Vector access
  T x() const { return e[0]; }
  T y() const { return e[1]; }
  void set_x(T val) { e[0] = val; }
  void set_y(T val) { e[1] = val; }
  T operator[](int i) const { return e[i]; } 
  T& operator[](int i) { return e[i]; } 

  // Vector negation
  Vec2<T> operator-() const { return Vec2(-e[0],-e[1]); }

  // Vector-vector addition
  Vec2<T>& operator+=(const Vec2<T> &v)
  {
    e[0] += v.e[0];
    e[1] += v.e[1];
    return *this;
  }
  // Vector-vector substraction
  Vec2<T>& operator-=(const Vec2<T> &v)
  {
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    return *this;
  }
  // Vector-vector multiplication
  Vec2<T>& operator*=(const Vec2<T> &v)
  {
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    return *this;
  }
  // Vector-vector division
  Vec2<T>& operator/=(const Vec2<T> &v)
  {
    e[0] /= v.e[0];
    e[1] /= v.e[1];
    return *this;
  }

  // Vector-scalar addition
  Vec2<T>& operator+=(const double t)
  {
    e[0] += t;
    e[1] += t;
    return *this;
  }
  // Vector-scalar substraction
  Vec2<T>& operator-=(const double t)
  {
    e[0] -= t;
    e[1] -= t;
    return *this;
  }
  // Vector-scalar multiplication
  Vec2<T>& operator*=(const double t)
  {
    e[0] *= t;
    e[1] *= t;
    return *this;
  }
  // Vector-scalar division
  Vec2<T>& operator/=(const double t)
  {
    return *this *= 1./t;
  }

  // Estimate vector length
  double length() const 
  {
    return std::sqrt(length_squared());
  }
  // Estimate squared vector length
  double length_squared() const 
  {
    return e[0]*e[0] + e[1]*e[1];
  }

  // Return true if each vector-value is close to zero 
  bool near_zero_values() const
  {
    const auto s = 1e-8;
    return (fabs(e[0]) < s) && 
           (fabs(e[1]) < s);
  }
  // Return true if vector-length is close to zero 
  bool near_zero_length() const
  {
    const auto s = 1e-8;
    return (length() < s);
  }

};

/***********************************************************
* Vec2 utility functions
***********************************************************/

// Output 
template <typename T>
inline std::ostream& operator<<(std::ostream &out, 
                                const Vec2<T> &v)
{
  return out << '(' << v[0] << ',' << v[1] << ')';
}

// Equality
template <typename T>
inline bool operator==(const Vec2<T>& a, const Vec2<T>& b)
{
  return (a.e[0]==b.e[0] && a.e[1]==b.e[1]);
}

// Vector-Vector addition
template <typename T>
inline Vec2<T> operator+(const Vec2<T> &u,const Vec2<T> &v)
{
  return Vec2<T>(u.e[0]+v.e[0], u.e[1]+v.e[1]);
}
// Vector-Vector substraction
template <typename T>
inline Vec2<T> operator-(const Vec2<T> &u,const Vec2<T> &v)
{
  return Vec2<T>(u.e[0]-v.e[0], u.e[1]-v.e[1]);
}
// Vector-Vector multiplication
template <typename T>
inline Vec2<T> operator*(const Vec2<T> &u,const Vec2<T> &v)
{
  return Vec2<T>(u.e[0]*v.e[0], u.e[1]*v.e[1]);
}
// Vector-Vector division
template <typename T>
inline Vec2<T> operator/(const Vec2<T> &u,const Vec2<T> &v)
{
  return Vec2<T>(u.e[0]/v.e[0], u.e[1]/v.e[1]);
}


// Vector-scalar addition
template <typename T>
inline Vec2<T> operator+(const Vec2<T> &u, const double v)
{
  return Vec2<T>(u.e[0]+v, u.e[1]+v);
}
// Vector-scalar substraction
template <typename T>
inline Vec2<T> operator-(const Vec2<T> &u, const double v)
{
  return Vec2<T>(u.e[0]-v, u.e[1]-v);
}
// Vector-scalar multiplication
template <typename T>
inline Vec2<T> operator*(const Vec2<T> &u, const double v)
{
  return Vec2<T>(u.e[0]*v, u.e[1]*v);
}
// Vector-scalar division
template <typename T>
inline Vec2<T> operator/(const Vec2<T> &u, const double v)
{
  return Vec2<T>(u.e[0]/v, u.e[1]/v);
}

// Scalar product
template <typename T>
inline T dot(const Vec2<T> &u, const Vec2<T> &v)
{
  return u.e[0] * v.e[0] + u.e[1] * v.e[1];
}
// Cross product
template <typename T>
inline T cross(const Vec2<T> &u, const Vec2<T> &v)
{
  return u.e[0] * v.e[1] - u.e[1] * v.e[0];
}

// Unit-vector
template <typename T>
inline Vec2<T> unit_vector(const Vec2<T>& u)
{
  return u / u.length();
}


/***********************************************************
* integer / double vector aliases
***********************************************************/
using Vec2i = Vec2<int>;
using Vec2d = Vec2<double>;
using Vec2f = Vec2<float>;


/***********************************************************
* Vec2 geometry functions
***********************************************************/
enum class Orient
{
  CW,     // Clockwise
  CCW,    // Counter-Clockwise
  CL      // Colinear
};

static constexpr double geometry_small = 1.0E-17;

/*----------------------------------------------------------
| Min() / Max() functions
----------------------------------------------------------*/
template <typename T>
static inline T minimum(T a, T b) { return a < b ? a : b; }
template <typename T>
static inline T maximum(T a, T b) { return a > b ? a : b; }
template <typename T>
static inline Vec2<T> bbox_min(const Vec2<T>& a, const Vec2<T>& b)
{ return Vec2<T> { minimum(a[0],b[0]), minimum(a[1], b[1]) }; }
template <typename T>
static inline Vec2<T> bbox_max(const Vec2<T>& a, const Vec2<T>& b)
{ return Vec2<T> { maximum(a[0],b[0]), maximum(a[1], b[1]) }; }

/*----------------------------------------------------------
| Check for orientation of three points (p, q, r)
----------------------------------------------------------*/
template <typename T>
static inline Orient orientation(const Vec2<T>& p,
                                 const Vec2<T>& q,
                                 const Vec2<T>& r)
{
  T area2 = (p[0]-r[0]) * (q[1]-r[1]) 
          - (q[0]-r[0]) * (p[1]-r[1]);
  
  if ( ( area2*area2 ) < geometry_small )
    return Orient::CL;

  if ( area2 > 0)
    return Orient::CCW;
  
  return Orient::CW;
}

/*----------------------------------------------------------
| Check if point r lies within a segment (p,q) 
----------------------------------------------------------*/
template <typename T>
static inline bool in_segment(const Vec2<T>& p,
                              const Vec2<T>& q,
                              const Vec2<T>& r)
{
  if (orientation(p,q,r) != Orient::CL)
    return false;

  Vec2<T> min_bb = bbox_min(p, q);
  Vec2<T> max_bb = bbox_min(p, q);

  const Vec2<T> d_qp  = q-p;
  const Vec2<T> d_rp  = r-p;
  const T t = dot(d_rp, d_qp) / d_qp.length_squared();

  if ( t > 0 && t < 1)
    return true;
  
  return false;
}

/*----------------------------------------------------------
| Check if two lines (p1,q1) and (p2,q2) intersect
| 
| * Returns true, if segments intersect at any point but
|   their edges
| * Returns true, if one line contains a part of the other
| * Returns false, if both lines share both end points
| * Returns false in all other cases
----------------------------------------------------------*/
template <typename T>
static inline bool line_intersection(const Vec2<T>& p1,
                                     const Vec2<T>& q1,
                                     const Vec2<T>& p2,
                                     const Vec2<T>& q2)
{
  Orient o1 = orientation(p1, q1, p2);
  Orient o2 = orientation(p1, q1, q2);
  Orient o3 = orientation(p2, q2, p1);
  Orient o4 = orientation(p2, q2, q1);

  if (  ( (o1 == Orient::CCW && o2 == Orient::CW ) ||
          (o1 == Orient::CW  && o2 == Orient::CCW) ) 
     && ( (o3 == Orient::CCW && o4 == Orient::CW ) ||
          (o3 == Orient::CW  && o4 == Orient::CCW) ) )
  {
    return true;
  }

  // (p1,q1) and p2 are collinear and p2 lies on segment (p1,q1)
  if ( (o1 == Orient::CL) && in_segment(p1,q1,p2) )
    return true;

  // (p1,q1) and q2 are collinear and q2 lies on segment (p1,q1)
  if ( (o2 == Orient::CL) && in_segment(p1,q1,q2) )
    return true;

  // (p2,q2) and p1 are collinear and p1 lies on segment (p2,q2)
  if ( (o3 == Orient::CL) && in_segment(p2,q2,p1) )
    return true;

  // (p2,q2) and q1 are collinear and q1 lies on segment (p2,q2)
  if ( (o4 == Orient::CL) && in_segment(p2,q2,q1) )
    return true;

  return false;
}