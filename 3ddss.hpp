/*! Name: Abinash Meher
 *  Roll No.: 13CS10003
 *  Assignment 1
 */
#ifdef DEBUG_ON
#define DPRINT(X); std::cout << X << std::endl; std::cout.flush();
#else
#define DPRINT(X); ;
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <cfloat>
#include <cmath>

typedef struct tdpoint {
    float x;
    float y;
    float z;
    tdpoint(float x_ = 0.0, float y_ = 0.0, float z_ = 0.0): x(x_), y(y_), z(z_) {};
    tdpoint operator+(const tdpoint& point) {
        tdpoint result(x + point.x, y + point.y, z + point.z);
        return result;
    }
    tdpoint operator-(const tdpoint& point) {
        tdpoint result(x - point.x, y - point.y, z - point.z);
        return result;
    }
    tdpoint& operator=(const tdpoint& point) {
        x = point.x;
        y = point.y;
        z = point.z;

        return *this;
    }

    bool operator<(const tdpoint& p) const {
        if (x < p.x) {
            return true;
        } else if (x > p.x) {
            return false;
        } else if (y < p.y) {
            return true;
        } else if (y > p.y) {
            return false;
        } else if (z < p.z) {
            return true;
        } else if (z > p.z) {
            return false;
        } else {
            return false;
        }
    }

} tdp;

typedef struct tdhexhed {
    tdp vertices[8];
} tdhexhed;

struct face {
    std::vector<int> corners;
    face() {};
    face(int one_, int two_, int three_, int four_): corners{one_, two_, three_, four_} {};
    face(int one_, int two_, int three_): corners{one_, two_, three_} {};
    void push_back(int f) {
        corners.push_back(f);
    }
    std::vector<int> getCorners() {
        return corners;
    }
};

tdp crossProduct(const tdp a, const tdp b) {
    tdp axb(a.y * b.z - b.y * a.z,
            a.z * b.x - b.z * a.x,
            a.x * b.y - b.x * a.y);

    return axb;
}

tdhexhed MakeVoxel(tdp centre, float side_length) {
    tdhexhed vox;

    for (int i = 0; i < 8; i++) {
        int j = i;
        vox.vertices[i] = centre;
        int rem = j % 2;
        j /= 2;

        if (rem == 0) {
            vox.vertices[i].x = centre.x + side_length / 2;
        } else {
            vox.vertices[i].x = centre.x - side_length / 2;
        }

        rem = j % 2;
        j /= 2;

        if (rem == 0) {
            vox.vertices[i].y = centre.y + side_length / 2;
        } else {
            vox.vertices[i].y = centre.y - side_length / 2;
        }

        rem = j % 2;
        j /= 2;

        if (rem == 0) {
            vox.vertices[i].z = centre.z + side_length / 2;
        } else {
            vox.vertices[i].z = centre.z - side_length / 2;
        }
    }

    return vox;
}

std::vector<face> GetFacesHexHed() {
    std::vector<face> faces;
    face temp1(5, 7, 8, 6);
    face temp2(8, 4, 2, 6);
    face temp3(7, 3, 4, 8);
    face temp4(1, 2, 4, 3);
    face temp5(1, 3, 7, 5);
    face temp6(1, 5, 6, 2);
    faces.push_back(temp1);
    faces.push_back(temp2);
    faces.push_back(temp3);
    faces.push_back(temp4);
    faces.push_back(temp5);
    faces.push_back(temp6);
    return faces;
}



class ObjWriter {
    int last_zero;
    int line_counter;
    std::ostream& file_;
    std::vector<tdp> points_;
    std::vector<face> faces_;
    void write(std::ostream& file) {
        for(std::vector<tdp>::iterator it = points_.begin(); it != points_.end(); it++) {
            file << "v " << it->x << " " << it->y << " " << it->z << "\n";
            line_counter++;
        }

        for(std::vector<face>::iterator it = faces_.begin(); it != faces_.end(); it++) {
            file << "f "    << last_zero + it->corners[0] << " " << last_zero + it->corners[1] << " "
                 << last_zero + it->corners[2] << " " << last_zero + it->corners[3] << "\n";
            //line_counter++;
        }

        last_zero = line_counter;
    }

public:
    ObjWriter(std::ostream& file): file_(file), line_counter(0), last_zero(0) {};
    ~ObjWriter() {
        //file_.close();
    }

