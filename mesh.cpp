#include <iostream>
#include <cassert>
#include "mesh.h"
#include <iomanip>
#include <vector>

// Generate mesh frame
Mesh::Mesh(int numVerticesX_, int numVerticesY_, int numVerticesZ_) {
    numVerticesX = numVerticesX_;
    numVerticesY = numVerticesY_;
    numVerticesZ = numVerticesZ_;
    //vertex_id_count = 0;
    //vertices = new Vertex[numVerticesX*numVerticesY*numVerticesZ];
    int id_count = 0;
    // Generate vertices
    for (int z = 0; z < numVerticesZ; z++) {
        for (int y = 0; y < numVerticesY; y++) {
            for (int x = 0; x < numVerticesX; x++) {
                vertices.push_back(new Vertex(VertexIndex(x, y, z), -1.0, id_count));
                id_count++;
                //addVertex(VertexIndex(x, y, z));
            }
        }
    }
}

// Cleanup dynamic memory
Mesh::~Mesh() {
    for (size_t i = 0; i < vertices.size(); i++) {
        delete vertices[i];
    }
}

// void Mesh::addVertex(VertexIndex index) {
//     vertices.push_back(new Vertex(index,  -1.0));
//     vertex_id_count++;
// }

Vertex* Mesh::getVertex(VertexIndex index) const {
    if (index.x >= numVerticesX || index.y >= numVerticesY || index.z >= numVerticesZ || index.x < 0 || index.y < 0 || index.z < 0) {
        /* std::cout << "ERROR: Trying to reach vertex that is out of bounds" << std::endl; */
        return nullptr;
    }
    return vertices[vertexArrayIndex(index.x, index.y, index.z)];
}

void Mesh::outputVertices(std::ostream& stream) const {
    for (int i = 0; i < numVerticesX*numVerticesY*numVerticesZ; i++) {
        Vertex* v = vertices[i];
        stream << v->getString() << std::endl;
    }
}

void Mesh::outputTriangleVertices(std::ostream& stream, bool include_normals) const {
    for (triangle_vertex_map::const_iterator it = triangle_vertices.begin(); it != triangle_vertices.end(); it++) {
        stream << "vertex\t" << it->second->id << "\t" 
        << it->second->p.x << "\t"
        << it->second->p.y << "\t"
        << it->second->p.z << "\t";
        if (include_normals) {
            stream << it->second->normal.x << "\t"
            << it->second->normal.y << "\t"
            << it->second->normal.z << "\t";
        }
        stream << std::endl;
    }
}

void Mesh::outputTriangles(std::ostream& stream) const {
    for (unsigned int i = 0; i < triangles.size(); i++) {
        //stream << triangles[i].getString() << std::endl;
        stream << "triangle\t" << triangles[i].vertex_ids[0] << "\t"
        << triangles[i].vertex_ids[1] << "\t"
        << triangles[i].vertex_ids[2] << "\t";
        stream << std::endl;
    }
}

std::vector<Vertex*> Mesh::getCell(VertexIndex index, bool cube) const {
    std::vector<Vertex*> verts;
    verts.push_back(getVertex(index));
    verts.push_back(getVertex(VertexIndex(index.x + 1, index.y, index.z)));
    verts.push_back(getVertex(VertexIndex(index.x + 1, index.y + 1, index.z)));
    verts.push_back(getVertex(VertexIndex(index.x, index.y + 1, index.z)));
    if (cube) {
        verts.push_back(getVertex(VertexIndex(index.x, index.y, index.z + 1)));
        verts.push_back(getVertex(VertexIndex(index.x + 1, index.y, index.z + 1)));
        verts.push_back(getVertex(VertexIndex(index.x + 1, index.y + 1, index.z + 1)));
        verts.push_back(getVertex(VertexIndex(index.x, index.y + 1, index.z + 1)));
    }
    return verts;
}

