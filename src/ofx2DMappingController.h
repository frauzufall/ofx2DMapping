#pragma once

#include "ofMain.h"
#include "Projector.h"
#include "ofxXmlSettings.h"

typedef ofPtr<ofxXmlSettings> ofxXmlSettings_ptr;
typedef ofPtr<ofFbo> ofFbo_ptr;
typedef ofPtr<ofImage> ofImage_ptr;
typedef ofPtr<ofTexture> ofTexture_ptr;
	
class ofx2DMappingController {

    public:

        ofx2DMappingController();
        ~ofx2DMappingController() {}

        void						setup(string xml_path, float width, float height);
        void						update();
        void                        updateFbo(int projector_id);
        void                        updateAreaFbo(int projector_id);

        void						setupMapping();
        void						reloadMapping(ofxXmlSettings_ptr xml);
        void                        setRectangle(ofxXmlSettings_ptr xml, ofPoint* rectangle);
        ofPoint                     getPoint(ofxXmlSettings_ptr xml);
        ofPolyline                  getPolyline(ofxXmlSettings_ptr xml);

        void                        drawCalibration(Projector *p);

        void						addProjector(float w, float h);
        Projector*                  getProjector(int id);

        ofPoint                     getPointInMappedArea(ofPoint last_p,ofPoint next_p);
        ofPoint                     intersectionPointPolyline(ofPoint last_p, ofPoint next_p, ofPolyline polyline);

        ofPoint                     getIntersection(ofPoint p1, ofPoint p2, ofPoint p3, ofPoint p4);
        bool                        isLeft(ofPoint p1, ofPoint p2, ofPoint p_test);
        bool                        isOnLine(ofPoint p1, ofPoint p2, ofPoint p_test);

        bool                        isCalibrating();
        void                        setCalibrating(bool calibrate);

        float                       getCalBorder();
        int                         getCalGrey();
        void                        setCalBorder(float border);
        void                        setCalGrey(int grey);

        float							contentWidth();
        float							contentHeight();
        float							outputWidth();
        float							outputHeight();
        float							controlWidth();
        float							controlHeight();
        float							vidMaxWidth();
        float							vidMaxHeight();

        void						setContentWidth(float val);
        void						setContentHeight(float val);
        void						setOutputWidth(float val);
        void						setOutputHeight(float val);
        void						setControlWidth(float val);
        void						setControlHeight(float val);
        void						setVidMaxWidth(float val);
        void						setVidMaxHeight(float val);

        ofFbo_ptr					getOutput();
        ofFbo_ptr					getArea();

        void                        setInputFbo(ofFbo_ptr fbo);

        ofPoint                     controlpoint;

        void keyPressed  (ofKeyEventArgs &args);
        void keyReleased(ofKeyEventArgs &args);

        void saveOutputImage();

        void addOption(string name, ofPolyline shape, ofFbo fbo);
        void addOption(string name, ofPolyline shape, ofFbo_ptr fbo);
        void addOption(string name, ofPolyline shape, ofTexture texture);
        void addOption(string name, ofPolyline shape, ofTexture_ptr texture);
        void addOption(string name, ofPolyline shape, ofImage image);
        void addOption(string name, ofPolyline shape, ofImage_ptr image);
        void addOption(string name, ofPolyline shape, ofColor color);
        void addOption(string name, ofPoint point);
        void addOption(string name, string text);

    private:

        bool use_mapping;

        string xml_mapping;

        float output_w, output_h;
        float content_w, content_h;
        float control_w, control_h;
        float vid_max_w, vid_max_h;

        vector<Projector>			projectors;
        GLfloat						matrix[16];
        ofPoint						plane[4];

        bool                        is_cal;
        float                       cal_border;
        int                         cal_grey;

        ofFbo_ptr                   src_fbo;
        ofEvent<ofFbo_ptr>          updatedFbo;

        ofFbo_ptr           		mapped_content_fbo;
        ofFbo_ptr                   mapped_area_fbo;

        void                        mappedContentToFbo(Projector *p);
        void                        mappedAreaToFbo(Projector *p);

        vector<MappingObject_ptr>   available_shapes;

};


