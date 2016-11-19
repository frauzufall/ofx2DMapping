#pragma once

#include "ofMain.h"
#include "ofx2DMappingObjectFactory.h"
#include "ofxTriangleMesh.h"
#include "ofxXmlSettings.h"
#include "ofx2DMappingHelper.h"
#include "IDGenerator.h"

typedef ofPtr<ofFbo> ofFbo_ptr;
typedef ofPtr<ofxXmlSettings> ofxXmlSettings_ptr;

class ofx2DMappingObject {
public:

	string id;
	string  nature;

	string name;
	ofColor color;

	ofx2DMappingObject() {
		stringstream id_str;
		id_str << IDGenerator::getInstance().next();
		id = id_str.str();
		newpos = true;
		newitem = true;

		nature = "OBJECT";

		editable.set(nature, false);
	}

	ofx2DMappingObject(const ofx2DMappingObject& obj) {
		stringstream id_str;
		id_str << IDGenerator::getInstance().next();
		id = id_str.str();
		newpos = true;
		newitem = true;
		this->color = obj.color;
		this->name = obj.name;
		this->nature = obj.nature;
		this->newitem = true;
		this->editable.set(obj.name, true);
	}

	virtual ofPtr<ofx2DMappingObject> clone() const = 0;

	void setColor(ofColor color) {
		this->color = color;
	}

	virtual void loadXml(ofxXmlSettings_ptr xml) {
		color.r = xml->getAttribute("color", "r", 255, 0);
		color.g = xml->getAttribute("color", "g", 255, 0);
		color.b = xml->getAttribute("color", "b", 255, 0);

	}

	virtual void saveXml(ofxXmlSettings_ptr xml) {
		xml->addTag("color");
		xml->addAttribute("color", "r", color.r, 0);
		xml->addAttribute("color", "g", color.g, 0);
		xml->addAttribute("color", "b", color.b, 0);
	}

	virtual void draw(float w, float h) = 0;
	virtual void drawArea(float w, float h) = 0;

	virtual void update(float w, float h) = 0;

	static void gaussian_elimination(float *input, int n)
	{
		// ported to c from pseudocode in
		// http://en.wikipedia.org/wiki/Gaussian_elimination

		float * A = input;
		int i = 0;
		int j = 0;
		int m = n-1;
		while (i < m && j < n)
		{
			// Find pivot in column j, starting in row i:
			int maxi = i;
			for(int k = i+1; k<m; k++)
			{
				if(fabs(A[k*n+j]) > fabs(A[maxi*n+j]))
				{
					maxi = k;
				}
			}
			if (A[maxi*n+j] != 0)
			{
				//swap rows i and maxi, but do not change the value of i
				if(i!=maxi)
					for(int k=0; k<n; k++)
					{
						float aux = A[i*n+k];
						A[i*n+k]=A[maxi*n+k];
						A[maxi*n+k]=aux;
					}
				//Now A[i,j] will contain the old value of A[maxi,j].
				//divide each entry in row i by A[i,j]
				float A_ij=A[i*n+j];
				for(int k=0; k<n; k++)
				{
					A[i*n+k]/=A_ij;
				}
				//Now A[i,j] will have the value 1.
				for(int u = i+1; u< m; u++)
				{
					//subtract A[u,j] * row i from row u
					float A_uj = A[u*n+j];
					for(int k=0; k<n; k++)
					{
						A[u*n+k]-=A_uj*A[i*n+k];
					}
					//Now A[u,j] will be 0, since A[u,j] - A[i,j] * A[u,j] = A[u,j] - 1 * A[u,j] = 0.
				}

				i++;
			}
			j++;
		}

		//back substitution
		for(int i=m-2; i>=0; i--)
		{
			for(int j=i+1; j<n-1; j++)
			{
				A[i*n+m]-=A[i*n+j]*A[j*n+m];
				//A[i*n+j]=0;
			}
		}
	}

	static void findHomography(ofPolyline& line, ofPoint dst_norm[4], float homography[16], bool points_normalized, float w, float h){
		ofPoint src_norm[4];
		src_norm[0] = line[0];
		src_norm[1] = line[1];
		src_norm[2] = line[2];
		src_norm[3] = line[3];
		findHomography(src_norm, dst_norm, homography, points_normalized, w, h);

	}