    void AddPoint(tdp elem) {
        points_.push_back(elem);
    }

    int AddFace(int one, int two, int three, int four) {
        face temp(one, two, three, four);
        faces_.push_back(temp);
        return 1;
    }

    int AddFace(face face_elem) {
        faces_.push_back(face_elem);
        return 1;
    }

    void EndObj() {
        write(file_);
        points_.clear();
        faces_.clear();
    }
};

class ObjReader {
    std::ifstream& infile_;
    int vertex_counter_;
    std::vector<tdp> vertex_list;
    std::vector<face> face_list;
public:
    ObjReader(std::ifstream& file): infile_(file), vertex_counter_(0) {
        std::string vftype;
        std::string line;
        int line_counter = 0;
        vertex_counter_ = 0;

        while(std::getline(infile_, line)) {
            line_counter++;
            std::stringstream ss(line);

            if (!(ss >> vftype)) {
                continue;
            }

            if (vftype == "v") {
                float val;
                tdp temp;

                if (ss >> val) {
                    temp.x = val;
                } else {
                    std::cout << "Error in reading x at " << line_counter << std::endl;
                    exit(1);
                }

                if (ss >> val) {
                    temp.y = val;
                } else {
                    std::cout << "Error in reading y at " << line_counter << std::endl;
                    exit(1);
                }

                if (ss >> val) {
                    temp.z = val;
                } else {
                    std::cout << "Error in reading z at " << line_counter << std::endl;
                    exit(1);
                }

                vertex_list.push_back(temp);
                vertex_counter_++;

            } else if (vftype == "f") {
                int val;
                face temp;

                // TODO: check if the vertex mentioned in faces is a valid one
                if (ss >> val) {
                    temp.push_back(val);
                } else {
                    std::cout << "Error in reading face at " << line_counter << std::endl;
                    exit(1);
                }

                if (ss >> val) {
                    temp.push_back(val);
                } else {
                    std::cout << "Error in reading face at " << line_counter << std::endl;
                    exit(1);
                }

                if (ss >> val) {
                    temp.push_back(val);
                } else {
                    std::cout << "Error in reading face at " << line_counter << std::endl;
                    exit(1);
                }

                if (ss >> val) {
                    temp.push_back(val);
                }

                face_list.push_back(temp);
            } else {

            }
        } //while

    };

    std::vector<face> getFaceList() const {
        return face_list;
    }

    std::vector<tdp> getVertexList() const {
        return vertex_list;
    }

    //TODO: http://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order

};

enum Axis {
    X, Y, Z
};

class Vector4 {
    friend class Matrix4;
    double data[4];
    double& x, y, z;
public:
    Vector4(): data {
        0, 0, 0, 1
    }, x(data[0]), y(data[1]), z(data[2]) {};
    Vector4(std::vector<double> a): data{a[0], a[1], a[2], a[3]}, x(data[0]), y(data[1]), z(data[2]) {};
    Vector4(tdp point): data{point.x, point.y, point.z, 1}, x(data[0]), y(data[1]), z(data[2]) {};
    Vector4(double x_coord, double y_coord, double z_coord): data{x_coord, y_coord, z_coord, 1}, x(data[0]), y(data[1]), z(data[2]) {};
    tdp getTDP() {
        return tdp(data[0], data[1], data[2]);
    }
    double& operator[](int indexb1) {
        return data[indexb1 - 1];
    }
    double at(int index) const {
        return data[index];
    }
    Vector4& operator=(const Vector4& operand) {
        data[0] = operand.data[0];
        data[1] = operand.data[1];
        data[2] = operand.data[2];
        data[3] = operand.data[3];
        return (*this);
    }
    /*
     *double& x() {
     *    return data[0];
     *}
     *double& y() {
     *    return data[1];
     *}
     *double& z() {
     *    return data[2];
     *}
     */
    std::string str() {
        std::ostringstream ss;
        ss << "[[" << data[0] << "],[" << data[1] << "],[" << data[2] << "],[" << data[3] << "]]";
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& output, const Vector4& vec) {
        output << "[[" << vec.at(0) << "],[" << vec.at(1) << "],[" << vec.at(2) << "],[" << vec.at(3) << "]]";
        return output;
    }

