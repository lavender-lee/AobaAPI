#include "AobaAPI/Ops/Create.hpp"

namespace Aoba {
namespace Ops {

const CreateGridResult CreateGrid(Core::Mesh* m, int divisionsX, int divisionsY, float sizeX, float sizeY) {
    std::size_t divsX = divisionsX;
    std::size_t divsY = divisionsY;
    // Create verts.
    std::vector<Core::Vert*> verts = std::vector<Core::Vert*>();
    verts.reserve((2 + divsX) * (2 + divsY));
    std::vector<Core::Vert*> boundaryVerts = std::vector<Core::Vert*>();
    boundaryVerts.reserve(divsX + 2 * (2 + divsX));
    std::vector<Core::Vert*> innerVerts = std::vector<Core::Vert*>();
    innerVerts.reserve(divsX * divsY);

    float xStep = sizeX / (divsX + 1);
    float yStep = sizeY / (divsY + 1);
    for(int x = 0; x <= divsX + 1; ++x) {
        for(int y = 0; y <= divsY + 1; ++y) {
            Core::Vert* newv = new Core::Vert();
            Core::MakeVert(m, newv);
            newv->co = Math::Vec3(-sizeX / 2 + x * xStep, -sizeY / 2 + y * yStep, 0);
            verts.push_back(newv);

            // push to inner/boundary vert
            if(x == 0 || x == divsX + 1 || y == 0 || y == divsY + 1) {
                boundaryVerts.push_back(newv);
            } else {
                innerVerts.push_back(newv);
            }
        }
    }

    std::vector<Core::Edge*> boundaryEdges = std::vector<Core::Edge*>();
    boundaryEdges.reserve(2 * (divsX + 2 + divsY));
    std::vector<Core::Edge*> innerEdges = std::vector<Core::Edge*>();
    innerEdges.reserve(divsX * (1 + divsY) + divsY * (1 + divsX));
    std::vector<Core::Edge*> edges = std::vector<Core::Edge*>();
    edges.reserve((2 * (divsX + 2 + divsY)) + (divsX * (1 + divsY) + divsY * (1 + divsX)));

    // create edges paralel to x axis
    for(std::size_t y = 0; y <= divsY + 1; ++y) {
        for(std::size_t x = 0; x <= divsX; ++x) {
            std::size_t idx = y * (2 + divsX) + x;
            Core::Edge* newe = new Core::Edge();
            Core::MakeEdge(verts.at(idx), verts.at(idx + 1), newe);
            edges.push_back(newe);
        }
    }

    // create edges paralel to y axis
    for(std::size_t x = 0; x <= divsX + 1; ++x) {
        for(std::size_t y = 0; y <= divsY; ++y) {
            std::size_t idx = x + (2 + divsX) * y;
            Core::Edge* newe = new Core::Edge();
            Core::MakeEdge(verts.at(idx), verts.at(idx + 2 + divsX), newe);
            edges.push_back(newe);
        }
    }

    std::vector<Core::Face*> boundaryFaces = std::vector<Core::Face*>();
    std::size_t innerFaceCount = 0;
    if(divsX >= 2 && divsY >= 2) {
        innerFaceCount = (divsX - 1) * (divsY - 1);
    }
    boundaryFaces.reserve((divsX + 1) * (divsY + 1) - innerFaceCount);
    std::vector<Core::Face*> innerFaces = std::vector<Core::Face*>();
    innerFaces.reserve(innerFaceCount);

    // Create edges and faces.
    for(std::size_t x = 0; x <= divsX; ++x) {
        for(std::size_t y = 0; y <= divsY; ++y) {
            Core::Edge* e0 = edges.at(y * (1 + divsX) + x);
            Core::Edge* e1 = edges.at((2 + divsY) * (1 + divsX) + x * (1 + divsY) + y);
            Core::Edge* e2 = edges.at((1 + y) + (1 + divsX) + x);
            Core::Edge* e3 = edges.at((2 + divsY) * (1 + divsX) + (1 + x) * (1 + divsY) + y);

            Core::Vert* v0 = verts.at(y * (2 + divsX) + x);
            Core::Vert* v1 = verts.at(y * (2 + divsX) + x + 1);
            Core::Vert* v2 = verts.at((1 + y) * (2 + divsX) + x + 1);
            Core::Vert* v3 = verts.at((1 + y) * (2 + divsX) + x);

            std::vector<Core::Edge*> loopEdges = {e0, e1, e2, e3};
            std::vector<Core::Vert*> loopVerts = {v0, v1, v2, v3};

            Core::Loop* newl = new Core::Loop();
            Core::MakeLoop(loopEdges, loopVerts, newl);
            Core::Face* newf = new Core::Face();
            Core::MakeFace(std::vector<Core::Loop*> {newl}, newf);

            // TODO: push faces, edges to appropriate vectors...
            bool isBoundaryFace = false;
            if(x == 0) {
                boundaryEdges.push_back(e0);
                isBoundaryFace = true;
            } else {
                innerEdges.push_back(e0);
            }
            if(x == divsX + 1) {
                boundaryEdges.push_back(e2);
                isBoundaryFace = true;
            } else {
                innerEdges.push_back(e2);
            }
            if(y == 0) {
                boundaryEdges.push_back(e3);
                isBoundaryFace = true;
            } else {
                innerEdges.push_back(e3);
            }
            if(x == divsY + 1) {
                boundaryEdges.push_back(e1);
                isBoundaryFace = true;
            } else {
                innerEdges.push_back(e1);
            }
            if(isBoundaryFace) {
                boundaryFaces.push_back(newf);
            } else {
                innerFaces.push_back(newf);
            }
        }
    }

    CreateGridResult result = CreateGridResult();
    result.innerEdges = innerEdges;
    result.innerFaces = innerFaces;
    result.innerVerts = innerVerts;
    result.outerEdges = boundaryEdges;
    result.outerFaces = boundaryFaces;
    result.outerVerts = boundaryVerts;

    return result;
}

} // namespace Ops
} // namespace Aoba
