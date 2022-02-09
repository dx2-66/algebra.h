#pragma once
#include "GL/glew.h"
#include "math.h"

#ifdef __clang__
typedef GLfloat vec2 __attribute__ ((ext_vector_type(2)));
typedef GLfloat vec3 __attribute__ ((ext_vector_type(4)));
typedef GLfloat vec4 __attribute__ ((ext_vector_type(4)));
typedef GLfloat vec16 __attribute__ ((ext_vector_type(16)));
typedef GLint vec4i __attribute__ ((ext_vector_type(4)));
typedef GLint vec16i __attribute__ ((ext_vector_type(16)));
#define __builtin_shuffle __builtin_shufflevector
#elif __GNUC__
typedef GLfloat vec2 __attribute__ ((vector_size (8)));
typedef GLfloat vec3 __attribute__ ((vector_size (16)));
typedef GLfloat vec4 __attribute__ ((vector_size (16)));
typedef GLfloat vec16 __attribute__ ((vector_size (64)));
typedef GLint vec4i __attribute__ ((vector_size (16)));
typedef GLint vec16i __attribute__ ((vector_size (64)));
#endif

typedef union
{
	struct 
	{
		vec4 x;
		vec4 y;
		vec4 z;
		union
		{
			vec4 t;
			vec4 w;
		};
	};
	vec16 vec;
} mat4;

static const mat4 identity =
{
	.x = {1, 0, 0, 0},
	.y = {0, 1, 0, 0},
	.z = {0, 0, 1, 0},
	.w = {0, 0, 0, 1}
};

static inline vec4 mat4_mul_vec4 (mat4 a, vec4 b)
{
	return a.x*b[0]+a.y*b[1]+a.z*b[2]+a.w*b[3];
}

static inline mat4 mat4_mul (mat4 a, mat4 b)
{
    return (mat4)
	{
		.x=mat4_mul_vec4 (a, b.x),
		.y=mat4_mul_vec4 (a, b.y),
		.z=mat4_mul_vec4 (a, b.z),
		.w=mat4_mul_vec4 (a, b.w)
	};
}

static inline GLfloat dot (vec4 a, vec4 b)
{ 
	register vec4 r = b*a;
	return r[0]+r[1]+r[2]+r[3];
}

static inline vec4 cross (vec4 a, vec4 b)
{
	return (vec4)
	{
		 a[1]*b[2] - a[2]*b[1],
		 a[2]*b[0] - a[0]*b[2],
		 a[0]*b[1] - a[1]*b[0],
		 0
	};
}

static inline GLfloat length (vec4 a)
{
	return sqrt (dot (a,a));
}

static inline vec4 normalize (vec4 a)
{ 
	return a/length (a);
}

static inline mat4 look_at (vec3 eye, vec3 center, vec3 up)
{
	register vec3 const view_dir = normalize (center - eye);

	register vec3 const s = normalize (cross (view_dir, up));
	
	register vec3 const t = cross (s, view_dir);
	
	return (mat4)
	{
		.x = {s[0], t[0], -view_dir[0], 0},
		.y = {s[1], t[1], -view_dir[1], 0},
		.z = {s[2], t[2], -view_dir[2], 0},
		.w = {-dot (s, eye), -dot (t, eye), dot (view_dir, eye), 1}
	};
}

static inline mat4 perspective (float fov, float aspect, float near_plane, float far_plane)
{
	register GLfloat const a = 1. / tan (fov / 2.);
	return (mat4)
	{
		.x = {a/aspect, 0, 0, 0},
		.y = {0, a, 0, 0},
		.z = {0, 0, -((far_plane + near_plane) / (far_plane - near_plane)), -1},
		.w = {0, 0,-((2. * far_plane * near_plane) / (far_plane - near_plane)), 0}
	};
}

static inline mat4 transpose (mat4 a)
{
    register vec16i mask = {0,4,8,12, 1,5,9,13, 2,6,10,14, 3,7,11,15};
	return (mat4)
	{
        .vec =__builtin_shuffle (a.vec, mask)
	};
}

static inline mat4 rotate_x (mat4 a, float angle)
{
	register GLfloat s = sin (angle);
	register GLfloat c = cos (angle);
	register mat4 r =
	{
		.x = {1,  0, 0, 0},
		.y = {0,  c, s, 0},
		.z = {0, -s, c, 0},
		.w = {0,  0, 0, 1}
	};
	return mat4_mul (a, r);
}

static inline mat4 rotate_y (mat4 a, float angle)
{
	register GLfloat s = sin (angle);
	register GLfloat c = cos (angle);
	register mat4 r =
	{
		.x = { c, 0, s, 0},
		.y = { 0, 1, 0, 0},
		.z = {-s, 0, c, 0},
		.w = { 0, 0, 0, 1}
	};
	return mat4_mul (a, r);
}
static inline mat4 rotate_z (mat4 a, float angle)
{
	register GLfloat s = sin (angle);
	register GLfloat c = cos (angle);
	register mat4 r =
	{
		.x = { c, s, 0, 0},
		.y = {-s, c, 0, 0},
		.z = { 0, 0, 1, 0},
		.w = { 0, 0, 0, 1}
	};
	return mat4_mul (a, r);
}

