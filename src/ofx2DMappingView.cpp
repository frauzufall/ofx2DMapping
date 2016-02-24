#include "ofx2DMappingView.h"

ofx2DMappingView::ofx2DMappingView() {

	control_rect.position = ofPoint(0,0);
	zoom = 1;
	zoom_pos = ofPoint(0.5,0.5);
	show_source = true;
	direct_edit = false;

}

void ofx2DMappingView::setControl(ofx2DMappingController *ctrl) {
	this->ctrl = ctrl;
}

void ofx2DMappingView::setup(float x, float y, float w, float h) {

	control_rect.x = x;
	control_rect.y = y;
	control_rect.width= w;
	control_rect.height= h;

	//MAPPING RECT PANEL
	mapping_forms.setup("MAPPING FORMS", ctrl->getProjector(0), &object_list, w, h);
	mapping_forms.rebuild();
	mapping_forms.setMappingBackground(ctrl->getOutput());

	//MAIN OPTIONS PANEL

	main_panel.setup("MAPPING");

	save_btn.addListener(ctrl, &ofx2DMappingController::saveMappingDefault);
	save_btn.setup("save");
	main_panel.add(&save_btn);

	import_btn.addListener(this, &ofx2DMappingView::importSvg);
	import_btn.setup("import svg");
	main_panel.add(&import_btn);

	edit_mode_btn.addListener(this, &ofx2DMappingView::setEditMode);
	edit_mode_btn.setup("direct edit", direct_edit);
	main_panel.add(&edit_mode_btn);

	//CALIBRATION OPTIONS

	calibration_options.setup("CALIBRATION OPTIONS");

	calibration_options.add(ctrl->getCalibrating());
	calibration_options.add(ctrl->getCalBorder());
	calibration_options.add(ctrl->getCalGrey());

	main_panel.add(&calibration_options);

	//OBJECT LIST PANEL

	list_panel.setup("MAPPING OBJECTS");

	add_buttons_panel.setup("ADD MAPPING OBJECTS");

	add_button_params.clear();
	vector<ofPtr<ofx2DMappingObject>> options = ctrl->getOptions();
	for(unsigned int i = 0; i < options.size(); i++) {
		add_button_params.push_back(ofParameter<bool>("add " + options.at(i)->name, true));
		add_buttons_panel.add(add_button_params.at(i));
		add_button_params.at(i) = false;
	}

	list_panel.add(&add_buttons_panel);

	//LIST MANIPULATION OPTIONS

	list_options.setup("OBJECT MANIPULATION");

	select_all_btn.addListener(this, &ofx2DMappingView::selectAllObjects);
	select_all_btn.setup("select all");
	list_options.add(&select_all_btn);

	deselect_all_btn.addListener(this, &ofx2DMappingView::deselectAllObjects);
	deselect_all_btn.setup("deselect all");
	list_options.add(&deselect_all_btn);

	delete_all_btn.addListener(this, &ofx2DMappingView::removeAllObjects);
	delete_all_btn.setup("delete all");
	list_options.add(&delete_all_btn);

	list_panel.add(&list_options);

	//OBJECT LIST

	object_list.setup("MAPPING OBJECT LIST");
	ofAddListener(object_list.elementRemoved, this, &ofx2DMappingView::removeForm);
	ofAddListener(object_list.elementMovedStepByStep, this, &ofx2DMappingView::reorderForm);
	object_list.setHeaderBackgroundColor(ofColor::black);

	list_panel.add(&object_list);

	setSubpanelPositions();

	updateObjectList();


}

void ofx2DMappingView::update() {

	mapping_forms.update();

	for(unsigned int i = 0; i < add_button_params.size(); i++) {
		if(add_button_params.at(i)) {
			ctrl->getProjector(0)->copyShape(ctrl->getOptions().at(i));
			mapping_forms.updateForms();
			updateObjectList();
			add_buttons_panel.getToggle(add_button_params.at(i).getName()) = false;
			//add_button_params.at(i) = false;
		}
	}

}