    friend std::istream& operator>>(std::istream& input, Vector4& vec) {
        input >> vec[1] >> vec[2] >> vec[3] >> vec[4];
        return input;
    }
private:
    double operator*(Vector4& b) {
        double sum = 0;

        for (int i = 1; i <= 4; i++) {
            sum += this->operator[](i) * b[i];
        }

        return sum;
    }
};

class Matrix4 {
    Vector4 data[4];
public:
    Matrix4(): data{
        Vector4({1, 0, 0, 0}), Vector4({0, 1, 0, 0}), Vector4({0, 0, 1, 0}), Vector4({0, 0, 0, 1})
    } {};
    Vector4& operator[](int indexb1) {
        return data[indexb1 - 1];
    }
    Vector4 at(int index) const {
        return data[index];
    }
    std::string print() {
        std::ostringstream ss;
        ss <<  "[[" << data[0][1] << ",\t" << data[0][2] << ",\t" << data[0][3] << ",\t" << data[0][4] << "],\n" <<
           " [" << data[1][1] << ",\t" << data[1][2] << ",\t" << data[1][3] << ",\t" << data[1][4] << "],\n" <<
           " [" << data[2][1] << ",\t" << data[2][2] << ",\t" << data[2][3] << ",\t" << data[2][4] << "],\n" <<
           " [" << data[3][1] << ",\t" << data[3][2] << ",\t" << data[3][3] << ",\t" << data[3][4] << "]]\n";
        return ss.str();
    }

    Vector4 operator*(Vector4& b) {
        Vector4 result;
        result[1] = data[0]*b;
        result[2] = data[1]*b;
        result[3] = data[2]*b;
        result[4] = data[3]*b;
        return result;
    }

    Matrix4 operator*(Matrix4& b) {
        Matrix4 result;
        for (int i = 1; i <= 4; i++) {
            for (int j = 1; j <= 4; j++) {
                result[i][j] = 0;
                for (int k = 1; k <=4; k++) {
                    result[i][j] += this->operator[](i)[k]*b[k][j];
                }
            }
        }
        return result;
    }

    Matrix4& rotateDegAlong(double degree, Axis a) {
        Matrix4 mat;
        double rad = (degree / 180) * M_PI;
        int one_index;
        switch(a) {
            case Axis::X:
                one_index = 0;
                break;
            case Axis::Y:
                one_index = 1;
                break;
            case Axis::Z:
                one_index = 2;
                break;
            default:
                one_index = 0;
                break;
        }
        double cost = cos(rad);
        double sint = sin(rad);
        int cos_index = (one_index + 1) % 3;
        mat.data[cos_index][cos_index + 1] = cost;
        int msin_index = (cos_index + 1) % 3;
        mat.data[cos_index][msin_index + 1] = -sint;
        cos_index = (one_index + 2) % 3;
        int sin_index = (cos_index - 1 + 3) % 3;
        mat.data[cos_index][cos_index + 1] = cost;
        mat.data[cos_index][sin_index + 1] = sint;
        Matrix4 result = mat*(*this);
        data[0] = result.at(0);
        data[1] = result.at(1);
        data[2] = result.at(2);
        data[3] = result.at(3);
        return (*this);
    }

