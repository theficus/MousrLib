//
//  ExtensionTests.swift
//  
//
//  Created by Adam Meltzer on 12/27/2021.
//

import XCTest

class ExtensionTests: XCTestCase {

    override func setUpWithError() throws {
        // Put setup code here. This method is called before the invocation of each test method in the class.
    }

    override func tearDownWithError() throws {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
    }

    func testTicks() {
        let now = Date.nowTicks()
        print(now)
        XCTAssertTrue(now > 0)
    }
}
