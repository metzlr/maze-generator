#include <iostream>
#include <cassert>
#include "mesh.h"
#include <iomanip>
#include <vector>

// Generate mesh frame
Mesh::Mesh(int num_voxelsX_, int num_voxelsY_, int num_voxelsZ_) {
    num_voxelsX = num_voxelsX_;
    num_voxelsY = num_voxelsY_;
    num_voxelsZ = num_voxelsZ_;
    int id_count = 0;
    // Generate voxels
    for (int z = 0; z < num_voxelsZ; z++) {
        for (int y = 0; y < num_voxelsY; y++) {
            for (int x = 0; x < num_voxelsX; x++) {
                voxels.push_back(new Voxel(VoxelIndex(x, y, z), -1.0, id_count));
                id_count++;
            }
        }
    }
}

// Cleanup dynamic memory
Mesh::~Mesh() {
    for (size_t i = 0; i < voxels.size(); i++) {
        delete voxels[i];
    }
    for (triangle_vertex_map::iterator it = triangle_vertices.begin(); it != triangle_vertices.end(); it++) {
        delete it->second;
    }
}

Voxel* Mesh::getVoxel(VoxelIndex index) const {
    if (index.x >= num_voxelsX || index.y >= num_voxelsY || index.z >= num_voxelsZ || index.x < 0 || index.y < 0 || index.z < 0) {
        return nullptr;
    }
    return voxels[voxelArrayIndex(index.x, index.y, index.z)];
}

void Mesh::outputVoxels(std::ostream& stream) const {
    for (int i = 0; i < num_voxelsX*num_voxelsY*num_voxelsZ; i++) {
        Voxel* v = voxels[i];
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
        stream << "triangle\t" << triangles[i].vertex_ids[0] << "\t"
        << triangles[i].vertex_ids[1] << "\t"
        << triangles[i].vertex_ids[2] << "\t";
        stream << std::endl;
    }
}

std::vector<Voxel*> Mesh::getCell(VoxelIndex index, bool cube) const {
    std::vector<Voxel*> voxels;
    voxels.push_back(getVoxel(index));
    voxels.push_back(getVoxel(VoxelIndex(index.x + 1, index.y, index.z)));
    voxels.push_back(getVoxel(VoxelIndex(index.x + 1, index.y + 1, index.z)));
    voxels.push_back(getVoxel(VoxelIndex(index.x, index.y + 1, index.z)));
    if (cube) {
        voxels.push_back(getVoxel(VoxelIndex(index.x, index.y, index.z + 1)));
        voxels.push_back(getVoxel(VoxelIndex(index.x + 1, index.y, index.z + 1)));
        voxels.push_back(getVoxel(VoxelIndex(index.x + 1, index.y + 1, index.z + 1)));
        voxels.push_back(getVoxel(VoxelIndex(index.x, index.y + 1, index.z + 1)));
    }
    return voxels;
}

