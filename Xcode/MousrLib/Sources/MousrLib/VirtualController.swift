//
//  VirtualController.swift
//
//  Created by Adam Meltzer on 12/01/2021.
//

import Foundation
import GameController

// Simple class for creating a virtual controller. Based on the great sample code from https://github.com/damikdk/SwiftUI-Games
public class VirtualController {
    public var virtualController : GCVirtualController? = nil

    public var handleDPad : GCControllerDirectionPadValueChangedHandler? = nil {
        didSet {
            if let dpad = virtualController?.controller?.extendedGamepad?.dpad {
                dpad.valueChangedHandler = handleDPad
            }
        }
    }

    // MARK: Delegate for events from the left analog stick
    public var handleLeftStick : GCControllerDirectionPadValueChangedHandler? = nil {
        didSet {
            if let leftStick = virtualController?.controller?.extendedGamepad?.leftThumbstick {
                leftStick.valueChangedHandler = handleLeftStick
            }
        }
    }

    // MARK: Delegate for events from the right analog stick
    public var handleRightStick : GCControllerDirectionPadValueChangedHandler? = nil {
        didSet {
            if let leftStick = virtualController?.controller?.extendedGamepad?.rightThumbstick {
                leftStick.valueChangedHandler = handleRightStick
            }
        }
    }

    public init(elements: Set<String>) {
        // TODO: Handle keyboard or real gamepad?
        virtualController = createVirtualController(elements)
    }

    public func connect() {
        if let virtualController = virtualController {
            virtualController.connect()
        }
    }

    public func disconnect() {
        if let virtualController = virtualController {
            virtualController.disconnect()
        }
    }

    let createVirtualController = { (elements: Set<String>) -> GCVirtualController in
        let configuration = GCVirtualController.Configuration()
        configuration.elements = elements // elements: [GCInputLeftThumbstick, GCInputRightThumbstick]
        let virtualController = GCVirtualController(configuration: configuration)
        return virtualController
    }
}
