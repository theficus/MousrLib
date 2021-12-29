//
//  mousrMessage.swift
//
//  Created by Adam Meltzer on 12/18/2021.
//

import Foundation

public enum mousrMessage : Byte {
    case autoAck = 0xf
    case firmwareVersion = 0x1c
    case hardwareVersion = 0x1d
    case initDone = 0x1e
    case robotPose = 0x30
    case resetTailFailInfo = 0x32
    case stuckTofMsg = 0x40
    case crashLogFinish = 0x5f
    case crashLogString = 0x60
    case debugInfo = 0x61
    case batteryVoltage = 0x62
    case robotStopped = 0x63
    case rcStuck = 0x64
    case nack = 0xff
    case none = 0x0
}

public extension Data {
    func getMousrMessageKind() -> mousrMessage {
        if self.count == 0 {
            return .none
        }

        // First byte will always contain the message kind
        return mousrMessage.init(rawValue: self[0]) ?? .none
    }

    static func createMousrMessage(_ messageType : mousrMessage) -> Data {
        var d : Data = Data()
        d += messageType.rawValue
        return d
    }
}
