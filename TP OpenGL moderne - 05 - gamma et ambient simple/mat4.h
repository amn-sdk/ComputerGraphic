#pragma once
#include <cmath>

// Matrice 4x4 en column-major (convention OpenGL)
struct Mat4
{
    float m[16];
    Mat4() { for (int i = 0; i < 16; i++) m[i] = 0.f; }
};

Mat4 identity()
{
    Mat4 r;
    r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.f;
    return r;
}

Mat4 multiply(const Mat4& a, const Mat4& b)
{
    Mat4 r;
    for (int col=0; col<4; col++)
        for (int row=0; row<4; row++)
            for (int k=0; k<4; k++)
                r.m[col*4+row]+=a.m[k*4+row]*b.m[col*4+k];
    return r;
}

// Matrice de translation
Mat4 makeTranslation(float x, float y, float z)
{
    Mat4 t = identity();
    t.m[12] = x;
    t.m[13] = y;
    t.m[14] = z;
    return t;
}

// Rotation autour de l'axe Y (Up)
Mat4 makeRotationY(float angle)
{
    Mat4 r = identity();
    r.m[0]  =  cosf(angle);
    r.m[2]  = -sinf(angle);
    r.m[8]  =  sinf(angle);
    r.m[10] =  cosf(angle);
    return r;
}

// Rotation autour de l'axe X (Right)
Mat4 makeRotationX(float angle)
{
    Mat4 r=identity();
    r.m[5]=cosf(angle);
    r.m[6]=sinf(angle);
    r.m[9]=-sinf(angle);
    r.m[10]=cosf(angle);
    return r;
}

// Rotation autour de l'axe Z (Forward)
Mat4 makeRotationZ(float angle)
{
    Mat4 r=identity();
    r.m[0]=cosf(angle);
    r.m[1]=sinf(angle);
    r.m[4]=-sinf(angle);
    r.m[5]=cosf(angle);
    return r;
}

// Matrice de projection perspective (fovY en degres)
Mat4 makePerspective(float fovYDeg, float aspect, float nearP, float farP)
{
    float fovYRad=fovYDeg*3.14159265f/180.f;
    float f=1.f/tanf(fovYRad*0.5f);

    Mat4 p;
    p.m[0]=f/aspect;
    p.m[5]=f;
    p.m[10]=(farP+nearP)/(nearP-farP);
    p.m[11]=-1.f;
    p.m[14]=(2.f*farP*nearP)/(nearP-farP);
    return p;
}