Point vertexInterpolation(double surface_cutoff, const Vertex* v1, const Vertex* v2) {
    if (std::abs(v1->getValue() - surface_cutoff) < 0.00001) {
        return Point(v1->getIndex().x, v1->getIndex().y, v1->getIndex().z);
    } else if (std::abs(v2->getValue() - surface_cutoff) < 0.00001) {
        return Point(v2->getIndex().x, v2->getIndex().y, v2->getIndex().z);
    } else if (std::abs(v1->getValue() - v2->getValue()) < 0.00001) {
        return Point(v1->getIndex().x, v1->getIndex().y, v1->getIndex().z);
    }
    double weight = std::abs(surface_cutoff - v1->getValue()) / std::abs(v1->getValue() - v2->getValue());
    double x = v1->getIndex().x + weight * (v2->getIndex().x - v1->getIndex().x);
    double y = v1->getIndex().y + weight * (v2->getIndex().y - v1->getIndex().y);
    double z = v1->getIndex().z + weight * (v2->getIndex().z - v1->getIndex().z);
    return Point(x, y, z);
}

TriangleVertex* Mesh::addTriangleVertex(Vertex* v1, Vertex* v2, double surfaceValue, bool guaranteeNew, bool useMidpoint) {
    if (!guaranteeNew) {    // Vertex is guaranteed to be unique
        triangle_vertex_map::iterator it = triangle_vertices.find(std::pair<Vertex*,Vertex*>(v1, v2));
        if (it != triangle_vertices.end()) {
            return it->second;
        }
    }
    Point p;
    if (useMidpoint)        // Find average position between vertices instead of linear interpolation based on value
        p = avgVertexPosition(v1, v2);
    else 
        p = vertexInterpolation(surfaceValue, v1, v2);
    TriangleVertex* vertex = new TriangleVertex(
        p,
        v1, v2,
        Point(0.0, 0.0, 0.0),
        triangle_vertices.size()
    );
    triangle_vertices[std::pair<Vertex*,Vertex*>(v1,v2)] = vertex;
    return vertex;
}

void Mesh::triangulate2D(double surfaceValue, bool blocky) {
    triangles.clear();
    triangle_vertices.clear();
    for (int y = 0; y < numVerticesY-1; y++) {
        for (int x = 0; x < numVerticesX-1; x++) {
            std::vector<Vertex*> corners = getCell(VertexIndex(x, y, 0), false);
            // Get triangle index from active points
            int triangleIndex = 0;
            if (corners[0]->getValue() < surfaceValue) triangleIndex |= 1;
            if (corners[1]->getValue() < surfaceValue) triangleIndex |= 2;
            if (corners[2]->getValue() < surfaceValue) triangleIndex |= 4;
            if (corners[3]->getValue() < surfaceValue) triangleIndex |= 8;
            if (triangleIndex == 0) continue; // No active corners

            // Get vertices from list and create list of triangle vertices
            std::vector<TriangleVertex*> verts;
            //std::vector<Point> points;
            const std::array<std::array<int, 8>, 16> * triangleArray;
            if (blocky)
                triangleArray = &TRIANGLES_2D_BLOCKY;
            else
                triangleArray = &TRIANGLES_2D_ROUNDED;

            for (int i = 0; (*triangleArray)[triangleIndex][i] != -1; i++) {
                int vertexIndex = (*triangleArray)[triangleIndex][i];
                TriangleVertex* v;
                // Point p;
                if (vertexIndex < 4) {
                    //p = Point(corners[vertexIndex]->getIndex().x, corners[vertexIndex]->getIndex().y);
                    v = addTriangleVertex(corners[vertexIndex], corners[vertexIndex], surfaceValue);
                } else if (vertexIndex == 4) {
                    //p = vertexInterpolation(surfaceValue, corners[0], corners[1]);
                    v = addTriangleVertex(corners[0], corners[1], surfaceValue);
                } else if (vertexIndex == 5) {
                    //p = vertexInterpolation(surfaceValue, corners[1], corners[2]);
                    v = addTriangleVertex(corners[1], corners[2], surfaceValue);
                } else if (vertexIndex == 6) {
                    //p = vertexInterpolation(surfaceValue, corners[2], corners[3]);
                    v = addTriangleVertex(corners[2], corners[3], surfaceValue);
                } else if (vertexIndex == 7) {
                    //p = vertexInterpolation(surfaceValue, corners[3], corners[0]);
                    v = addTriangleVertex(corners[3], corners[0], surfaceValue);
                } else if (vertexIndex == 8 && blocky) {
                    v = addTriangleVertex(corners[0], corners[2], surfaceValue, true, true);
                } else {
                    std::cerr << "Error: Unknown vertex index " << vertexIndex << std::endl;
                    return;
                }
                verts.push_back(v);
            }

            // Get triangles from list of points
            if (verts.size() >= 3) {
                triangles.push_back(Triangle(verts[0]->id, verts[1]->id, verts[2]->id));
            }
            if (verts.size() >= 4) {
                triangles.push_back(Triangle(verts[0]->id, verts[2]->id, verts[3]->id));
            }
            if (verts.size() >= 5) {
                triangles.push_back(Triangle(verts[0]->id, verts[3]->id, verts[4]->id));
            }
            if (verts.size() >= 6) {
                triangles.push_back(Triangle(verts[0]->id, verts[4]->id, verts[5]->id));
            }
        }
    }
}

