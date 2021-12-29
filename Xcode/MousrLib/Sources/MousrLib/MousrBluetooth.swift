//
//  MousrBluetooth.swift
//
//  Created by Adam Meltzer on 12/11/2021.
//

import Foundation
import CoreBluetooth

// Bluetooth communication delegate with Mousr
public class MousrBluetooth : NSObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    var _device : Device?
    var _peripheral : CBPeripheral?
    var _writeCharacteristic : CBCharacteristic?
    var _centralManager : CBCentralManager?

    var peripheral : CBPeripheral {
        get {
            return self._peripheral!
        }
    }

    var device : Device {
        get {
            return self._device!
        }
    }

    var writeCharacteristic : CBCharacteristic {
        get {
            return self._writeCharacteristic!
        }
    }

    var centralManager : CBCentralManager {
        get {
            return self._centralManager!
        }
    }

    convenience override init() {
        self.init()
        self._device = nil
        self._centralManager = nil
    }

    public required init(_ device : Device) {
        super.init()
        self._centralManager = CBCentralManager(delegate: self, queue: nil)
        self._device = device
    }

    public func discover() {
        self.centralManager.scanForPeripherals(withServices: nil, options: nil)
    }

    // Discover services
    public func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        logVerbose("Getting services ...")
        peripheral.discoverServices(nil)
    }

    // Discover characterisitcs
    public func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        logVerbose("[PERIPHERAL] didDiscoverServices peripheral: \(peripheral) error: \(String(describing: error))")
        for service in peripheral.services! {
            logVerbose("Service: \(service.uuid)")
            peripheral.discoverCharacteristics(nil, for: service)
        }
    }

    public func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if (central.state.rawValue == CBManagerState.poweredOn.rawValue) {
            logVerbose("CBCentralManager state change: \(central.state.rawValue)")
            logVerbose("Starting scan ...")
            central.scanForPeripherals(withServices: nil, options: nil)
        } else {
            logError("CBCentralManager state not usable: \(central.state.rawValue)")
        }
    }

    // Scan for peripherals and discover device
    public func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        logVerbose("[CENTRALMANAGER] didDiscover peripheral: \(peripheral) advertisementData: \(advertisementData) rssi: \(RSSI)")
        if (peripheral.name == "Mousr") {
            var isConnectable = false

            for d in advertisementData {
                //self.device.appendUIDeviceProperty(d.key, d.value)
                let v = d.value as? Int
                if(d.key == "kCBAdvDataIsConnectable") {
                    isConnectable = v == 1
                    break
                }
            }

            logVerbose("Name: \(peripheral.name ?? "(unknown)") ID: \(peripheral.identifier) State: \(peripheral.state.rawValue) RSSI: \(RSSI) Is Connectable: \(isConnectable)")
            logVerbose("Found Mousr!")
            logVerbose("\(advertisementData.description)")
            logVerbose("Stopping scan")
            central.stopScan()

            if (isConnectable == false) {
                return
            }

            peripheral.delegate = self
            self._peripheral = peripheral // The Internet says that we need a strong reference
            self.device.onDiscovered(advertisementData: advertisementData, rssi: RSSI)
        }
    }

    // Discover characteristics
    public func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        logVerbose("[PERIPHERAL] didDiscoverCharacteristicsFor service: \(service) error: \(String(describing: error))")

        for characteristic in service.characteristics! {
            logVerbose("Characteristic: UUID: \(characteristic.uuid) Properties: \(characteristic.properties) Value: \(String(format: "0x%x", characteristic.properties.rawValue))")

            logVerbose("Discovering descriptors ...")
            peripheral.discoverDescriptors(for: characteristic)

            if (characteristic.uuid == bleConstants.UartSubscribeCharacteristic) {
                logVerbose("Subscribing to notifications for characteristic \(characteristic.uuid)")
                peripheral.setNotifyValue(true, for: characteristic)
            }

            if (characteristic.uuid == bleConstants.UartWriteCharacteristic) {
                self._writeCharacteristic = characteristic
            }
        }

        self.device.onConnected(nil)
    }

    public func sendData(_ data : Data) {
        self.peripheral.writeValue(data, for: writeCharacteristic, type: .withoutResponse)
    }


    public func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        self.device.bleMessageQueue.enqueue(BleMessage(characteristic, characteristic.value, error))
    }

    // Start connection
    public func connectPeripheral() {
        self.centralManager.connect(self.peripheral, options: nil)
    }

    private func logVerbose(_ message: String) {
        self.device.log(message, .verbose)
    }

    private func logError(_ message: String) {
        self.device.log(message, .error)
    }
}
