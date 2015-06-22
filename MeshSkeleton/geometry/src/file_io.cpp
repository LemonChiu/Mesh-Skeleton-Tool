#include "file_io.h"

#include "point_set.h"
#include "stop_watch.h"
#include "file_system.h"
#include "color.h"
#include "skeleton.h"
#include "mesh.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

// output to console
std::string FileIO::title()
{
    return "[FileIO]: ";
}

// read Object Model
Object* FileIO::readObject(const std::string &file_name, ObjectType &type)
{
    Object *object = NULL;
    type = OBJECT_UNKOWN_TYPE;
    const std::string &ext = FileSystem::extension(file_name);

    // .obj or .ply file
    if (ext == "obj" || ext == "ply"){
        object = readMesh(file_name);
        if (object != NULL) {
            type = OBJECT_MESH_TYPE;
        } else {
            if (ext == "ply") {
                object = readPointSet(file_name);
                if (object != NULL) {
                    type = OBJECT_POINTSET_TYPE;
                }
            }
        }
    }

    return object;
}

Skeleton* FileIO::readSkeleton(const std::string &file_name)
{
    std::ifstream in(file_name.c_str()) ;
    if (in.fail()) {
        std::cerr << title() << "cannot open file: " << file_name << std::endl;
        return NULL;
    }
    std::cout << title()
        << "reading file..." << std::endl;

    Stopwatch clock;
    const std::string &ext = FileSystem::extension(file_name);
    Skeleton* skl = new Skeleton(file_name);

    if (ext == "skl") {
        load_skl(skl, in);
    } else if (ext == "cg") {
        load_cg(skl, in);
    } else if (ext=="out") {
        load_out(skl, in);
    } else {
        std::cout << title()
            << "reading file failed (unknown file format)" << std::endl;
        delete skl;
        in.close();
        return NULL;
    }

    if (skl->nb_nodes() < 1) {
        std::cout << title()
            << "reading file failed (no data exist)" << std::endl;
        delete skl;
        in.close();
        return NULL;
    }

    in.close();
    std::cout << title() << "reading file done. Timing: "
        << clock.elapsed_user_time() << " seconds" << std::endl;

    return skl;
}

bool FileIO::saveSkeleton(Skeleton *skl, const std::string &file_name, Normalizer *normalizer /* = NULL */)
{
    if (normalizer == NULL) {
        std::cerr << title() << "normalizer is null..." << std::endl ;
        return false;
    }

    std::ofstream out(file_name.c_str()) ;
    if (out.fail()) {
        std::cerr << title()
            << "cannot open file: \'" << file_name << "\' for writing" << std::endl ;
        return false;
    }
    std::cout << title()
        << "saving file..." << std::endl;

    Stopwatch w;
    const std::string &ext = FileSystem::extension(file_name);

    out.precision(16);
    if (ext == "cg") {
        save_cg(skl, out, normalizer);
    } else if (ext == "out") {
        save_out(skl, out, normalizer);
    }else{
        std::cout << title() << "saving file failed (unknown file format)" << std::endl;
        return false;
    }
    skl->set_file_name(file_name);

    std::cout << title()
        << "saving file done. Timing: "
        << w.elapsed_user_time() << " seconds" << std::endl;

    return true;

}

Mesh* FileIO::readMesh(const std::string &file_name)
{
    std::ifstream in(file_name.c_str()) ;
    if (in.fail()) {
        std::cerr << title() << "cannot open file: " << file_name << std::endl ;
        return NULL;
    }
    std::cout << title() << "reading file..." << std::endl;

    Stopwatch clock;
    const std::string &ext = FileSystem::extension(file_name);
    Mesh *mesh = new Mesh(file_name);

    if (ext == "obj") {
        load_obj(mesh, in);
    } else if (ext == "ply") {
        load_ply(mesh, in);
    } else {
        std::cout << title() << "reading file failed (unknown file format)" << std::endl;
        delete mesh;
        in.close();
        return NULL;
    }

    if (mesh->nb_vertices() < 1) {
        std::cout << title()
            << "reading file failed (no data exist)" << std::endl;
        delete mesh;
        in.close();
        return NULL;
    }

    in.close();
    std::cout << title()
        << "reading file done. Timing: "
        << clock.elapsed_user_time() << " seconds" << std::endl;

    return mesh;
}

