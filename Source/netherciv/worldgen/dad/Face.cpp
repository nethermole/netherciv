// Fill out your copyright notice in the Description page of Project Settings.


#include "netherciv/worldgen/dad/Face.h"

int Face::Nfaces = 0;

Face::Face(int N)
{
	Verts = std::vector<int>(7);						// Inefficient since index 0 is not used
	Adjacent = std::vector<int>(7);						// Inefficient since index 0 is not used
	Flags = std::vector<int>(7);						// Inefficient since index 0 is not used
	Face::Nfaces = Face::Nfaces + 1;								// Next face index in array
	index = Face::Nfaces;										// A face must know its own index in array
	edges = N;		
	for (int i = 0; i <= 6; i++) {
		Verts[i] = 0;
		Adjacent[i] = 0;
		Flags[i] = 0;
	}
}

Face::Face(int P1, int P2, int P3, int P4, int P5, int Adj1, int Adj2, int Adj3, int Adj4, int Adj5)
{
	int N = 5;
	Verts = std::vector<int>(7);						// Inefficient since index 0 is not used
	Adjacent = std::vector<int>(7);						// Inefficient since index 0 is not used
	Flags = std::vector<int>(7);						// Inefficient since index 0 is not used
	Face::Nfaces = Face::Nfaces + 1;					// Next face index in array
	index = Face::Nfaces;								// A face must know its own index in array
	edges = N;
	for (int i = 0; i <= 6; i++) {
		Verts[i] = 0;
		Adjacent[i] = 0;
		Flags[i] = 0;
	}
	
	SetVerts(P1, P2, P3, P4, P5, 0, Adj1, Adj2, Adj3, Adj4, Adj5, 0);
}

Face::Face(int P1, int P2, int P3, int P4, int P5, int P6, int Adj1, int Adj2, int Adj3, int Adj4, int Adj5, int Adj6)
{
	int N = 6;
	Verts = std::vector<int>();						// Inefficient since index 0 is not used
	Adjacent = std::vector<int>();						// Inefficient since index 0 is not used
	Flags = std::vector<int>();						// Inefficient since index 0 is not used
	Face::Nfaces = Face::Nfaces + 1;					// Next face index in array
	index = Face::Nfaces;								// A face must know its own index in array
	edges = N;
	for (int i = 0; i <= 6; i++) {
		Verts[i] = 0;
		Adjacent[i] = 0;
		Flags[i] = 0;
	}
	///

	SetVerts(P1, P2, P3, P4, P5, P6, Adj1, Adj2, Adj3, Adj4, Adj5, Adj6);
}

Face::~Face()
{
}

void Face::SetVerts(int P1, int P2, int P3, int P4, int P5, int P6, int Adj1, int Adj2, int Adj3, int Adj4, int Adj5, int Adj6)
{
	Verts[1] = P1;
	Verts[2] = P2;
	Verts[3] = P3;
	Verts[4] = P4;
	Verts[5] = P5;
	Verts[6] = P6;
	Adjacent[1] = Adj1;
	Adjacent[2] = Adj2;
	Adjacent[3] = Adj3;
	Adjacent[4] = Adj4;
	Adjacent[5] = Adj5;
	Adjacent[6] = Adj6;
}

void Face::ClearFlags()
{
	int i;
	for (i = 0; i <= 6; i++) {
		Flags[i] = 0;
	}
}

void Face::Divide(std::vector<Vertex*> &Vertices, std::vector<Face*> &Faces)
{

	int i, j;												// Simple iterator
	int Nverts;

	// Find center before making any modifications
	NormX = 0;
	NormY = 0;
	NormZ = 0;

	for (i = 1; i <= edges; i++) {	

		// Find center of Face
		NormX = NormX + Vertices[Verts[i]]->xLoc;
		NormY = NormY + Vertices[Verts[i]]->yLoc;
		NormZ = NormZ + Vertices[Verts[i]]->zLoc;
	}
	NormX = NormX / edges;
	NormY = NormY / edges;
	NormZ = NormZ / edges;


	// Create new faces and connect near and far adjacent faces
	for (i = 1; i <= edges; i++) {						// Check each edge
		if (Flags[i] == 0) {								// Face has not been previously created
			Face* face = new Face(6);
			Faces[face->index] = face;				// Create new face
			Faces[Face::Nfaces]->Adjacent[5] = Adjacent[i];		// Assign outside adjacent face to new face
			Faces[Face::Nfaces]->Adjacent[2] = index;			// Assign original face at new face		
			//				for (j = 1; j <= 6; j++) {						// Determine which adjacent edge points here
			for (j = 6; j >= 1; j--) {						// Determine which adjacent edge points here
				if (Faces[Adjacent[i]]->Adjacent[j] == index) {
					Faces[Adjacent[i]]->Adjacent[j] = Faces[Nfaces]->index;		// Assign new face as adjacent to outside face
					Faces[Adjacent[i]]->Flags[j] = 1;							// Set flag that this face has been created
				}
			}
			Adjacent[i] = Faces[Nfaces]->index;			// Assign new face as adjacent to original
		}
	}

	// Assigned internal adjacent face indexes for new faces
	for (i = 1; i <= edges; i++) {						// Check each edge
		if (Flags[i] == 0) {								// Face has not been previously created
			Faces[Adjacent[i]]->Adjacent[1] = Adjacent[(i % edges) + 1];
			Faces[Adjacent[i]]->Adjacent[3] = Adjacent[(i + edges - 2) % edges + 1];
		}
		else {
			Faces[Adjacent[i]]->Adjacent[4] = Adjacent[(i % edges) + 1];
			Faces[Adjacent[i]]->Adjacent[6] = Adjacent[(i + edges - 2) % edges + 1];
		}
	}

	// Create new vertices and assign to new and previously created faces

	Nverts = Vertices[1]->Nverts;
	for (i = 1; i <= edges; i++) {											// Create the new internal points
		Vertices[Nverts + i] = new Vertex();
		Vertices[Nverts + i]->Middle(Vertices[Verts[i]], NormX, NormY, NormZ);
	}

	for (i = 1; i <= edges; i++) {
		if (Flags[i] == 0) {								// Face has not been previously created
			Faces[Adjacent[i]]->Verts[1] = Verts[(i % edges) + 1];
			Faces[Adjacent[i]]->Verts[2] = Nverts + i % edges + 1;
			Faces[Adjacent[i]]->Verts[3] = Nverts + i;
			Faces[Adjacent[i]]->Verts[4] = Verts[i];

			//				System.out.format("%2d:%2d, 1-%2d, 2-%2d, 3-%2d, 4-%2d", index, Faces[Adjacent[i]].index, 
			//						this.Verts[(i % edges) +1], Nverts + i % edges+ 1, Nverts + i, this.Verts[i]);
		}
		else {											// Face previously created, fill in final points
			Faces[Adjacent[i]]->Verts[5] = Nverts + i % edges + 1;
			Faces[Adjacent[i]]->Verts[6] = Nverts + i;
			//				System.out.format("%2d:%2d, 5-%2d, 6-%2d", index, Faces[Adjacent[i]].index, 
			//						Nverts + i % edges + 1, Nverts + i);
		}
		//			System.out.println();
	}
	for (i = 1; i <= edges; i++) {						// Change vertices for original face to new ones 
		Verts[i] = Nverts + i;
	}
}
