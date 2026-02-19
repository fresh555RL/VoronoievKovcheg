#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <acc_prof.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3 Point_3;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
const int sph_pts = 1000;
const int sph_total = sph_pts*3;
using namespace std;

// for(auto [a,b]:vector<pair>){
 // a -> 
// }

void to_file(vector<glm::vec3> vertices, vector<unsigned> idxes){
    ofstream file("sphere.txt");
    if (!file.is_open()) return;
    file<<vertices.size()<<"\n";
    file<<idxes.size()<<'\n';
    for(auto v:vertices){
        file<<v.x<<" "<<v.y<<" "<<v.z<<'\n';
    }
    
    for(int i=0;i<idxes.size();i+=3){
        file<<idxes[i]<<" "<<idxes[i+1]<<" "<<idxes[i+2]<<'\n';
    }
    file.close();
}

int main(){

    vector<Point_3> vertices_Sphereraw(sph_pts);
    
    double golden_ratio = (1.0f+sqrt(5.0f))/2.0f;
    for(int i=0;i<sph_pts;i++){
        double theta = 2.0f*(M_PI)*i/golden_ratio;
        double phi = acos(1.0f-2.0f*(i)/sph_pts);
        vertices_Sphereraw[i] = {cos(theta)*sin(phi), sin(theta)*sin(phi), cos(phi)}; 
    }
    CGAL::Polyhedron_3<K> poly;
    CGAL::convex_hull_3(vertices_Sphereraw.begin(), vertices_Sphereraw.end(), poly);
    vector<glm::vec3> vertices_Sphere;
    for(auto v = poly.vertices_begin();v!=poly.vertices_end();v++){
        vertices_Sphere.push_back(glm::vec3(v->point().x(),v->point().y(),v->point().z() ));
    }

    vector<unsigned int> indexes_Sphere;
    for(auto f = poly.faces_begin();f!=poly.faces_end();f++){
        auto h = f->facet_begin();
        unsigned int tri[3];
        for(int i=0;i<3;i++){
            tri[i] = distance(poly.vertices_begin(), h->vertex());
            h++;
        }
        indexes_Sphere.push_back(tri[0]);
        indexes_Sphere.push_back(tri[1]);
        indexes_Sphere.push_back(tri[2]);
    }
    // cout<<"vertx:\n";
    // for(int i=0;i<vertices_Sphere.size();i++){
    //     cout<<vertices_Sphere[i].x<<", "<<vertices_Sphere[i].y<<", "<<vertices_Sphere[i].z<<",\n";
    // }
    // cout<<"\n\n\n";
    // for(int i=0;i<indexes_Sphere.size();i+=3){
    //     cout<<indexes_Sphere[i]<<", "<<indexes_Sphere[i+1]<<", "<<indexes_Sphere[i+2]<<",\n";
    // }

    to_file(vertices_Sphere, indexes_Sphere);
}