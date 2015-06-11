#pragma once

#include "MappingContentShape.h"

class MappingImage : public MappingContentShape {

    public:

    ofImage     image;
    string      img_src;

    MappingImage() {
        this->image.clear();
        this->img_src = "";
        this->nature = "IMAGE";
    }

    MappingImage(const MappingImage& obj) : MappingContentShape(obj) {
        this->image = obj.image;
        this->img_src = obj.img_src;
    }

    ofPtr<MappingObject> clone() const {
        return ofPtr<MappingObject>(new MappingImage(*this));
    }

    void loadXml(ofxXmlSettings_ptr xml) {
        MappingContentShape::loadXml(xml);
        string url = xml->getValue("url", "images/notfound.png");
        loadImage(url);
    }

    virtual void saveXml(ofxXmlSettings_ptr xml) {
        MappingContentShape::saveXml(xml);
        xml->addValue("url", img_src);
    }

//    void draw(float w, float h) {

//        if(this->image.isAllocated()) {
//            ofPushMatrix();
//            glMultMatrixf(this->matrix_dst_norm.getPtr());

//            ofSetColor(255);
//            ofFill();

//            this->image.bind();

//                glBegin(GL_QUADS);

//                glTexCoord2f(this->src[0].x*this->image.getWidth(), this->src[0].y*this->image.getHeight());	glVertex3f(0, 0, 0);
//                glTexCoord2f(this->src[1].x*this->image.getWidth(), this->src[1].y*this->image.getHeight());	glVertex3f(w, 0, 0);
//                glTexCoord2f(this->src[2].x*this->image.getWidth(), this->src[2].y*this->image.getHeight());	glVertex3f(w, h, 0);
//                glTexCoord2f(this->src[3].x*this->image.getWidth(), this->src[3].y*this->image.getHeight());	glVertex3f(0, h, 0);

//                glEnd();

//            this->image.unbind();
//            glPopMatrix();
//        }

//    }

    void loadImage(string url) {
        image.clear();
        if(image.load(url)) {
            img_src = url;
            src_width = image.getWidth();
            src_height = image.getHeight();
            setTexture(&image.getTexture());
        }
        else {
            ofLogError("MappingImage::loadImage()", "Could not load " + url + ".");
        }
    }

};