	static ofMatrix4x4 findHomography(ofRectangle r1, ofRectangle r2){
		ofMatrix4x4 m;
		ofPoint src[4], dst[4];
		src[0] = r1.getTopLeft();
		src[1] = r1.getTopRight();
		src[2] = r1.getBottomRight();
		src[3] = r1.getBottomLeft();
		dst[0] = r2.getTopLeft();
		dst[1] = r2.getTopRight();
		dst[2] = r2.getBottomRight();
		dst[3] = r2.getBottomLeft();
		findHomography(src, dst, (GLfloat*) m.getPtr(), false, 0, 0);
		return m;
	}

	static ofMatrix4x4 findHomography(ofPoint src_norm[4], ofPoint dst_norm[4]){
		ofMatrix4x4 m;
		findHomography(src_norm, dst_norm, (GLfloat*) m.getPtr(), false, 0, 0);
		return m;
	}

	static void findHomography(ofPoint src_norm[4], ofPoint dst_norm[4], float homography[16], bool points_normalized, float w, float h)
	{

		ofPoint src[4];
		ofPoint dst[4];

		if(points_normalized) {
			for(int i = 0; i < 4; i++) {
				src[i].x=src_norm[i].x*w;
				src[i].y=src_norm[i].y*h;
				dst[i].x=dst_norm[i].x*w;
				dst[i].y=dst_norm[i].y*h;
			}
		}
		else {
			for(int i = 0; i < 4; i++) {
				src[i].x=src_norm[i].x;
				src[i].y=src_norm[i].y;
				dst[i].x=dst_norm[i].x;
				dst[i].y=dst_norm[i].y;
			}
		}

		// create the equation system to be solved
		//
		// from: Multiple View Geometry in Computer Vision 2ed
		//       Hartley R. and Zisserman A.
		//
		// x' = xH
		// where H is the homography: a 3 by 3 matrix
		// that transformed to inhomogeneous coordinates for each point
		// gives the following equations for each point:
		//
		// x' * (h31*x + h32*y + h33) = h11*x + h12*y + h13
		// y' * (h31*x + h32*y + h33) = h21*x + h22*y + h23
		//
		// as the homography is scale independent we can let h33 be 1 (indeed any of the terms)
		// so for 4 points we have 8 equations for 8 terms to solve: h11 - h32
		// after ordering the terms it gives the following matrix
		// that can be solved with gaussian elimination:

		float P[8][9]=
		{
			{-src[0].x, -src[0].y, -1,   0,   0,  0, src[0].x*dst[0].x, src[0].y*dst[0].x, -dst[0].x }, // h11
			{  0,   0,  0, -src[0].x, -src[0].y, -1, src[0].x*dst[0].y, src[0].y*dst[0].y, -dst[0].y }, // h12

			{-src[1].x, -src[1].y, -1,   0,   0,  0, src[1].x*dst[1].x, src[1].y*dst[1].x, -dst[1].x }, // h13
			{  0,   0,  0, -src[1].x, -src[1].y, -1, src[1].x*dst[1].y, src[1].y*dst[1].y, -dst[1].y }, // h21

			{-src[2].x, -src[2].y, -1,   0,   0,  0, src[2].x*dst[2].x, src[2].y*dst[2].x, -dst[2].x }, // h22
			{  0,   0,  0, -src[2].x, -src[2].y, -1, src[2].x*dst[2].y, src[2].y*dst[2].y, -dst[2].y }, // h23

			{-src[3].x, -src[3].y, -1,   0,   0,  0, src[3].x*dst[3].x, src[3].y*dst[3].x, -dst[3].x }, // h31
			{  0,   0,  0, -src[3].x, -src[3].y, -1, src[3].x*dst[3].y, src[3].y*dst[3].y, -dst[3].y }, // h32
		};

		gaussian_elimination(&P[0][0],9);

		// gaussian elimination gives the results of the equation system
		// in the last column of the original matrix.
		// opengl needs the transposed 4x4 matrix:
		float aux_H[]= { P[0][8],P[3][8],0,P[6][8], // h11  h21 0 h31
						 P[1][8],P[4][8],0,P[7][8], // h12  h22 0 h32
						 0      ,      0,0,0,       // 0    0   0 0
						 P[2][8],P[5][8],0,1
					   };      // h13  h23 0 h33

		for(int i=0; i<16; i++) homography[i] = aux_H[i];
	}

	bool        newpos;
	bool        newitem;
	ofParameter<bool> editable;
	ofParameter<bool> pleaseCopyMe;

	~ofx2DMappingObject(){}

protected:

	ofPoint getPoint(ofxXmlSettings_ptr xml) {

		ofPoint res;

		res.x = xml->getValue("x", 0.);
		res.y = xml->getValue("y", 0.);

		return res;

	}

};

