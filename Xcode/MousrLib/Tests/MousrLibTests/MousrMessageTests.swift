//
//  MousrMessageTests.swift
//
//  Created by Adam Meltzer on 12/11/2021.
//

import XCTest
import MousrLib

final class MousrMessageTests : XCTestCase {
    let device = SyntheticDevice()

    func testComplexOperation() {
        let expected = "300000c8420000803f0000b4420200"
        device.move(speed: 100, hold: 1, angle: 90)
        print(self.device.sentData!.hexEncodedString())
        XCTAssert(expected == self.device.sentData!.hexEncodedString())
    }

    func testSimpleOperation() {
        let expected = "300000000000000000000000000000"
        device.stop()
        print(self.device.sentData!.hexEncodedString())
        XCTAssert(expected == self.device.sentData!.hexEncodedString())
    }

    func testInitializeMessage() {
        let actual = MousrMessaging.createInitializeCommand()
        print(actual.hexEncodedString())
    }

    func testExample() throws {
        // This is an example of a functional test case.
        // Use XCTAssert and related functions to verify your tests produce the correct results.
    }
}
