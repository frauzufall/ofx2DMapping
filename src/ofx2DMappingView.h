#pragma once
#include "ofMain.h"
#include "ofx2DMappingController.h"
#include "ofxSortableList.h"
#include "ofx2DFormMapping.h"

class ofx2DMappingView {

public:
    ofx2DMappingView();
    void setup(float x, float y, float w, float h);
    void update();
    void draw();
    void draw(ofPoint pos);
    void setMappingBackground(ofFbo_ptr fbo);
    void showSource(bool show);
    void setEditMode(bool &direct_edit);

    void setControl(ofx2DMappingController* ctrl);

    void importSvg();

    ofx2DFormMapping* getFormMapping();

    void setShape(ofRectangle shape);
    ofRectangle getShape();

private:

    ofx2DMappingController* ctrl;

    void setSubpanelPositions();
    void updateObjectList();
    void selectAllObjects();
    void deselectAllObjects();
    void removeAllObjects();

    bool show_source, direct_edit;

    ofRectangle control_rect;

    ofxSortableList object_list;

    ofPoint pos_main_options, pos_calibration_options, pos_add_buttons, pos_shape_list;

    ofxGuiGroup add_buttons_panel;
    ofx2DFormMapping mapping_forms;
    ofxPanel main_panel, list_panel;
    ofxGuiGroup calibration_options, list_options;

    void removeForm(RemovedElementData& data);
    void reorderForm(MovingElementData& data);

    ofxButton save_btn, import_btn, select_all_btn, deselect_all_btn, delete_all_btn;
    ofxToggle edit_mode_btn;
    vector<ofParameter<bool>> add_button_params;

    float zoom;
    ofPoint zoom_pos;


};
