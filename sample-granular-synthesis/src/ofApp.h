#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"

// type definition of window function to easily switch between options.
// https://en.wikipedia.org/wiki/Window_function
// available options in ofxMaxim - see maxiGrains.h for details:
// - hannWinFunctor
// - hammingWinFunctor
// - cosineWinFunctor
// - rectWinFunctor
// - triangleWinFunctor
// - triangleNZWinFunctor
// - blackmanHarrisWinFunctor
// - blackmanNutallWinFunctor
// - gaussianWinFunctor
typedef hannWinFunctor windowFunction;


class ofApp : public ofBaseApp {
    
    maxiSample m_sample,m_sample2;
    maxiTimePitchStretch<windowFunction, maxiSample> *m_sample_stretcher;
    maxiTimePitchStretch<windowFunction, maxiSample> *m_sample2_stretcher;
    maxiMix m_mixer;
    
    //low high pass filter
    maxiFFT m_fft;
    maxiFilter m_filter_lp;
    maxiFilter m_filter_hp;
    
   
    unsigned int m_sample_rate;
    unsigned int m_buffer_size;
    double m_mixer_output[2];
    double m_grain_rate;
    double m_freeze_grain_rate;
    double m_freeze_grain_length;
    double m_grain_length;
    unsigned int m_fft_size;
    
    ofxPanel m_gui;
    ofParameter<bool> m_param_enable_mouse;
    ofParameter<double> m_param_grain_speed;
    ofParameter<double> m_param_grain_rate;
    ofParameter<double> m_param_grain_length;
    ofParameter<int> m_param_grain_overlaps;
    ofParameter<bool> m_param_enable_resonance;
    ofParameterGroup m_group_nores;
    ofParameter<double> m_param_lp_cutoff;
    ofParameter<double> m_param_hp_cutoff;
    ofParameterGroup m_group_res;
    ofParameter<double> m_param_lpr_cutoff;
    ofParameter<double> m_param_hpr_cutoff;
    ofParameter<double> m_param_lpr_resonance;
    ofParameter<double> m_param_hpr_resonance;
    
    ofParameter<bool> m_param_sample2;
    
    bool textureDraw;
    bool freezeStretcher;
    
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    void audioOut(float * output, int bufferSize, int nChannels);
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    double audioData;
    bool soundopen;
    
    /* video stuff */
    ofVideoGrabber myCamera;
    ofTexture myTexture;
    ofPixels currentPix;
    ofPixels backgroundPix;
    int camWidth;
    int camHeight;
};
