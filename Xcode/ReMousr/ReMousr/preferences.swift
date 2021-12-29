//
//  preferences.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/08/2021.
//

import Foundation
import SwiftUI

struct SpeedPreferenceKey : PreferenceKey {
    static var defaultValue : Float = 50.0
    static func reduce(value: inout Float, nextValue: () -> Float) {
        value = nextValue()
    }
}