// load obj with mesh
void FileIO::load_obj(Mesh *mesh, std::ifstream &in)
{
    std::list<double> vlist;
    std::list<int> flist;
    std::vector<TriangleFace> faces;
    std::string line;
    std::string delims = "\t, ";
    std::string delims2 = "/";

    double vertices_x;
    double vertices_y;
    double vertices_z;
    int face_index1;
    int face_index2;
    int face_index3;

    while (!in.eof()) {
        getline(in, line);
        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_any_of(delims));
        // start with v
        if (tokens[0].compare("v") == 0) {
            try {
                vertices_x = boost::lexical_cast<double>(tokens[1]);
                vertices_y = boost::lexical_cast<double>(tokens[2]);
                vertices_z = boost::lexical_cast<double>(tokens[3]);
            } catch(boost::bad_lexical_cast &) {
                continue;
            }
            vlist.push_back(vertices_x);
            vlist.push_back(vertices_y);
            vlist.push_back(vertices_z);

            mesh->add_vertex(Point3d(vertices_x, vertices_y, vertices_z));
        } else if (tokens[0].compare("f") == 0) { // start with f
            std::vector<std::string> tokens_i1;
            std::vector<std::string> tokens_i2;
            std::vector<std::string> tokens_i3;
            boost::split(tokens_i1, tokens[1], boost::is_any_of(delims2));
            boost::split(tokens_i2, tokens[2], boost::is_any_of(delims2));
            boost::split(tokens_i3, tokens[3], boost::is_any_of(delims2));
            try {
                face_index1 = boost::lexical_cast<int>(tokens_i1[0]) - 1;
                face_index2 = boost::lexical_cast<int>(tokens_i2[0]) - 1;
                face_index3 = boost::lexical_cast<int>(tokens_i3[0]) - 1;
            } catch (boost::bad_lexical_cast &) {
                continue;
            }
            flist.push_back(face_index1);
            flist.push_back(face_index2);
            flist.push_back(face_index3);

            faces.push_back(TriangleFace(face_index1, face_index2, face_index3));
        }
    }

    mesh->setFaces(faces);
    mesh->initEdges();
}

// load ply with mesh
void FileIO::load_ply(Mesh *mesh, std::ifstream &in)
{
    bool stop = false;
    unsigned int vertex_num = 0;
    unsigned int face_num = 0;
    unsigned int num_in_line = 0;
    float x, y, z;
    std::string line;

    while (stop == false) {
        getline(in, line);
        std::string key_word, proper_word;
        // read points number
        std::istringstream line_input(line);
        line_input >> key_word;
        if (key_word == "element") {
            line_input >> key_word;
            if (key_word == "vertex") {
                line_input >> vertex_num;
            } else if (key_word == "face") {
                line_input >> face_num;
            }
        } else if (key_word == "end_header") {
            stop = true;
        }
    }

    if (face_num == 0) {
        return ;
    } else { //face
        std::vector<TriangleFace> faces;
        for (unsigned int i = 0; i < vertex_num; i++) {
            getline(in, line);
            std::istringstream line_input(line);
            line_input >> x >> y >> z;
            mesh->add_vertex(Point3d(x, y, z));
        }

        for (unsigned int i = 0; i < face_num; i++) {
            getline(in,line);
            std::istringstream line_input(line);
            int i1;
            int i2;
            int i3;
            line_input >> num_in_line >> i1 >> i2 >> i3;
            faces.push_back(TriangleFace(i1, i2, i3));
        }

        mesh->setFaces(faces);
        mesh->initEdges();
    }
}

PointSet *FileIO::readPointSet(const std::string &file_name,
    bool fast /*= false */,
    unsigned int maxnum  /*= 1000*/)
{
    std::ifstream in(file_name.c_str()) ;
    if (in.fail()) {
        std::cerr << title() << "cannot open file: " << file_name << std::endl ;
        return NULL;
    }
    std::cout << title() << "reading file..." << std::endl;

    Stopwatch clock;
    const std::string &ext = FileSystem::extension(file_name);
    PointSet *point_set = new PointSet(file_name);

    if (ext == "ply") {
        load_ply(point_set,in, fast, maxnum);
    } else if (ext == "obj") {
        load_obj(point_set, in, fast, maxnum);
    } else {
        std::cout << title() << "reading file failed (unknown file format)" << std::endl;
        delete point_set;
        in.close();
        return NULL;
    }

    if (point_set->nb_vertices() < 1) {
        std::cout << title() << "reading file failed (no data exist)" << std::endl;
        delete point_set;
        in.close();
        return NULL;
    }

    in.close();
    std::cout << title() << "reading file done. Timing: "
        << clock.elapsed_user_time() << " seconds" << std::endl;

    return point_set;
}