void ofx2DMappingView::draw() {
	draw(control_rect.getPosition());
}

void ofx2DMappingView::draw(ofPoint pos) {

	if(pos != control_rect.position) {
		control_rect.position = pos;
		mapping_forms.setPosition(control_rect.x, mapping_forms.getPosition().y);
		mapping_forms.rebuild();
		setSubpanelPositions();
	}

	ofPushStyle();

	mapping_forms.draw(show_source);

	main_panel.draw();
	list_panel.draw();

	ofPopStyle();

}

void ofx2DMappingView::updateObjectList() {

	object_list.clear();

	ofx2DMappingProjector *p = ctrl->getProjector(0);

	for(unsigned int i = 0; i < p->shapeCount(); i++) {

		ofPtr<ofx2DMappingObject> mq = p->getMappingObject(i);
		if(mq) {
//            string objname = mq->name;
			//insert toggles at beginning of list
			mq->editable.setName(mq->name);
			object_list.add(mq->editable,false);
			object_list.getListItems().at(0)->setBorderColor(mq->color);
		}
	}
}

void ofx2DMappingView::importSvg() {
	ctrl->importSvg();
	mapping_forms.updateForms();
	object_list.clear();
	updateObjectList();
}

void ofx2DMappingView::removeForm(RemovedElementData &data) {

	ofx2DMappingProjector *p = ctrl->getProjector(0);
	int index = p->shapeCount()-1-data.index;
	if(p->removeShape(index)) {
		p->updateOutlines();
		mapping_forms.updateForms();
	}

}

void ofx2DMappingView::reorderForm(MovingElementData &data) {

	ofx2DMappingProjector *p = ctrl->getProjector(0);

	int index1 = p->shapeCount()-1-data.old_index;
	int index2 = p->shapeCount()-1-data.new_index;

	bool swapped = p->swapShapes(index1,index2);
	if(swapped) {
		mapping_forms.updateForms();
	}

}

void ofx2DMappingView::setSubpanelPositions() {
	float margin = 10;
	main_panel.setPosition(
				control_rect.x+margin,
				control_rect.y+margin);
	list_panel.setPosition(
				control_rect.x+margin,
				control_rect.y+margin+main_panel.getHeight()+margin*2);
	mapping_forms.setPosition(
				main_panel.getPosition().x + main_panel.getWidth()+margin,
				control_rect.y+margin);
}

void ofx2DMappingView::setMappingBackground(ofFbo_ptr fbo) {
	mapping_forms.setMappingBackground(fbo);
}

void ofx2DMappingView::showSource(bool show) {
	show_source = show;
}

void ofx2DMappingView::setEditMode(bool &direct_edit) {
	this->direct_edit = direct_edit;
	mapping_forms.setEditMode(direct_edit);
	setSubpanelPositions();
	//TODO trigger button
}

ofx2DFormMapping *ofx2DMappingView::getFormMapping() {
	return &mapping_forms;
}

void ofx2DMappingView::selectAllObjects() {
	for(unsigned int i = 0; i < ctrl->getProjector(0)->shapeCount(); i++) {
		ctrl->getProjector(0)->getMappingObject(i)->editable = true;
	}
}

void ofx2DMappingView::deselectAllObjects() {
	for(unsigned int i = 0; i < ctrl->getProjector(0)->shapeCount(); i++) {
		ctrl->getProjector(0)->getMappingObject(i)->editable = false;
	}
}

void ofx2DMappingView::removeAllObjects() {
	ctrl->getProjector(0)->removeAllShapes();
	ctrl->getProjector(0)->updateOutlines();
	mapping_forms.updateForms();
	updateObjectList();
}

ofRectangle ofx2DMappingView::getShape() {
	return control_rect;
}

void ofx2DMappingView::setShape(ofRectangle shape) {
	control_rect = shape;
	int margin = 10;
	mapping_forms.setSize(control_rect.getWidth()-object_list.getWidth()-margin*3, control_rect.getHeight()-main_panel.getHeight()-margin*3);
	setSubpanelPositions();
}
