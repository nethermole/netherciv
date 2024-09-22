//  Read CSV files with altitudes and determine good mining locations

// import java.awt.*;
// import java.awt.event.InputEvent;
// import java.awt.image.*;
// import java.io.File;
import java.io.BufferedReader;  
import java.io.BufferedWriter;
import java.io.FileReader; 
import java.io.FileWriter;
import java.io.IOException;  
// import java.awt.event.KeyEvent;
// import javax.imageio.ImageIO;
import java.lang.Math.*;

public class Mesh {

	public static void main(String[] args) {
		int Nfaces = 0;
		int i, N;
		
		Vertex Vertices[];
		Vertices = new Vertex[50000000];
		
		Face Faces[];
		Faces = new Face[50000000];
		
		TwelvePents(Vertices, Faces);
		
		for (N= 1; N <= 3; N++) {
			Nfaces = Faces[1].Nfaces;
			for (i= 1; i<= Nfaces; i++) {
				Faces[i].ClearFlags();
			}
			for (i= 1; i<= Nfaces; i++) {
				Faces[i].Divide(Vertices, Faces);
			}
		}
	
		PrintVerts(Vertices);
		PrintFaces(Faces);
		System.out.println("Done");
		
	}

	static void TwelvePents(Vertex[] Vertices, Face[] Faces) {
		double GR = (1+ Math.sqrt(5.))/2;
		
		Vertices[12] = new Vertex(-1., -1., -1.);
		Vertices[20] = new Vertex(-1., -1.,  1.);
		Vertices[10] = new Vertex(-1.,  1., -1.);
		Vertices[16] = new Vertex(-1.,  1.,  1.);
		Vertices[ 6] = new Vertex( 1., -1., -1.);
		Vertices[14] = new Vertex( 1., -1.,  1.);
		Vertices[ 1] = new Vertex( 1.,  1., -1.);
		Vertices[ 8] = new Vertex( 1.,  1.,  1.);
		Vertices[ 5] = new Vertex( 0., -1./GR, -1.*GR);
		Vertices[19] = new Vertex( 0., -1./GR,     GR);
		Vertices[ 4] = new Vertex( 0.,  1./GR, -1.*GR);
		Vertices[15] = new Vertex( 0.,  1./GR,     GR);
		Vertices[18] = new Vertex(-1./GR, -1.*GR, 0.);
		Vertices[ 9] = new Vertex(-1./GR,  1.*GR, 0.);
		Vertices[13] = new Vertex( 1./GR, -1.*GR, 0.);
		Vertices[ 3] = new Vertex( 1./GR,  1.*GR, 0.);
		Vertices[11] = new Vertex(-1.*GR, 0., -1./GR);
		Vertices[17] = new Vertex(-1.*GR, 0.,  1./GR);
		Vertices[ 2] = new Vertex( 1.*GR, 0., -1./GR);
		Vertices[ 7] = new Vertex( 1.*GR, 0.,  1./GR);
		
		Faces[ 1] = new Face(  1,  2,  7,  8,  3,    3,  4,  8,  5,  2);
		Faces[ 2] = new Face(  1,  3,  9, 10,  4,    1,  5,  9,  6,  3);
		Faces[ 3] = new Face(  1,  4,  5,  6,  2,    2,  6,  7,  4,  1);
		Faces[ 4] = new Face(  2,  6, 13, 14,  7,    3,  7, 11,  8,  1);
		Faces[ 5] = new Face(  3,  8, 15, 16,  9,    1,  8, 12,  9,  2);
		Faces[ 6] = new Face(  4, 10, 11, 12,  5,    2,  9, 10,  7,  3);
		Faces[ 7] = new Face(  5, 12, 18, 13,  6,    6, 10, 11,  4,  3);
		Faces[ 8] = new Face(  7, 14, 19, 15,  8,    4, 11, 12,  5,  1);
		Faces[ 9] = new Face(  9, 16, 17, 11, 10,    5, 12, 10,  6,  2);
		Faces[10] = new Face( 11, 17, 20, 18, 12,    9, 12, 11,  7,  6);
		Faces[11] = new Face( 13, 18, 20, 19, 14,    7, 10, 12,  8,  4);
		Faces[12] = new Face( 15, 19, 20, 17, 16,    8, 11, 10,  9,  5);
	}
	
	static void PrintVerts(Vertex[] V) {
		int i;
		System.out.println("Coordinates for " +  V[1].Nverts + " Vertices");
		for(i = 1; i <= V[1].Nverts; i++) {
			System.out.format("%5d; %6.3f, %6.3f, %6.3f %n", i, V[i].xLoc, V[i].yLoc, V[i].zLoc);
		}
	}
	static void PrintFaces(Face[] F) {
		int i, j;
		System.out.format("%n%nVertex and adjacent side indices for %d faces %n", F[1].Nfaces);
		for(i = 1; i <= F[1].Nfaces; i++) {
			System.out.format("%7d, %2d; ", i, F[i].edges);
			for (j = 1; j <= 6; j++) {
				if (F[i].Verts[j] > 0) {System.out.format("%5d,", F[i].Verts[j]); } else {System.out.format(" - ,");}
			}
			System.out.format("   ");
			for (j = 1; j <= 6; j++) {
				if (F[i].Adjacent[j] > 0) {System.out.format("%5d,", F[i].Adjacent[j]); } else {System.out.format(" - ,");}
			}

			System.out.format("   ");
			for (j = 1; j <= 6; j++) {
				System.out.format("%2d,", F[i].Flags[j]);
			}
			System.out.format("   ");
			System.out.format("%6.3f, %6.3f, %6.3f", F[i].NormX, F[i].NormY, F[i].NormZ);
		
			System.out.println();
		}
	}
}			
