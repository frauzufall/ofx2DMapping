#pragma once

#include "MappingContentShape.h"

class MappingFbo : public MappingContentShape {

    public:

    ofFbo_ptr fbo;

    MappingFbo() {
        this->nature = "FBO";
    }

    MappingFbo(const MappingFbo& obj) : MappingContentShape(obj) {
        this->fbo = obj.fbo;
    }

    ofPtr<MappingObject> clone() const {
        return ofPtr<MappingObject>(new MappingFbo(*this));
    }

    void setFbo(ofFbo_ptr &fbo) {
        this->fbo = fbo;
        src_width = fbo->getWidth();
        src_height = fbo->getHeight();
        setTexture(&fbo->getTexture());
    }

};
