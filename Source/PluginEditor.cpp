/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    peakFreqSliderAttachment(audioProcessor.apvts, "Peak Freq", peakFreqSlider),
    peakGainSliderAttachment(audioProcessor.apvts, "Peak Gain", peakGainSlider),
    peakQualitySliderAttachment(audioProcessor.apvts, "Peak Quality", peakQualitySlider),
    lowCutFreqSliderAttachment(audioProcessor.apvts, "Low Cut Freq", lowCutFreqSlider),
    highCutFreqSliderAttachment(audioProcessor.apvts, "High Cut Freq", highCutFreqSlider),
    lowCutSlopeSliderAttachment(audioProcessor.apvts, "Low Cut Slope", lowCutSlopeSlider),
    highCutSlopeSliderAttachment(audioProcessor.apvts, "High Cut Slope", highCutSlopeSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    for (auto* comp: getComps())
    {
      addAndMakeVisible(comp);
    }

    setSize (600, 400);
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.333);

    auto w = responseArea.getWidth();

    auto& lowCut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highCut = monoChain.get<ChainPositions::HighCut>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> mags;

    mags.resize(w);

    for (int i = 0; i < w; ++i)
    {
      double mag = 1.f;
      auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

      if (!monoChain.isBypassed<ChainPositions::Peak>())
      {
        mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);
      }
      if (!lowCut.isBypassed<0>())
      {
        mag *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      }
      if (!lowCut.isBypassed<1>())
      {
        mag *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      }
      if (!lowCut.isBypassed<2>())
      {
        mag *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      }
      if (!lowCut.isBypassed<3>())
      {
        mag *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      }

      if (!highCut.isBypassed<0>())
      {
        mag *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      }
      if (!highCut.isBypassed<1>())
      {
        mag *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      }
      if (!highCut.isBypassed<2>())
      {
        mag *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      }
      if (!highCut.isBypassed<3>())
      {
        mag *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
      }

      mags[i] = Decibels::gainToDecibels(mag);
    }

    Path responseCurve;

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
      return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };

    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

    for (int i = 1; i < mags.size(); ++i)
    {
      responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }

    g.setColour(Colours::orange);
    g.drawRoundedRectangle(responseArea.toFloat(), 4.f, 1.f);

    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));


}

void SimpleEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.333);

    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.333);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
    
    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);
    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.333));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);

}

void SimpleEQAudioProcessorEditor::parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
{
  parametersChanged.set(true);
}

void SimpleEQAudioProcessorEditor::timerCallback()
{
  if (parametersChanged.compareAndSetBool(false, true))
  {
    //update the mono chain
    //signal a repaint

  }
}

std::vector<juce::Component*> SimpleEQAudioProcessorEditor::getComps()
{
  return
  {
    &peakFreqSlider,
    &peakGainSlider,
    &peakQualitySlider,
    &lowCutFreqSlider,
    &highCutFreqSlider,
    &lowCutSlopeSlider,
    &highCutSlopeSlider
  };
}