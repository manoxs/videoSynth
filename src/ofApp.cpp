#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    oscReceiver.setup( 12345 );
    ofSetWindowTitle("XHGZ");
    ofSetWindowShape(1280, 720);
    ofSetFrameRate(60);
    ofBackground(ofColor::white);
    ofSetFullscreen( true );
    
    gui.setup( "Parameters", "settings.xml" );
    gui.add( countX.setup( "countX", 50, 0, 200 ) );
    gui.add( stepX.setup( "stepX", 20, 0, 200 ) );
    gui.add( twistX.setup( "twistX", 5, -45, 45 ) );
    
    gui.add( countY.setup( "countY", 0,0,50 ) );
    gui.add( stepY.setup( "stepY", 20, 0, 200 ) );
    gui.add( twistY.setup( "twistY", 20, 0, 200 ) );
    gui.add( pinchY.setup("pinchY", 0, 0, 1) );
    
    globalGroup.setup( "Global" );
    globalGroup.add( Scale.setup( "Scale", 1, 0.0, 1 ) );
    globalGroup.add( Rotate.setup( "Rotate", 0, -180, 180 ) );
    globalGroup.add( Background.setup("Background",255,0, 255));
    gui.add( &globalGroup );
    
    primGroup.setup( "Primitive" );
    primGroup.add( shiftY.setup("shiftY",0.0,-1000.0,1000.0 ) );
    primGroup.add( rotate.setup("rotate",0.0,-180.0,180.0 ) );
    primGroup.add( size.setup( "size",
                              ofVec2f(6,6),
                              ofVec2f(0,0),
                              ofVec2f(20,20) ) );
    primGroup.add( color.setup( "color",
                               ofColor::black,
                               ofColor(0,0,0,0),
                               ofColor::white ) );
    primGroup.add( filled.setup( "filled", false ) );
    primGroup.add( type.setup( "type", false ) );
    gui.add( &primGroup );
    
    mixerGroup.setup( "Mixer" );
    mixerGroup.setHeaderBackgroundColor(ofColor::darkRed);
    mixerGroup.setBorderColor(ofColor:: darkRed);
    mixerGroup.add( imageAlpha.setup( "image", 100,0,255 ) );
    mixerGroup.add( videoAlpha.setup( "video", 200,0,255 ) );
    mixerGroup.add( cameraAlpha.setup( "camera", 100,0,255 ) );
    shader.load( "kaleido" );
    mixerGroup.add( kenabled.setup( "kenabled", true ) );
    mixerGroup.add( ksectors.setup( "ksectors", 10, 1, 100 ) );
    mixerGroup.add( kangle.setup( "kangle", 0, -180, 180 ) );
    mixerGroup.add( kx.setup( "kx", 0.5, 0, 1 ) );
    mixerGroup.add( ky.setup( "ky", 0.5, 0, 1 ) );
    //Automation lfos
    mixerGroup.add(automate.setup( "automate", true) );
    gui.minimizeAll();
    gui.add( &mixerGroup );
  
    //load raster image
    
    ofLoadImage(image, "baby.jpg");
    
    video.load("flowing.mp4");
    video.play();
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB); // Offscreen Buffer for shader
    
    showGui = true;
    
    // Values for the LFO in dynamic mode
    phase = 0;
    frequency = 0.1;
    
    // Initialize the soundlevel var
    soundLevel = 0;
    ofSoundStreamSetup( 0, 1, 44100, 128, 4);
    
    gui.loadFromFile("preset.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    
    while ( oscReceiver.hasWaitingMessages() ) {
        ofxOscMessage m;
        oscReceiver.getNextMessage( m );
        if ( m.getAddress() == "/countX" ) {
            countX = m.getArgAsFloat( 0 );
        }
        if ( m.getAddress() == "/stepX" ) {
            stepX = m.getArgAsFloat( 0 );
        }
        if ( m.getAddress() == "/twistX" ) {
            twistX = m.getArgAsFloat( 0 );
        }
        if  ( m.getAddress() == "/countY" ) {
            countY = m.getArgAsFloat( 0 );
        }
        if  ( m.getAddress() == "/stepY" ) {
            stepY = m.getArgAsFloat( 0 );
        }
        if  ( m.getAddress() == "/twistY" ) {
            twistY = m.getArgAsFloat( 0 );
        }
        if ( m.getAddress() == "/pinchY" ) {
            pinchY = m.getArgAsFloat( 0 );
        }
        //button kenabled ToOsc
        if ( m.getAddress() == "/kenabled" ) {
            kenabled = m.getArgAsFloat( 0 );
        }
        //Background ToOsc
        if ( m.getAddress() == "/Background" ) {
            Background = m.getArgAsFloat( 0 );
        }
        //filled Button ToOsc
        if ( m.getAddress() == "/filled" ) {
            filled = m.getArgAsFloat( 0 );
        }
        //type Button ToOsc
        if ( m.getAddress() == "/type" ) {
            type = m.getArgAsFloat( 0 );
        }
        //color ToOsc
        if ( m.getAddress() == "/color" ) {
            type = m.getArgAsRgbaColor(0);
        }
       //imageAlphazzl
        if ( m.getAddress() == "/imageAlpha" ) {
            imageAlpha = m.getArgAsFloat( 0 );
        }
       //videoAlpha
        if ( m.getAddress() == "/videoAlpha" ) {
            videoAlpha = m.getArgAsFloat( 0 );
        }
       // automate Button ToOsc
        if ( m.getAddress() == "/automate" ) {
            automate = m.getArgAsFloat( 0 );
        }
    }
    video.update();
    if ( camera.isInitialized() ) camera.update();
    if ( automate ) {
        // A Simple LFO
        //float phase = 0.1 * ofGetElapsedTimef() * M_TWO_PI;
        float dt = 1.0 / 60.0;
        phase += dt * frequency * M_TWO_PI;
        float valueS = sin( phase );
        float valueC = cos( phase );
        kx = ofMap(valueS, -1, 1, 0.45, 0.55);
        ky = ofMap(valueC, -1, 1, 0.25, 0.85);
        // Implements ofNoise as random LFO
        float phase1 = 0.5 * ofGetElapsedTimef();
        float value1 = ofNoise ( phase1, 59.56 );
        ksectors = ofMap(value1, -1, 1, 1, 100);
        //Random kangle
        float phase2 = 0.2 * ofGetElapsedTimef();
        float value2 = ofNoise ( phase2, 333.64 );
        kangle = ofMap(value2, -1, 1, -180, 180);
        
        // Add frequency spectrum to get a sound signal
//        float *spectrum = ofSoundGetSpectrum(128);
//        double level = 0;
//        for (int i=0; i<128; i++) {
//            level += spectrum[i] * spectrum[i];
//        }
//        level = sqrt( level / 128 );
//        level += soundLevel;
//        float newkangle = ofMap( level, 0, 1, 100, 200, true );
//        kangle = kangle + 0.1 * (newkangle-kangle);
    }
   
}

