//
//  Device.swift
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
    func onDiscovered(advertisementData: [String : Any], rssi RSSI: NSNumber)
    func log(_ message : String, _ messageType : LogMessageType)
}

// Wraps messages
public extension Device {
    // MARK: Initiate debug dump
    func debugDump() {
        MousrMessaging.sendCommand(self, .robotPose, .getDebugLog, nil)
    }

    // MARK: Full stop
    func stop() {
        MousrMessaging.sendCommand(self, .robotPose, .stop, nil)
    }

    func setTailMode(_ tailMode : mousrTailMode) {
        var d = Data()
        d += tailMode.rawValue
        MousrMessaging.sendCommand(self, .robotPose, .flickSignal, d)
    }

    // MARK: Configure driver assist
    func setDriverAssist(_ settings : mousrDriverAssistConfiguration) {
        var d = Data()
        d += settings.rawValue
        MousrMessaging.sendCommand(self, .robotPose, .configDriverAssist, d)
    }

    func setVolume(volume : UInt8) {
        if (volume > 100) {
            self.log("Invalid volume level: \(volume)", .error)
        }

        var d = Data()
        d += volume
        MousrMessaging.sendCommand(self, .robotPose, .soundVolume, d)
    }

    func resetHeading() {
        MousrMessaging.sendCommand(self, .robotPose, .resetHeadding, nil)
    }

    func chirp() {
        chirp(fx: 6 /* same thing the app does */)
    }

    func chirp(fx : Byte) {
        var d = Data()
        d += Byte(0)
        d += Byte(fx)
        MousrMessaging.sendCommand(self, .robotPose, .chirp, d)
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
