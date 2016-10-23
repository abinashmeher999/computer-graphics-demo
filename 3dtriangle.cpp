//#define DEBUG_ON
#include "3ddss.hpp"
#include <unistd.h>
#include <set>

/*
 *enum trtype {
 *    TOP_FLAT,
 *    BOTTOM_FLAT,
 *    NORMAL
 *};
 */

void deduplicate(std::vector<tdp>& points) {
    std::set<tdp> s;
    unsigned size = points.size();

    for( unsigned i = 0; i < size; ++i ) {
        s.insert( points[i] );
    }

    points.assign( s.begin(), s.end() );
}

void sortin(Axis axis, tdp& pa, tdp& pb, tdp& pc, bool& anb_eq, bool& bnc_eq) {
    float a, b, c;

    if (axis == Axis::X) {
        a = pa.x;
        b = pb.x;
        c = pc.x;
    } else if (axis == Axis::Y) {
        a = pa.y;
        b = pb.y;
        c = pc.y;
    } else {
        a = pa.z;
        b = pb.z;
        c = pc.z;
    }

    tdp min, max, med;

    if( a > b ) {
        if( a > c ) {
            max = pa;

            if( b > c ) {
                med = pb;
                min = pc;
            } else {
                med = pc;
                min = pb;
            }
        } else {
            med = pa;

            if( b > c ) {
                max = pb;
                min = pc;
            } else {
                max = pc;
                min = pb;
            }
        }
    } else {
        if( b > c ) {
            max = pb;

            if( a > c ) {
                med = pa;
                min = pc;
            } else {
                med = pc;
                min = pa;
            }
        } else {
            med = pb;
            max = pc;
            min = pa;
        }
    }

    if (a == b) {
        anb_eq = true;
    } else {
        anb_eq = false;
    }

    if (b == c) {
        bnc_eq = true;
    } else {
        bnc_eq = false;
    }

    pa = max;
    pb = med;
    pc = min;
}

void sort(Axis& max, tdp& cross_product, tdp& pa, tdp& pb, tdp& pc, bool& anb_eq, bool& bnc_eq) {
    tdp pab, pac;
    pab = pb - pa;
    pac = pc - pa;

    tdp pabxpac(pab.y * pac.z - pac.y * pab.z,
                pab.z * pac.x - pac.z * pab.x,
                pab.x * pac.y - pac.x * pab.y);

    cross_product = pabxpac;

    if (abs(pabxpac.x) >= abs(pabxpac.y) && abs(pabxpac.x) >= abs(pabxpac.z)) {
        max = Axis::X;
        DPRINT("\tX");
    } else if (abs(pabxpac.y) >= abs(pabxpac.x) && abs(pabxpac.y) >= abs(pabxpac.z)) {
        max = Axis::Y;
        DPRINT("\tY");
    } else {
        max = Axis::Z;
        DPRINT("\tZ");
    }

    Axis plane_along_down;

    switch (max) {
        case Axis::X:
            plane_along_down = Axis::Z;
            break;

        case Axis::Y:
            plane_along_down = Axis::X;
            break;

        case Axis::Z:
            plane_along_down = Axis::Y;
            break;
    }

    sortin(plane_along_down, pa, pb, pc, anb_eq, bnc_eq);
}

int find3rdcoord(Axis max_axis, int min, int max, tdp point, int A, int B, int C, int D) {
    int val, curr;

    switch (max_axis) {
        case Axis::X:
            curr = min;
            val = 2 * (A * curr + B * point.y + C * point.z + D);

            while (val <= -abs(A) || val > abs(A)) {
                if (curr == max) {
                    std::cout << "Error in finding 3D x." << std::endl;
                }

                curr++;
                val = 2 * (A * curr + B * point.y + C * point.z + D);
            }

            return curr;

            break;

        case Axis::Y:
            curr = min;
            val = 2 * (A * point.x + B * curr + C * point.z + D);

            while (val <= -abs(B) || val > abs(B)) {
                if (curr == max) {
                    std::cout << "Error in finding 3D y." << std::endl;
                }

                curr++;
                val = 2 * (A * point.x + B * curr + C * point.z + D);
            }

            return curr;
            break;

        case Axis::Z:
            curr = min;
            val = 2 * (A * point.x + B * point.y + C * curr + D);

            while (val <= -abs(C) || val > abs(C)) {
                if (curr == max) {
                    std::cout << "Error in finding 3D z." << std::endl;
                }

                curr++;
                val = 2 * (A * point.x + B * point.y + C * curr + D);
            }

            return curr;
            break;
    }
}
// Assumes a > b > c in the 2D plane. Sort function should be called before this
void ProjectTo3D(std::vector<tdp>& points, tdp cross_product, tdp a, tdp b, tdp c, Axis max_axis) {
    int D = -(cross_product.x * a.x + cross_product.y * a.y + cross_product.z * a.z);
    int A = cross_product.x, B = cross_product.y, C = cross_product.z;

    int min, max, coeff;

    switch (max_axis) {
        case Axis::X:
            max = Maxof(a.x, b.x, c.x);
            min = Minof(a.x, b.x, c.x);
            coeff = A;

            for (auto& it : points) {
                it.x = find3rdcoord(max_axis, min, max, it, A, B, C, D);
            }

            break;

        case Axis::Y:
            max = Maxof(a.y, b.y, c.y);
            min = Minof(a.y, b.y, c.y);
            coeff = B;

            for (auto& it : points) {
                it.y = find3rdcoord(max_axis, min, max, it, A, B, C, D);
            }

            break;

        case Axis::Z:
            max = Maxof(a.z, b.z, c.z);
            min = Minof(a.z, b.z, c.z);
            coeff = C;

            for (auto& it : points) {
                it.z = find3rdcoord(max_axis, min, max, it, A, B, C, D);
            }

            break;
    }
}

