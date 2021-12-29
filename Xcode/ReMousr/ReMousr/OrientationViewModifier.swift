//
//  OrientationViewModifier.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/05/2021.
//

import Foundation
import SwiftUI

// Change content based on device orientation
struct OrientationViewModifier : ViewModifier {
    let action : (UIDeviceOrientation) -> Void

    func body(content: Content) -> some View {
        content
            .onAppear()
            .onReceive(
                NotificationCenter.default.publisher(
                    for: UIDevice.orientationDidChangeNotification)) { _ in
                        action(UIDevice.current.orientation)
                    }
    }
}
