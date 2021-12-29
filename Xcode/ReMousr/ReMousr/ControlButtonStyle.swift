//
//  ControlButtonStyle.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/18/2021.
//

import SwiftUI
import WrappingHStack

struct ControlButtonStyle : ButtonStyle {
    let bordered = BorderedButtonStyle()
    var backgroundColor : Color = Color(UIColor.systemGray5)
    var imageWidth : CGFloat? = 30
    var imageHeight : CGFloat? = 30
    
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .font(.system(size: 30))
            .scaledToFill()
            .frame(width: self.imageWidth, height: self.imageHeight)
            .padding(20) // Inner padding
            .background(
                ZStack {
                    RoundedRectangle(cornerRadius: 10, style: .continuous)
                        .blendMode(.overlay)
                    RoundedRectangle(cornerRadius: 10, style: .continuous)
                        .fill(backgroundColor)
                }
            )
            //.padding(.bottom, 8) // Outer padding
            .scaleEffect(configuration.isPressed ? 0.95: 1)
    }
}

struct ControlButtonStyle_Previews: PreviewProvider {
    static var previews: some View {
        WrappingHStack {
            Button(action: {}) {
                Image(systemName: "arrow.left")
            }
            .buttonStyle(ControlButtonStyle())
            Button(action: {}) {
                Image(systemName: "arrow.right")
            }
            .buttonStyle(ControlButtonStyle())
            Button(action: {}) {
                Image(systemName: "arrow.left")
            }
            .buttonStyle(ControlButtonStyle())
            Button(action: {}) {
                Image(systemName: "arrow.right")
            }
            .buttonStyle(ControlButtonStyle())
            Button(action: {}) {
                Image(systemName: "arrow.left")
            }
            .buttonStyle(ControlButtonStyle())
            Button(action: {}) {
                Image(systemName: "arrow.right")
            }
            .buttonStyle(ControlButtonStyle())
        }
        .padding()
    }
}
