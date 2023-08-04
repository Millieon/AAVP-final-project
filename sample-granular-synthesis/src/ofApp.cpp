#include "ofApp.h"


void ofApp::setup() {
    
    ofBackground(0);
    textureDraw=false;
    freezeStretcher=false;
    
    /* Video Setup */
    camWidth = 640;  // try to grab at this size.
    camHeight = 480;

    myCamera.setDeviceID(0);
    myCamera.setDesiredFrameRate(60);
    myCamera.initGrabber(camWidth, camHeight);

    backgroundPix.allocate(camWidth, camHeight, 3); //width, height, number of channels
    
    
    ofBackground(0);
    ofSetFrameRate(60);
    ofSetWindowTitle("Sample Granular Synthesis");
    
    // set audio sample rate to standard high resolution: 44.1kHz => 44100 samples per second
    m_sample_rate = 44100;
    
    // set number of samples per buffer
    m_buffer_size = 1024;
    
    // set initial values to sample stretcher
    m_grain_rate = 1.0;
    m_grain_length = 0.05;
    m_freeze_grain_rate = 1.0;
    m_freeze_grain_length = 0.05;
    
    // load sample data
    //m_sample.load(ofToDataPath("counting-mono.wav"));
    //m_sample.load(ofToDataPath("CaliforniaDreaming.wav"));
    //m_sample.load(ofToDataPath("BarbieGirl.wav"));//metal_on_metal.wav//80s_beat_90bpm.wav
    m_sample.load(ofToDataPath("90bpm.wav"));
    m_sample2.load(ofToDataPath("counting-mono.wav"));
    
    m_fft_size = 1024;
    m_fft.setup(m_fft_size, 512, 256);
    
    
    // setup pointer to sample stretcher
    m_sample_stretcher = new maxiTimePitchStretch<windowFunction, maxiSample>(&m_sample);
    m_sample2_stretcher = new maxiTimePitchStretch<windowFunction, maxiSample>(&m_sample2);
    
    
    // define global settings for ofxMaxim.
    maxiSettings::setup(m_sample_rate, 2, m_buffer_size);
    
    // sets up and starts a global ofSoundStream.
    ofSoundStreamSetup(2, 0, m_sample_rate, m_buffer_size, 4);
    
    // setup gui to toggle sample looping
    m_gui.setup("Audio Controls");
    m_gui.setPosition(10, 10);
    m_gui.add(m_param_enable_mouse.set("enable mouse", true));
    m_gui.add(m_param_grain_speed.set("grain speed", 1.0, 0.0, 2.0));
    m_gui.add(m_param_grain_rate.set("grain rate", 0.0, -1.5, 1.5));
    m_gui.add(m_param_grain_length.set("grain length", 0.05, 0.01, 0.1));
    m_gui.add(m_param_grain_overlaps.set("grain overlaps", 4, 1, 10));
    m_gui.add(m_param_enable_resonance.set("enable resonance", false));
    m_gui.add(m_param_sample2.set("sample 2", false));
    
    m_group_nores.setName("No Resonance Params");
    m_group_nores.add(m_param_lp_cutoff.set("low-pass cutoff", 1.0, 0.0, 1.0));
    m_group_nores.add(m_param_hp_cutoff.set("high-pass cutoff", 0.0, 0.0, 1.0));
    m_gui.add(m_group_nores);
    
    m_group_res.setName("Resonance Params");
    m_group_res.add(m_param_lpr_cutoff.set("low-pass cutoff", 10.0, 10.0, m_sample_rate));
    m_group_res.add(m_param_lpr_resonance.set("low-pass resonance", 1.0, 1.0, 10.0));
    m_group_res.add(m_param_hpr_cutoff.set("high-pass cutoff", 10.0, 10.0, m_sample_rate));
    m_group_res.add(m_param_hpr_resonance.set("high-pass resonance", 1.0, 1.0, 10.0));
    m_gui.add(m_group_res);
    
    
}


void ofApp::update() {
    
    myCamera.update();

    if(myCamera.isFrameNew())
    {
        currentPix = myCamera.getPixels();
        for(size_t i = 0; i < currentPix.size(); ++i)
        {
            //Simple
            //currentPix[i] = abs(currentPix[i] - backgroundPix[i])

            //Thresholded
            if (abs(currentPix[i] - backgroundPix[i]) < 64 * mouseX/ofGetWidth()) currentPix[i] = 0;
        }
        myTexture.allocate(currentPix);
    }
}


