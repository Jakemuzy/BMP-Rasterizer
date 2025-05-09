
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

    //  Assume vertex is vec4 with 4th value of 1
    
    Vec4 operator*(const Vertex& other) const {
        Vec4 vertex = {other.x, other.y, other.z, 1.0};

        Vec4 New;
        New.x = m11 * vertex.x + m12 * vertex.y + m13 * vertex.z + m14 * vertex.w;
        New.y = m21 * vertex.x + m22 * vertex.y + m23 * vertex.z + m24 * vertex.w;
        New.z = m31 * vertex.x + m32 * vertex.y + m33 * vertex.z + m34 * vertex.w;
        New.w = m41 * vertex.x + m42 * vertex.y + m43 * vertex.z + m44 * vertex.w;

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
