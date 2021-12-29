//
//  BleMessage.swift
//  
//
//  Created by Adam Meltzer on 12/28/2021.
//

import Foundation
import CoreBluetooth

public struct BleMessage {
    public let characteristic : CBCharacteristic?
    public let data : Data?
    public let error : Error?

    public var messageType : mousrMessage {
        get {
            if data == nil ||
                data?.count == 0 ||
                data?.first == nil {
                return .none
            }

            let b = data!.first!
            return mousrMessage.init(rawValue: b) ?? .none
        }
    }

    public init(_ characteristic : CBCharacteristic?, _ data : Data?, _ error : Error?) {
        self.characteristic = characteristic
        self.data = data
        self.error = error
    }
}
