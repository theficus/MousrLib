//
//  BatteryView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/08/2021.
//

import SwiftUI

struct BatteryView: View {
    @EnvironmentObject var model : DeviceModel
    
    var body: some View {
        ProgressView("Battery",
                     value: model.batteryPct,
                     total: 100.0)
            .accentColor(
                    model.batteryPct > 50 ? Color.green :
                    model.batteryPct > 25 ? Color.yellow :
                    Color.red)
            .font(.system(size: 8))
            .frame(width: 30)
    }
}

struct BatteryView_Previews: PreviewProvider {
    static var model : DeviceModel = DeviceModel()
    static var previews: some View {
        BatteryView()
            .environmentObject(model)
    }
}