Point normalize(Point v) {
    double mag = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2) + std::pow(v.z, 2));
    return Point(v.x/mag, v.y/mag, v.z/mag);
}

Point getSurfaceNormal(Point a, Point b, Point c) {
    // Get vectors between a,b and a,c
    double v1[3] = { b.x - a.x, b.y - a.y, b.z - a.z };
    double v2[3] = { c.x - a.x, c.y - a.y, c.z - a.z };
    // Compute cross product to get surface normal
    Point p = Point(
        v1[1]*v2[2] - v1[2]*v2[1],
        v1[2]*v2[0] - v1[0]*v2[2],
        v1[0]*v2[1] - v1[1]*v2[0]
    );
    return p;
}

void Mesh::triangulate3D(double surfaceValue) {
    triangles.clear();
    triangle_vertices.clear();
    for (int z = 0; z < numVerticesZ-1; z++) {
        for (int y = 0; y < numVerticesY-1; y++) {
            for (int x = 0; x < numVerticesX-1; x++) {
                // Corner vertices
                std::vector<Vertex*> corners = getCell(VertexIndex(x, y, z), true);
                int cubeIndex = 0;
                if (corners[0]->getValue() < surfaceValue) cubeIndex |= 1;
                if (corners[1]->getValue() < surfaceValue) cubeIndex |= 2;
                if (corners[2]->getValue() < surfaceValue) cubeIndex |= 4;
                if (corners[3]->getValue() < surfaceValue) cubeIndex |= 8;
                if (corners[4]->getValue() < surfaceValue) cubeIndex |= 16;
                if (corners[5]->getValue() < surfaceValue) cubeIndex |= 32;
                if (corners[6]->getValue() < surfaceValue) cubeIndex |= 64;
                if (corners[7]->getValue() < surfaceValue) cubeIndex |= 128;

                if (EDGE_TABLE_3D[cubeIndex] == 0) continue;
                TriangleVertex* vertList[12];
                // Get and calculate necessary edge vertices
                if (EDGE_TABLE_3D[cubeIndex] & 1) {
                    vertList[0] = addTriangleVertex(corners[0], corners[1], surfaceValue);
                }
                if (EDGE_TABLE_3D[cubeIndex] & 2) {
                    vertList[1] = addTriangleVertex(corners[1], corners[2], surfaceValue);
                }
                if (EDGE_TABLE_3D[cubeIndex] & 4) {
                    vertList[2] = addTriangleVertex(corners[2], corners[3], surfaceValue);
                }
                if (EDGE_TABLE_3D[cubeIndex] & 8) {
                    vertList[3] = addTriangleVertex(corners[3], corners[0], surfaceValue);
                }     
                if (EDGE_TABLE_3D[cubeIndex] & 16) {
                    vertList[4] = addTriangleVertex(corners[4], corners[5], surfaceValue);
                }   
                if (EDGE_TABLE_3D[cubeIndex] & 32) {
                    vertList[5] = addTriangleVertex(corners[5], corners[6], surfaceValue);
                } 
                if (EDGE_TABLE_3D[cubeIndex] & 64) {
                    vertList[6] = addTriangleVertex(corners[6], corners[7], surfaceValue);
                }     
                if (EDGE_TABLE_3D[cubeIndex] & 128) {
                    vertList[7] = addTriangleVertex(corners[7], corners[4], surfaceValue);
                }   
                if (EDGE_TABLE_3D[cubeIndex] & 256) {
                    vertList[8] = addTriangleVertex(corners[0], corners[4], surfaceValue);
                }
                if (EDGE_TABLE_3D[cubeIndex] & 512) {
                    vertList[9] = addTriangleVertex(corners[1], corners[5], surfaceValue);
                }     
                if (EDGE_TABLE_3D[cubeIndex] & 1024) {
                    vertList[10] = addTriangleVertex(corners[2], corners[6], surfaceValue);
                } 
                if (EDGE_TABLE_3D[cubeIndex] & 2048) {
                    vertList[11] = addTriangleVertex(corners[3], corners[7], surfaceValue);
                }    

                // Create triangles
                for (int i = 0; TRIANGLE_TABLE_3D[cubeIndex][i] != -1; i += 3) {
                    int verts[3] = { 
                        TRIANGLE_TABLE_3D[cubeIndex][i], 
                        TRIANGLE_TABLE_3D[cubeIndex][i+2],
                        TRIANGLE_TABLE_3D[cubeIndex][i+1]
                    };
                    Point surfaceNormal = getSurfaceNormal(vertList[verts[0]]->p, vertList[verts[1]]->p, vertList[verts[2]]->p);
                    for (int v = 0; v < 3; v++) {
                        vertList[verts[v]]->normal.x += surfaceNormal.x;
                        vertList[verts[v]]->normal.y += surfaceNormal.y;
                        vertList[verts[v]]->normal.z += surfaceNormal.z;
                    }
                    Triangle t = Triangle(vertList[verts[0]]->id, vertList[verts[1]]->id, vertList[verts[2]]->id);
                    triangles.push_back(t);
                }
            }
        }
    }
    // Normalize vertex normals
    for (triangle_vertex_map::iterator it = triangle_vertices.begin(); it != triangle_vertices.end(); it++) {
        it->second->normal = normalize(it->second->normal);
    }
}

