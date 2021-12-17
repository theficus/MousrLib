//
//  Device.swift
//  IOSSwiftUIPlayground
//
//  Created by Adam Meltzer on 12/11/2021.
//

import Foundation

// Represents the stubs for a device
public protocol Device {
    func send(_ data : Data)
    func onSend(_ data : Data?, _ error : Error?)
    func onReceive(_ data : Data?, _ error : Error?)
    func onPositionEvent(_ speed : Float, _ hold : Float, _ angle : Float)
    func onBatteryEvent(_ batteryPct : UInt8)
    func connect()
    func onConnected(_ error : Error?)
    func disconnect()
    func onDisconnected()
    func discover()
    func log(_ message : String, _ messageType : LogMessageType)
}

// Wraps messages
public extension Device {
    func stop() {
        MousrMessaging.sendCommand(self, .robotPose, .stop, nil)
    }

    func resetHeading() {
        MousrMessaging.sendCommand(self, .robotPose, .resetHeadding, nil)
    }

    func chirp() {
        MousrMessaging.sendCommand(self, .robotPose, .chirp, nil)
    }

    func initialize() {
        MousrMessaging.sendCommand(self, .robotPose, .initializeDevice, nil)
        MousrMessaging.sendCommand(self, .robotPose, .tailCalibrationSignal, nil)
    }

    func reverse() {
        MousrMessaging.sendCommand(self, .robotPose, .reverseSignal, nil)
    }

    func spin(angle : Float) {
        var d = Data()
        d += Float(0.0)
        d += Float(0.0)
        d += angle
        MousrMessaging.sendCommand(self, .robotPose, .spin, d)
    }

    func move(speed : Float, hold : Float, angle : Float) {
        var d = Data()
        d += speed
        d += hold
        d += angle
        MousrMessaging.sendCommand(self, .robotPose, .move, d)
    }

    func flick() {
        MousrMessaging.sendCommand(self, .robotPose, .flickSignal, nil)
    }
}
