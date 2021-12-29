//
//  MousrDevice.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/10/2021.
//

import Foundation
import SwiftUI
import MousrLib

// MARK: Represents a real Mousr device
class MousrDevice : DeviceBase {
    var isConnected: Bool = false
    var isInitialized: Bool = false
    var bleMessageQueue: BleMessageQueue
    var model: DeviceModel
    var _ble : MousrBluetooth? = nil

    private var ble : MousrBluetooth {
        get {
            return self._ble!
        }
    }

    init(_ model : DeviceModel) {
        self.model = model
        self.bleMessageQueue = BleMessageQueue()
        self._ble = MousrBluetooth(self)
        self.log("initializing MousrDevice", .verbose)
    }

    func onDisconnected() {
    }

    func appendUIDeviceProperty(_ key : String, _ value : Any) {
        self.model.deviceProperties.append(DeviceProperty(key, value))
    }

    func discover() {
        self.model.isBusy = true
        self.ble.discover()
    }

    func send(_ data: Data) {
        let operation = MousrMessaging.getCommand(data)
        if operation == .move {
            let accel = MousrMessaging.getMovement(data)
            self.model.controllerAngle = CGFloat(accel.angle)
        }

        self.ble.sendData(data)
        self.model.incrementSend()
    }

    func onSend(_ data: Data?, _ error: Error?) {
    }

    func connect() {
        if self.model.isDiscovered == false {
            return
        }

        self.model.isBusy = true
        self.bleMessageQueue.begin()
        self.ble.connectPeripheral()
    }

    func disconnect() {
    }

    func onDiscovered(advertisementData: [String : Any], rssi RSSI: NSNumber) {
        self.model.isDiscovered = true
        let realName : NSString = advertisementData["kCBAdvDataLocalName"] as! NSString
        let deviceId : NSData = advertisementData["kCBAdvDataManufacturerData"] as! NSData

        self.appendUIDeviceProperty("Name", String(realName))
        self.appendUIDeviceProperty("RSSI", RSSI)
        self.appendUIDeviceProperty("ID", Data(deviceId).hexEncodedString())
        
        self.model.isBusy = false
    }

    func onConnected() {
        self.model.onConnected()
    }
}
