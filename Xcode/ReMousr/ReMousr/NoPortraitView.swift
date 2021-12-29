//
//  NoPortraitView.swift
//  ReMousr
//
//  Created by Adam Meltzer on 12/05/2021.
//

import SwiftUI

struct NoPortraitView: View {
    @Environment(\.verticalSizeClass) var horizontalSizeClass

    var body: some View {
        if horizontalSizeClass != .compact {
            ZStack {
                VStack {
                    Image(systemName: "rotate.right")
                        .font(.system(size:100))

                    Text("Landscape mode required")
                }
                .zIndex(100)

                Rectangle()
                    .foregroundColor(Color(UIColor.systemGray))
                    .opacity(0.8)
            }
            .frame(maxWidth: .infinity, maxHeight: .infinity)
        }
    }
}

struct NoPortraitView_Previews: PreviewProvider {
    static var previews: some View {
        ZStack {
            VStack {
                Text("abcdefghijklmnopqrstuvwxyz")
                Text("0123456789")
                Text("abcdefghijklmnopqrstuvwxyz")
                Text("0123456789")
                Text("abcdefghijklmnopqrstuvwxyz")
                Text("0123456789")
                Text("abcdefghijklmnopqrstuvwxyz")
                Text("0123456789")
                Text("abcdefghijklmnopqrstuvwxyz")
                Text("0123456789")
            }

            NoPortraitView()
                .previewInterfaceOrientation(.portrait)
        }
    }
}
