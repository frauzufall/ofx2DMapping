#include "ofx2DMappingView.h"

ofx2DMappingView::ofx2DMappingView(const ofJson &config):ofxPanel() {

	zoom = 1;
	zoom_pos = ofPoint(0.5,0.5);
	direct_edit.set("direct edit", false);
	setup_done = false;
	mapping_forms = nullptr;
	list_panel = nullptr;
	main_panel = nullptr;
	object_list = nullptr;

	_setConfig(config);

}

ofx2DMappingView::~ofx2DMappingView(){
	if(setup_done){
		vector<ofPtr<ofx2DMappingObject>> options = ctrl->getOptions();
		for(uint i = 0; i < options.size(); i++) {
			options.at(i)->pleaseCopyMe.removeListener(this, &ofx2DMappingView::addedObject);
		}
		save.removeListener(ctrl, &ofx2DMappingController::saveMappingDefault);
		import.removeListener(this, &ofx2DMappingView::importSvg);
		direct_edit.removeListener(this, &ofx2DMappingView::setEditMode);
		select_all.removeListener(this, &ofx2DMappingView::selectAllObjects);
		deselect_all.removeListener(this, &ofx2DMappingView::deselectAllObjects);
		delete_all.removeListener(this, &ofx2DMappingView::removeAllObjects);
		ofRemoveListener(object_list->elementRemoved, this, &ofx2DMappingView::removeForm);
		ofRemoveListener(object_list->elementMovedStepByStep, this, &ofx2DMappingView::reorderForm);
	}
}

void ofx2DMappingView::setControl(ofx2DMappingController *ctrl) {
	this->ctrl = ctrl;
}

void ofx2DMappingView::setup(float x, float y, float w, float h) {

	setup_done = true;

	mapping_forms = this->add<ofx2DFormMapping>();
	main_panel = this->addPanel("MAPPING");
	list_panel = this->addGroup("MAPPING OBJECTS");
	main_panel->setAttribute("float", LayoutFloat::LEFT);
	list_panel->setAttribute("float", LayoutFloat::LEFT);
	mapping_forms->setAttribute("float", LayoutFloat::RIGHT);
	mapping_forms->setConfig(ofJson({{"width", "50%"}}));

	//MAIN OPTIONS PANEL
	save.addListener(ctrl, &ofx2DMappingController::saveMappingDefault);
	main_panel->add(save.set("save"));

	import.addListener(this, &ofx2DMappingView::importSvg);
	main_panel->add(import.set("import svg"));

	direct_edit.addListener(this, &ofx2DMappingView::setEditMode);
	main_panel->add(direct_edit);

	//CALIBRATION OPTIONS

	calibration_options = main_panel->addGroup("CALIBRATION OPTIONS");

	calibration_options->add(ctrl->getCalibrating());
	calibration_options->add(ctrl->getCalBorder());
	calibration_options->add(ctrl->getCalGrey());

	//OBJECT LIST PANEL

	add_buttons_panel = list_panel->addGroup("ADD MAPPING OBJECTS");

	vector<ofPtr<ofx2DMappingObject>> options = ctrl->getOptions();
	for(uint i = 0; i < options.size(); i++) {
//		ofxToggle::Config config = toggle_config;
//		ofColor c = options.at(i)->color;
//		if(c.getBrightness() < 200){
//			c.setBrightness(200);
//		}
//		config.textColor = c;
		add_buttons_panel->add(options.at(i)->pleaseCopyMe.set("add " + options.at(i)->name, false));
		options.at(i)->pleaseCopyMe.addListener(this, &ofx2DMappingView::addedObject);
	}


	//LIST MANIPULATION OPTIONS

	list_options = list_panel->addGroup("OBJECT MANIPULATION");

	select_all.addListener(this, &ofx2DMappingView::selectAllObjects);
	list_options->add(select_all.set("select all"));

	deselect_all.addListener(this, &ofx2DMappingView::deselectAllObjects);
	list_options->add(deselect_all.set("deselect all"));

	delete_all.addListener(this, &ofx2DMappingView::removeAllObjects);
	list_options->add(delete_all.set("delete all"));

	//OBJECT LIST

//	ofxGuiGroup::Config object_list_config = group_config;
//	object_list_config.spacing = 1;
	object_list = list_panel->add<ofxSortableList>("MAPPING OBJECT LIST");
	ofAddListener(object_list->elementRemoved, this, &ofx2DMappingView::removeForm);
	ofAddListener(object_list->elementMovedStepByStep, this, &ofx2DMappingView::reorderForm);

	//MAPPING RECT PANEL

	mapping_forms->setup("MAPPING FORMS", ctrl->getProjector(), object_list, w, h);
	mapping_forms->setMappingBackground(ctrl->getOutput());


	setName("Mapping");
	setShowHeader(false);

	setShape(x,y,w,h);

	updateObjectList();


}

void ofx2DMappingView::addedObject(bool & clickstart){
	if(clickstart){
		ctrl->update();
		mapping_forms->updateForms();
		updateObjectList();
	}
}

void ofx2DMappingView::updateObjectList() {

	object_list->clear();

	ofPtr<ofx2DMappingProjector> p = ctrl->getProjector();

	for(uint i = 0; i < p->shapeCount(); i++) {

		ofPtr<ofx2DMappingObject> mq = p->getMappingObject(i);
		if(mq) {
			//insert toggles at beginning of list
			mq->editable.setName(mq->name);
			ofColor c = mq->color;
			if(c.getBrightness() < 200){
				c.setBrightness(200);
			}
//			ofxToggle::Config config = toggle_config;
//			config.textColor = c;
			object_list->add(mq->editable);
		}
	}
}

void ofx2DMappingView::importSvg() {
	ctrl->importSvg();
	mapping_forms->updateForms();
	object_list->clear();
	updateObjectList();
}

void ofx2DMappingView::removeForm(RemovedElementData &data) {

	ofPtr<ofx2DMappingProjector> p = ctrl->getProjector();
	int index = p->shapeCount()-1-data.index;
	if(p->removeShape(index)) {
		p->updateOutlines();
		mapping_forms->updateForms();
	}

}

void ofx2DMappingView::reorderForm(MovingElementData &data) {

	ofPtr<ofx2DMappingProjector> p = ctrl->getProjector();

	int index1 = p->shapeCount()-1-data.old_index;
	int index2 = p->shapeCount()-1-data.new_index;

	bool swapped = p->swapShapes(index1,index2);
	if(swapped) {
		mapping_forms->updateForms();
	}

}

void ofx2DMappingView::setMappingBackground(ofFbo_ptr fbo) {
	mapping_forms->setMappingBackground(fbo);
}

void ofx2DMappingView::showSource(bool show) {
	mapping_forms->getShowSource().set(show);
}

void ofx2DMappingView::setEditMode(bool &) {
	mapping_forms->setEditMode(direct_edit);
}

ofx2DFormMapping *ofx2DMappingView::getFormMapping() {
	return mapping_forms;
}

void ofx2DMappingView::selectAllObjects() {
	for(uint i = 0; i < ctrl->getProjector()->shapeCount(); i++) {
		ctrl->getProjector()->getMappingObject(i)->editable = true;
	}
}

void ofx2DMappingView::deselectAllObjects() {
	for(uint i = 0; i < ctrl->getProjector()->shapeCount(); i++) {
		ctrl->getProjector()->getMappingObject(i)->editable = false;
	}
}

void ofx2DMappingView::removeAllObjects() {
	ctrl->getProjector()->removeAllShapes();
	ctrl->getProjector()->updateOutlines();
	mapping_forms->updateForms();
	updateObjectList();
}
