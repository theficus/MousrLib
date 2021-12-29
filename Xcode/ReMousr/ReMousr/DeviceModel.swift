//
//  DummyDevice.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/03/2021.
//

import Foundation
import SwiftUI
import MousrLib

// UI model wrapping our devices
final class DeviceModel : ObservableObject {
    private static let maxLogEntries = 100

    private var sendTimer : Timer? = nil
    private var recvTimer : Timer? = nil

    var deviceProperties : [DeviceProperty] = []

    // Connection state
    @Published var isDiscovered : Bool = false
    @Published var isConnected : Bool = false

    // Model state
    @Published var isBusy : Bool = false
    @Published var isError : Bool = false

    // Position
    @Published var speed : CGFloat = 0.0
    @Published var angle : CGFloat = -180.0
    @Published var hold : CGFloat = 0.0
    @Published var controllerAngle : CGFloat = 0.0
    
    // Status
    @Published var batteryPct : Float = 0.1
    @Published var maxSpeed : Float = 99.0
    @Published var isFlipped : Bool = false

    // Logging configuration
    @Published var isLogVerbose : Bool = false
    @Published var isLogUnknown : Bool = false
    @Published var isLogMovement : Bool = false
    @Published var isLogBattery : Bool = false
    @Published var messages : [LogMessage] = [LogMessage]()

    // Radio status
    @Published var isSending : Bool = false
    @Published var isReceiving : Bool = false
    @Published var sentPkts : Int64 = 0
    @Published var recvPkts : Int64 = 0

    // Properties
    @Published var volume : Float = 100.0
    var tailMode : mousrTailMode = .unknown

    // Alerting
    @Published var showingAlert : Bool = false
    @Published var alertDetails : String = ""

    @Published var showController : Bool = true

    private var lastSend : Date? = nil
    private var lastRecv : Date? = nil
    private var _device : Device?

    var device : Device {
        get {
            return self._device!
        }
    }

    init() {
        print("Device.init() called!")
        UIDevice.current.beginGeneratingDeviceOrientationNotifications()
#if targetEnvironment(simulator)
        self._device = DummyDevice(self)
#else
        self._device = MousrDevice(self)
#endif

        print("Device: \(String(describing: self._device))")

        self.sendTimer = Timer.scheduledTimer(withTimeInterval: 1, repeats: true, block: {_ in
            if self.isConnected == false ||
                self.sentPkts == 0 {
                return
            }

            if self.isSending == true &&
                self.lastSend!.addingTimeInterval(1.0) > Date() {
                DispatchQueue.main.async {
                    self.isSending = false
                }
            }
        })

        self.recvTimer = Timer.scheduledTimer(withTimeInterval: 1, repeats: true, block: {_ in
            if self.isConnected == false ||
                self.recvPkts == 0 {
                return
            }

            if self.isReceiving == true &&
                self.lastRecv!.addingTimeInterval(1.0) > Date() {
                DispatchQueue.main.async {
                    self.isReceiving = false
                }
            }
        })
    }

    func getVolumeText() -> (text: String, imageName: String) {
        var text : String = "unknown"
        var imageName : String = "questionmark"

        if self.volume <= 0 {
            text = "off"
        } else if self.volume <= 33 {
            text = "low"
        } else if self.volume <= 66 {
            text = "mid"
        } else {
            text = "loud"
        }

        imageName = "music.quarternote.3"
        return (text: text, imageName: imageName)
    }

    func getTailModeText() -> (text: String, imageName: String) {
        var text : String
        var imageName : String
        switch self.tailMode {
        case .bounce:
            text = "bounce"
            break
        case .chase:
            text = "chase"
            break
        case .flick:
            text = "flick"
            break
        default:
            text = "unknown"
        }

        imageName = "person.fill.and.arrow.left.and.arrow.right"

        return (text: text, imageName: imageName)
    }

    func setTailMode(tailMode : mousrTailMode, tailText : inout String, imageName : inout String) {
        self.device.setTailMode(tailMode)
        self.tailMode = tailMode
        let v = getTailModeText()
        tailText = v.text
        imageName = v.imageName
    }

    func setVolume(volume : UInt8, volumeText : inout String, imageName : inout String) {
        if (volume > 100) {
            self.log("Invalid volume level: \(volume)", .error)
        }

        self.device.setVolume(volume: volume)
        self.volume = Float(volume)
        let v = getVolumeText()
        volumeText = v.text
        imageName = v.imageName
    }

    // MARK: Auto-dispatched
    func incrementSend() {
        DispatchQueue.main.async {
            self.sentPkts += 1
            self.isSending = true
            self.lastSend = Date.now
        }
    }

    // MARK: Auto-dispatched
    func incrementRecv() {
        DispatchQueue.main.async {
            self.recvPkts += 1
            self.isReceiving = true
            self.lastRecv = Date.now
        }
    }

    // NOOP unless in the simulator
    func startSimulation() {
        if self.device is DummyDevice {
            (self.device as! DummyDevice).startSimulation()
        }
    }

    // NOOP unless in the simulator
    func endSimulation() {
        if self.device is DummyDevice {
            (self.device as! DummyDevice).endSimulation()
        }
    }

    // MARK: Set the positional data in the UI
    func setPosition(_ speed : Float, _ hold : Float, _ angle : Float) {
        DispatchQueue.main.async {
            self.angle = CGFloat(angle)
            self.hold = CGFloat(hold)
            self.speed = CGFloat(speed)
        }
    }

    func setBattery(_ batteryPct : UInt8) {
        DispatchQueue.main.async {
            self.batteryPct = Float(batteryPct)
        }
    }

    func onDisconnected() {
        DispatchQueue.main.async {
            self.isConnected = false
            self.isDiscovered = false
            self.sentPkts = 0
            self.recvPkts = 0
        }
    }

    func onConnected() {
        DispatchQueue.main.async {
            self.isBusy = false
            self.isConnected = true
            self.isDiscovered = true
        }
        
        self.device.initialize()
    }

    func clearLog() {
        messages.removeAll()
    }

    func log(_ message : String, _ messageType : LogMessageType) {
        DispatchQueue.main.async {
            while (self.messages.count > DeviceModel.maxLogEntries) {
                //print("Message count \(self.messages.count) greater than max \(DeviceModel.maxLogEntries). Pruning...")
                self.messages.remove(at: 0)
            }

            print("\(messageType): \(message)")

            self.messages.append(
                LogMessage(
                    message: message,
                    messageType: messageType))
        }
    }
}