void Mesh::recursiveBlend(VertexIndex index, VertexIndex center, double weight, double radius, std::vector<VertexIndex> &already_set) {
    if (index.x >= numVerticesX || index.y >= numVerticesY || index.z >= numVerticesZ || index.x < 0 || index.y < 0 || index.z < 0) return;
    for (size_t i = 0; i < already_set.size(); i++) {
        if (already_set[i] == index) return;
    }
    //if (getVertex(index)->getValue() != -1.0 && !initial) return;
    double distance = indexDistance(index, center);
    
    if (distance != 0.0) {
        if (lessThanOrEqual(distance, radius)) {
            already_set.push_back(index);
            double value = weight/distance;
            if (getVertex(index)->getValue() < value) { getVertex(index)->setValue(value); }
        } else { return; }
    }
    recursiveBlend(VertexIndex(index.x, index.y + 1, index.z), center, weight, radius, already_set);
    recursiveBlend(VertexIndex(index.x, index.y - 1, index.z), center, weight, radius, already_set);
    recursiveBlend(VertexIndex(index.x + 1, index.y, index.z), center, weight, radius, already_set);
    recursiveBlend(VertexIndex(index.x - 1, index.y, index.z), center, weight, radius, already_set);
    recursiveBlend(VertexIndex(index.x, index.y, index.z + 1), center, weight, radius, already_set);
    recursiveBlend(VertexIndex(index.x, index.y, index.z - 1), center, weight, radius, already_set);
}

void Mesh::blend(double radius, double weight) {
    if (radius <= 0) return;
    for (int z = 0; z < numVerticesZ; z++) {
        for (int y = 0; y < numVerticesY; y++) {
            for (int x = 0; x < numVerticesX; x++) {
                VertexIndex index = VertexIndex(x, y, z);
                Vertex* v = getVertex(index);
                if (v->getValue() < 1.0) continue;
                std::vector<VertexIndex> already_set;
                recursiveBlend(index, index, weight, radius, already_set);
                already_set.empty();
            }
        }
    }
}

