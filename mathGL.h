#include <math.h>

#define MIN(a,b)   ((a) < (b) ? (a) : (b))
#define MAX(a,b)   ((a) > (b) ? (a) : (b))


static const double PI = 4.0*atan(1.0);

struct vec4;

// Declare vec3 and corrensponding operator overloading and functions
// so that i don't have to include a separate library for it (glm.h)
struct vec3 {
    float x, y, z;
    vec3();
    vec3(const float &, const float &, const float &);
    vec3(const vec4 &);
};

vec3 operator+(const vec3 &, const vec3 &);
vec3 operator-(const vec3 &, const vec3 &);
vec3 operator-(const vec3 &);
vec3 operator*(const vec3 &, const float &);
vec3 operator*(const float &, const vec3 &);

vec3 normalize(const vec3 &);
float dot(const vec3 &, const vec3 &);
vec3 cross(const vec3 &, const vec3 &);

// Declare vec4 and corrensponding operator overloading and functions
struct vec4 {
    float x, y, z, w;
    vec4();
    vec4(const float &, const float &, const float &, const float &);
    vec4(const vec3 &, const float &);
};

vec4 operator+(const vec4 &, const vec4 &);
vec4 operator-(const vec4 &, const vec4 &);
vec4 operator-(const vec4 &);
vec4 operator*(const vec4 &, const float &);
vec4 operator*(const float &, const vec4 &);

vec4 normalize(const vec4 &);
float dot(const vec4 &, const vec4 &);

// Declare mat4 and corrensponding operator overloading and functions
struct mat4 {
    union {
        struct {float m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44;};
        float m[16];
        float M[4][4];
    };

    mat4(const float &);
    mat4(const vec4 &, const vec4 &, const vec4 &, const vec4 &);
};

vec4 operator*(const mat4 &, const vec4 &);
mat4 operator*(const mat4 &, const mat4 &);

mat4 translate(const vec3 &);
mat4 scale(const vec3 &);
mat4 rotate(const vec3 &, const float &);
mat4 view(const vec3 &, const vec3 &, const vec3 &, const vec3 &);
mat4 lookAt(const vec3 &, const vec3 &, const vec3 &);
mat4 projection(const float &, const float &, const float &, const float &);
mat4 ortho(float x1, float x2, float y1, float y2, float z1, float z2);




vec3::vec3() : x(0.0f), y(0.0f), z(0.0f) {

}

vec3::vec3(const float &x, const float &y, const float &z) : x(x), y(y), z(z) {

}

vec3::vec3(const vec4 &v) : x(v.x), y(v.y), z(v.z) {

}

vec3 operator+(const vec3 &lhs, const vec3 &rhs) {
    return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}
vec3 operator-(const vec3 &lhs, const vec3 &rhs) {
    return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}
vec3 operator-(const vec3 &lhs) {
    return vec3(-lhs.x, -lhs.y, -lhs.z);
}
vec3 operator*(const vec3 &lhs, const float &rhs) {
    return vec3(lhs.x*rhs, lhs.y*rhs, lhs.z*rhs);
}
vec3 operator*(const float &lhs, const vec3 &rhs) {
    return vec3(lhs*rhs.x, lhs*rhs.y, lhs*rhs.z);
}

float dot(const vec3 &a, const vec3 &b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3 normalize(const vec3 &in) {
    float norm = 1.0/sqrt(dot(in, in));
    return in*norm;
}

vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.y*v.z - u.z*v.y, u.z*v.x - u.x*v.z, u.x*v.y - u.y*v.x);
}


vec4::vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {
    // vec4 empty constructor
}
vec4::vec4(const float &x, const float &y, const float &z, const float &w) : x(x), y(y), z(z), w(w) {
    // vec4 constructor using explicit values
}
vec4::vec4(const vec3 &v, const float &w) : x(v.x), y(v.y), z(v.z), w(w) {
    // vec4 constructor using vec3
}

vec4 operator+(const vec4 &lhs, const vec4 &rhs) {
    return vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}
vec4 operator-(const vec4 &lhs, const vec4 &rhs) {
    return vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}
vec4 operator-(const vec4 &lhs) {
    return vec4(-lhs.x, -lhs.y, -lhs.z, -lhs.w);
}
vec4 operator*(const vec4 &lhs, const float &rhs) {
    return vec4(lhs.x*rhs, lhs.y*rhs, lhs.z*rhs, lhs.w*rhs);
}
vec4 operator*(const float &lhs, const vec4 &rhs) {
    return vec4(lhs*rhs.x, lhs*rhs.y, lhs*rhs.z, lhs*rhs.w);
}