    friend std::ostream& operator<<(std::ostream& output, const Matrix4& mat) {
        output <<  "[[" << mat.at(0).at(0) << "," << mat.at(0).at(1) << "," << mat.at(0).at(2) << "," << mat.at(0).at(3) << "],\n" <<
           " [" << mat.at(1).at(0) << "," << mat.at(1).at(1) << "," << mat.at(1).at(2) << "," << mat.at(1).at(3) << "],\n" <<
           " [" << mat.at(2).at(0) << "," << mat.at(2).at(1) << "," << mat.at(2).at(2) << "," << mat.at(2).at(3) << "],\n" <<
           " [" << mat.at(3).at(0) << "," << mat.at(3).at(1) << "," << mat.at(3).at(2) << "," << mat.at(3).at(3) << "]]\n";
        return output;
    }

    friend std::istream& operator>>(std::istream& input, Matrix4& mat) {
        input >> mat[1] >> mat[2] >> mat[3] >> mat[4];
        return input;
    }
};

bool isAntiClockwise(face f, std::vector<tdp>& vertex_list, tdp view) {
    std::vector<int> corners = f.getCorners();
    float sum = 0;
    int length = corners.size(); // TODO: fix with correct name

    tdp a = vertex_list[corners[2]] - vertex_list[corners[1]];
    tdp b = vertex_list[corners[0]] - vertex_list[corners[1]];
    tdp p = vertex_list[corners[1]] - view;
    tdp cp = crossProduct(a, b);
    DPRINT("a :" << a.x << " " << a.y << " " << a.z);
    DPRINT("b :" << b.x << " " << b.y << " " << b.z);

    float dot = p.x * cp.x + p.y * cp.y + p.z * cp.z;

    if (dot < 0) {
        return true;
    } else {
        return false;
    }

    /*
     *    for (int it = 0; it < length; it++) {
     *        int second = (it + 1) % length;
     *        sum += (vertex_list[corners[second]].x - vertex_list[corners[it]].x) * (vertex_list[corners[second]].y + vertex_list[corners[it]].y);
     *    }
     *
     *    DPRINT("sum " << sum);
     *
     *    if (sum < 0) {
     *        return true;
     *    } else {
     *        return false;
     *    }
     */

    DPRINT("isAntiClockwise\n");
}

tdp project(const tdp point, const tdp view) {
    tdp temp;
    // TODO: point and view z shouldn't be same
    temp.x = view.x - view.z * (point.x - view.x) / (point.z - view.z);
    temp.y = view.y - view.z * (point.y - view.y) / (point.z - view.z);
    temp.z = 0;
    return temp;
}

class FaceComp {
private:
    std::vector<tdp> vertices;
public:
    FaceComp(std::vector<tdp> vertex_list): vertices(vertex_list) {};
    bool operator() (face a, face b) {
        float az = 0, bz = 0;

        for (auto ita : a.getCorners()) {
            tdp vertex = vertices[ita - 1];
            az += vertex.z;
        }

        for (auto itb : b.getCorners()) {
            tdp vertex = vertices[itb - 1];
            bz += vertex.z;
        }

        if (az <= bz) {
            return true;
        } else {
            return false;
        }
    }
};

void sort(std::vector<face>& faces, std::vector<tdp> vertices) {
    FaceComp comparator(vertices);
    std::sort(faces.begin(), faces.end(), comparator);

    for (auto fit : faces) {
        for (auto vnumit : fit.getCorners()) {
            tdp vit = vertices[vnumit - 1];
            //DPRINT("Verz : " << vit.z);
        }
    }
}

