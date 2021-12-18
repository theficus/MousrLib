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
    case unknown = 0x0
}
