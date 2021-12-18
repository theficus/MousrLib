//
//  bleConstants.swift
//
//  Created by Adam Meltzer on 12/18/2021.
//

import Foundation
import CoreBluetooth

// Constants for BLE communication
struct bleConstants {
    // MARK: Mousr sevice UUID
    public static let UartServiceUuid = CBUUID(string: "6e400001-b5a3-f393-e0a9-e50e24dcca9e")

    // MARK: Mousr RX destination
    public static let UartWriteCharacteristic = CBUUID(string: "6e400002-b5a3-f393-e0a9-e50e24dcca9e")

    // MARK: Mousr TX destination
    public static let UartSubscribeCharacteristic = CBUUID(string: "6e400003-b5a3-f393-e0a9-e50e24dcca9e")
}
