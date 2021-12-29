//
//  ControlsView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/08/2021.
//

import SwiftUI
import WrappingHStack
import MousrLib

struct ControlsView: View {
    @EnvironmentObject var model : DeviceModel
    
    @State private var volumeText : String = "#value#"
    @State private var soundImage : String = "questionmark"
    @State private var tailText : String = "#value#"
    @State private var tailImage : String = "questionmark"
    @State private var isShowingPopover : Bool = false
    
    var tailMenu : some View {
        VStack {
            //Picker("Tail mode", selection: self.$model.tailMode) {
            Button(action: {
                self.model.setTailMode(tailMode: .flick,
                                       tailText: &tailText,
                                       imageName: &tailImage)
            }) {
                Label("Flick", systemImage: "XXX")
            }.id(mousrTailMode.flick)
            
            Button(action: {
                self.model.setTailMode(tailMode: .chase,
                                       tailText: &tailText,
                                       imageName: &tailImage)
            }) {
                Label("Chase", systemImage: "XXX")
            }.id(mousrTailMode.chase)
            
            Button(action: {
                self.model.setTailMode(tailMode: .bounce,
                                       tailText: &tailText,
                                       imageName: &tailImage)
            }) {
                Label("Bounce", systemImage: "XXX")
            }.id(mousrTailMode.bounce)
            //}
        }
    }
    
    var soundMenu : some View {
        Group {
            Section {
                Button(action: {
                    self.model.setVolume(volume: 0,
                                         volumeText: &self.volumeText,
                                         imageName: &self.soundImage)
                }) {
                    Label("Off", systemImage: "speaker.slash")
                }
                
                Button(action: {
                    self.model.setVolume(volume: 33,
                                         volumeText: &self.volumeText,
                                         imageName: &self.soundImage)
                }) {
                    Label("Low", systemImage: "speaker.wave.1")
                }
                
                Button(action: {
                    self.model.setVolume(volume: 66,
                                         volumeText: &self.volumeText,
                                         imageName: &self.soundImage)
                }) {
                    Label("Mid", systemImage: "speaker.wave.2")
                }
                
                Button(action: {
                    self.model.setVolume(volume: 100,
                                         volumeText: &self.volumeText,
                                         imageName: &self.soundImage)
                }) {
                    Label("High", systemImage: "speaker.wave.3")
                }
            }
            
            Section {
                Button(action: {
                }) {
                    Label("Sound board", systemImage: "headphones")
                }
            }
        }
    }
    
    var body: some View {
        WrappingHStack {
            // Emergency stop
            Button(action: {
                self.model.device.stop()
            }) {
                Image(systemName: "exclamationmark.octagon.fill")
                    .foregroundColor(.red)
            }
            .buttonStyle(ControlButtonStyle())
            .accessibilityHint("Stop")
            
            // Flip/Reverse
            Button(action: {
                self.model.device.stop()
            }) {
                Image(systemName: self.model.isFlipped ? "arrowshape.zigzag.forward.fill" : "arrow.down.to.line")
                    .foregroundColor(self.model.isFlipped ? .orange : .blue)
            }
            .buttonStyle(ControlButtonStyle())
            .simultaneousGesture(
                LongPressGesture(minimumDuration: 0.0)
                    .onEnded { v in
                        self.model.device.reverse()
                        print("longPress onEnded \(v)")
                    }
            )
            // Settings
            Button(action: {
                self.isShowingPopover = true
            }) {
                Image(systemName: "gear")
                    .foregroundColor(Color(UIColor.systemGray))
            }
            .buttonStyle(ControlButtonStyle())
            .popover(isPresented: $isShowingPopover) {
                SettingsView()
            }
            
            HeadingView()
                .environmentObject(model)
            
            // Flick tail
            Button(action: {
                self.model.device.flick()
            }) {
                VStack {
                    Image(systemName: self.tailImage)
                        .foregroundColor(.purple)
                    Spacer()
                    Text(self.tailText)
                        .font(.system(size: 8))
                }
            }
            .buttonStyle(ControlButtonStyle())
            .contextMenu {
                tailMenu
            }
            
            // Chirp
            Button(action: {
                self.model.device.chirp()
            }) {
                VStack {
                    Image(systemName: self.soundImage)
                        .foregroundColor(.cyan)
                    Spacer()
                    Text(self.volumeText)
                        .font(.system(size: 8))
                }
            }
            .buttonStyle(ControlButtonStyle())
            .contextMenu {
                soundMenu
            }
            
            // Show/hide controller
            Button(action: {
                self.model.showController = !self.model.showController
            }) {
                Image(systemName:
                        self.model.showController ? "gamecontroller.fill" : "gamecontroller")
                    .foregroundColor(Color(UIColor.systemBackground))
            }
            .buttonStyle(ControlButtonStyle())
            
            // Debug dump
            Button(action: {
                self.model.device.debugDump()
            }) {
                Image(systemName: "ladybug.fill")
                    .foregroundColor(.brown)
            }
            .buttonStyle(ControlButtonStyle())
        }
        .onAppear {
            var v = self.model.getVolumeText()
            self.volumeText = v.text
            self.soundImage = v.imageName
            
            v = self.model.getTailModeText()
            self.tailText = v.text
            self.tailImage = v.imageName
        }
    }
}

struct ControlsView_Previews: PreviewProvider {
    static var model : DeviceModel = DeviceModel()
    static var previews: some View {
        ControlsView()
            .environmentObject(model)
            .previewInterfaceOrientation(.portrait)
    }
}
