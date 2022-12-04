cmake_minimum_required(VERSION 3.18)

set(COMPANY_NAME "SAYAN AUDIO")
set(PROJECT_NAME "SYNDistortionModule")
set(BUNDLE_ID "com.sayan.audio")
set(PLUGIN_MANUFACTURER_CODE "SYNA")
set(PLUGIN_CODE "D1s7") 

set(EXPORT_NAME "${PROJECT_NAME}" CACHE STRING "Name of plugin")
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type of build. [Release]/Debug" FORCE)

set(EXPORT_FORMAT VST3)

project(${PROJECT_NAME} VERSION 0.0.1)

# Don't make Xcode schemes, not sure what this means check later
set(CMAKE_XCODE_GENERATE_SCHEME OFF)

set(CMAKE_OSX_ARCHITECTURES arm64 x86_64)

add_subdirectory(JUCE)

juce_add_plugin("${PROJECT_NAME}"
  PLUGIN_NAME ${EXPORT_NAME}
  COMPANY_NAME ${COMPANY_NAME}
  BUNDLE_ID ${BUNDLE_ID}
  COPY_PLUGIN_AFTER_BUILD TRUE
  PLUGIN_MANUFACTURER_CODE ${PLUGIN_MANUFACTURER_CODE}
  PLUGIN_CODE ${PLUGIN_CODE}
  FORMATS ${EXPORT_FORMAT}
  PRODUCT_NAME ${EXPORT_NAME}
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
)

set(SOURCE_FILES
  source/ParameterIds.h
  source/ParameterReferences.h
  source/ClipperBase.h
  source/NonInvertingOpAmpClipper.h
  source/PluginEditor.cpp
  source/PluginEditor.h
  source/PluginProcessor.cpp
  source/PluginProcessor.h
)

juce_generate_juce_header("${PROJECT_NAME}")

target_sources("${PROJECT_NAME}" PRIVATE ${SOURCE_FILES})

target_compile_definitions("${PROJECT_NAME}"
  PUBLIC
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries("${PROJECT_NAME}"
  PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)