void ofApp::draw() {
    

    if(textureDraw)
    {myTexture.draw(0, 0,640*2,480*2);}
    // draw gui
    m_gui.draw();
    float inc_x = (float) ofGetWidth() / ((float) m_fft_size / 2.f);
    for (unsigned int i = 0; i < m_fft_size; ++i) {
        
        ofDrawRectangle(i * inc_x, ofGetHeight(), inc_x, -m_fft.magnitudesDB[i] * 10-200);
        
    }
    
}


void ofApp::exit() {
    
    // stops the global sound stream and also cleans up the stream's resources
    ofSoundStreamClose();
    
}


void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    
    // define variable to hold sample dataposmod
    double data,data2,data_clear;
    
    // loop over all samples in buffer
    for (unsigned int i = 0; i < bufferSize; ++i) {
        
        data_clear=m_sample.play();
        // read wave data from sample file wia sample stretcher
        if (m_param_enable_mouse) { // check if interactive mode is enabled
            
            if(!freezeStretcher)
            {
                m_freeze_grain_rate=m_grain_rate;
                m_freeze_grain_length=m_grain_length;
                data = m_sample_stretcher->play(1., m_grain_rate, m_grain_length, 4, 0.0);
                //data2 = m_sample2_stretcher->play(1., m_grain_rate, m_grain_length, 4, 0.0);
            }
            else{
                data = m_sample_stretcher->play(1., m_freeze_grain_rate, m_freeze_grain_length, 4, 0.0);
                //data2 = m_sample2_stretcher->play(1., m_freeze_grain_rate, m_freeze_grain_length, 4, 0.0);
            };//m_freeze_grain_rate keep unchanged
            
            
        } else {
            data = m_sample_stretcher->play(m_param_grain_speed, m_param_grain_rate, m_param_grain_length, m_param_grain_overlaps, 0.0);
            //data2 = m_sample2_stretcher->play(m_param_grain_speed, m_param_grain_rate, m_param_grain_length, m_param_grain_overlaps, 0.0);
        }
        
        if (m_param_enable_resonance) {
            
            data = m_filter_lp.lores(data, m_param_lpr_cutoff, m_param_lpr_resonance);
            data = m_filter_hp.hires(data, m_param_hpr_cutoff, m_param_hpr_resonance);
            
        } else {
            
            data = m_filter_lp.lopass(data, m_param_lp_cutoff);
            data = m_filter_hp.hipass(data, m_param_hp_cutoff);
        
        }
        
        if (m_fft.process(data)){
            m_fft.magsToDB();
        }
        
        // mix current sample to stereo channels.
        // panning of 0.5 sets sample output equally to both channels. 0.0 = left, 1.0 = right
        //m_mixer.stereo(data2, m_mixer_output, 0.5);
        if(m_param_sample2)
        {data2=m_sample2.play();}
        else{
            data2=0;
        }
        
        m_mixer.stereo(data+data2, m_mixer_output, 0.5);
        //m_mixer.stereo(data, m_mixer_output, 0.5);
        
        
        // write sample data to buffer
        output[i * nChannels] = m_mixer_output[0]; // left channel
        output[i * nChannels + 1] = m_mixer_output[1]; // right channel
        
    }
    audioData=data;
    std::cout<<data<<endl;
}


void ofApp::mouseMoved(int x, int y ) {
    
    // map grain rate to horizontal mouse movement
    m_grain_rate = ofMap(x, 0, ofGetWidth(), -1.5, 1.5);
    // map grain length to vertical mouse movement
    m_grain_length = ofMap(y, 0, ofGetHeight(), 0.01, 0.1);
    
}

void ofApp::keyPressed(int key){
    

    if(key==' ')
    {
        if(soundopen)
        {ofSoundStreamStop();
            soundopen=!soundopen;
        }
        else{
            ofSoundStreamStart();
            soundopen=!soundopen;
        }
        
    } else if(key=='d')
    {
        textureDraw=!textureDraw;
        
    } else if(key=='f')
    {
        freezeStretcher=!freezeStretcher;
        
    } else if(key == 's')
    {
        //Caputuring background pixels to subtract
        backgroundPix = myCamera.getPixels();
    }
    else if(key == '2')
    {
        if(m_param_sample2)
        {m_param_sample2=!m_param_sample2;}else{
            m_param_sample2=true;
        }
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------


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
