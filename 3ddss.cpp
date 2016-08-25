#include "3ddss.hpp"

int main(int argc, char** argv) {
    if (argc != 7) {
        std::cerr << "Wrong number of arguments" << std::endl;
        exit(1);
    }

    std::stringstream ss;
    tdp start;
    //std::cout << " Enter the coordinates of the start point <x y z>:" << std::endl;
    ss << argv[1] << " " << argv[2] << " " << argv[3];
    ss >> start.x >> start.y >> start.z;
    tdp end;
    //std::cout << " Enter the coordinates of the end point <x y z>:" << std::endl;
    //
    std::stringstream ss1;
    ss1 << argv[4] << " " << argv[5] << " " << argv[6];
    ss1 >> end.x >> end.y >> end.z;

    std::vector<tdp> line_points = Get3Dline(start, end, 1);
//  std::stringstream file_name;
//  file_name   << start.x << "_" << start.y << "_" << start.z << "_to_"
//            << end.x << "_" << end.y << "_" << end.z << ".obj";

//  std::ofstream file(file_name.str());
    ObjWriter objw(std::cout);
    std::vector<face> facelist = GetFacesHexHed();
    TDLine tdl(line_points, Axis::Z, true);
    //std::cout << "\n";

    for (std::vector<tdp>::iterator it = line_points.begin(); it != line_points.end(); it++) {
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

    return 0;
}
