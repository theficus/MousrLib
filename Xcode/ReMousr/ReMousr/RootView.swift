//
//  RootView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/04/2021.
//

import SwiftUI
import GameController
import Foundation
import MousrLib

struct RootView: View {
    private var controller = VirtualController(elements: [
        GCInputLeftThumbstick,
        GCInputButtonA,
        //GCInputDirectionalDpad,
        //GCInputDirectionalTouchSurfaceButton,
        GCInputRightThumbstick
         ])

    @State private var selectedTab = 0
    @State private var orientation = UIDevice.current.orientation
    @StateObject var model : DeviceModel = DeviceModel()

    var body: some View {
        VStack {
            HStack {
                RadioView()
                    .accessibilityLabel("radioStats")
                Spacer()
                BatteryView()
            }

            Rectangle()
                .frame(height: 0.5)

            TabView(selection: $selectedTab) {
                ConnectView()
                    .tabItem {
                        Label("Connect", systemImage: "antenna.radiowaves.left.and.right.circle")
                    }
                    .tag(1)

                //if self.$device.isConnected.wrappedValue {
                DeviceView()
                    .tabItem {
                        Label("Controls", systemImage: "gamecontroller")
                    }
                    .tag(2)

                LogView()
                    .tabItem {
                        Label("Log", systemImage: "doc.plaintext")
                    }
                    .tag(3)
            }
        }
        .onChange(of: self.model.isConnected) { newValue in
            print("onChange() for isConnected")
            if newValue == true {
                selectedTab = 2
            }
        }
        .onChange(of: self.selectedTab) { _ in
            print("onChange() for selectedTab")
            self.showController()
        }
        .onChange(of: self.model.showController) { _ in
            print("onChange() for showController")
            self.showController()
        }
        .onRotate { o in
            print("onRotate()")
            print("Got rotation event: \(o.rawValue)")
            self.model.log("Got rotation event \(o.rawValue)", .verbose)

            print("Selected tab: \(self.selectedTab)")
            self.orientation = o
            self.showController()
        }
        .onAppear {
            print("onAppear()")
            self.showController()
        }
        .alert(isPresented: self.$model.showingAlert) {
            Alert(title: Text("Error"),
                  message: Text(self.model.alertDetails))
        }
        .environmentObject(model)
    }

    func showController() {
        // Make sure this is always up to date in case the orientation event doesn't fire
        self.orientation = UIDevice.current.orientation

        print("showController() selectedTab=\(self.selectedTab) isLandscape=\(self.orientation.isLandscape) showController=\(self.model.showController)")
        if self.orientation.isLandscape &&
            self.selectedTab == 2 &&
            self.model.showController == true {
            print("Showing controller ...")
            controller.connect()
            
            controller.virtualController?.controller?.extendedGamepad?.leftThumbstick.valueChangedHandler = {
                (dpad, x, y) in

                let direction = dpad.getDirection()
                print(direction)

                if direction.zeroed {
                    self.model.device.stop()
                } else {
                    self.model.device.move(speed: direction.speed,
                                           held: 0,
                                           angle: direction.angle)
                }
            }

            controller.virtualController?.controller?.extendedGamepad?.rightThumbstick.valueChangedHandler = {
                (dpad, x, y) in
                let direction = dpad.getDirection()
                print(direction)
                if direction.zeroed {
                    self.model.device.stop()
                } else {
                    self.model.device.spin(angle: direction.angle)
                }
            }
        } else {
            print("Hiding controller ...")
            hideController()
        }
    }

    func hideController() {
        controller.disconnect()
    }
}

struct RootView_Previews: PreviewProvider {
    static var model : DeviceModel = DeviceModel()
    static var previews: some View {
        RootView()
            .environmentObject(model)
    }
}