//--------------------------------------------------------------
void ofApp::stripePattern() {
    ofSetColor( color );
    ofSetLineWidth( 1 );
    if (filled) ofFill(); else ofNoFill();
    for ( int i =-countX; i<countX; i++) {
        ofPushMatrix();
        ofTranslate( i * stepX, 0 );
        ofRotate( i * twistX );
        
        ofTranslate( 0, shiftY );
        ofRotate( rotate );
        ofScale( size->x, size->y);
        if ( type ) ofDrawRectangle( -50, -50, 100, 100); else ofDrawTriangle( 0, 0, -50, 100, 50, 100 );

        ofPopMatrix();
    }
}


//--------------------------------------------------------------
void ofApp::matrixPattern() {
    for (int y=-countY; y<=countY; y++){
        ofPushMatrix();
        //-----------
        if ( countY > 0 ){
            float scl = ofMap(y, -countY, countY, 1-pinchY, 1 );
            ofScale( scl, scl );
        }
        ofTranslate(0, y * stepY);
        ofRotate( y * twistY );
        stripePattern();
        //--------------
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void ofApp::audioIn(float *input, int bufferSize, int Channels){
    double v = 0;
    for (int i=0; i<bufferSize; i++){
        v += input[i] * input[i];
    }
    v = sqrt( v / bufferSize );
    soundLevel = v;
}

//--------------------------------------------------------------
void ofApp::exit(){
    gui.saveToFile( "settings.xml" );
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.begin();
    draw2d();
    fbo.end();
    ofSetColor( 255 );
    if ( kenabled ) {
        shader.begin();
        shader.setUniform1i( "ksectors", ksectors );
        shader.setUniform1f( "kangleRad", ofDegToRad(kangle) );
        shader.setUniform2f( "kcenter", kx*ofGetWidth(), ky*ofGetHeight() );
        shader.setUniform2f( "screenCenter", 0.5*ofGetWidth(), 0.5*ofGetHeight() );
    }
    fbo.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    if ( kenabled ) shader.end();   
    if ( showGui ) gui.draw();
}

//--------------------------------------------------------------
void ofApp::draw2d(){
    ofBackground( Background );
    
    ofDisableSmoothing();
    ofEnableBlendMode( OF_BLENDMODE_ADD );
    ofSetColor( 255, imageAlpha );
    image.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    ofSetColor( 255, videoAlpha );
    video.draw(0, 0, ofGetWidth(), ofGetHeight());
    if ( camera.isInitialized() ) {
        ofSetColor( 255, cameraAlpha );
        camera.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    }
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    //--
    float Scl = pow( Scale, 4.0f);
    ofScale( Scl, Scl );
    ofRotate( Rotate );
    matrixPattern();
    //--
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if ( key == 'z' ) showGui = !showGui;
    if ( key == OF_KEY_RETURN ) ofSaveScreen( "screenshot" + ofToString(ofRandom(0, 100), 0 ) + ".png" );
    if ( key == 's' ) {
        ofFileDialogResult res;
        res = ofSystemSaveDialog("preset.xml", "Saving Preset" );
        if ( res.bSuccess )gui.saveToFile( res.filePath );
    }
    if ( key == 'l' ) {
        ofFileDialogResult res;
        res = ofSystemLoadDialog( "Loading Preset" );
        if( res.bSuccess )gui.loadFromFile( res.filePath );
    }
    if ( key == 'c' ) {
        camera.setDeviceID( 0 );
        camera.setDesiredFrameRate( 30 );
        camera.initGrabber( 1280, 720 );
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