static inline mat4 translate (mat4 a, float x, float y, float z)
{
	register mat4 t = identity;
	t.vec[12] = x;
	t.vec[13] = y;
	t.vec[14] = z;
	return mat4_mul (a, t);
}

static inline mat4 scale (mat4 a, float k)
{
	register mat4 s = a;
	s.vec[0]*=k;
	s.vec[5]*=k;
	s.vec[10]*=k;
	return s;
}

// Adapted from Nis Wegmann's SMI lib, which is itself based on Intel SSE sample.
static inline mat4 inverse (mat4 a)
{
    register vec4 row0, row1, row2, row3;
    register vec4 col0, col1, col2, col3;
    register vec4 det, tmp1;

    /* Transpose: */
    
    #ifdef __clang__
    #define mask0415 0, 4, 1, 5
    #define mask2637 2, 6, 3, 7
    #define mask2301 2, 3, 0, 1
    #define mask1076 1, 0, 7, 6
    #define mask2345 2, 3, 4, 5
    #elif __GNUC__
    register vec4i mask0415 = {0, 4, 1, 5};
    register vec4i mask2637 = {2, 6, 3, 7};
    register vec4i mask2301 = {2, 3, 0, 1};
    register vec4i mask1076 = {1, 0, 7, 6};
    register vec4i mask2345 = {2, 3, 4, 5};
    #endif
    
    tmp1 = __builtin_shuffle (a.x, a.z, mask0415);
    row1 = __builtin_shuffle (a.y, a.w, mask0415);

    row0 = __builtin_shuffle (tmp1, row1, mask0415);
    row1 = __builtin_shuffle (tmp1, row1, mask2637);

    tmp1 = __builtin_shuffle (a.x, a.z, mask2637);
    row3 = __builtin_shuffle (a.y, a.w, mask2637);

    row2 = __builtin_shuffle (tmp1, row3, mask0415);
    row3 = __builtin_shuffle (tmp1, row3, mask2637);

    /* Compute adjoint: */

    row1 = __builtin_shuffle (row1, row1, mask2301);
    row3 = __builtin_shuffle (row3, row3, mask2301);

    tmp1 = row2 * row3;
    tmp1 = __builtin_shuffle (tmp1, tmp1, mask1076);

    col0 = row1 * tmp1;
    col1 = row0 * tmp1;

    tmp1 = __builtin_shuffle (tmp1, tmp1, mask2345);

    col0 = row1 * tmp1 - col0;
    col1 = row0 * tmp1 - col1;
    col1 = __builtin_shuffle (col1, col1, mask2345);

    tmp1 = row1 * row2;
    tmp1 = __builtin_shuffle (tmp1, tmp1, mask1076);

    col0 = row3 * tmp1 + col0;
    col3 = row0 * tmp1;

    tmp1 = __builtin_shuffle (tmp1, tmp1, mask2345);

    col0 = col0 - row3 * tmp1;
    col3 = row0 * tmp1 - col3;
    col3 = __builtin_shuffle (col3, col3, mask2345);

    tmp1 = __builtin_shuffle (row1, row1, mask2345) * row3;
    tmp1 = __builtin_shuffle (tmp1, tmp1, mask1076);
    row2 = __builtin_shuffle (row2, row2, mask2345);

    col0 = row2 * tmp1 + col0;
    col2 = row0 * tmp1;

    tmp1 = __builtin_shuffle (tmp1, tmp1, mask2345);

    col0 = col0 - row2 * tmp1;
    col2 = row0 * tmp1 - col2;
    col2 = __builtin_shuffle (col2, col2, mask2345);

    tmp1 = row0 * row1;
    tmp1 = __builtin_shuffle (tmp1, tmp1, mask1076);

    col2 = row3 * tmp1 + col2;
    col3 = row2 * tmp1 - col3;

    tmp1 = __builtin_shuffle (tmp1, tmp1, mask2345);

    col2 = row3 * tmp1 - col2;
    col3 = col3 - row2 * tmp1;

    tmp1 = row0 * row3;
    tmp1 = __builtin_shuffle (tmp1, tmp1, mask1076);

    col1 = col1 - row2 * tmp1;
    col2 = row1 * tmp1 + col2;

    tmp1 = __builtin_shuffle (tmp1, tmp1, mask2345);

    col1 = row2 * tmp1 + col1;
    col2 = col2 - row1 * tmp1;

    tmp1 = row0 * row2;
    tmp1 = __builtin_shuffle (tmp1, tmp1, mask1076);

    col1 = row3 * tmp1 + col1;
    col3 = col3 - row1 * tmp1;

    tmp1 = __builtin_shuffle (tmp1, tmp1, mask2345);

    col1 = col1 - row3 * tmp1;
    col3 = row1 * tmp1 + col3;

    /* Compute determinant: */

    det = row0 * col0;
    det = __builtin_shuffle (det, det, mask2345) + det;
    det = __builtin_shuffle (det, det, mask1076) + det;

    /* Compute reciprocal of determinant: */

    det = 1.0f / det;

    return (mat4)
	{
		.x = col0*det,
		.y = col1*det,
		.z = col2*det,
		.w = col3*det
	};
