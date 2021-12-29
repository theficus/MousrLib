//
//  DummyDevice.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/10/2021.
//

import Foundation
import SwiftUI
import MousrLib

// MARK: Simulated device for validation purposes
class DummyDevice : DeviceBase {
    var isConnected: Bool = false
    var isInitialized: Bool = false
    var bleMessageQueue: BleMessageQueue
    var model: DeviceModel
    
    private var isSimulationRunning = false

    init(_ model : DeviceModel) {
        self.model = model
        self.bleMessageQueue = BleMessageQueue()
        model.deviceProperties.append(DeviceProperty("name", "mydevicename"))
        model.deviceProperties.append(DeviceProperty("uuid", UUID()))
        model.deviceProperties.append(DeviceProperty("rssi", -82))
    }

    func discover() {
        model.isBusy = true

        DispatchQueue.global(qos: .background).async {
            //Thread.sleep(forTimeInterval: 2)
            DispatchQueue.main.async {
                self.model.isBusy = false
                self.model.isDiscovered = true
                self.model.isConnected = false
            }
        }
    }

    func onDiscovered(advertisementData: [String : Any], rssi RSSI: NSNumber) {
    }

    func startSimulation() {
        if (isSimulationRunning) {
            return
        }

        model.log("Starting simulation...", .verbose)

        DispatchQueue.global(qos: .utility).async {
            self.isSimulationRunning = true

            var holdDirection = 1
            var angleDirection = 1
            var speedDirection = 1
            while self.isSimulationRunning {
                var newSpeed, newAngle, newHold : Float
                var newBatteryPct : Float
                var newIsFlipped : Bool

                newSpeed = Float(self.model.speed)
                if(newSpeed > 100) {
                    speedDirection = 0
                    newSpeed = 100
                } else if (newSpeed < -100) {
                    newSpeed = -100
                    speedDirection = 1
                }

                if(speedDirection == 1) {
                    newSpeed += 5
                } else {
                    newSpeed -= 5
                }

                newBatteryPct = self.model.batteryPct
                if(newBatteryPct <= 1) {
                    newBatteryPct = 100
                } else {
                    newBatteryPct -= 5
                }

                newAngle = Float(self.model.angle)
                if (newAngle > 180) {
                    angleDirection = 0
                    newAngle = 180
                } else if (newAngle < -180) {
                    angleDirection = 1
                    newAngle = -180
                }

                if (angleDirection == 1) {
                    newAngle += 5
                } else {
                    newAngle -= 5
                }

                newHold = Float(self.model.hold)
                if (newHold > 90) {
                    holdDirection = 0
                    newHold = 90
                } else if(newHold < -90) {
                    holdDirection = 1
                    newHold = -90
                }

                if(holdDirection == 1) {
                    newHold += 5
                } else {
                    newHold -= 5
                }

                if (newHold > 45 || newHold < -45) {
                    newIsFlipped = true
                } else {
                    newIsFlipped = false
                }

                var movementData = Data()
                movementData += newSpeed
                movementData += newHold
                movementData += newAngle

                var batteryData = Data()
                batteryData += Byte(0)
                batteryData += Byte(newBatteryPct)

                self.bleMessageQueue.enqueue(BleMessage(nil, MousrMessaging.createMessage(.robotPose, movementData), nil))
                self.bleMessageQueue.enqueue(BleMessage(nil, MousrMessaging.createMessage(.batteryVoltage, batteryData), nil))

                DispatchQueue.main.async {
                    self.model.isFlipped = newIsFlipped

                }

                Thread.sleep(forTimeInterval: 0.01)
            }
        }
    }

    func onSend(_ data: Data?, _ error: Error?) {
    }

    func send(_ data: Data) {
        model.log("Got command: \(data.hexEncodedString())", .verbose)
        self.model.incrementSend()

        let operation = MousrMessaging.getCommand(data)
        if operation == .move {
            let accel = MousrMessaging.getMovement(data)
            self.model.controllerAngle = CGFloat(accel.angle)
        }

        print("Operation: \(String(describing: operation))")
        if operation == .resetHeadding {
            self.model.angle = 0
            self.model.hold = 0
            self.model.speed = 0
        }

        if operation == .reverseSignal {
            let data = Data(hexString: "0xff18000003000000000000000300000000000000")
            self.bleMessageQueue.enqueue(BleMessage(nil, data, nil))
        }
    }

    func endSimulation() {
        self.isSimulationRunning = false
        model.log("Stopping simulation...", .verbose)
    }

    func disconnect() {
        self.model.onDisconnected()
    }

    func connect() {
        self.model.isBusy = true
        self.bleMessageQueue.begin()

        DispatchQueue.global(qos: .background).async {
            while self.isInitialized == false {
                self.bleMessageQueue.signal.wait()
                while self.bleMessageQueue.isEmpty() == false {
                    let m = self.bleMessageQueue.dequeue()
                    if m == nil {
                        continue
                    }

                    if m!.messageType == .initDone {
                        self.isInitialized = true
                        break
                    }
                }
            }

            self.startSimulation()
            self.onConnected(nil)
        }

        self.bleMessageQueue.enqueue(BleMessage(nil, MousrMessaging.createMessage(.initDone, nil), nil))
    }

    func onDisconnected() {
    }
}