std::pair<tdp, tdp> getBoundingBox(const std::vector<face>& faces, const std::vector<tdp>& pr_vertices) {
    tdp leftdown_corner, rightup_corner;
    leftdown_corner.z = 0;
    rightup_corner.z = 0;
    leftdown_corner.x = FLT_MAX;
    leftdown_corner.y = FLT_MAX;
    rightup_corner.x = - FLT_MAX;
    rightup_corner.y = - FLT_MAX;

    for (auto fit : faces) {
        for (auto vnumit : fit.getCorners()) {
            tdp vit = pr_vertices[vnumit - 1];

            if (vit.x < leftdown_corner.x) {
                leftdown_corner.x = vit.x;
                //DPRINT(leftdown_corner.x);
            }

            if (vit.y < leftdown_corner.y) {
                leftdown_corner.y = vit.y;
                //DPRINT(leftdown_corner.y);
            }

            if (vit.x > rightup_corner.x) {
                rightup_corner.x = vit.x;
                //DPRINT(rightup_corner.x);
            }

            if (vit.y > rightup_corner.y) {
                rightup_corner.y = vit.y;
                //DPRINT(rightup_corner.y);
            }
        }
    }

    return std::make_pair(leftdown_corner, rightup_corner);
}

void move2origin(const std::vector<face>& pr_faces, std::vector<tdp>& pr_vertices) {
    auto bounding_box = getBoundingBox(pr_faces, pr_vertices);
    tdp leftdown_corner = bounding_box.first;
    tdp rightup_corner = bounding_box.second;

    tdp origin(0, 0, 0);
    tdp transform = origin - leftdown_corner;

    for (auto fit : pr_faces) {
        for (auto vnumit : fit.getCorners()) {
            tdp& vit = pr_vertices[vnumit - 1];
            vit = vit + transform;
        }
    }
}

// The bounding box should be at the origin to perform this
void scaleTo(float length, float breadth, const std::vector<face>& pr_faces, std::vector<tdp>& pr_vertices) {
    auto bounding_box = getBoundingBox(pr_faces, pr_vertices);
    tdp leftdown_corner = bounding_box.first;
    tdp rightup_corner = bounding_box.second;

    float current_length = rightup_corner.x - leftdown_corner.x;
    float current_breadth = rightup_corner.y - leftdown_corner.y;

    float scale_x = length / current_length;
    float scale_y = breadth / current_breadth;

    float scale = length < breadth ? scale_x : scale_y;

    for (auto& it : pr_vertices) {
        it.x = it.x * scale;
        it.y = it.y * scale;
    }
}

class SVGDrawer {
    float fr_height;
    float fr_width;
    std::vector<tdp> vertex_list;
    std::vector<face> face_list;
public:
    SVGDrawer(int height, int width, std::vector<tdp> vertices, std::vector<face> sorted_faces):
        fr_height(height), fr_width(width), vertex_list(vertices), face_list(sorted_faces) {};
    void draw(std::string filename) {
        std::ofstream file(filename);
        //file << "<!DOCTYPE html>\n<html>\n<body>\n";
        file << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" height=\"" << fr_height << "\" width=\"" << fr_width << "\">" << std::endl;

        for (auto fit = face_list.begin(); fit != face_list.end(); fit++) {
            file << "    <polygon points=\"";

            for (auto vnumit : fit->getCorners()) {
                tdp vertex = vertex_list[vnumit - 1];
                file << vertex.x << "," << vertex.y << " ";
            }

            file << "\" style=\"fill:lime;stroke:purple;stroke-width:5;fill-rule:nonzero;\" />" << std::endl;
        }

        file << "</svg>" << std::endl;
        //file << "</body>\n</html>\n" ;
        file.close();
    }
};

void bringToObjSpace(std::vector<tdp>& vertices) {
    float maxz = 0;

    for (auto it : vertices) {
        DPRINT("v " << it.z);
    }

    for (auto it : vertices) {
        if (it.z > maxz) {
            maxz = it.z;
        }
    }

    for (auto& it : vertices) {
        it.z -= maxz + 1;
        DPRINT(it.z);
    }
}