// load obj with pointset
void FileIO::load_obj(PointSet *pointSet, std::ifstream &in, bool fast /* = false  */, unsigned int maxnum /* = 1000 */)
{
    std::string  line, word;

    if (fast == false) {
        while (getline(in,line)) {
            float x, y,z;
            std::istringstream line_input2(line);
            line_input2 >> word >> x >> y >> z;

            if(word == "v")
                pointSet->add_vertex(Point3d(x,y,z)) ;
        }
    } else {
        std::vector<Point3d> points;
        while (getline(in,line)) {
            float x, y,z;
            std::istringstream line_input2(line);
            line_input2 >> word >>  x >> y >> z;
            if (word == "v") {
                points.push_back(Point3d(x,y,z));
            }
        }
        unsigned int step = points.size()/maxnum ;
        step = (step < 1) ? 1 : step;
        for(unsigned int i = 0; i < points.size(); i+=step) {
            pointSet->add_vertex(points[i]);
        }

        points.clear();
    }
}

// load ply with pointset
void FileIO::load_ply(PointSet *pointSet, std::ifstream &in,
    bool fast /* = false */,
    unsigned int maxnum /* = 1000 */)
{
    bool stop = false;
    unsigned int num;
    unsigned int per_line_num = 0;
    float x, y, z;

    std::string line;

    while(stop== false)
    {
        getline(in, line);
        std::string key_word, proper_word;
        // read points number
        std::istringstream line_input(line);
        line_input >> key_word;
        if(key_word == "element")
        {
            line_input >> key_word;
            if(key_word == "vertex")
                line_input >> num;
        }
        else if(key_word == "property")
            ++per_line_num;
        else if(key_word == "end_header")
            stop = true;
    }

    unsigned int step = num/maxnum ;
    step = (step<1)?1:step;

    if(fast == false)
        step = 1;

    for(unsigned int i = 0; i<num; i+=step)
    {
        for(unsigned j = 0; j < step; j++)
        {
            getline(in, line);
            std::istringstream line_input(line);
            line_input >> x >> y >> z;
        }
        pointSet->add_vertex(Point3d(x, y, z));
    }
}

void FileIO::load_cg(Skeleton* skl, std::ifstream &in)
{
    unsigned int numV = 0;
    unsigned int numE = 0;
    unsigned int numD = 0;
    std::vector<Vertex*> vertices;
    std::string first_line;
    std::string word;
    char c;
    getline(in, first_line);
    std::istringstream first_line_input(first_line) ;
    first_line_input >> word >> c >> c >> numD >> c >> c >> c >>  numV >> c >> c >> c >> numE;

    for (unsigned int i = 0; i< numV; i++) {
        std::string line;
        getline(in, line);
        std::istringstream line_input(line) ;

        float x, y, z;
        line_input >> word >> x >> y >> z ;
        skl->add_a_node(Point3d(x,y,z));
    }
    for(std::list<Vertex*>::iterator lit = (skl->get_nodes()).begin(); lit != (skl->get_nodes()).end(); ++lit) {
        vertices.push_back(*lit);
    }

    std::set<std::pair<unsigned int, unsigned int>> edges;
    for (unsigned int i = 0; i<numE; i++) {
        std::string line;
        getline(in, line);
        std::istringstream line_input(line) ;
        unsigned int e1, e2;
        line_input >> word >> e1 >> e2;
        if (e1 < e2) {
            edges.insert(std::make_pair(e1, e2));
        } else {
            edges.insert(std::make_pair(e2, e1));
        }
    }
    for(std::set<std::pair<unsigned int, unsigned int>>::iterator sit = edges.begin(); sit != edges.end(); ++sit) {
        skl->add_a_bone(vertices[sit->first], vertices[sit->second]);
    }
}

