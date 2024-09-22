// package generic;

import java.awt.*;
import java.awt.event.InputEvent;
// import java.awt.event.*;
import java.awt.image.*;
import java.io.File;
import java.awt.event.KeyEvent;
import javax.imageio.ImageIO;

public class Face {
	public int index, edges;
	public int[] Verts, Adjacent;
	public int[] Flags;
	public static int Nfaces = 0;
	public double NormX, NormY, NormZ;
	
	public Face(int N) {
		Verts = new int[7];
		Adjacent = new int[7];
		Flags = new int[7];
		edges = N;
		index = Nfaces + 1;
		int i;
		
		for (i = 0; i <= 6; i++) {
			this.Verts[1] = 0;
			this.Adjacent[1] = 0;
			this.Flags[i] = 0;
		}
		this.Nfaces = Nfaces + 1;
	}

	public Face(int P1, int P2, int P3, int P4, int P5,int Adj1, int Adj2, int Adj3, int Adj4, int Adj5) {
		Verts = new int[7];
		Adjacent = new int[7];
		Flags = new int[7];
		edges = 5;
		index = Nfaces + 1;
		int i;
		
		this.Verts[1] = P1;
		this.Verts[2] = P2;
		this.Verts[3] = P3;
		this.Verts[4] = P4;
		this.Verts[5] = P5;
		this.Verts[6] = 0;
		this.Adjacent[1] = Adj1;
		this.Adjacent[2] = Adj2;
		this.Adjacent[3] = Adj3;
		this.Adjacent[4] = Adj4;
		this.Adjacent[5] = Adj5;
		this.Adjacent[6] = 0;
		for (i = 0; i <= 6; i++) {
			this.Flags[i] = 0;
		}
		
		this.Nfaces = Nfaces + 1;
	}
	
	public void ClearFlags() {
		int i;
		for (i = 0; i <= 6; i++) {
			this.Flags[i] = 0;
		}
	}
		
	public void Divide(Vertex[] Vertices, Face[] Faces) {
		int i,j;												// Simple iterator
		int Nverts;
			
		// Find center before making any modifications
		NormX = 0;
		NormY = 0;
		NormZ = 0;

		for (i= 1; i <= this.edges; i++) {											// Find center of Face
			NormX = NormX + Vertices[Verts[i]].xLoc;
			NormY = NormY + Vertices[Verts[i]].yLoc;
			NormZ = NormZ + Vertices[Verts[i]].zLoc;
		}
		NormX= NormX/this.edges;
		NormY= NormY/this.edges;
		NormZ= NormZ/this.edges;

		
		// Create new faces and connect near and far adjacent faces
		for (i= 1; i <= this.edges; i++) {						// Check each edge
			if (Flags[i] == 0) {								// Face has not been previously created
				Faces[Nfaces + 1] = new Face(6);				// Create new face
				Faces[Nfaces].Adjacent[5] = Adjacent[i];		// Assign outside adjacent face to new face
				Faces[Nfaces].Adjacent[2] = this.index;			// Assign original face at new face		
				for (j = 1; j <= 6; j++) {						// Determine which adjacent edge points here
					if (Faces[this.Adjacent[i]].Adjacent[j] == this.index) {
						Faces[this.Adjacent[i]].Adjacent[j] = Faces[Nfaces].index;		// Assign new face as adjacent to outside face
						Faces[this.Adjacent[i]].Flags[j] = 1;							// Set flag that this face has been created
					}
				}
				this.Adjacent[i] = Faces[Nfaces].index;			// Assign new face as adjacent to original
			}
		}

		// Assigned internal adjacent face indexes for new faces
		for (i= 1; i <= this.edges; i++) {						// Check each edge
			if (Flags[i] == 0) {								// Face has not been previously created
				Faces[this.Adjacent[i]].Adjacent[1] =this.Adjacent[(i % this.edges) +1];
				Faces[this.Adjacent[i]].Adjacent[3] =this.Adjacent[(i+this.edges-2) % this.edges + 1];
			} else {
				Faces[this.Adjacent[i]].Adjacent[4] =this.Adjacent[(i % edges) +1];
				Faces[this.Adjacent[i]].Adjacent[6] =this.Adjacent[(i+this.edges-2) % this.edges + 1];
			}
		}		

		// Create new vertices and assign to new and previously created faces
/*		NormX = 0;
		NormY = 0;
		NormZ = 0;

		for (i= 1; i <= this.edges; i++) {											// Find center of Face
			NormX = NormX + Vertices[Verts[i]].xLoc;
			NormY = NormY + Vertices[Verts[i]].yLoc;
			NormZ = NormZ + Vertices[Verts[i]].zLoc;
		}
		NormX= NormX/this.edges;
		NormY= NormY/this.edges;
		NormZ= NormZ/this.edges;
*/
		
		Nverts = Vertices[1].Nverts;
		for (i= 1; i <= this.edges; i++) {											// Create the new internal points
			Vertices[Nverts + i] = new Vertex();
			Vertices[Nverts + i].Middle(Vertices[Verts[i]], NormX, NormY, NormZ);
		}

		for (i= 1; i <= this.edges; i++) {	
			if (Flags[i] == 0) {								// Face has not been previously created
				Faces[this.Adjacent[i]].Verts[1] = this.Verts[(i % edges) +1];
				Faces[this.Adjacent[i]].Verts[2] = Nverts + i % edges+ 1;
				Faces[this.Adjacent[i]].Verts[3] = Nverts + i;
				Faces[this.Adjacent[i]].Verts[4] = this.Verts[i];
			} else {											// Face previously created, fill in final points
				Faces[this.Adjacent[i]].Verts[5] = Nverts + i % edges + 1;
				Faces[this.Adjacent[i]].Verts[6] = Nverts + i;
			}
		}
		for (i= 1; i <= this.edges; i++) {						// Change vertices for original face to new ones 
			this.Verts[i] = Nverts + i;
		}		
	}
}


