#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetColor(255);
	ofEnableDepthTest();

	this->cap.open("D:\\MP4\\video01.mp4");
	this->cap_size = cv::Size(480, 270);

	this->image.allocate(this->cap_size.width, this->cap_size.height, OF_IMAGE_COLOR);
	this->frame = cv::Mat(cv::Size(this->image.getWidth(), this->image.getHeight()), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);

	this->number_of_frames = this->cap.get(cv::CAP_PROP_FRAME_COUNT);
	for (int i = 0; i < this->number_of_frames; i++) {

		cv::Mat src, tmp;
		this->cap >> src;
		if (src.empty()) {

			continue;
		}

		cv::resize(src, tmp, this->cap_size);
		cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

		this->frame_list.push_back(tmp);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	int n = (int)(ofGetFrameNum() * 0.54) % this->number_of_frames;
	this->frame_list[n].copyTo(this->frame);
	this->image.update();

	this->mesh_list.clear();

	int x_span = 10;
	for (int x = this->cap_size.width * -0.5; x < this->cap_size.width * 0.5; x += x_span) {

		ofMesh mesh;
		int index = mesh.getNumVertices();

		mesh.addVertex(glm::vec3(x, this->cap_size.height * -0.5, 0));
		mesh.addVertex(glm::vec3(x + x_span, this->cap_size.height * -0.5, 0));
		mesh.addVertex(glm::vec3(x + x_span, this->cap_size.height * 0.5, 0));
		mesh.addVertex(glm::vec3(x, this->cap_size.height * 0.5, 0));

		mesh.addTexCoord(glm::vec3(x + this->cap_size.width * 0.5, 0, 0));
		mesh.addTexCoord(glm::vec3(x + x_span + this->cap_size.width * 0.5, 0, 0));
		mesh.addTexCoord(glm::vec3(x + x_span + this->cap_size.width * 0.5, this->cap_size.height, 0));
		mesh.addTexCoord(glm::vec3(x + this->cap_size.width * 0.5, this->cap_size.height, 0));

		mesh.addIndex(index + 0); mesh.addIndex(index + 1); mesh.addIndex(index + 2);
		mesh.addIndex(index + 0); mesh.addIndex(index + 2); mesh.addIndex(index + 3);

		this->mesh_list.push_back(mesh);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateX(180);

	for (auto& mesh : this->mesh_list) {

		int noise_y = ofMap(ofNoise(mesh.getVertex(0).x * 0.01, ofGetFrameNum() * 0.005), 0, 1, this->cap_size.height * -0.5, this->cap_size.height * 0.5);
		int param = ofGetFrameNum() % 90;
		noise_y *= param < 45 ? ofMap(param, 0, 45, 1, 0) : ofMap(param, 45, 90, 0, 1);

		for (int i = -1; i <= 1; i++) {

			ofPushMatrix();
			ofTranslate(0, noise_y + this->cap_size.height * i);

			this->image.bind();
			mesh.draw();
			this->image.unbind();

			ofPopMatrix();
		}
	}

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}