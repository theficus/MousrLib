//
//  MousrController.swift
//
//  Created by Adam Meltzer on 12/11/2021.
//

import Foundation

// Processes data from/to Mousr
public struct MousrMessaging {
    public static let mbApiVer : UInt32 = 3 // Mousr API version

    // MARK: Processes a data packet
    public static func receiveMessage(_ device: Device, _ message : BleMessage?) -> Bool {
        if message == nil {
            return false
        }

        let command : mousrMessage = message!.messageType
        if message!.data == nil {
            return false
        }

        switch command {
        case .robotPose:
            let accel = getMovement(message!.data!)
            device.onPositionEvent(accel.speed, accel.hold, accel.angle)
            break
            /*
        case .firmwareVersion:
            let majorVersion : Byte = data.valueFromData(3)
            let minorVersion : UInt16 = data.valueFromData(4)
            let commitNumber : UInt16 = data.valueFromData(6)
            let commitHash : UInt32 = data.valueFromData(8)
            let mousrVersion : Byte = data.valueFromData(13)
            let hwVersion : UInt16 = data.valueFromData(14)
            let bootloaderVersion : UInt16 = data.valueFromData(17)
            device.onFirmwareVersionEvent(majorVersion, minorVersion, commitNumber, commitHash, mousrVersion: mousrVersion, hwVersion, bootloaderVersion)
            break
             */
        case .batteryVoltage:
            let battery : Byte = message!.data!.valueFromData(1)
            device.onBatteryEvent(battery)
            break
            /*
        case .resetTailFailInfo:
            device.onError(data, error: "Unable to reset tail")
            break
        case .nack:
            device.onError(data, error: "Mousr refused command: \(String(describing: mousrCommand.init(rawValue: data[1])))")
            break
             */
        default:
            device.log("Unsupported command type: \(command) / \(command.rawValue) Data: 0x\(message!.data!.hexEncodedString())", .warning)
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

    public static func createCommand(_ message : mousrMessage, _ command : mousrCommand, _ data : Data? = nil) -> Data {
        var raw = createMessage(message, data, 15)
        raw[13] = command.rawValue
        return raw
    }

    public static func createInitializeCommand() -> Data {
        return createInitializeCommand(date: Date.now)
    }

    public static func createInitializeCommand(date : Date) -> Data {
        var d : Data = Data()
        d += MousrMessaging.mbApiVer // Only know about version 3 so we'll just stick with that.

        // Mousr's initialize command apparently only needs the current time in ticks and doesn't care about the date. This logic is basically copy/pasted
        // from the disassembled Mousr app code.
        let time : Int32 = Int32(Double(((Date.ticks(date) & 0x3fffffffffffffff) - (Date.ticksAt1970() & 0x3fffffffffffffff))) * 1e-07)
        d += time
        return createCommand(.robotPose, .initializeDevice, d)
    }

    public static func createMessage(_ message : mousrMessage, _ data : Data? = nil) -> Data {
        return createMessage(message, data, 20)
    }

    // MARK: Handles the minutiae of building the message packet
    static func createMessage(_ message : mousrMessage, _ data : Data? = nil, _ length : Int) -> Data {
        var raw = Data.createMousrMessage(message)
        var requiredFillerBytes : Int = length - 1
        if data != nil {
            requiredFillerBytes -= data!.count
            raw += data!
        }

        // Fill in remaining bytes
        while (requiredFillerBytes > 0) {
            raw += Byte(0)
            requiredFillerBytes -= 1
        }

        return raw
    }

    static func sendCommand(_ device : Device, _ message : mousrMessage, _ command : mousrCommand, _ data : Data?) {
        device.send(createCommand(message, command, data))
    }
}
