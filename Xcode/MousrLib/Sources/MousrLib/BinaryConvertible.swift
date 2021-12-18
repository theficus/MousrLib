//
//  BinaryConvertible.swift
//
//  Created by Adam Meltzer on 12/11/2021.
//

import Foundation

public typealias Byte = UInt8

// Helper classes to easily support appending different data types to a byte blob
public protocol BinaryConvertible {
    static func + (lhs: Data, rhs: Self) -> Data
    static func += (lhs: inout Data, rhs: Self)
}

public extension BinaryConvertible {
    static func + (lhs: Data, rhs: Self) -> Data {
        var value = rhs
        let data = withUnsafeBytes(of: &value) {
            Data($0)
        }

        return lhs + data
    }

    static func += (lhs: inout Data, rhs: Self) {
        lhs = lhs + rhs
    }

    static func fromBinary<T>(_ value: [Byte]) -> T where T : BinaryConvertible {
        return value.withUnsafeBufferPointer {
            $0.baseAddress!
              .withMemoryRebound(to: T.self, capacity: 1) {
                $0.pointee
            }
        }
    }
}

// Extend various primitive types to allow them to be convertible to binary
extension UInt8 : BinaryConvertible {}
extension UInt32 : BinaryConvertible {}
extension Int32 : BinaryConvertible {}
extension Float : BinaryConvertible {}
extension Int : BinaryConvertible {}
extension Data : BinaryConvertible {
    public static func + (lhs: Data, rhs: Data) -> Data {
        var data = Data()
        data.append(lhs)
        data.append(rhs)
        return data
    }
}
