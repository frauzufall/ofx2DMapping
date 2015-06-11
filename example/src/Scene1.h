#pragma once

#include "ofMain.h"

class Scene1
{
public:
    Scene1();
    void setup(float w, float h);
    void update();
    ofPtr<ofFbo> &getFbo();

private:
    ofPtr<ofFbo> fbo;
    ofTrueTypeFont myfont;
};
