//
//  MousrController.swift
//
//  Created by Adam Meltzer on 12/11/2021.
//

import Foundation

// Processes data from/to Mousr
public struct MousrMessaging {
    // MARK: Processes a data packet
    public static func receiveMessage(_ device: Device, _ data : Data) -> Bool {
        let command : mousrMessage = mousrMessage.init(rawValue: data.valueFromData(0)) ?? .unknown
        switch command {
        case .robotPose:
            let accel = getMovement(data)
            device.onPositionEvent(accel.speed, accel.hold, accel.angle)
            break
        case .batteryVoltage:
            let battery : Byte = data.valueFromData(1)
            device.onBatteryEvent(battery)
            break
        case .resetTailFailInfo:
            device.log("Unable to reset tail!", .error)
            break
        default:
            device.log("Unsupported command type: \(command) / \(command.rawValue) Data: 0x\(data.hexEncodedString())", .warning)
            return false
        }

        return true
    }

    // MARK: Gets movement coordinates from a data packet
    public static func getMovement(_ data : Data) -> (speed : Float, hold : Float, angle : Float) {
        var speed : Float = 0.0
        var hold : Float = 0.0
        var angle : Float = 0.0
        if (data.count >= 15) {
            speed = data.valueFromData(1)
            hold = data.valueFromData(5)
            angle = data.valueFromData(9)
        }

        return (speed, hold, angle)
    }

    // MARK: Gets the command type in a data packet
    public static func getCommand(_ data : Data) -> mousrCommand {
        if data.count < 15 {
            print("Invalid data with \(data.count) bytes.")
        }

        return mousrCommand(rawValue: data[13]) ?? .unknown
    }

    // MARK: Gets the message type in a data packet
    public static func getMessage(_ data : Data) -> mousrMessage {
        return mousrMessage(rawValue: data[0]) ?? .unknown
    }

    // MARK: Handles the minutiae of building the message packet
    static func sendCommand(_ device : Device, _ message : mousrMessage, _ command : mousrCommand, _ data : Data?) {
        var raw = Data()
        var requiredFillerBytes = 12
        // First two bytes: message
        raw += message.rawValue
        if data != nil {
            requiredFillerBytes -= data!.count
            raw += data!
        }

        // Fill in remaining bytes
        while (requiredFillerBytes > 0) {
            raw += Byte(0)
            requiredFillerBytes -= 1
        }

        raw += command.rawValue
        raw += Byte(0) // Command continuation byte. Always 0 in my observations
        device.send(raw)
    }
}
