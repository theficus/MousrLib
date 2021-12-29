//
//  HeadingView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/18/2021.
//

import SwiftUI

struct HeadingView: View {
    @EnvironmentObject var model : DeviceModel
    var body: some View {
        // Reset heading
        Button(action: {
            self.model.device.resetHeading()
        }, label: {
            CircleSliderView(outerSize: 50,
                             grabberSize: 50,
                             degrees: $model.angle)
        })
            .buttonStyle(ControlButtonStyle())
            .accessibilityHint("Reset Heading")
    }
}

struct HeadingView_Previews: PreviewProvider {
    static var model : DeviceModel = DeviceModel()
    static var previews: some View {
        HeadingView()
            .environmentObject(model)
    }
}
