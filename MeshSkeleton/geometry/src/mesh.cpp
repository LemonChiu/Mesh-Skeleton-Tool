#include "mesh.h"

#include "attribute.h"
#include "stop_watch.h"

Mesh::Mesh(const std::string &name)
: Object(name, OBJECT_MESH_TYPE)
{
}

Mesh::Mesh(const Mesh &m) : Object(m.name(), OBJECT_MESH_TYPE)
{
    const std::vector<Vertex*> &vertices = m.vertices();
    for (unsigned int i = 0; i< vertices.size(); i++) {
        add_vertex(vertices[i]->point(), vertices[i]->normal(), vertices[i]->color());
    }
    triangle_faces_ = m.getFaces();
    edges_ = m.getEdges();
}

void Mesh::setFaces(const std::vector<TriangleFace> &faces)
{
    triangle_faces_ = faces;
}

Mesh::~Mesh() {
    clear();
}

void Mesh::clear() {
    Object::clear();
    triangle_faces_.clear();
    edges_.clear();
}

void Mesh::draw() const
{
    glPointSize(global::g_fPointSize);
    glLineWidth(global::g_fLineWidth);

    if(global::g_bLighting == true)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);

    if (global::g_meshDrawMode == POINTSMODE) {
        // set the same color with mesh
        Colorf &points_color = Colorf(global::g_meshColor.r(),global::g_meshColor.g(),global::g_meshColor.b());
        glDisable(GL_LIGHTING);
        glPointSize(global::g_fPointSize);
        glBegin(GL_POINTS);
        if (global::g_bPointsRealColor) {
            for (unsigned int i = 0; i < vertices_.size(); i++) {
                vertices_[i]->draw();
            }
        } else {
            for(unsigned int i = 0; i< vertices_.size(); i++) {
                //use custom color to draw mesh
                vertices_[i]->draw(points_color);
            }
        }
        glEnd();
        glEnable(GL_LIGHTING);
    } else {
        //render with gray glColor3f(0.6f, 0.6f, 0.6f);
        glColor3f(global::g_meshColor.r(),global::g_meshColor.g(),global::g_meshColor.b());

        if (global::g_meshDrawMode == ALLLINESMODE) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
        }
        if (global::g_meshDrawMode == FRONTLINESMODE) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
        if (global::g_meshDrawMode == SURFACEMODE) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glShadeModel(GL_SMOOTH);
        }

        glBegin(GL_TRIANGLES);
        for (unsigned int i = 0; i < triangle_faces_.size(); i++) {
            Point3d p1 = vertices_[ triangle_faces_[i].a_id_ ]->point();
            Point3d p2 = vertices_[ triangle_faces_[i].b_id_ ]->point();
            Point3d p3 = vertices_[ triangle_faces_[i].c_id_ ]->point();

            Vector3d n1 = vertices_[ triangle_faces_[i].a_id_ ]->normal();
            Vector3d n2 = vertices_[ triangle_faces_[i].b_id_ ]->normal();
            Vector3d n3 = vertices_[ triangle_faces_[i].c_id_ ]->normal();

            glNormal3d(n1.x(), n1.y(), n1.z());
            glNormal3d(n2.x(), n2.y(), n2.z());
            glNormal3d(n3.x(), n3.y(), n3.z());

            glVertex3d(p1.x(), p1.y(), p1.z());
            glVertex3d(p2.x(), p2.y(), p2.z());
            glVertex3d(p3.x(), p3.y(), p3.z());
        }
        glEnd();
    }

    //display normal
    if (global::g_bShowNormal) {
        //close light
        glDisable(GL_LIGHTING);
        glClear(GL_COLOR_BUFFER_BIT);
        //set blue color
        glColor3f(0.52f, 0.80f, 0.92f);
        glBegin(GL_LINES);
        for (unsigned int i = 0; i < vertices_.size(); i++) {
            Point3d p1 = vertices_[i]->point();
            Point3d p2 = p1 + vertices_[i]->normal() * global::g_fNormalLength;
            glVertex3d(p1.x(), p1.y(), p1.z());
            glVertex3d(p2.x(), p2.y(), p2.z());
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }
}

