//
//  DeviceProperty.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/10/2021.
//

import Foundation
import SwiftUI

struct DeviceProperty : Identifiable {
    let id = UUID()
    var key : String
    var value : Any
    var stringValue : String

    init(_ key: String, _ value: Any) {
        self.key = key
        self.value = value
        self.stringValue = String(describing: value)
    }
}

struct DevicePropertyRow : View {
    var prop : DeviceProperty

    var body : some View {
        HStack {
            // Key
            Image(systemName: "circle")
            Text(prop.key)
                .frame(width: 100)

            // Value
            Text(prop.stringValue)
                .frame(maxWidth: .infinity, alignment: .leading)
        }
    }
}

struct DevicePropertyRow_Previews : PreviewProvider {
    static var previews: some View {
        DevicePropertyRow(prop: DeviceProperty("myKey", UUID()))
    }
}