std::vector<tdp> FillTriangle(tdp a, tdp b, tdp c, int scale) {
    tdp pr_a, pr_b, pr_c, cross_product;
    Axis max_axis;
    bool anb_eq, bnc_eq;
    /*! Project the 3 points on the XY plane */
    sort(max_axis, cross_product, a, b, c, anb_eq, bnc_eq);
    pr_a = a;
    pr_b = b;
    pr_c = c;

    if (cross_product.x == 0 && cross_product.y == 0 && cross_product.z == 0) {
        int max = 0;
        std::vector<tdp>* this_one;
        std::vector<tdp> a2b = Get3Dline(a, b, 1);
        if (a2b.size() > max) {
            max = a2b.size();
            this_one = &a2b;
        }
        std::vector<tdp> b2c = Get3Dline(b, c, 1);
        if (b2c.size() > max) {
            max = b2c.size();
            this_one = &b2c;
        }
        std::vector<tdp> a2c = Get3Dline(a, c, 1);
        if (a2c.size() > max) {
            max = a2c.size();
            this_one = &a2c;
        }
        return *this_one;
    }

    Axis crawl_down_axis;

    switch (max_axis) {
        case Axis::X: {
            pr_a.x = pr_b.x = pr_c.x = 0;
            crawl_down_axis = Axis::Z;
            break;
        }

        case Axis::Y: {
            pr_a.y = pr_b.y = pr_c.y = 0;
            crawl_down_axis = Axis::X;
            break;
        }

        case Axis::Z: {
            pr_a.z = pr_b.z = pr_c.z = 0;
            crawl_down_axis = Axis::Y;
            break;
        }
    }


    std::vector<tdp> pr_a2b, pr_a2c, pr_b2c;
    pr_a2b = Get3Dline(pr_a, pr_b, 1);
    pr_a2c = Get3Dline(pr_a, pr_c, 1);
    pr_b2c = Get3Dline(pr_b, pr_c, 1);

    DPRINT("\nA -> B");
    print_tdp_set(pr_a2b);
    DPRINT("\nA -> C");
    print_tdp_set(pr_a2c);
    DPRINT("\nB -> C");
    print_tdp_set(pr_b2c);

    TDLine PRA2B(pr_a2b, crawl_down_axis, false);
    TDLine PRA2C(pr_a2c, crawl_down_axis, false);
    TDLine PRB2C(pr_b2c, crawl_down_axis, false);

    std::vector<tdp> tinner_points;
    std::vector<tdp>::iterator l_it, r_it;
    l_it = PRA2B.begin();
    r_it = PRA2C.begin();

    float l_last_ma = PRA2B.val_along_axis();
    float r_last_ma = PRA2C.val_along_axis();
    float first_ma = l_last_ma;

    float l_curr_ma = l_last_ma;
    float r_curr_ma = r_last_ma;

    bool l_not_end = true;
    bool r_not_end = true;

    float l_curr_perp;
    float r_curr_perp;

    float first_left_perp = PRA2B.val_perp_axis();

    /*
     *if (true) {
     *    tinner_points.push_back(pr_a);
     *    DPRINT("Printed apex");
     *}
     */

    //DPRINT("Printing first half");

    while (l_not_end) {

        while (l_curr_ma == l_last_ma) {
            if (l_it != PRA2B.end()) {
                tinner_points.push_back(*l_it);
            } else {
                l_not_end = false;
                break;
            }

            l_curr_perp = PRA2B.val_perp_axis();
            l_curr_ma = PRA2B.val_along_axis();
            l_it = PRA2B.get_next();
        }

        if (!l_not_end) {
            break;
        }

        while (r_curr_ma == r_last_ma) {
            if (r_it != PRA2C.end()) {
                tinner_points.push_back(*r_it);
            } else {
                r_not_end = false;
                break;
            }

            r_curr_perp = PRA2C.val_perp_axis();
            r_curr_ma = PRA2C.val_along_axis();
            r_it = PRA2C.get_next();
            //trickle down
        }

        if (l_curr_perp > r_curr_perp) {
            std::swap(l_curr_perp, r_curr_perp);
        }

        for (int i = l_curr_perp; i < r_curr_perp; i++) {
            tdp temp;
            PRA2B.set_tdp_perp(temp, l_curr_ma, i);
            tinner_points.push_back(temp);
        }

        l_last_ma = l_curr_ma;
        r_last_ma = r_curr_ma;
    }

    l_it = PRB2C.begin();
    l_not_end = true;

    while (l_not_end) {

        while (l_curr_ma == l_last_ma) {
            if (l_it != PRB2C.end()) {
                tinner_points.push_back(*l_it);
            } else {
                l_not_end = false;
                break;
            }

            l_curr_perp = PRB2C.val_perp_axis();
            l_curr_ma = PRB2C.val_along_axis();
            l_it = PRB2C.get_next();
        }

        if (!l_not_end) {
            break;
        }

        while (r_curr_ma == r_last_ma) {
            if (r_it != PRA2C.end()) {
                tinner_points.push_back(*r_it);
            } else {
                r_not_end = false;
                break;
            }

            r_curr_perp = PRA2C.val_perp_axis();
            r_curr_ma = PRA2C.val_along_axis();
            r_it = PRA2C.get_next();
            //trickle down
        }

        if (l_curr_perp > r_curr_perp) {
            std::swap(l_curr_perp, r_curr_perp);
        }

        for (int i = l_curr_perp; i < r_curr_perp; i++) {
            tdp temp;
            PRA2B.set_tdp_perp(temp, l_curr_ma, i);
            tinner_points.push_back(temp);
        }

        l_last_ma = l_curr_ma;
        r_last_ma = r_curr_ma;
    }

/*
 *    while (l_not_end && r_not_end) {
 *        DPRINT("First");
 *        while (l_curr_ma != l_last_ma - 1) {
 *            //DPRINT("L trickling down");
 *
 *            if (l_it != PRA2B.end()) {
 *                tinner_points.push_back(*l_it);
 *                l_it = PRA2B.get_next();
 *
 *                if (l_it == PRA2B.end()) {
 *                    l_not_end = false;
 *                    break;
 *                }
 *
 *
 *                l_curr_perp = PRA2B.val_perp_axis();
 *                l_curr_ma = PRA2B.val_along_axis();
 *            } else {
 *                l_not_end = false;
 *            }
 *        }
 *
 *        while (r_curr_ma != r_last_ma - 1) {
 *            //DPRINT("R trickling down");
 *
 *            if (r_it != PRA2C.end()) {
 *                tinner_points.push_back(*r_it);
 *                r_it = PRA2C.get_next();
 *
 *                if (r_it == PRA2C.end()) {
 *                    r_not_end = false;
 *                    break;
 *                }
 *
 *
 *                r_curr_perp = PRA2C.val_perp_axis();
 *                r_curr_ma = PRA2C.val_along_axis();
 *            } else {
 *                r_not_end = false;
 *            }
 *        }
 *
 *        if (l_not_end && r_not_end) {
 *
 *            if (l_curr_perp > r_curr_perp) {
 *                int t;
 *                t = l_curr_perp;
 *                l_curr_perp = r_curr_perp;
 *                r_curr_perp = t;
 *            }
 *
 *            for (l_curr_perp++; l_curr_perp < r_curr_perp; l_curr_perp++) {
 *                tdp temp;
 *                PRA2B.set_tdp_perp(temp, l_curr_ma, l_curr_perp);
 *                tinner_points.push_back(temp);
 *            }
 *
 *            l_last_ma = l_curr_ma;
 *            r_last_ma = r_curr_ma;
 *        }
 *    }
 *
 *    if (!l_not_end) {
 *        if (first_ma == l_last_ma) {
 *            r_curr_perp = PRB2C.val_perp_axis();
 *            l_curr_perp = first_left_perp;
 *
 *            for (; l_curr_perp <= r_curr_perp; l_curr_perp++) {
 *                tdp temp;
 *                PRA2B.set_tdp_perp(temp, l_last_ma, l_curr_perp);
 *                tinner_points.push_back(temp);
 *            }
 *        }
 *
 *        l_it = PRB2C.begin();
 *
 *        if (r_not_end) {
 *            r_it = PRA2C.get_prev();
 *        }
 *
 *        l_not_end = true;
 *        r_not_end = true;
 *
 *        while (l_not_end && r_not_end) {
 *            DPRINT("Second");
 *            while (l_curr_ma != l_last_ma - 1) {
 *                DPRINT("L");
 *                if (l_it != PRB2C.end()) {
 *                    tinner_points.push_back(*l_it);
 *                    l_it = PRB2C.get_next();
 *
 *                    if (l_it == PRB2C.end()) {
 *                        l_not_end = false;
 *                        break;
 *                    }
 *
 *
 *                    l_curr_perp = PRB2C.val_perp_axis();
 *                    l_curr_ma = PRB2C.val_along_axis();
 *                } else {
 *                    l_not_end = false;
 *                }
 *            }
 *
 *            while (r_curr_ma != r_last_ma - 1) {
 *                DPRINT("R");
 *                if (r_it != PRA2C.end()) {
 *                    tinner_points.push_back(*r_it);
 *                    r_it = PRA2C.get_next();
 *
 *                    if (r_it == PRA2C.end()) {
 *                        r_not_end = false;
 *                        break;
 *                    }
 *
 *                    r_curr_perp = PRA2C.val_perp_axis();
 *                    r_curr_ma = PRA2C.val_along_axis();
 *                } else {
 *                    r_not_end = false;
 *                }
 *            }
 *
 *            if (l_not_end && r_not_end) {
 *
 *                if (l_curr_perp > r_curr_perp) {
 *                    int t;
 *                    t = l_curr_perp;
 *                    l_curr_perp = r_curr_perp;
 *                    r_curr_perp = t;
 *                }
 *
 *                DPRINT("l_curr_perp" << l_curr_perp);
 *                DPRINT("r_curr_perp" << r_curr_perp);
 *
 *                for (; l_curr_perp <= r_curr_perp; l_curr_perp++) {
 *                    tdp temp;
 *                    PRB2C.set_tdp_perp(temp, l_curr_ma, l_curr_perp);
 *                    tinner_points.push_back(temp);
 *                    DPRINT("Pushed back");
 *                }
 *
 *                l_last_ma = l_curr_ma;
 *                r_last_ma = r_curr_ma;
 *            }
 *
 *
 *        }
 *    }
 */

    ProjectTo3D(tinner_points, cross_product, a, b, c, max_axis);
    deduplicate(tinner_points);

    DPRINT("Ended");

    return tinner_points;
}