void Mesh::computeNormal()
{
    std::cout << "[MeshNormal]: " << "Compute mesh normal..." << std::endl;
    Stopwatch clock;

    Vector3dAttribute normals(this, "normal");
    normals.set_default(Vector3d(0.0f, 0.0f, 0.0f));

    for (unsigned int i = 0; i< triangle_faces_.size(); i++) {
        TriangleFace &face = triangle_faces_[i];
        Point3d p1 = vertices_[ face.a_id_ ]->point();
        Point3d p2 = vertices_[ face.b_id_ ]->point();
        Point3d p3 = vertices_[ face.c_id_ ]->point();

        // face_normal
        Vector3d v = CGAL::cross_product(p2-p1, p3-p1);
        normals[ face.a_id_ ] = normals[ face.a_id_ ] + v;
        normals[ face.b_id_ ] = normals[ face.b_id_ ] + v;
        normals[ face.c_id_ ] = normals[ face.c_id_ ] + v;
    }

    for (unsigned int i = 0; i < vertices_.size(); i++) {
        normals[i] = normals[i] / std::sqrt(normals[i].squared_length());
        vertices_[i]->set_normal(normals[i]);
    }

    std::cout << "[MeshNormal]: " << "Normal computation done. Time: "
              << clock.elapsed_user_time() << " second" << std::endl;
}

/**
* may cause errors:
*/
float Mesh::computeMeanLength()
{
    float sum_len = 0;
    for(std::set<Edge3D>::iterator mit = edges_.begin(); mit != edges_.end(); ++mit)
    {
        const Edge3D &e = *mit;
        Vector3d v = (vertices_[e.index0_])->point() - (vertices_[e.index1_ ])->point();
        sum_len += std::sqrt(v.squared_length());
    }
    float mean_len = sum_len / edges_.size() ;
    return mean_len;

}

void Mesh::initEdges()
{
    edges_.clear();
    for (unsigned int i = 0; i < triangle_faces_.size(); i++) {
        TriangleFace &face = triangle_faces_[i];
        unsigned int index[4] = {face.a_id_, face.b_id_, face.c_id_,face.a_id_};
        for (unsigned int j = 0; j < 4; j++) {
            Edge3D e(index[0], index[1]);
            if (edges_.find(e) != edges_.end()) {
                e.neighbor1_ = i;
                e.isBorder_ = false;
            } else {
                e.neighbor0_ = i;
                edges_.insert(e);
            }
        }
    }
}

void Mesh::computeEdgesLength(std::vector<Vector3d> &edgevs)
{
    edgevs.clear();
    edgevs.resize(edges_.size());
    unsigned int count  = 0;

    for(std::set< Edge3D >::iterator mit = edges_.begin(); mit != edges_.end(); ++mit) {
        Vector3d v = vertices_[mit->index0_]->point() - vertices_[mit->index1_]->point();
        edgevs[count] = v;
        ++count;
    }
}

void Mesh::computeLaplacianMatrix(std::vector<std::map<int,double>> &rows)
{
    unsigned int num_v = vertices_.size();
    rows.clear();
    rows.resize(num_v);

    for(std::set<Edge3D>::iterator mit = edges_.begin(); mit!= edges_.end(); ++mit)
    {
        const Edge3D &e = *mit;

        TriangleFace &face0 = triangle_faces_[e.neighbor0_];
        unsigned int c0 = face0.a_id_+face0.b_id_+face0.c_id_-e.index0_-e.index1_;
        Vector3d v1 = vertices_[e.index0_]->point() - vertices_[c0]->point();
        Vector3d v2 = vertices_[e.index1_]->point() - vertices_[c0]->point();
        float w0 = std::sqrt(CGAL::cross_product(v1,v2).squared_length()) / (v1*v2);

        TriangleFace &face1 = triangle_faces_[e.neighbor1_];
        unsigned int c1 = face1.a_id_ + face1.b_id_ + face1.c_id_;
        v1 = vertices_[e.index0_]->point()-vertices_[c1]->point();
        v2 = vertices_[e.index1_]->point() - vertices_[c1]->point();
        float w1 = std::sqrt(CGAL::cross_product(v1,v2).squared_length()) / (v1*v2);

        float w = (w1+w0)/2;
        rows[e.index0_][e.index1_] = rows[e.index1_][e.index0_] = double(w);
    }

    for(unsigned int i = 0; i< num_v; i++)
    {
        double sum_w = 0.0;
        for(std::map<int,double>::iterator mit = rows[i].begin(); mit != rows[i].end(); ++mit)
            sum_w += mit->second;
        rows[i][i] = -sum_w;
    }
}
