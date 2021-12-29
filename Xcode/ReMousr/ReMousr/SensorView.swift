//
//  SensorView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/08/2021.
//

import SwiftUI

struct SensorView: View {
    @EnvironmentObject var model : DeviceModel

    var body: some View {
        VStack {
            // Tilt
            HStack {
                Image(systemName: "arrow.right")
                    .frame(width: 30, height: 30)
                    .rotationEffect(.degrees($model.hold.wrappedValue))

                if $model.isFlipped.wrappedValue == true {
                    Image(systemName: "exclamationmark.triangle.fill")
                        .foregroundColor(.red)
                        .font(.system(size: 30))
                } else {
                    Image(systemName: "checkmark")
                        .foregroundColor(.green)
                        .font(.system(size: 30))
                }
            }
            .frame(width: 70, height: 50)

            HStack {
                Text("tilt")
                    .font(.system(size:8))
                Text(String(format: "%.1f", $model.hold.wrappedValue))
                    .font(.system(size:8))
            }

            // Acceleration
            Slider(value: $model.speed, in: -100...100)
                .frame(width: 100)
                .allowsHitTesting(false)

            HStack {
                Text("speed")
                    .font(.system(size:8))
                Text(String(format: "%.1f", $model.speed.wrappedValue))
                    .font(.system(size:8))
            }

            Text("Movement degrees: \(model.controllerAngle)")
        }
    }
}

struct SensorView_Previews: PreviewProvider {
    static var model = DeviceModel()
    static var previews: some View {
        SensorView()
            .environmentObject(model)
    }
}
