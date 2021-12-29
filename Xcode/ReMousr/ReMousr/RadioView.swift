//
//  RadioView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/09/2021.
//

import SwiftUI

struct RadioView: View {
    @EnvironmentObject var model : DeviceModel

    var body: some View {
        HStack {
            Image(systemName: model.isConnected ? "antenna.radiowaves.left.and.right" : "antenna.radiowaves.left.and.right.slash")
                .foregroundColor($model.isConnected.wrappedValue ? Color(UIColor.systemGreen) : Color(UIColor.systemRed))

            VStack {
                Image(systemName: "arrowtriangle.up.fill")
                    .foregroundColor(model.isSending ? .green : .gray)
                Text(String(describing: model.sentPkts))
                    .font(.system(size:8))
            }

            VStack {
                Image(systemName: "arrowtriangle.down.fill")
                    .foregroundColor(model.isReceiving ? .green : .gray)
                Text(String(describing: model.recvPkts))
                    .font(.system(size:8))
            }
        }
    }
}

struct RadioView_Previews: PreviewProvider {
    static var device = DeviceModel()
    static var previews: some View {
        RadioView()
            .environmentObject(device)
    }
}
