//package generic;

import java.awt.*;
import java.awt.event.InputEvent;
// import java.awt.event.*;
import java.awt.image.*;
import java.io.File;
import java.awt.event.KeyEvent;
import javax.imageio.ImageIO;
import java.lang.Math.*;

public class Vertex {
	public double xLoc, yLoc, zLoc;
	public static int Nverts = 0;
	
	public Vertex(double x, double y, double z) {
		double L = Math.sqrt(x*x + y*y + z*z);
		this.xLoc = x/L;
		this.yLoc = y/L;
		this.zLoc = z/L;
		this.Nverts = Nverts + 1;
	}
	public Vertex() {
		this.xLoc = 0;
		this.yLoc = 0;
		this.zLoc = 0;
		this.Nverts = Nverts + 1;
	}
	
	public void Middle(Vertex P1, double X, double Y, double Z) {
		this.xLoc = (P1.xLoc + X)/2.;
		this.yLoc = (P1.yLoc + Y)/2.;
		this.zLoc = (P1.zLoc + Z)/2.;
		double L = Math.sqrt(xLoc*xLoc + yLoc*yLoc + zLoc*zLoc);
		this.xLoc = xLoc/L;
		this.yLoc = yLoc/L;
		this.zLoc = zLoc/L;
	}
}