void FileIO::save_cg(Skeleton *skl, std::ofstream &out, Normalizer *normalizer /* = NULL */)
{
    std::set<std::pair<unsigned int,unsigned int>> bones;
    std::map<unsigned int, unsigned int> indexs;
    const std::list<Vertex*> &nodes = skl->get_nodes();
    const std::list<SBone*> &edges = skl->get_bones();
    unsigned int count = 0;
    out <<"# D:3 NV:" << skl->nb_nodes() << " NE:" << skl->nb_edges() << std::endl;

    for (std::list<Vertex*>::const_iterator lit = nodes.begin(); lit != nodes.end(); ++lit) {
        Vertex *v = *lit;
        indexs[v->idx()] = count;
        Point3d p = v->point();
        out << std::setprecision(6);
        out << "v " << normalizer->restore(p) << std::endl;
        ++count;
    }
    for(std::list<SBone*>::const_iterator lit = edges.begin(); lit != edges.end(); ++lit) {
        SBone *bone = *lit;
        out << "e " << indexs[bone->get_v1()->idx()] << " " << indexs[bone->get_v2()->idx()]<< std::endl;
    }
}

void FileIO::load_skl(Skeleton *skl, std::ifstream &in)
{
    unsigned int num;
    std::string line2;
    getline(in, line2);
    std::istringstream line_input2(line2) ;
    line_input2 >> num;
    std::set<std::pair<unsigned int, unsigned int>> edges;

    for (unsigned int count = 0; count < num; count++) {
        std::string line;
        getline(in, line);
        std::istringstream line_input(line) ;
        float x, y, z;
        line_input >> x >> y >> z ;
        Point3d pos(x,y,z);
        unsigned int i;
        skl->add_a_node(pos);
        while (line_input >> i) {
            if (i > count) {
                edges.insert(std::make_pair(count,i));
            } else {
                edges.insert(std::make_pair(i, count));
            }
        }
    }

    std::vector<Vertex*> vertices;
    for(std::list<Vertex*>::iterator lit = (skl->get_nodes()).begin(); lit != (skl->get_nodes()).end(); ++lit) {
        vertices.push_back(*lit);
    }

    for(std::set<std::pair<unsigned int, unsigned int>>::iterator sit = edges.begin(); sit != edges.end(); ++sit) {
        skl->add_a_bone(vertices[sit->first], vertices[sit->second]);
    }
}

void FileIO::load_out(Skeleton *skl, std::ifstream &in)
{
    std::vector<Vertex*> vertices;
    std::set<std::pair<unsigned int, unsigned int>> edges;
    int vertexStart;
    int vertexEnd;
    float x, y, z;
    std::string line;

    while (getline(in, line)) {;
        std::istringstream line_input(line) ;
        line_input >> vertexStart >> x >> y >> z >> vertexEnd;
        skl->add_a_node(Point3d(x,y,z));
        if (vertexStart != 0) {
            if (vertexStart < vertexEnd) {
                edges.insert(std::make_pair(vertexStart, vertexEnd));
            } else {
                edges.insert(std::make_pair(vertexEnd, vertexStart));
            }
        }
    }

    for (std::list<Vertex*>::iterator lit = (skl->get_nodes()).begin(); lit != (skl->get_nodes()).end(); ++lit) {
        vertices.push_back(*lit);
    }

    for (std::set<std::pair<unsigned int, unsigned int>>::iterator sit = edges.begin(); sit != edges.end(); ++sit) {
        skl->add_a_bone(vertices[sit->first], vertices[sit->second]);
    }
}

void FileIO::save_out(Skeleton *skl, std::ofstream &out, Normalizer *normalizer /* = NULL */)
{
    std::set<std::pair<unsigned int,unsigned int>> bones;
    std::map<unsigned int, unsigned int> indexs;
    const std::list<Vertex*> &nodes = skl->get_nodes();
    const std::list<SBone*> &edges = skl->get_bones();
    unsigned int count = 0;

    for (std::list<Vertex*>::const_iterator vit = nodes.begin(); vit != nodes.end(); ++vit) {
        Vertex *v = *vit;
        indexs[v->idx()] = count;
        Point3d p = v->point();
        out << std::setprecision(6);
        if (count == 0) {
            out << count << " " << normalizer->restore(p) << " -1" << std::endl;
        } else {
            for(std::list<SBone*>::const_iterator sit = edges.begin(); sit != edges.end(); ++sit) {
                SBone *bone = *sit;
                //v1 < v2
                if (count == indexs[bone->get_v2()->idx()]) {
                    out << count << " " << normalizer->restore(p) << " " << indexs[bone->get_v1()->idx()] << std::endl;
                }
            }
        }
        ++count;
    }
}
