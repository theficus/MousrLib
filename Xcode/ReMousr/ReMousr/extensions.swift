//
//  extensions.swift
//  ReMousr
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
