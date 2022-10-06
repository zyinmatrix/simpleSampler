/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleSamplerAudioProcessorEditor::SimpleSamplerAudioProcessorEditor (SimpleSamplerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    //Attack Slider
    mAttackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mAttackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    mAttackSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::purple);
    mAttackSlider.setRange(0.0f, 2.0f, 0.01f);
    mAttackSlider.addListener(this); //add listener
    addAndMakeVisible(mAttackSlider);
    //set label
    mAttackLabel.setFont(10.0f);
    mAttackLabel.setText("Attack", juce::NotificationType::dontSendNotification);
    mAttackLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::yellow);
    mAttackLabel.setJustificationType(juce::Justification::centredTop);
    mAttackLabel.attachToComponent(&mAttackSlider, false);

    //Decay Slider
    mDecaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDecaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    mDecaySlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::purple);
    mDecaySlider.setRange(0.0f, 2.0f, 0.01f);
    mDecaySlider.addListener(this);
    addAndMakeVisible(mDecaySlider);
    
    mDecayLabel.setFont(10.0f);
    mDecayLabel.setText("Decay", juce::NotificationType::dontSendNotification);
    mDecayLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::yellow);
    mDecayLabel.setJustificationType(juce::Justification::centredTop);
    mDecayLabel.attachToComponent(&mDecaySlider, false);

    //Sustain Slider
    mSustainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mSustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    mSustainSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::purple);
    mSustainSlider.setRange(0.0f, 1.0f, 0.01f);
    mSustainSlider.addListener(this);
    addAndMakeVisible(mSustainSlider);
    
    mSustainLabel.setFont(10.0f);
    mSustainLabel.setText("Sustain", juce::NotificationType::dontSendNotification);
    mSustainLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::yellow);
    mSustainLabel.setJustificationType(juce::Justification::centredTop);
    mSustainLabel.attachToComponent(&mSustainSlider, false);
    
    //Release Slider
    mReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mReleaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    mReleaseSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::purple);
    mReleaseSlider.setRange(0.0f, 2.0f, 0.01f);
    mReleaseSlider.addListener(this);
    addAndMakeVisible(mReleaseSlider);
    
    mReleaseLabel.setFont(10.0f);
    mReleaseLabel.setText("Release", juce::NotificationType::dontSendNotification);
    mReleaseLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::yellow);
    mReleaseLabel.setJustificationType(juce::Justification::centredTop);
    mReleaseLabel.attachToComponent(&mReleaseSlider, false);
    
    mAttackSlider.setValue(0.0);
    mDecaySlider.setValue(0.0);
    mSustainSlider.setValue(0.0);
    mReleaseSlider.setValue(0.0);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 450);
}

SimpleSamplerAudioProcessorEditor::~SimpleSamplerAudioProcessorEditor()
{
}

//==============================================================================
void SimpleSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (juce::Colours::cadetblue.darker());
    g.fillAll (juce::Colour(51,153, 102));
    //get original waveform
    auto waveform = audioProcessor.getWaveForm();
    //draw waveform on screen
    if (waveform.getNumSamples() > 0)
    {
        juce::Path p;
        p.clear();
        mAudioPoints.clear(); //clear previous audio points
        //find ratio: sample = 44100 (1 sec)... x axis of window = 600 sampleLength / getwidth()
        auto ratio = waveform.getNumSamples() / getWidth(); //calculate the ratio for resizing
        auto buffer = waveform.getReadPointer(0);
        //scale audio file to window on x axis
        for (int sample = 0; sample < waveform.getNumSamples(); sample += ratio)
        {
            //store audio data to AudioPoints every other "(int)ratio" samples
            mAudioPoints.push_back (buffer[sample]);
        }
        g.setColour(juce::Colours::yellow); // set colour
        p.startNewSubPath(0, getHeight() / 2); //set start point
        
        //scale on y axis
        for (int sample = 0; sample <mAudioPoints.size(); ++sample)
        {
            //recaling the amplitude to pixels
            auto point = juce::jmap<float> (mAudioPoints[sample], -1, 1, getHeight()/2+150, getHeight()/2-150);
            p.lineTo (sample, point);
        }
        //draw recaled waveform
        g.strokePath(p, juce::PathStrokeType(2));
        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
        auto textbounds = getLocalBounds().reduced(10, 10);
        g.drawFittedText(mFileName, textbounds, juce::Justification::topRight, 1); // print the file name
    }
    else
    {
        g.setColour(juce::Colours::white);
        g.setFont(40.0f);
        g.drawFittedText("Drop an Audio File to Load", getLocalBounds(), juce::Justification::centred, 1);
    }
}

void SimpleSamplerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    const auto startX = 0.6f;
    const auto startY = 0.75f;
    const auto dialWidth = 0.1f;
    const auto dialHeight = 0.2f;
    
    //set the position for sliders
    mAttackSlider.setBoundsRelative(startX, startY, dialWidth , dialHeight);
    mDecaySlider.setBoundsRelative(startX + dialWidth, startY, dialWidth, dialHeight);
    mSustainSlider.setBoundsRelative(startX + dialWidth * 2, startY, dialWidth, dialHeight);
    mReleaseSlider.setBoundsRelative(startX + dialWidth * 3, startY, dialWidth, dialHeight);
    
}

bool SimpleSamplerAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray &files){
    
    //check audio file format name
    for (auto file : files)
    {
        if(file.contains(".wav") || file.contains(".mp3"))
        {
            return true;
        }
    }
    return false;

}

void SimpleSamplerAudioProcessorEditor::filesDropped (const juce::StringArray &files, int x, int y){
    
    for (auto file : files)
    {
        if (isInterestedInFileDrag (file))
        {
            auto myFile = std::make_unique<juce::File>(file);
            mFileName = myFile->getFileNameWithoutExtension();
            
            //load this file
            audioProcessor .loadFile(file);
            //set ADSR parameters according to initial value of sliders
            audioProcessor.getADSRParams().attack = mAttackSlider.getValue();
            audioProcessor.getADSRParams().decay = mDecaySlider.getValue();
            audioProcessor.getADSRParams().sustain = mSustainSlider.getValue();
            audioProcessor.getADSRParams().release = mReleaseSlider.getValue();
            audioProcessor.updateADSR();
        }
    }
    repaint();
}

void SimpleSamplerAudioProcessorEditor::sliderValueChanged(juce::Slider *slider){
    //reset ADSR Parameters when user changes slider values
    if (slider == &mAttackSlider){
        audioProcessor.getADSRParams().attack = mAttackSlider.getValue();
    }else if(slider == &mDecaySlider){
        audioProcessor.getADSRParams().decay = mDecaySlider.getValue();
    }else if(slider == &mSustainSlider){
        audioProcessor.getADSRParams().sustain = mSustainSlider.getValue();
    }else if(slider == &mReleaseSlider){
        audioProcessor.getADSRParams().release = mReleaseSlider.getValue();
    }
    //update ADSR parameters from sliders
    audioProcessor.updateADSR();
}