void make_obj(std::vector<tdp>& triangle, std::string file_name) {
    //std::ofstream file(file_name);
    ObjWriter objw(std::cout);
    std::vector<face> facelist = GetFacesHexHed();

    for (std::vector<tdp>::iterator it = triangle.begin(); it != triangle.end(); it++) {
        tdhexhed temp_vox = MakeVoxel(*it, 1);
        //std::cout << it->x << "_" << it->y << "_" << it->z << std::endl;

        for (int i = 0; i < 8; i++) {
            objw.AddPoint(temp_vox.vertices[i]);
        }

        for (std::vector<face>::iterator fit = facelist.begin(); fit != facelist.end(); fit++) {
            objw.AddFace(*fit);
        }

        objw.EndObj();
    }
}

int main (int argc, char** argv) {
    tdp a, b, c;
    if (argc != 10) {
        std::cerr << "Insufficient Arguments" << std::endl;
        exit(1);
    }
    //std::cout << " Enter the coordinates of the first point <x y z>:" << std::endl;
    std::stringstream ss;
    ss << argv[1] << " " << argv[2] << " " << argv[3];
    ss >> a.x >> a.y >> a.z;
    //std::cerr << a.x << " " << a.y << " " << a.z << std::endl;
    //std::cout << " Enter the coordinates of the second point <x y z>:" << std::endl;
    std::stringstream ss1;
    ss1 << argv[4] << " " << argv[5] << " " << argv[6];
    ss1 >> b.x >> b.y >> b.z;
    //std::cerr << b.x << " " << b.y << " " << b.z << std::endl;
    //std::cout << " Enter the coordinates of the third point <x y z>:" << std::endl;
    std::stringstream ss2;
    ss2 << argv[7] << " " << argv[8] << " " << argv[9];
    ss2 >> c.x >> c.y >> c.z;
    //std::cerr << c.x << " " << c.y << " " << c.z << std::endl;

    std::stringstream file_name;
    file_name   << "test_triangle.obj";

    std::vector<tdp> triangle = FillTriangle(a, b, c, 1);
    for(auto p : triangle) {
        std::cout << p.x << " " << p.y << " " << p.z << std::endl;
    }
    //DPRINT(triangle.size());
    //print_tdp_set(triangle);
    //make_obj(triangle, file_name.str());
}