SVGDrawer drawSVG(ObjReader obj, int height, int width, int margin, tdp view) {
    float bb_length = width - 2 * margin;
    float bb_width = height - 2 * margin;

    auto faces = obj.getFaceList();
    auto vertices = obj.getVertexList();

    bringToObjSpace(vertices);
    //for (auto it: vertices) {
    //DPRINT("b2o :  " << it.z);
    //}
    //for (auto fit: faces) {
    //for (auto vnumit: fit.getCorners()) {
    //tdp vit = vertices[vnumit - 1 ];
    //DPRINT("b2O : " << vit.z);
    //}
    //}
    DPRINT("In Obj Space");
    sort(faces, vertices);

    auto fit = faces.begin();

    while (fit != faces.end()) {
        if (!isAntiClockwise(*fit, vertices, view)) {
            fit = faces.erase(fit);
        } else {
            fit++;
        }
    }


    //for (auto fit: faces) {
    //for (auto vnumit: fit.getCorners()) {
    //tdp vit = vertices[vnumit - 1];
    //DPRINT("Verz : " << vit.z);
    //}
    //}

    for (auto& vit : vertices) {
        vit = project(vit, view);
    }

    DPRINT("Projected");

    move2origin(faces, vertices);
    DPRINT("Moved to origin");

    scaleTo(bb_length, bb_width, faces, vertices);
    DPRINT("Scaled");

    // Displace by margin on both x and y axes
    for (auto& vit : vertices) {
        vit.x += margin;
        vit.y += margin;
    }

    DPRINT("Displaced by margin");

    // Flip the y axis so that coordinates of svg are generated as expected
    for (auto& vit : vertices) {
        vit.y = height - vit.y;
    }

    DPRINT("Axes flipped");

    DPRINT("Clockwise removed");
    DPRINT("Faces num " << faces.size());

    SVGDrawer drawer(height, width, vertices, faces);
    return drawer;
}

int Maxof(int a, int b, int c) {
    if(a >= b && a >= c) {
        return a;
    } else if(b >= a && b >= c) {
        return b;
    } else {
        return c;
    }
}

int Minof(int a, int b, int c) {
    if(a <= b && a <= c) {
        return a;
    } else if(b <= a && b <= c) {
        return b;
    } else {
        return c;
    }
}

std::vector<tdp> Draw3DLine(tdp start, tdp end, Axis max_axis) {
    int x = (int)start.x;
    int y = (int)start.y;
    int z = (int)start.z;

    int dx = (int)(end.x - start.x);
    int dy = (int)(end.y - start.y);
    int dz = (int)(end.z - start.z);

    bool dx_neg = false, dy_neg = false, dz_neg = false;

    std::vector<tdp> pointList;
    tdp zth(x, y, z);
    pointList.push_back(zth);

    if(dx < 0) {
        dx = -dx;
        dx_neg = true;
    }

    if(dy < 0) {
        dy = -dy;
        dy_neg = true;
    }

    if(dz < 0) {
        dz = -dz;
        dz_neg = true;
    }

    int fxy = 2 * dy - dx;
    int fxz = 2 * dz - dx;
    int fyx = 2 * dx - dy;
    int fyz = 2 * dz - dy;
    int fzx = 2 * dx - dz;
    int fzy = 2 * dy - dz;

    switch(max_axis) {
        case Axis::X: // when dx is maximum
            while(x < end.x) {
                x++;

                if(fxy > 0) {
                    y++;
                    fxy += 2 * (dy - dx);
                } else {
                    fxy += 2 * dy;
                }

                if(fxz > 0) {
                    z++;
                    fxz += 2 * (dz - dx);
                } else {
                    fxz += 2 * dz;
                }

                tdp temp;

                if(dx_neg) {
                    temp.x = 2 * start.x - x;
                } else {
                    temp.x = x;
                }

                if(dy_neg) {
                    temp.y = 2 * start.y - y;
                } else {
                    temp.y = y;
                }

                if(dz_neg) {
                    temp.z = 2 * start.z - z;
                } else {
                    temp.z = z;
                }

                pointList.push_back(temp);

            }

            break;

        case Axis::Y: // when dy is maximum
            while(y < end.y) {
                y++;

                if(fyx > 0) {
                    x++;
                    fyx += 2 * (dx - dy);
                } else {
                    fyx += 2 * dx;
                }

                if(fyz > 0) {
                    z++;
                    fyz += 2 * (dz - dy);
                } else {
                    fyz += 2 * dz;
                }

                tdp temp;

                if(dx_neg) {
                    temp.x = 2 * start.x - x;
                } else {
                    temp.x = x;
                }

                if(dy_neg) {
                    temp.y = 2 * start.y - y;
                } else {
                    temp.y = y;
                }

                if(dz_neg) {
                    temp.z = 2 * start.z - z;
                } else {
                    temp.z = z;
                }

                pointList.push_back(temp);
            }

            break;

        case Axis::Z: // when dz is maximum
            while(z < end.z) {
                z++;

                if(fzy > 0) {
                    y++;
                    fzy += 2 * (dy - dz);
                } else {
                    fzy += 2 * dy;
                }

                if(fzx > 0) {
                    x++;
                    fzx += 2 * (dx - dz);
                } else {
                    fzx += 2 * dx;
                }

                tdp temp;

                if(dx_neg) {
                    temp.x = 2 * start.x - x;
                } else {
                    temp.x = x;
                }

                if(dy_neg) {
                    temp.y = 2 * start.y - y;
                } else {
                    temp.y = y;
                }

                if(dz_neg) {
                    temp.z = 2 * start.z - z;
                } else {
                    temp.z = z;
                }

                pointList.push_back(temp);
            }

            break;
    }

    return pointList;
}