void Mesh::depthFill(int floorSize) {
    for (size_t i = 0; i < vertices.size(); i++) {
        Vertex* v = vertices[i];
        if (v->getValue() != -1.0) {
            for (int z = floorSize; z < numVerticesZ-1; z++) {
                VertexIndex index = v->getIndex();
                index.z = z;
                getVertex(index)->setValue(v->getValue());
            }
        }
    }
    for (int y = 0; y < numVerticesY; y++) {
        for (int x = 0; x < numVerticesX; x++) {
            getVertex(VertexIndex(x, y, numVerticesZ-1))->setValue(1.0);
        }   
    }
}






/* 
void recursive_split(Vertex v1, Vertex v2, int num_divides, std::vector<Vertex>& vertices, int direction, bool initial=true) {
    if (initial) vertices.push_back(v1);
    if (num_divides > 0) {
        //Point midPos = avgVertexPosition(&v1, &v2);
        VertexIndex midIndex = v1.getIndex();
        //midIndex.x += 1;
        Vertex midVertex(midIndex, avgValue(v1.getValue(), v2.getValue()));
        recursive_split(v1, midVertex, num_divides-1, vertices, false);
        recursive_split(midVertex, v2, num_divides-1, vertices, false);
    } else {
        VertexIndex index;
        if (direction == 0) {           // X split
            index = VertexIndex(vertices[0].getIndex().x + vertices.size(), v2.getIndex().y, v2.getIndex().z);
        } else if (direction == 1) {    // Y split
            index = VertexIndex(v2.getIndex().x, vertices[0].getIndex().y + vertices.size(), v2.getIndex().z);
        } else {                        // Z split
            index = VertexIndex(v2.getIndex().x, v2.getIndex().y, vertices[0].getIndex().z + vertices.size());
        }
        v2.setIndex(index);
        vertices.push_back(v2);
    }
}


void Mesh::increaseResolution(int divides) {
    int factor = 2^divides;
    //int newZ = 1;
    //if (numVerticesZ > 1) { newZ = numVerticesZ * factor - 1; }
    //int newX = numVerticesX * factor - 1;
    //int newY = numVerticesZ * factor - 1;
    //Vertex* new_vertices = new Vertex[newZ * newY * newX];
    std::vector<Vertex> new_vertices;
    std::vector<Vertex> block;
    //int vertex_count = 0;
    for (int z = 0; z < (numVerticesZ == 1 ? 1 : numVerticesZ - 1); z++) {
        for (int y = 0; y < numVerticesY - 1; y++) {
            for (int x = 0; x < numVerticesX - 1; x++) {
                std::vector<const Vertex*> cell_vertices = getCell(VertexIndex(x,y,x), false);
                block.clear();
                recursive_split(*cell_vertices[0], *cell_vertices[1], 1, block, 0);
                for (unsigned i = 0; i < block.size(); i++) {
                    //block[i].setID(vertex_count);
                    new_vertices.push_back(block[i]);
                    //new_vertices[vertex_count] = block[i];
                    //std::cout << new_vertices[vertex_count].getString() << std::endl;
                    //vertex_count++;
                }
            }
        }
    }
    for (int i = 0; i < new_vertices.size(); i++) {
        std::cout << new_vertices[i].getString() << std::endl;
    }
} */

void Mesh::debugPrint2D(int z) const {
    for (int y = 0; y < numVerticesY; y++) {
        for (int x = 0; x < numVerticesX; x++) {
            Vertex* v = getVertex(VertexIndex(x, y, z));
            char fill = ' ';
            if (v->getValue() < 0.0) fill = '+';
            std::cout << fill << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Mesh::debugPrintValues(int z) const {
    for (int y = 0; y < numVerticesY; y++) {
        for (int x = 0; x < numVerticesX; x++) {
            Vertex* v = getVertex(VertexIndex(x, y, z));
            std::cout << std::setw(3) << v->getValue();
        }
        std::cout << std::endl;
    }
}