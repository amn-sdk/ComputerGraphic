#pragma once
#include <cmath>
#include "vec.h"

struct Mat4
{
    float m[16];
    Mat4() { 
        for (int i = 0; i < 16; i++){
        m[i]= 0.f; 
        }
    }
};

Mat4 identity()
{
    Mat4 r;
    r.m[0]=r.m[5]=r.m[10]=r.m[15]=1.f;
    return r;
}

Mat4 multiply(const Mat4& a, const Mat4& b)
{
    Mat4 r;
    for (int col=0; col<4; col++)
        for (int row=0; row<4; row++)
            for (int k=0; k<4; k++)
                r.m[col*4+row] += a.m[k*4+row]*b.m[col*4+k];
    return r;
}
Mat4 makeTranslation(float x, float y, float z)
{
    Mat4 t= identity();
    t.m[12]= x;
    t.m[13]= y;
    t.m[14]= z;
    return t;
}
Mat4 makeScale(float sx, float sy, float sz)
{
    Mat4 s= identity();
    s.m[0]= sx;
    s.m[5]= sy;
    s.m[10]= sz;
    return s;
}
Mat4 makeRotationY(float angle)
{
    Mat4 r= identity();
    r.m[0]=cosf(angle);
    r.m[2]=-sinf(angle);
    r.m[8]=sinf(angle);
    r.m[10]=cosf(angle);
    return r;
}
Mat4 makeRotationX(float angle)
{
    Mat4 r= identity();
    r.m[5]=cosf(angle);
    r.m[6]=sinf(angle);
    r.m[9]=-sinf(angle);
    r.m[10]=cosf(angle);
    return r;
}
Mat4 makeRotationZ(float angle)
{
    Mat4 r =identity();
    r.m[0]=cosf(angle);
    r.m[1]=sinf(angle);
    r.m[4]=-sinf(angle);
    r.m[5]=cosf(angle);
    return r;
}
Mat4 makePerspective(float fovYDeg, float aspect, float nearP, float farP)
{
    float fovYRad= fovYDeg*3.14159265f/180.f;
    float f= 1.f/tanf(fovYRad*0.5f);
    Mat4 p;
    p.m[0]= f/aspect;
    p.m[5]= f;
    p.m[10]= (farP+nearP)/(nearP-farP);
    p.m[11]= -1.f;
    p.m[14]= (2.f*farP*nearP)/(nearP-farP);
    return p;
}
Mat4 makeLookAt(const vec3& position, const vec3& target, const vec3& up)
{
    vec3 forward = normalize(sub(position, target));
    vec3 right = normalize(cross(up, forward));
    vec3 upCorrected = cross(forward, right);
    Mat4 v;
    v.m[0]= right.x;
    v.m[1]= upCorrected.x;
    v.m[2]=forward.x;
    v.m[3]= 0.f;
    v.m[4]= right.y;
    v.m[5]=upCorrected.y;
    v.m[6]=forward.y;
    v.m[7]=0.f;
    v.m[8]=right.z;
    v.m[9]=upCorrected.z;
    v.m[10]=forward.z;
    v.m[11]=0.f;
    v.m[12]=-dot(right, position);
    v.m[13]=-dot(upCorrected,position);
    v.m[14] =-dot(forward, position);
    v.m[15]=1.f;
    
    return v;
}