// New 3DGetLine function
std::vector<tdp> Get3Dline(tdp start, tdp end, int scale) {
    int dx = (int)(end.x - start.x);
    int dy = (int)(end.y - start.y);
    int dz = (int)(end.z - start.z);
    int max = Maxof(abs(dx), abs(dy), abs(dz));
    Axis max_axis;

    if(max == abs(dx)) {
        max_axis = Axis::X;

        if(dx <= 0) {
            std::swap(start, end);
        }
    } else if(max == abs(dy)) {
        max_axis = Axis::Y;

        if(dy <= 0) {
            std::swap(start, end);
        }
    } else {
        max_axis = Axis::Z;

        if(dz <= 0) {
            std::swap(start, end);
        }
    }

    return Draw3DLine(start, end, max_axis);
}

// Pass a vector that contains the points of a straight line. If the vector doesn't
// contain points of the straight line the class might misbehave
class TDLine {
private:
    enum Mode {
        FORWARD, BACKWARD
    };
    enum Seq {
        INC, DEC
    };
    Mode mode;
    Axis ax;
    Seq sequence;
    std::vector<tdp>::iterator forward;
    std::vector<tdp>& line; //Reference of the line on which this will iterate

public:
    /*! true for forward iteration, false for backward iteration
     */
    TDLine(std::vector<tdp>& tdline, Axis a, bool iter_mode = true): line(tdline), mode(iter_mode ? Mode::FORWARD : Mode::BACKWARD), ax(a) {
        tdp front = line.front();
        tdp back = line.back();
        //std::cout << "Constructor\n";

        /*
         *if (mode == Mode::FORWARD) {
         *    std::cout << "FORWARD\n";
         *} else {
         *    std::cout << "BACKWARD\n";
         *}
         */
        switch (a) {
            case Axis::X:
                if (front.x <= back.x) {
                    sequence = Seq::INC;
                    //std::cout << "XINC\n";
                } else {
                    sequence = Seq::DEC;
                    //std::cout << "XDEC\n";
                }

                break;

            case Axis::Y:
                if (front.y <= back.y) {
                    sequence = Seq::INC;
                    //std::cout << "YINC\n";
                } else {
                    sequence = Seq::DEC;
                    //std::cout << "YDEC\n";
                }

                break;

            case Axis::Z:
                if (front.z <= back.z) {
                    sequence = Seq::INC;
                    //std::cout << "INC\n";
                } else {
                    sequence = Seq::DEC;
                    //std::cout << "DEC\n";
                }

                break;
        }

        if (mode == Mode::FORWARD) {
            if (sequence == Seq::INC) {
                forward = line.begin();
                //std::cout << "forward = first\n";
            } else {
                if (line.size() == 0) {
                    forward = line.end();
                }

                forward = std::prev(line.end());
                //std::cout << "forward = last\n";
            }
        } else { // Backward
            //DPRINT("back");
            if (sequence == Seq::INC) {
                //DPRINT("increasing");
                if (line.size() == 0) {
                    forward = line.end();
                }

                forward = std::prev(line.end());
                //std::cout << "forward = last\n";
            } else {
                //DPRINT("came here");
                forward = line.begin();
                //std::cout << "forward = first\n";
            }
        }
    }