Point vertexInterpolation(double surface_cutoff, const Voxel* v1, const Voxel* v2) {
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

TriangleVertex* Mesh::addTriangleVertex(Voxel* v1, Voxel* v2, double surfaceValue, bool guaranteeNew, bool useMidpoint) {
    std::pair<Voxel*,Voxel*> voxel_pair;
    if (v1->getID() < v2->getID()) {        // Make sure smaller voxel id is first
        voxel_pair = std::pair<Voxel*,Voxel*>(v1, v2);
    } else {
        voxel_pair = std::pair<Voxel*,Voxel*>(v2, v1);
    }
    if (!guaranteeNew) {    // Vertex isnt guaranteed to be unique
        triangle_vertex_map::iterator it = triangle_vertices.find(voxel_pair);
        if (it != triangle_vertices.end()) {
            return it->second;
        }
    }
    Point p;
    if (useMidpoint)        // Find average position between voxels instead of linear interpolation based on value
        p = avgVoxelPosition(v1, v2);
    else 
        p = vertexInterpolation(surfaceValue, v1, v2);
    TriangleVertex* vertex;
    vertex = new TriangleVertex(p, voxel_pair.first, voxel_pair.second,Point(0.0, 0.0, 0.0),triangle_vertices.size());
    
    triangle_vertices[voxel_pair] = vertex;
    return vertex;
}

void Mesh::triangulate2D(double surfaceValue, bool blocky) {
    triangles.clear();
    triangle_vertices.clear();
    for (int y = 0; y < num_voxelsY-1; y++) {
        for (int x = 0; x < num_voxelsX-1; x++) {
            std::vector<Voxel*> corners = getCell(VoxelIndex(x, y, 0), false);
            // Get triangle index from active points
            int triangleIndex = 0;
            if (corners[0]->getValue() < surfaceValue) triangleIndex |= 1;
            if (corners[1]->getValue() < surfaceValue) triangleIndex |= 2;
            if (corners[2]->getValue() < surfaceValue) triangleIndex |= 4;
            if (corners[3]->getValue() < surfaceValue) triangleIndex |= 8;
            if (triangleIndex == 0) continue; // No active corners

            // Get voxels from list and create list of triangle voxels
            std::vector<TriangleVertex*> verts;
            const std::array<std::array<int, 8>, 16> * triangleArray;
            if (blocky)
                triangleArray = &TRIANGLES_2D_BLOCKY;
            else
                triangleArray = &TRIANGLES_2D_ROUNDED;

            for (int i = 0; (*triangleArray)[triangleIndex][i] != -1; i++) {
                int VoxelIndex = (*triangleArray)[triangleIndex][i];
                TriangleVertex* v;
                // Point p;
                if (VoxelIndex < 4) {
                    v = addTriangleVertex(corners[VoxelIndex], corners[VoxelIndex], surfaceValue);
                } else if (VoxelIndex == 4) {
                    v = addTriangleVertex(corners[0], corners[1], surfaceValue);
                } else if (VoxelIndex == 5) {
                    v = addTriangleVertex(corners[1], corners[2], surfaceValue);
                } else if (VoxelIndex == 6) {
                    v = addTriangleVertex(corners[2], corners[3], surfaceValue);
                } else if (VoxelIndex == 7) {
                    v = addTriangleVertex(corners[3], corners[0], surfaceValue);
                } else if (VoxelIndex == 8 && blocky) {
                    v = addTriangleVertex(corners[0], corners[2], surfaceValue, true, true);
                } else {
                    std::cerr << "Error: Unknown vertex index " << VoxelIndex << std::endl;
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
    double mag = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
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
    for (int z = 0; z < num_voxelsZ-1; z++) {
        for (int y = 0; y < num_voxelsY-1; y++) {
            for (int x = 0; x < num_voxelsX-1; x++) {
                // Corner vertices
                std::vector<Voxel*> corners = getCell(VoxelIndex(x, y, z), true);
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

void Mesh::recursiveBlend(VoxelIndex index, VoxelIndex center, double weight, double radius, std::vector<VoxelIndex> &already_set) {
    if (index.x >= num_voxelsX || index.y >= num_voxelsY || index.z >= num_voxelsZ || index.x < 0 || index.y < 0 || index.z < 0) return;
    for (size_t i = 0; i < already_set.size(); i++) {
        if (already_set[i] == index) return;
    }
    double distance = indexDistance(index, center);
    
    if (distance != 0.0) {
        if (lessThanOrEqual(distance, radius)) {
            already_set.push_back(index);
            double value = weight/distance;
            if (getVoxel(index)->getValue() < value) { getVoxel(index)->setValue(value); }
        } else { return; }
    }
    recursiveBlend(VoxelIndex(index.x, index.y + 1, index.z), center, weight, radius, already_set);
    recursiveBlend(VoxelIndex(index.x, index.y - 1, index.z), center, weight, radius, already_set);
    recursiveBlend(VoxelIndex(index.x + 1, index.y, index.z), center, weight, radius, already_set);
    recursiveBlend(VoxelIndex(index.x - 1, index.y, index.z), center, weight, radius, already_set);

    // Blend vertically as well
    // recursiveBlend(VoxelIndex(index.x, index.y, index.z + 1), center, weight, radius, already_set);
    // recursiveBlend(VoxelIndex(index.x, index.y, index.z - 1), center, weight, radius, already_set);
}

void Mesh::blend(double radius, int z, double weight) {
    if (radius <= 0) return;
    for (int y = 0; y < num_voxelsY; y++) {
        for (int x = 0; x < num_voxelsX; x++) {
            VoxelIndex index = VoxelIndex(x, y, z);
            Voxel* v = getVoxel(index);
            if (v->getValue() < 1.0) continue;
            std::vector<VoxelIndex> already_set;
            recursiveBlend(index, index, weight, radius, already_set);
            already_set.empty();
        }
    }
}

void Mesh::depthFill(int floor_size) {
    // Extend empty voxels upwards
    for (int y = 0; y < num_voxelsY; y++) {
        for (int x = 0; x < num_voxelsX; x++) {
            Voxel* v = getVoxel(VoxelIndex(x, y, floor_size));
            if (v->getValue() != -1.0) {
                for (int z = floor_size; z < num_voxelsZ-1; z++) {
                    VoxelIndex index = v->getIndex();
                    index.z = z;
                    getVoxel(index)->setValue(v->getValue());
                }
            }
        }
    }
}

void Mesh::cutoff(bool flat) {
    // Empty sides - Top and bottom
    for (int x = 0; x < num_voxelsX; x++) {
        for (int z = 0; z < num_voxelsZ; z++) {
            getVoxel(VoxelIndex(x, 0, z))->setValue(1.0); 
            getVoxel(VoxelIndex(x, num_voxelsY-1, z))->setValue(1.0);
        }
    }
    // Empty sides - Right and left
    for (int y = 0; y < num_voxelsY; y++) {
        for (int z = 0; z < num_voxelsZ; z++) {
            getVoxel(VoxelIndex(0, y, z))->setValue(1.0); 
            getVoxel(VoxelIndex(num_voxelsX-1, y, z))->setValue(1.0);
        }
    }
    if (!flat) {    // Mesh is 3D, also empty Top and bottom
        // Empty top and bottom
        for (int y = 0; y < num_voxelsY; y++) {
            for (int x = 0; x < num_voxelsX; x++) {
                getVoxel(VoxelIndex(x, y, 0))->setValue(1.0);
                getVoxel(VoxelIndex(x, y, num_voxelsZ-1))->setValue(1.0);
            }   
        }
    }
}

void Mesh::debugPrint2D(int z) const {
    for (int y = 0; y < num_voxelsY; y++) {
        for (int x = 0; x < num_voxelsX; x++) {
            Voxel* v = getVoxel(VoxelIndex(x, y, z));
            char fill = ' ';
            if (v->getValue() < 0.0) fill = '+';
            std::cout << fill << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Mesh::debugPrintSolution2D(int z, const std::vector<Voxel*>& sol) const {
    for (int y = 0; y < num_voxelsY; y++) {
        for (int x = 0; x < num_voxelsX; x++) {
            Voxel* v = getVoxel(VoxelIndex(x, y, z));
            char fill = ' ';
            bool onSolutionPath = false;
            for (uint i = 0; i < sol.size(); i++) {
                if (v->getID() == sol[i]->getID()) {
                    onSolutionPath = true;
                    break;
                }
            }
            if (onSolutionPath) fill = '*';
            else if (v->getValue() < 0.0) fill = '+';
            std::cout << fill << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Mesh::debugPrintValues(int z) const {
    for (int y = 0; y < num_voxelsY; y++) {
        for (int x = 0; x < num_voxelsX; x++) {
            Voxel* v = getVoxel(VoxelIndex(x, y, z));
            std::cout << std::setw(3) << v->getValue();
        }
        std::cout << std::endl;
    }
}