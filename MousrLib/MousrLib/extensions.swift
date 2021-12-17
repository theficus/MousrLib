//
//  extensions.swift
//  IOSSwiftUIPlayground
//
//  Created by Adam Meltzer on 12/04/2021.
//

import Foundation
import SwiftUI
import GameController

extension View {
    // Hack to fill entire screen
    func inExpandingRectangle() -> some View {
        ZStack {
            Rectangle()
                .fill(Color.clear)
            self
        }
    }

    // Event to capture rotation events
    func onRotate(perform action: @escaping (UIDeviceOrientation) -> Void) -> some View {
        self.modifier(OrientationViewModifier(action: action))
    }
}

extension GCControllerDirectionPad {
    func getSpeed() -> Float {
        return sqrt(pow(self.xAxis.value, 2) + pow(self.yAxis.value, 2))
    }

    func getAngle() -> Float {
        let radian = atan2f(-self.xAxis.value, self.yAxis.value)
        print("y=\(self.yAxis.value) x=\(self.xAxis.value) rad=\(radian) angle=\(radian * Float(180 / Float.pi))")
        return radian * Float(180 / Float.pi)
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