    std::vector<tdp>::iterator get_iter() {
        //std::cout << "get_iter()\n";
        return forward;
    }

    float val_along_axis() {
        switch (ax) {
            case Axis::X:
                return forward->x;

            case Axis::Y:
                return forward->y;

            case Axis::Z:
                return forward->z;
        }
    }

    float val_perp_axis() {
        switch (ax) {
            case Axis::X:
                return forward->z;

            case Axis::Y:
                return forward->x;

            case Axis::Z:
                return forward->y;
        }
    }

    void set_tdp_perp(tdp& p, float along, float perp) {
        switch (ax) {
            case Axis::X:
                p.x = along;
                p.z = perp;
                p.y = 0;
                return;

            case Axis::Y:
                p.y = along;
                p.x = perp;
                p.z = 0;
                return;

            case Axis::Z:
                p.z = along;
                p.y = perp;
                p.x = 0;
                return;
        }
    }

    std::vector<tdp>::iterator get_next() {
        if (forward == line.end()) {
            return line.end();
        }

        if (mode == Mode::FORWARD) {
            if (sequence == Seq::INC) {
                forward++;

                if (forward == line.end()) {
                    return line.end();
                }

                return forward;
            } else { // Seq::DEC
                if (forward == line.begin()) {
                    forward = line.end();


                    return line.end();
                }

                --forward;
                return forward;
            }
        } else { // BACKWARD
            if (sequence == Seq::INC) {
                if (forward == line.begin()) {
                    forward = line.end();


                    return line.end();
                }

                --forward;
                return forward;
            } else { // Seq::DEC
                forward++;

                if (forward == line.end()) {
                    return line.end();
                }

                return forward;
            }

        }
    }

    std::vector<tdp>::iterator get_prev() {
        if (forward == line.end()) {
            return line.end();
        }

        if (mode == Mode::FORWARD) {
            if (sequence == Seq::INC) {
                if (forward == line.begin()) {
                    forward = line.end();


                    return line.end();
                }

                --forward;
                return forward;
            } else { // Seq::DEC
                forward++;

                if (forward == line.end()) {
                    return line.end();
                }

                return forward;
            }
        } else { // BACKWARD
            if (sequence == Seq::INC) {
                forward++;

                if (forward == line.end()) {
                    return line.end();
                }

                return forward;
            } else { // Seq::DEC
                if (forward == line.begin()) {
                    forward = line.end();


                    return line.end();
                }

                --forward;
                return forward;
            }

        }
    }

    std::vector<tdp>::iterator begin() {
        //std::cout << "begin()\n";
        if (mode == Mode::FORWARD) {
            if (sequence == Seq::INC) {
                return line.begin();
            } else {
                if (line.size() == 0) {
                    return line.end();
                }

                return std::prev(line.end());
            }
        } else { // Backward
            if (sequence == Seq::INC) {
                if (line.size() == 0) {
                    return line.end();
                }

                return std::prev(line.end());
            } else {
                return line.begin();
            }
        }
    }

    std::vector<tdp>::iterator end() {
        return line.end();
    }
};

void print_tdp_set(std::vector<tdp> vector) {
    for (std::vector<tdp>::iterator it = vector.begin(); it != vector.end(); it++) {
        std::stringstream ss;
        ss << it->x << "_" << it->y << "_" << it->z ;
        DPRINT(ss.str());
    }
}

//int main() {

//}
