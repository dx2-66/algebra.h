# Pure C linear algebra (intended for OpenGL)

**Features:**
- based upon GCC/clang vectors, as much SIMD as compiler optimization allows;

**Implemented data structures:**
- GLfloat (single precision) vectors: *vec2*, *vec4*, *vec3* (alias for vec4 for alignment reasons);
- GLint vector: *vec4i*;
- GLfloat matrix (4x4): *mat4*:
	- a structure of four vec4 accessed as fields .x, .y, .z, .t (**or** .w)
	- *or* a single vector of length 16 accessed as field .vec

**Implemented math:**
- **identity** (const mat4);
- **products**:
	- **dot product** *dot (vec4 a, vec4 b)*: returns GLfloat;
	- **cross product** *cross (vec4 a, vec4 b)*: returns vec4;
	- **matrix product** *mat4_mul (mat4 a, mat4 b)*: returns mat4;
- **vector length** *length (vec4 a)*: returns GLfloat;
- **normalized vector** *normalize (vec4 a)*: returns vec4;
- **transposed matrix** *transpose (mat4 a)*: returns mat4;
- **inverted matrix** *inverse (mat4 a)*: returns mat4;
- **model axis rotation** (returns mat4):
	- *rotate_x (mat4 a, float angle)*
	- *rotate_y (mat4 a, float angle)*
	- *rotate_z (mat4 a, float angle)*
- **model translation** *translate (mat4 a, float x, float y, float z)*: returns mat4;
- **model scaling** *scale (mat4 a, float k)*: returns mat4;
- **view matrix** *look_at (vec3 eye, vec3 center, vec3 up)*: returns mat4;
- **projection matrix** *perspective (float fov, float aspect, float near_plane, float far_plane)*: returns mat4;

**Requirements:**
- any recent GCC or clang version;
- GLEW (interchangeable with any other GL types providing header).