float dot(const vec4 &a, const vec4 &b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

vec4 normalize(const vec4 in) {
    float norm = 1.0/sqrt(dot(in, in));
    return norm*in;
}



mat4::mat4(const float & diag = 1.0f) {
    // create diagonal matrix
    for (int i = 0; i < 16; i++)
        m[i] = ((i / 4) == (i % 4) ? diag : 0.0f);

}
mat4::mat4(const vec4 &a, const vec4 &b, const vec4 &c, const vec4 &d) {
    // create matrix from 4 column vectors
    m11 = a.x, m12 = b.x, m13 = c.x, m14 = d.x;
    m21 = a.y, m22 = b.y, m23 = c.y, m24 = d.y;
    m31 = a.z, m32 = b.z, m33 = c.z, m34 = d.z;
    m41 = a.w, m42 = b.w, m43 = c.w, m44 = d.w;
}

vec4 operator*(const mat4 &lhs, const vec4 &rhs) {
    // matrix-vector product
    vec4 out;
    out.x = lhs.m11*rhs.x + lhs.m12*rhs.y + lhs.m13*rhs.z + lhs.m14*rhs.w;
    out.y = lhs.m21*rhs.x + lhs.m22*rhs.y + lhs.m23*rhs.z + lhs.m24*rhs.w;
    out.z = lhs.m31*rhs.x + lhs.m32*rhs.y + lhs.m33*rhs.z + lhs.m34*rhs.w;
    out.w = lhs.m41*rhs.x + lhs.m42*rhs.y + lhs.m43*rhs.z + lhs.m44*rhs.w;

    return out;
}

mat4 operator*(const mat4 &lhs, const mat4 &rhs) {
    mat4 out(0.0f);
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            for (int k = 0; k < 4; k++) {
                out.M[j][i] += lhs.M[k][i]*rhs.M[j][k];
            }
        }
    }
    return out;
}

mat4 translate(const vec3 &move) {
    // 1 0 0 x 
    // 0 1 0 y
    // 0 0 1 z
    // 0 0 0 1
    mat4 out(1.0f);
    out.m14 = move.x, out.m24 = move.y, out.m34 = move.z;
    return out;
}

mat4 scale(const vec3 &scale) {
    // x 0 0 0 
    // 0 y 0 0
    // 0 0 z 0
    // 0 0 0 1
    mat4 out(1.0f);
    out.m11 = scale.x;
    out.m22 = scale.y;
    out.m33 = scale.z;
    return out;
}

mat4 rotate(const vec3 &axis, const float &angle) {
    vec3 axis_ = normalize(axis);
    // Goggle: "Q38: How do I generate a rotation matrix for a selected axis and angle?""
    // need to negate the angle
    float rcos = cos(-angle*PI/180.0f);
    float rsin = sin(-angle*PI/180.0f);
    float u = axis_.x;
    float v = axis_.y;
    float w = axis_.z;

    mat4 mat = mat4(1.0f);
    mat.m11 =      rcos + u*u*(1.0f - rcos);
    mat.m12 =  w * rsin + v*u*(1.0f - rcos);
    mat.m13 = -v * rsin + w*u*(1.0f - rcos);
    mat.m21 = -w * rsin + u*v*(1.0f - rcos);
    mat.m22 =      rcos + v*v*(1.0f - rcos);
    mat.m23 =  u * rsin + w*v*(1.0f - rcos);
    mat.m31 =  v * rsin + u*w*(1.0f - rcos);
    mat.m32 = -u * rsin + v*w*(1.0f - rcos);
    mat.m33 =      rcos + w*w*(1.0f - rcos);

    return mat;
}

mat4 view(const vec3 &r, const vec3 &u, const vec3 &f, const vec3 &p) {
    // multiply the model matrix by this matrix to transform a vertex
    // into camera space
    //
    //  rx  ry  rz  -dot(r,p)
    //  ux  uy  uz  -dot(u,p)
    // -fx -fy -fz   dot(f,p)
    //   0   0   0          1
    mat4 mat(1.0f);
    mat.m11 =  r.x, mat.m12 =  r.y, mat.m13 =  r.z, mat.m14 = -dot(r, p);
    mat.m21 =  u.x, mat.m22 =  u.y, mat.m23 =  u.z, mat.m24 = -dot(u, p);
    mat.m31 = -f.x, mat.m32 = -f.y, mat.m33 = -f.z, mat.m34 =  dot(f, p);

    return mat;
}

mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
    // Convert camera direction to a right handed coordinate system
    // with one vector pointing in the direction of the camera (y-axis), 
    // one vector poiting to the "right" of this one (x-axis)
    // and one orthogonal to these two, the "up" axis (z-axis). r x f = u
    // 
    // These are automatically normalized. "Easily" derived by hand. 
    // Physicist notation is used, i.e. theta is the polar axis. 
    vec3 f = normalize(vec3(center.x - eye.x, center.y - eye.y, center.z - eye.z));
    vec3 u = normalize(up);
    vec3 r = normalize(cross(f,u));
    u = cross(r,f);

    return view(r, u ,f, eye);
}

mat4 projection(const float &fov, const float &ratio, const float &zmin, const float &zmax) {
    // transforms camera space into clip space, perspective projection. 
    // Multiply with modelView to make modelViewProjection matrix (MVP)
    //
    // f/ratio  0               0                          0
    //     0    f               0                          0
    //     0    0  (zmax + zmin)/(zmin - zmax)  2*zmax*zmin/(zmin - zmax)
    //     0    0              -1                          0
    mat4 out(0.0f);
    float f = 1.0/tan(fov*PI/180.0/2.0);
    out.m11 = f/ratio; 
    out.m22 = f;
    out.m33 = (zmax + zmin)/(zmin - zmax);
    out.m34 = 2*zmax*zmin/(zmin - zmax);
    out.m43 = -1.0;

    return out;
}

// https://msdn.microsoft.com/en-us/library/windows/desktop/dd373965(v=vs.85).aspx
mat4 ortho(float l, float r, float b, float t, float n, float f) {
    mat4 out(1.0f);
    out.m11 = 2/(r-l);
    out.m22 = 2/(t-b);
    out.m33 = -2/(f-n);

    out.m14 = -(r+l)/(r-l);
    out.m24 = -(t+b)/(t-b);
    out.m34 = -(f+n)/(f-n);

    return out;
}