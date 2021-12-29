//
//  extensions.swift
//
//  Created by Adam Meltzer on 12/04/2021.
//

import Foundation
import GameController

public extension GCControllerDirectionPad {
    // MARK: Gets the direction of the controller
    func getDirection() -> (angle : Float, speed : Float, zeroed : Bool) {
        let speed = sqrt(pow(self.xAxis.value, 2) + pow(self.yAxis.value, 2))

        let radian = atan2f(-self.xAxis.value, self.yAxis.value)
        print("y=\(self.yAxis.value) x=\(self.xAxis.value) rad=\(radian) angle=\(radian * Float(180 / Float.pi))")
        let angle = -radian * Float(180 / Float.pi)

        return (angle: angle, speed: speed, zeroed: speed == 0 && angle == 0)
    }
}

public extension Date {
    static func ticks(_ date : Date) -> Int64 {
        if date == Date.distantPast {
            return dateConstants.ticksMinValue
        }

        if date == Date.distantFuture {
            return dateConstants.ticksMaxValue
        }

        let seconds = Double(date.timeIntervalSince1970)
        let ticksSince1970 = Int64(round(seconds * dateConstants.ticksPerSecond))
        return dateConstants.ticksAt1970 + ticksSince1970
    }

    static func ticksAt1970() -> Int64 {
        return dateConstants.ticksAt1970
    }

    static func nowTicks() -> Int64 {
        return ticks(Date.now)
    }
}

// Great example of a simple locking method call via: https://stackoverflow.com/a/62620203
public extension DispatchSemaphore {
    @discardableResult
    func with<T>(_ block: () throws -> T) rethrows -> T {
        wait()
        defer {
            signal()
        }
        return try block()
    }
}

public extension Data {
    // Convert hex string into Data
    init?(hexString: String) {
        let start = hexString.starts(with: "0x") ? 2 : 0
        let len = (hexString.count) / 2

        var data = Data(capacity: len - start)
        var i = hexString.startIndex
        for _ in 0..<len {
            let j = hexString.index(i, offsetBy: 2)
            let b = hexString[i..<j]
            if b != "0x" {
                if var num = UInt8(b, radix: 16) {
                    data.append(&num, count: 1)
                } else {
                    return nil
                }
            }

            i = j
        }

        self = data
    }

    func hexEncodedString() -> String {
        let format = "%02hhx"
        return self.map { String(format: format, $0) }.joined()
    }

    func valueFromData<T>(_ start : Int = 0) -> T {
        let length = MemoryLayout<T>.size
        var value : T? = nil
        if count >= start + length {
            value = subdata(in: start ..< start + length).withUnsafeBytes { $0.load(as: T.self) }
        }

        return value!
    }
    
    func valueFromDataWithPos<T>(_ start: Int = 0) -> (T, position : Int) {
        let length = MemoryLayout<T>.size
        var value : T? = nil
        if count >= start + length {
            value = subdata(in: start ..< start + length).withUnsafeBytes { $0.load(as: T.self) }
        }

        return (value!, start + length)
    }
}
