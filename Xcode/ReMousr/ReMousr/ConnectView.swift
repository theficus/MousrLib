//
//  ConnectView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/03/2021.
//

import SwiftUI

struct ConnectView: View {
    let columns = [
        GridItem(.fixed(100.0)), // Name
        GridItem(.fixed(500.0)), // Property
    ]

    @EnvironmentObject var model : DeviceModel

    var body: some View {
        ZStack {
            VStack {
                Group {
                    if !self.$model.isError.wrappedValue {
                        if !self.$model.isDiscovered.wrappedValue &&
                            !self.$model.isConnected.wrappedValue {
                            // If not discovered
                            Image(systemName: "antenna.radiowaves.left.and.right")
                                .font(.system(size: 100))
                                .imageScale(.large)
                        } else if self.$model.isDiscovered.wrappedValue &&
                                    !self.$model.isConnected.wrappedValue {
                            // If discovered but not connected
                            Image(systemName: "antenna.radiowaves.left.and.right.circle")
                                .font(.system(size: 100))
                                .imageScale(.large)
                        } else {
                            // Unexpected state
                            Image(systemName: "exclamationmark.triangle.fill")
                                .font(.system(size: 100))
                                .foregroundColor(.yellow)
                                .imageScale(.large)
                        }
                    } else {
                        Image(systemName: "antenna.radiowaves.left.and.right.slash")
                            .resizable()
                            .foregroundColor(.red)
                    }
                }
                .frame(width: 150, height: 150)

                if !self.$model.isConnected.wrappedValue &&
                    !self.$model.isDiscovered.wrappedValue {
                    Text("Discovering...")
                } else if self.$model.isDiscovered.wrappedValue &&
                            !self.$model.isConnected.wrappedValue {
                    Button("Connect") {
                        // Connect
                        self.model.device.connect()
                    }
                    .buttonStyle(.borderedProminent)
                } else {
                    Button("WTF happened") {
                        self.model.onDisconnected()
                    }
                    .labelStyle(.titleAndIcon)
                }

                Spacer()

                if self.$model.isDiscovered.wrappedValue ||
                    self.$model.isConnected.wrappedValue {
                    List(model.deviceProperties) {
                        p in
                        DevicePropertyRow(prop: p)
                    }
                    .background(Color(UIColor.systemGray6))
                }

                Spacer()
            }
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            .zIndex(1)

            // Spinner
            if self.$model.isBusy.wrappedValue {
                ProgressView()
                    .scaleEffect(x: 4, y: 4)
                    .zIndex(999)
            }
        }
        .onAppear {
            if model.isConnected == false && model.isDiscovered == false {
                model.device.discover()
            }
        }
    }
}

struct ConnectView_Previews: PreviewProvider {
    static var device : DeviceModel = DeviceModel()
    static var previews: some View {
        ConnectView()
            .previewInterfaceOrientation(.portrait)
            .environmentObject(device)
    }
}
