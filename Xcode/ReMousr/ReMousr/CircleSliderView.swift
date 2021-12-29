//
//  KnobView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/05/2021.
//

import SwiftUI

struct CircleSliderView : View {
    @State var outerSize : CGFloat = 60
    @State var grabberPosition : CGFloat = 0
    @State var grabberSize : CGFloat = 60
    @Binding var degrees : CGFloat
    @State var offset : CGFloat = 0

    var imageFontSize : CGFloat = 30

    var body: some View {
        VStack {
            ZStack {
                // Main circle
                Circle()
                    .fill(Color(UIColor.systemGray4))
                    .frame(width: outerSize, height: outerSize)

                // Knob
                Image(systemName: "arrow.right")
                    .font(.system(size: imageFontSize))
                    .opacity(0.9)
                    .frame(width: grabberSize, height: grabberSize)
                    .offset(x: offset, y: 0)
                    .rotationEffect(.degrees(degrees))
            }.rotationEffect(.degrees(-90)) // HACK: Point up

            // Debugging only
            Text(String(format: "%.2f", $degrees.wrappedValue))
                .font(.system(size:8))
        }
    }
}

struct CircleSliderView_Previews: PreviewProvider {
    static var previews: some View {
        CircleSliderView(degrees: .constant(CGFloat(0)))
    }
}
