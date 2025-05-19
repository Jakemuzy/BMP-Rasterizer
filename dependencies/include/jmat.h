
struct Vec2 {
    float x, y;
};

struct Vec3 {
    float x, y, z;

    float dot(const Vec3 &two)
    {
        return (x * two.x + y * two.y + z * two.z);
    }

    Vec3 normalize()
    {
        float length = std::sqrt(x * x + y * y + z * z);
        if (length > 0.0f)
        {
            return {x / length, y / length, z / length};
        }

        //  Default return if 0 length
        return {0, 0, 0}; 
    }

    Vec3 operator*(float scalar) const
    {
        return {x * scalar, y * scalar, z * scalar};
    }

    Vec3 operator*(const Vec3 &other) const
    {
        return {x * other.x, y * other.y, z * other.z};
    }

    Vec3 operator+(const Vec3 &other) const
    {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vec3 operator-(const Vec3 &other) const
    {
        Vec3 New;
        New = {x - other.x, y - other.y, z - other.z};
        return New;
    }

    Vec3 operator-() const
    {
        Vec3 New;
        New = {-x, -y, -z};
        return New;
    }
};

struct Vec4 
{
    float x, y, z, w;

    void Vec4::print(){
        std::cout << x << " " << y << " " << z << " " << w << "\n";
    };
};

struct Mat4
{
    float
        m11, m12, m13, m14,
        m21, m22, m23, m24,
        m31, m32, m33, m34,
        m41, m42, m43, m44;

    //  Types of matrices intializaitons
    void Mat4::ident(){
        m11 = 1; m22 = 1; m33 = 1; m44 =1;
        m12 = 0; m13 = 0; m14 = 0;
        m21 = 0; m23 = 0; m24 = 0;
        m31 = 0; m32 = 0; m34 = 0;
        m41 = 0; m42 = 0; m43 = 0;
    };

    void Mat4::perspective(float fovY, float aspect, float zNear, float zFar){  //  Fov in degrees, aspect ratio of screen, near and far clipping planes
        float PI = 3.14159265358979323846264338327950288419716939937510;
        float f = 1.0f / tan((fovY * PI / 180) / 2.0f);
        float zRange = zFar - zNear;

        (*this).ident();
        m11 = f / aspect;
        m22 = f;
        m33 = (zFar + zNear) / zRange;
        m34 = (2 * zFar * zNear) / zRange;
        m43 = -1;
    }

    void transpose(const Mat4& mat, Mat4& out) 
    {
        out.m11 = mat.m11; out.m12 = mat.m21; out.m13 = mat.m31; out.m14 = mat.m41;
        out.m21 = mat.m12; out.m22 = mat.m22; out.m23 = mat.m32; out.m24 = mat.m42;
        out.m31 = mat.m13; out.m32 = mat.m23; out.m33 = mat.m33; out.m34 = mat.m43;
        out.m41 = mat.m14; out.m42 = mat.m24; out.m43 = mat.m34; out.m44 = mat.m44;
    }


    //  Didn't feel like writing so just copied for now 
    bool inverse(const Mat4 &mat, Mat4 &out)
    {
        float m[16] = {
            mat.m11, mat.m12, mat.m13, mat.m14,
            mat.m21, mat.m22, mat.m23, mat.m24,
            mat.m31, mat.m32, mat.m33, mat.m34,
            mat.m41, mat.m42, mat.m43, mat.m44
        };

        float inv[16], det;
        int i;

        inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

        inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];

        inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

        inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

        inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] +m[13] * m[3] * m[10];

        inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

        inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] -m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

        inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] -m[8] * m[1] * m[14] +m[8] * m[2] * m[13] +m[12] * m[1] * m[10] -m[12] * m[2] * m[9];

        inv[2] = m[1] * m[6] * m[15] -m[1] * m[7] * m[14] -m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] -m[13] * m[3] * m[6];

        inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] +m[4] * m[2] * m[15] -m[4] * m[3] * m[14] - m[12] * m[2] * m[7] +m[12] * m[3] * m[6];

        inv[10] = m[0] * m[5] * m[15] -m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

        inv[14] = -m[0] * m[5] * m[14] +m[0] * m[6] * m[13] + m[4] * m[1] * m[14] -m[4] * m[2] * m[13] -m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

        inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -m[5] * m[3] * m[10] - m[9] * m[2] * m[7] +m[9] * m[3] * m[6];

        inv[7] = m[0] * m[6] * m[11] -m[0] * m[7] * m[10] -m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +m[8] * m[2] * m[7] -m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] +m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

        inv[15] = m[0] * m[5] * m[10] -m[0] * m[6] * m[9] -m[4] * m[1] * m[10] +m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

        det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

        if (det == 0.0f)
            return false;

        det = 1.0f / det;

        for (i = 0; i < 16; i++)
            inv[i] *= det;

        out = {
            inv[0], inv[1], inv[2], inv[3],
            inv[4], inv[5], inv[6], inv[7],
            inv[8], inv[9], inv[10], inv[11],
            inv[12], inv[13], inv[14], inv[15]
        };

        return true;
    }

    //  Matrix transformations
    void Mat4::scale(float x, float y, float z){
        m11 *= x;
        m22 *= y;
        m33 *= z;
    }

    void Mat4::rotate(float angleX, float angleY, float angleZ){
        Mat4 X, Y, Z;
        X.ident(); Y.ident(); Z.ident();

        X.m22 = cos(angleX);    X.m33 = cos(angleX);    X.m23 = -sin(angleX);     X.m32 =  sin(angleX);
        Y.m11 = cos(angleY);    Y.m33 = cos(angleY);    Y.m13 =  sin(angleY);     Y.m31 = -sin(angleY);
        Z.m11 = cos(angleZ);    Z.m22 = cos(angleZ);    Z.m12 = -sin(angleZ);     Z.m21 =  sin(angleZ);

        (*this) = (*this) * X * Y * Z;
    }

    void Mat4::setRotation(float angleX, float angleY, float angleZ) {
        // Rotation matrices for each axis
        Mat4 X, Y, Z;
        X.ident(); Y.ident(); Z.ident();

        // Set the rotation matrices for each axis
        X.m22 = cos(angleX);    X.m33 = cos(angleX);    X.m23 = -sin(angleX);     X.m32 = sin(angleX);
        Y.m11 = cos(angleY);    Y.m33 = cos(angleY);    Y.m13 = sin(angleY);      Y.m31 = -sin(angleY);
        Z.m11 = cos(angleZ);    Z.m22 = cos(angleZ);    Z.m12 = -sin(angleZ);     Z.m21 = sin(angleZ);

        // Set the matrix to the result of the rotations (X * Y * Z)
        *this = X * Y * Z;
    }

    void Mat4::translate(float x, float y, float z){
        m14 += x;
        m24 += y;
        m34 += z;
    }

    void Mat4::print()
    {
        std::cout << m11 << ' ' << m12 << ' ' << m13 << ' ' << m14 << '\n' << m21 << ' ' << m22 << ' ' << m23 << ' ' << m24 << '\n' << m31 << ' ' << m32 << ' ' << m33 << ' ' << m34 << '\n' << m41 << ' ' << m42 << ' ' << m43 << ' ' << m44 << '\n';
    };

    //  Matrix operations
    Mat4 operator*(const Mat4 &other) const
    {
        Mat4 New;

        New.m11 = (*this).m11 * other.m11 + (*this).m12 * other.m21 + (*this).m13 * other.m31 + (*this).m14 * other.m41;
        New.m12 = (*this).m11 * other.m12 + (*this).m12 * other.m22 + (*this).m13 * other.m32 + (*this).m14 * other.m42;
        New.m13 = (*this).m11 * other.m13 + (*this).m12 * other.m23 + (*this).m13 * other.m33 + (*this).m14 * other.m43;
        New.m14 = (*this).m11 * other.m14 + (*this).m12 * other.m24 + (*this).m13 * other.m34 + (*this).m14 * other.m44;

        New.m21 = m21 * other.m11 + m22 * other.m21 + m23 * other.m31 + m24 * other.m41;
        New.m22 = m21 * other.m12 + m22 * other.m22 + m23 * other.m32 + m24 * other.m42;
        New.m23 = m21 * other.m13 + m22 * other.m23 + m23 * other.m33 + m24 * other.m43;
        New.m24 = m21 * other.m14 + m22 * other.m24 + m23 * other.m34 + m24 * other.m44;

        New.m31 = m31 * other.m11 + m32 * other.m21 + m33 * other.m31 + m34 * other.m41;
        New.m32 = m31 * other.m12 + m32 * other.m22 + m33 * other.m32 + m34 * other.m42;
        New.m33 = m31 * other.m13 + m32 * other.m23 + m33 * other.m33 + m34 * other.m43;
        New.m34 = m31 * other.m14 + m32 * other.m24 + m33 * other.m34 + m34 * other.m44;

        New.m41 = m41 * other.m11 + m42 * other.m21 + m43 * other.m31 + m44 * other.m41;
        New.m42 = m41 * other.m12 + m42 * other.m22 + m43 * other.m32 + m44 * other.m42;
        New.m43 = m41 * other.m13 + m42 * other.m23 + m43 * other.m33 + m44 * other.m43;
        New.m44 = m41 * other.m14 + m42 * other.m24 + m43 * other.m34 + m44 * other.m44;

        return New;
    };

    Vec4 operator*(const Vec4& other) const {
        Vec4 New;

        New.x = m11 * other.x + m12 * other.y + m13 * other.z + m14 * other.w;
        New.y = m21 * other.x + m22 * other.y + m23 * other.z + m24 * other.w;
        New.z = m31 * other.x + m32 * other.y + m33 * other.z + m34 * other.w;
        New.w = m41 * other.x + m42 * other.y + m43 * other.z + m44 * other.w;

        return New;
    }
    
};

void clipToNDC(Vec4& vertex)
{
    if(vertex.w == 0.0)
        return;

    vertex.x /= vertex.w;
    vertex.y /= vertex.w;
    vertex.z /= vertex.w;
}
void ndcToScreen(Vec4& vertex, const InfoHeader& infoHeader)
{
    vertex.x += 1;
    vertex.y += 1;
    vertex.z += 1;

    vertex.x /= 2;
    vertex.y /= 2;
    vertex.z /= 2;

    vertex.x *= infoHeader.width;
    vertex.y *= infoHeader.height;

    vertex.y = infoHeader.height - vertex.y;    //  Flipped for bmp coordinates

    return;
}

struct Mat3
{
    float
        m11,
        m12, m13,
        m21, m22, m23,
        m31, m32, m33;

    Mat3(const Mat4 &mat4)
    {
        m11 = mat4.m11;
        m12 = mat4.m12;
        m13 = mat4.m13;
        m21 = mat4.m21;
        m22 = mat4.m22;
        m23 = mat4.m23;
        m31 = mat4.m31;
        m32 = mat4.m32;
        m33 = mat4.m33;
    };

    Vec3 operator*(const Vec3 &other)
    {
        Vec3 New;
        New.x = m11 * other.x + m12 * other.y + m13 * other.z;
        New.y = m21 * other.x + m22 * other.y + m23 * other.z;
        New.z = m31 * other.x + m32 * other.y + m33 * other.z;

        return New;
    }
};