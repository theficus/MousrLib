//
//  SettingsView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/04/2021.
//

import SwiftUI

struct SettingsView: View {
    @EnvironmentObject var model : DeviceModel
    @Environment(\.presentationMode) var presentation

    var body: some View {
        VStack {
            ScrollView {
                VStack {
                    HStack {
                        Text("Max Speed")
                            .frame(width: 150, alignment: .leading)
                        VStack {
                            Slider(value: $model.maxSpeed, in: 0...100)
                            Text(String(format: "%.f%%", $model.maxSpeed.wrappedValue))
                                .font(.system(size: 10))
                        }
                    }
                    HStack {
                        Text("Volume")
                            .frame(width: 150, alignment: .leading)
                        VStack {
                            Slider(value: $model.volume, in: 0...100)
                            Text(String(format: "%.f%%", $model.volume.wrappedValue))
                                .font(.system(size: 10))
                        }
                    }
                }
            }

            Spacer()
            
            Button("Dismiss") {
                self.presentation.wrappedValue.dismiss()
            }.buttonStyle(.borderedProminent)
        }
    }
}

struct SettingsView_Previews: PreviewProvider {
    static var model : DeviceModel = DeviceModel()
    static var previews: some View {
        SettingsView()
            .environmentObject(model)
    }
}
