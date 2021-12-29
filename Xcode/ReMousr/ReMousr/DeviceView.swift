//
//  ContentView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/01/2021.
//

import SwiftUI
import Foundation
import GameController

struct DeviceView : View {
    @EnvironmentObject var model : DeviceModel

    var body: some View {
        HStack {
            // Controls
            ControlsView()
                .frame(maxHeight: .infinity, alignment: .top)
                .padding(EdgeInsets(top: 20, leading: 0, bottom: 0, trailing: 0))

            Spacer()

            // Sensor display
            VStack {
                SensorView()
                    .frame(alignment: .top)
                    .background(Color(UIColor.systemGray6))
                    .padding(EdgeInsets(top: 30, leading: 0, bottom: 0, trailing: 0))
                Spacer()
            }
        }
        .padding(.horizontal, 10.0)
    }
}

struct DeviceView_Previews: PreviewProvider {
    static var model = DeviceModel()

    static var previews: some View {
        DeviceView()
            .previewInterfaceOrientation(.landscapeRight)
            .environmentObject(model)
    }
}
