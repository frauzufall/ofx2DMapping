#pragma once

#include "ofMain.h"

class Scene2
{
public:
    Scene2();
    void setup(float w, float h);
    void update();
    ofPtr<ofFbo> &getFbo();

private:
    ofPtr<ofFbo> fbo;
    ofTrueTypeFont myfont;
};
