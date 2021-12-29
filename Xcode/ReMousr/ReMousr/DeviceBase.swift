//
//  DeviceBase.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/20/2021.
//

import Foundation
import MousrLib

protocol DeviceBase : Device {
    var isInitialized : Bool {
        get
        set
    }

    var isConnected : Bool {
        get
        set
    }
    
    // MARK: Device model
    var model : DeviceModel {
        get
    }
}

extension DeviceBase {
    func onConnected(_ error : Error?) {
        var d = self
        d.isConnected = true
        self.model.onConnected()
        DispatchQueue.global(qos: .background).async {
            while d.isConnected {
                d.bleMessageQueue.signal.wait()
                while d.bleMessageQueue.isEmpty() == false {
                    let msg = d.bleMessageQueue.dequeue()
                    _ = MousrMessaging.receiveMessage(d, msg)
                    d.onReceive(msg?.data, msg?.error)
                }
            }
        }
    }

    func onFirmwareVersionEvent(_ majorVersion: Byte, _ minorVersion: UInt16, _ commitNumber: UInt16, _ commitHash: UInt32, mousrVersion: Byte, _ hwVersion: UInt16, _ bootloaderVersion: UInt16) {
    }

    /*
     func onInitializeEvent() {
     }
     */

    func onReceive(_ data: Data?, _ error: Error?) {
        if data == nil {
            return
        }

        self.model.incrementRecv()
    }

    func onPositionEvent(_ speed : Float, _ hold : Float, _ angle : Float) {
        self.model.setPosition(speed, hold, angle)
    }

    func onBatteryEvent(_ batteryPct : UInt8) {
        self.model.setBattery(batteryPct)
    }

    func log(_ message: String, _ messageType: LogMessageType) {
        self.model.log(message, messageType)
    }

    func onError(_ data: Data?, error: String) {
        self.model.alertDetails = String("Error: \(error) Details: \(String(describing: data))")
        self.model.showingAlert = true
    }
}
