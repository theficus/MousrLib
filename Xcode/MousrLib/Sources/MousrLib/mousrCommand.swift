//
//  mousrCommand.swift
//
//  Created by Adam Meltzer on 12/18/2021.
//

import Foundation

// Represents a "pose" command
public enum mousrCommand : Byte {
    case stop = 0x0
    case spin = 0x1
    case move = 0x2
    case resetHeadding = 0x3
    case getDebugLog = 0x4
    case spinPlan = 0x5
    case enterDfuMode = 0x8
    case powerOff = 0x9
    case sleep = 0xa
    case configAutoMode = 0xf
    case chirp = 0x12
    case soundVolume = 0x13
    case flickSignal = 0x17
    case reverseSignal = 0x18
    case tailCalibrationSignal = 0x19
    case setTailSignal = 0x1a
    case initializeDevice = 0x1c
    case flipRobot = 0x1f
    case configDriverAssist = 0x29
    case tutorialStep = 0x2d
    case cmdSetTime = 0x2e
    case invalid = 0x64
    case unknown = 0xff
}
