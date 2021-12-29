//
//  SyntheticDevice.swift
//
//  Created by Adam Meltzer on 12/18/2021.
//

import Foundation
import MousrLib

// Synthetic device to use for testing the communication channel
class SyntheticDevice : Device {
    func onReceive(_ data: Data?, _ error: Error?) {
    }

    var bleMessageQueue : BleMessageQueue
    var initializationComplete: Bool = true

    init() {
        self.bleMessageQueue = BleMessageQueue()
    }

    func onFirmwareVersionEvent(_ majorVersion: Byte, _ minorVersion: UInt16, _ commitNumber: UInt16, _ commitHash: UInt32, mousrVersion: Byte, _ hwVersion: UInt16, _ bootloaderVersion: UInt16) {
    }

    func onInitializeEvent() {
    }

    func onError(_ data: Data?, error: String) {
    }

    func onDiscovered(advertisementData: [String : Any], rssi RSSI: NSNumber) {
    }

    func onPositionEvent(_ angle: Float, _ hold: Float, _ speed: Float) {
    }

    func onBatteryEvent(_ batteryPct: UInt8) {
    }

    var sentData : Data?
    var sentCommand : mousrCommand?

    func onSend(_ data : Data, _ operation : mousrCommand) {

    }

    func onSend(_ data: Data?, _ error: Error?) {
        self.sentData = data
        self.sentCommand = mousrCommand.init(rawValue: data![14])
    }

    func onConnected(_ error: Error?) {
    }

    func onDisconnected() {
    }

    func send(_ data: Data) {
        self.onSend(data, nil)
    }

    func connect() {
    }

    func disconnect() {
    }

    func discover() {
    }

    func log(_ message: String, _ messageType: LogMessageType) {
    }
}
