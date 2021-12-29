//
//  MessageQueueTests.swift
//  
//
//  Created by Adam Meltzer on 12/28/2021.
//

import XCTest
import MousrLib

class BleMessageQueueTests: XCTestCase {

    override func setUpWithError() throws {
    }

    override func tearDownWithError() throws {
    }

    func testReceiveMessage() {
        let d = SyntheticDevice()
        d.bleMessageQueue.begin()
        d.bleMessageQueue.enqueue(BleMessage(nil, Data(hexString: "0x012345"), nil))
        print("Waiting for signal ...")
        d.bleMessageQueue.signal.wait()
        print("Got signal!")
        XCTAssertNotNil(d.bleMessageQueue.peek())
        let data = d.bleMessageQueue.dequeue()
        XCTAssertNotNil(data)
        XCTAssertEqual(3, data?.data?.count)
    }
}
